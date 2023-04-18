#ifndef TEENSY_RESAMPLING_INDEXABLE_FILE_H
#define TEENSY_RESAMPLING_INDEXABLE_FILE_H

#include <Arduino.h>
#include <SD.h>
#include <vector>
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
			buffer = (int16_t*) extmem_malloc(samples * sizeof *buffer); 
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
public:
	enum {constructed = '.', loaded = 'l', unused = 'u', read = 'r', 
		 loop_start = 'a', loop_start_next = 'b', loop_finish_prev = 'y', loop_finish = 'z'};
    static_assert(isPowerOf2(BUFFER_SIZE), "BUFFER_SIZE must be a power of 2");
    
    virtual TFile open(const char *filename) = 0;

    static constexpr size_t element_size = sizeof(int16_t);
    size_t buffer_to_index_shift;
	size_t buffer_mask;
	int fails;
	
    IndexableFile(const char *filename) : 
        buffer_to_index_shift(log2(BUFFER_SIZE)), 
		buffer_mask(~(BUFFER_SIZE - 1)),
		fails(0),
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
		while (!_buffers.empty() && unused == _buffers[0]->status)
		{
			size_t nextIdx=0;
			char buf1[20],buf2[20];
			
			//getStatus(buf1);
			
			indexedbuffer* reload = _buffers[0];
			_buffers.erase(_buffers.begin());	
            _buffers.push_back(reload);

			if (playbackRate >= 0.0f)
				nextIdx = findMaxBuffer()->index + 1; // could wrap, but unlikely...
			else
			{
				nextIdx = findMinBuffer()->index;	// might well be 0
				if (nextIdx > 0)
					nextIdx--;
			}
			nextIdx <<= buffer_to_index_shift;

			loadBuffer(reload,nextIdx);
			//getStatus(buf2);
			
			//Serial.printf("Loaded from index %d: %s -> %s\n",nextIdx,buf1,buf2);
		}
	}
	
	void resetStatus(void) 
	{
		for (auto && x : _buffers)
			x->status = loaded;
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
		size_t max=0;
		indexedbuffer* rv = nullptr;
		
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
				if (loop_start != x->status 
				 && loop_start_next != x->status 
				 && loop_finish_prev != x->status 
				 && loop_finish != x->status 
				 && x->index > max)
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
		size_t min=-1; // actually will be maximum value, as size_t is unsigned
		indexedbuffer* rv = nullptr;
		
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
				if (loop_start != x->status 
				 && loop_start_next != x->status 
				 && loop_finish_prev != x->status 
				 && loop_finish != x->status 
				 && x->index < min)
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
		size_t loaded = 0;
		
		_bufInPSRAM = bufInPSRAM;
		
		for (int bufn = 0; bufn < MAX_NUM_BUFFERS; bufn++)
		{
			indexedbuffer* buf;
			if (bufn >= numInVector)
			{
				buf = new indexedbuffer(BUFFER_SIZE, bufInPSRAM);
				_buffers.push_back(buf);
			}
			else
				buf = _buffers[bufn];
			
			loaded += loadBuffer(buf,i);
			if (forwards)
				i += BUFFER_SIZE;
			else
				i -= BUFFER_SIZE;
		}
		
		return loaded;
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
	void setLoopStart(int32_t l) { _loop_start = l; }
	void setLoopFinish(int32_t l) { _loop_finish = l; }

protected:
    TFile _file;
    char *_filename;
	
	// we need a copy of the loop parameters in
	// order to pre-load the buffers correctly
    loop_type _loop_type = loop_type::looptype_none;
    int32_t _loop_start = 0;
    int32_t _loop_finish = 0;    
	
	bool _bufInPSRAM = false;
	
	// vector of audio buffers with status etc.
	std::vector<indexedbuffer*> _buffers;

	// search vector of buffers for the one with a 
	// specific sample (set) in it.
	// return pointer to correct indexedbuffer, or nullptr if not buffered
    indexedbuffer* find_with_index(uint32_t i) {
        for (auto && x : _buffers){
            if (x->index == i) {
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
