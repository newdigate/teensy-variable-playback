#ifndef TEENSY_RESAMPLING_INDEXABLE_FILE_H
#define TEENSY_RESAMPLING_INDEXABLE_FILE_H

#include <Arduino.h>
#include <SD.h>
#include <vector>

namespace newdigate {


struct indexedbuffer {
    uint32_t index;
    int16_t buffer_size;
    int16_t *buffer;
};

constexpr bool isPowerOf2(size_t value){
    return !(value == 0) && !(value & (value - 1));
}

template<size_t BUFFER_SIZE, size_t MAX_NUM_BUFFERS, class TFile> // BUFFER_SIZE needs to be a power of two
class IndexableFile {
protected:
    TFile _file;
    char *_filename;
    std::vector<indexedbuffer*> _buffers;

    indexedbuffer* find_with_index(uint32_t i) {
        for (auto && x : _buffers){
            if (x->index == i) {
                return x;
            }
        }
        return nullptr;
    }

public:
    static_assert(isPowerOf2(BUFFER_SIZE), "BUFFER_SIZE must be a power of 2");
    
    virtual TFile open(const char *filename) = 0;

    static constexpr size_t element_size = sizeof(int16_t);
    size_t buffer_to_index_shift;
    IndexableFile(const char *filename) : 
        _buffers(),
        buffer_to_index_shift(log2(BUFFER_SIZE)) 
    {
            _filename = new char[strlen(filename)+1] {0};
            memcpy(_filename, filename, strlen(filename));
    }
    
    virtual ~IndexableFile() {
        close();
    }

    int16_t &operator[](int i) {
        int32_t indexFor_i = i >> buffer_to_index_shift;
        indexedbuffer *match = find_with_index(indexFor_i);
        if (match == nullptr) {
            if (_buffers.size() > MAX_NUM_BUFFERS - 1) {
                indexedbuffer *first = _buffers[0];
                _buffers.erase(_buffers.begin());
                delete [] first->buffer;
                delete first;
            }
            indexedbuffer *next = new indexedbuffer();
            next->index = indexFor_i;
            next->buffer = new int16_t[BUFFER_SIZE];
            size_t basePos = indexFor_i << buffer_to_index_shift;
            size_t seekPos = basePos * element_size;
            _file.seek(seekPos);
            int16_t bytesRead = _file.read(next->buffer, BUFFER_SIZE * element_size);
            #ifndef TEENSYDUINO
            if (!_file.available()){  
                _file.close();
                _file = open(_filename);
            }
            #endif
            next->buffer_size = bytesRead;
            _buffers.push_back(next);
            match = next;
        }
        return match->buffer[i % BUFFER_SIZE];
    }

    void close() {
        if (_file.available()) {
            _file.close();
        }

       for (auto && x : _buffers){
            delete [] x->buffer;
            delete x;
        }
        _buffers.clear();

        if (_filename != nullptr) {
            delete [] _filename;
            _filename = nullptr;
        }    
    }
};

}



#endif 
