#ifndef TEENSY_RESAMPLING_INDEXABLE_FILE_H
#define TEENSY_RESAMPLING_INDEXABLE_FILE_H

#include <Arduino.h>
#include <SD.h>
#include <vector>

namespace newdigate {


class indexedbuffer 
{
public:
	indexedbuffer(int16_t samples) { buffer = new int16_t[samples]; }
	~indexedbuffer(void) { delete [] buffer; }
    uint32_t index;
    int16_t buffer_size;
    int16_t *buffer;
};

constexpr bool isPowerOf2(size_t value){
    return !(value == 0) && !(value & (value - 1));
}

template<size_t BUFFER_SIZE, size_t MAX_NUM_BUFFERS, class TFile> // BUFFER_SIZE needs to be a power of two
class IndexableFile {
public:
    static_assert(isPowerOf2(BUFFER_SIZE), "BUFFER_SIZE must be a power of 2");
    
    virtual TFile open(const char *filename) = 0;

    static constexpr size_t element_size = sizeof(int16_t);
    size_t buffer_to_index_shift;
	size_t buffer_mask;
    IndexableFile(const char *filename) : 
        buffer_to_index_shift(log2(BUFFER_SIZE)), 
		buffer_mask(~(BUFFER_SIZE - 1)),
        _buffers()
    {
            _filename = new char[strlen(filename)+1] {0};
            memcpy(_filename, filename, strlen(filename));
    }
  
  
    virtual ~IndexableFile() {
        close();
    }

	
	size_t getBufferSize(void) {return BUFFER_SIZE; }

	
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

		return bytesRead;
	}
	
	
	/**
	 * Preload buffere.
	 */
	size_t preLoadBuffers(int i) //!< first sample number to load
	{
		size_t numInVector = _buffers.size();
		size_t loaded = 0;
		
		for (int bufn = 0; bufn < MAX_NUM_BUFFERS; bufn++)
		{
			indexedbuffer* buf;
			if (bufn >= numInVector)
			{
				buf = new indexedbuffer(BUFFER_SIZE);
				_buffers.push_back(buf);
			}
			else
				buf = _buffers[bufn];
			
			loaded += loadBuffer(buf,i);
			i += BUFFER_SIZE;
		}
		return loaded;
	}
	
	/*
	 Retrieve a single sample from an "indexed file".
	 
	 By using a set of buffers as an in-memory cache, we aim to retrieve from the
	 cache most of the time, and only do a file read on a cache miss.
	 
	 Note that the index is independent of the channel count.
	*/
    int16_t &operator[](int i) {
        int32_t indexFor_i = i >> buffer_to_index_shift;
        indexedbuffer *match = find_with_index(indexFor_i); // find which buffer has the sample
		
        if (match == nullptr)  // none of the buffers contains the required sample
		{
            if (_buffers.size() > MAX_NUM_BUFFERS - 1) // reached limit of number of allowed buffers
			{
                match = _buffers[0]; // assume the oldest buffer is no longer needed
                _buffers.erase(_buffers.begin());	// remove from vector
				// we already have the indexedbuffer and its sample storage - keep those
            }
			else
			{
				match = new indexedbuffer(BUFFER_SIZE);  // create new indexedbuffer object
			}
			
			loadBuffer(match,i);

			// Add the newly-loaded indexedbuffer at the back of the vector: it's the newest one.
			// Do this here because it may be either a new or a recycled buffer.
            _buffers.push_back(match);			
        }
		
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

protected:
    TFile _file;
    char *_filename;
    std::vector<indexedbuffer*> _buffers;

	// search vector of buffers for the one with a 
	// specific sample (set) in it.
	// return pointer to correct indexedbuffer, or nullptr if not buffered
    indexedbuffer* find_with_index(uint32_t i) {
        for (auto && x : _buffers){
            if (x->index == i) {
                return x;
            }
        }
        return nullptr;
    }
};

}



#endif 
