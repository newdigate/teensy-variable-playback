#ifndef TEENSY_RESAMPLING_INDEXABLE_FILE_H
#define TEENSY_RESAMPLING_INDEXABLE_FILE_H

#include <Arduino.h>
#include <SD.h>
#include <vector>
#include <algorithm> // to get std::reverse
#include "loop_type.h"


namespace newdigate {


class indexedbuffer 
{
public:
	indexedbuffer(int16_t samples, bool usePSRAM = false) 
		: status('.'), 
		  bufInPSRAM(usePSRAM) 
	{ 
		if (usePSRAM)
		{
//digitalWriteFast(36,1);
			buffer = (int16_t*) extmem_malloc(samples * sizeof *buffer); 
//digitalWriteFast(36,0);
		}
		else
			buffer = new int16_t[samples]; 
	}
	~indexedbuffer(void) 
	{ 
		if (bufInPSRAM)
			extmem_free(buffer);
		else
			delete [] buffer; 
	}
    uint32_t index;
    int16_t buffer_size;
	char status;
	bool bufInPSRAM; // true if buffer points to memory allocated in PSRAM
    int16_t *buffer;
};


constexpr bool isPowerOf2(size_t value){
    return !(value == 0) && !(value & (value - 1));
}


template<size_t BUFFER_SIZE, size_t MAX_NUM_BUFFERS, class TFile> // BUFFER_SIZE needs to be a power of two
class IndexableFile
{
	enum bufferAction_e {nopBuffer, moveBuffer, reverseBuffer};
public:
	enum {constructed = '.', loaded = 'l', unused = 'u', read = 'r'};
    static_assert(isPowerOf2(BUFFER_SIZE), "BUFFER_SIZE must be a power of 2");
    
    virtual TFile open(const char *filename) = 0;

    static constexpr size_t element_size = sizeof(int16_t);
    size_t buffer_to_index_shift;
	size_t buffer_mask;
	int fails;
	float prevPlaybackRate; // record of playback rate in force on previous reload
	
    IndexableFile(const char *filename) : 
        buffer_to_index_shift(log2(BUFFER_SIZE)), 
		buffer_mask(~(BUFFER_SIZE - 1)),
		fails(0),
		prevPlaybackRate(0.0f),
        _buffers()
    {
		_filename = new char[strlen(filename)+1] {0};
		memcpy(_filename, filename, strlen(filename));
    }
  
  
    virtual ~IndexableFile() {
        close();
    }

	
	size_t getBufferSize(void) {return (size_t) fails;}//BUFFER_SIZE; }
	
	size_t getBufferCount(void) {return MAX_NUM_BUFFERS; }
	
	
	/**
	 * Function to reload unused buffer.
	 *
	 * This is executed via EventResponder code from yield(), so
	 * not from interrupt and effectively within the context of the
	 * user sketch.
	 *
	 * The playback rate parameter is used to determine whether
	 * to reload the buffer with data from after the maximum
	 * available (forward playback), or before the minimum
	 * (reverse playback).
	 */
	void triggerReload(float playbackRate) //!< direction and speed of playback
	{
		float pbr = playbackRate;
		
		// check if pingpong has changed direction since last reload
		if (loop_type::looptype_pingpong == _loop_type
		 && ((playbackRate >= 0.0f && prevPlaybackRate <  0.0f)
		  || (playbackRate <  0.0f && prevPlaybackRate >= 0.0f)))
		{
			// correct timing of buffer reverse vs.
			// next index depends on check using
			// the old direction:
			playbackRate = prevPlaybackRate; // say playback is in old direction
		}
		
		while (!_buffers.empty() && unused == _buffers[0]->status)
		{
			int nextIdx=0;
			bool doLoad = true;
			bufferAction_e bufferAction = moveBuffer;
			
			//getStatus(buf1);
			
			// We expect to use buffers starting at front of vector
			indexedbuffer* reload = _buffers[0]; // point to buffer to re-use

			if (playbackRate >= 0.0f)
			{
				nextIdx = findMaxBuffer()->index + 1; // could wrap, but unlikely...
				switch (_loop_type)
				{
					case loop_type::looptype_none:
						break;
						
					case loop_type::looptype_repeat:
						if (nextIdx > _loop_finish_blocks) 	// trying to go too far...
						{
							/*
							indexedbuffer* nl = _buffers.back();
							int nextLast = nl->index; // get index of previous reload
							*/
							int nextLast = _buffers[MAX_NUM_BUFFERS - 1]->index;
							if (nextLast + 1 == nextIdx) 	// haven't looped back already...
								nextIdx = _loop_start_blocks;	// ...go back to start
							else
								nextIdx = nextLast + 1; // ...preload next block
						}
						break;
						
					case loop_type::looptype_pingpong: // forwards, we're pinging
						if (nextIdx > _loop_finish_blocks) 	// trying to go too far...
						{
							bufferAction = nopBuffer;
							if (read == _buffers[MAX_NUM_BUFFERS - 1]->status)  // last update used last buffer, so...
							{
								bufferAction = reverseBuffer;					// ...reverse the order, start ponging
							}
							doLoad = false;					// ...no load needed
						}
						break;
						
				}
			}
			else
			{
				nextIdx = findMinBuffer()->index - 1;	// might well be < 0
				switch (_loop_type)
				{
					case loop_type::looptype_none:
						if (nextIdx < 0)
							doLoad = false;
						break;
						
					case loop_type::looptype_repeat:
						if (nextIdx < _loop_start_blocks) 	// trying to go too far...
						{
							int nextLast = _buffers[MAX_NUM_BUFFERS - 1]->index;
							if (nextLast - 1 == nextIdx) 	// haven't looped back already...
								nextIdx = _loop_finish_blocks;	// ...go back to loop start (end of file)
							else
								nextIdx = nextLast - 1; // ...preload next block
						}
						break;
						
					case loop_type::looptype_pingpong: // reverse, we're ponging
						if (nextIdx < _loop_start_blocks) 	// trying to go too far...
						{
							bufferAction = nopBuffer;
							if (read == _buffers[MAX_NUM_BUFFERS - 1]->status)  // last update used last buffer, so...
							{
								bufferAction = reverseBuffer;					// ...reverse the order, start pinging
							}
							doLoad = false;					// ...no load needed
						}
						break;
						
				}			
			}
			nextIdx <<= buffer_to_index_shift;
			
			if (doLoad)
				loadBuffer(reload,nextIdx);

//			if (nopBuffer != bufferAction) // do stuff to buffer ordering
			{
				// Mustn't be interrupted by audio update.
				// A quick vector shuffle won't leave updates
				// disabled for too long, so this is OK.
				bool intEnabled = NVIC_IS_ENABLED(IRQ_SOFTWARE) != 0; 
				AudioNoInterrupts(); // ===============================================
				switch (bufferAction)
				{
					case reverseBuffer: // reverse the order
						std::reverse(_buffers.begin(), _buffers.end()); 
						prevPlaybackRate = -prevPlaybackRate; // record new playback direction
						break;
						
					case moveBuffer: // move front element to back
						_buffers.erase(_buffers.begin());	
						_buffers.push_back(reload);
						break;
						
					default:
						break;
				}
								
				if (intEnabled)
					AudioInterrupts(); // ===============================================
			}
//			else
	
			if (nopBuffer == bufferAction) // do stuff to buffer ordering
				break; // didn't move buffers so first one will remain in "unused" state
			
			//Serial.printf("Loaded from index %d: %s -> %s\n",nextIdx,buf1,buf2);
		}
		resetStatus(); // reset ready for next update to mark blocks read
		if (0.0f == prevPlaybackRate)
			prevPlaybackRate = pbr;
	}
	
	void resetStatus(void) 
	{
		bool intEnabled = NVIC_IS_ENABLED(IRQ_SOFTWARE) != 0; 
		AudioNoInterrupts(); // ===============================================
		for (auto && x : _buffers)
			x->status = loaded;
		if (intEnabled)
			AudioInterrupts(); // ===============================================
	}
	
	void getStatus(char* buf)
	{
		for (auto && x : _buffers)
			*buf++ = x->status;
		*buf=0;
	}

	/**
	 * Find the buffer with the maximum index value.
	 *
	 * If we're playing forwards, this buffer will be the last
	 * to be played, and if we want to do a speculative pre-load
	 * we should probably load (at least) the next block.
	 *
	 * Normally, if we're keeping the loop ends buffered, we
	 * must disregard these as they'll give an unhelpful
	 * return value...
	 *
	 * \return pointer to buffer
	 */
	indexedbuffer* findMaxBuffer(bool includeLoopEnds = false)
	{
		indexedbuffer* rv = _buffers.at(0);
		size_t max = rv->index;
		
		if (includeLoopEnds)
		{
			for (auto && x : _buffers)
			{
				if (x->index > max)
				{
					max = x->index;
					rv = x;
				}
			}
		}
		else
		{
			for (auto && x : _buffers)
			{
				if (x->index > max)
				{
					max = x->index;
					rv = x;
				}
			}
		}	
		
		return rv;
	}

	/**
	 * Find buffer with minimum index value
	 */
	indexedbuffer* findMinBuffer(bool includeLoopEnds = false)
	{
		indexedbuffer* rv = _buffers.at(0);
		size_t min = rv->index;
		
		if (includeLoopEnds)
		{
			for (auto && x : _buffers)
			{
				if (x->index < min)
				{
					min = x->index;
					rv = x;
				}
			}
		}
		else
		{
			for (auto && x : _buffers)
			{
				if (x->index < min)
				{
					min = x->index;
					rv = x;
				}
			}
		}	
		
		return rv;
	}


	indexedbuffer* bufs;
	/**
	 * Load provided buffer with sample data.
	 */
	size_t loadBuffer(indexedbuffer* buf,	//!< buffer to load
					  int i)				//!< index of first sample
	{		
// digitalWriteFast(35,1);
		// figure out file position to load into the buffer
		size_t basePos = i & buffer_mask;
		size_t seekPos = basePos * element_size;
		
		// load the sample data from the file: note BUFFER_SIZE is in samples
		_file.seek(seekPos);
		size_t bytesRead = _file.read(buf->buffer, BUFFER_SIZE * element_size);
		#ifndef TEENSYDUINO
		if (!_file.available()){  
			_file.close();
			_file = open(_filename);
		}
		#endif
		
		// fill in remaining indexedbuffer information: at the end of the file
		// we may not have enough samples to fill the buffer.
		buf->index = i >> buffer_to_index_shift;	// say which samples it'll have in it
		buf->buffer_size = bytesRead; // amount of data (bytes, not samples...)
		buf->status = loaded;
		
		bufs = _buffers[0];
		
// digitalWriteFast(35,0);
		return bytesRead;
	}
	
	
	/**
	 * Preload buffere.
	 */
	size_t preLoadBuffers(int i, 				//!< first sample number to load
						 bool bufInPSRAM,		//!< true if we want to use PSRAM to buffer audio
						 bool forwards = true) 	//!< true if playback is forwards (at the start)
	{
		size_t numInVector = _buffers.size();
		size_t total = 0, loaded;
		
		_bufInPSRAM = bufInPSRAM;
		
		for (unsigned int bufn = 0; bufn < MAX_NUM_BUFFERS; bufn++)
		{
			indexedbuffer* buf;
			if (bufn >= numInVector)
			{
				// this is SLOW (~200us) - why? Only if in PSRAM?
				buf = new indexedbuffer(BUFFER_SIZE, bufInPSRAM);
				
				bool intEnabled = NVIC_IS_ENABLED(IRQ_SOFTWARE) != 0; 
				AudioNoInterrupts();
				_buffers.push_back(buf);
				if (intEnabled)
					AudioInterrupts();
			}
			else
				buf = _buffers[bufn];
			
			loaded = loadBuffer(buf,i);
			total += loaded;
			if (loaded < BUFFER_SIZE) // end of file, no more buffers needed
				break;
			
			if (forwards)
				i += BUFFER_SIZE;
			else
				i -= BUFFER_SIZE;
		}
		
		return total;
	}
	
	/*
	 Retrieve a single sample from an "indexed file".
	 
	 By using a set of buffers as an in-memory cache, we aim to retrieve from the
	 cache most of the time, and only do a file read on a cache miss.
	 
	 Note that the index is independent of the channel count.
	*/
	int16_t zero = 0;
    int16_t &operator[](int i) {
        int32_t indexFor_i = i >> buffer_to_index_shift;
        indexedbuffer *match = find_with_index(indexFor_i); // find which buffer has the sample
		
        if (match == nullptr)  // none of the buffers contains the required sample
		{
			fails++;
			zero = 0; 		// in case someone wrote to the reference at some point!
			return zero;	// stutter, but don't crash due to reading filesystem under interrupt
        }
		match->status = 'r';
		
        return match->buffer[i & ~buffer_mask];
    }

    void close() {
		// close file if open
        if (_file.available()) {
            _file.close();
        }

		// delete all buffered data and clear the vector
		for (auto && x : _buffers){
            delete x;
        }
        _buffers.clear();

		// delete the filename
        if (_filename != nullptr) {
            delete [] _filename;
            _filename = nullptr;
        }    
    }
	
	void setLoopType(loop_type l) { _loop_type = l; }
	void setLoopStart(int32_t l) { _loop_start_blocks = l >> buffer_to_index_shift; }
	void setLoopFinish(int32_t l) { _loop_finish_blocks = l >> buffer_to_index_shift; }

protected:
    TFile _file;
    char *_filename;
	
	// we need a copy of the loop parameters in
	// order to pre-load the buffers correctly
    loop_type _loop_type = loop_type::looptype_none;
    int32_t _loop_start_blocks = 0;
    int32_t _loop_finish_blocks = 0;    
	
	bool _bufInPSRAM = false;
	
	// vector of audio buffers with status etc.
	std::vector<indexedbuffer*> _buffers;

	// search vector of buffers for the one with a 
	// specific sample (set) in it.
	// return pointer to correct indexedbuffer, or nullptr if not buffered
    indexedbuffer* find_with_index(uint32_t i) {
        for (auto && x : _buffers){
            if (x->index == i && x->buffer_size > 0) {
                return x;
            }
			if (read != x->status)
				x->status = unused;
        }
        return nullptr;
    }
};

}



#endif 
