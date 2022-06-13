#ifndef TEENSY_RESAMPLING_INDEXABLE_FILE_H
#define TEENSY_RESAMPLING_INDEXABLE_FILE_H

#include <Arduino.h>
#include <SD.h>
#include <LittleFS.h>
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

template<size_t BUFFER_SIZE, size_t MAX_NUM_BUFFERS> // BUFFER_SIZE needs to be a power of two
class IndexableFile {
public:
    static_assert(isPowerOf2(BUFFER_SIZE), "BUFFER_SIZE must be a power of 2");

    static constexpr size_t element_size = sizeof(int16_t);
    size_t buffer_to_index_shift;
    IndexableFile(const char *filename) : 
        _buffers(),
        buffer_to_index_shift(log2(BUFFER_SIZE)) {
            _filename = new char[strlen(filename)+1] {0};
            memcpy(_filename, filename, strlen(filename));
            _file = SD.open(_filename);
    }
    ~IndexableFile() {
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
                __disable_irq();
                _file.close();
                _file = SD.open(_filename);
                __enable_irq();
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
            __disable_irq();
            _file.close();
            __enable_irq();
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

private:
    File _file;
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
};

template<size_t BUFFER_SIZE, size_t MAX_NUM_BUFFERS> // BUFFER_SIZE needs to be a power of two
class IndexableLittleFSFile {
public:
    static_assert(isPowerOf2(BUFFER_SIZE), "BUFFER_SIZE must be a power of 2");

    static constexpr size_t element_size = sizeof(int16_t);
    size_t buffer_to_index_shift;
    IndexableLittleFSFile(LittleFS &fs, const char *filename) : 
        _buffers(),
        _myFS(fs),
        buffer_to_index_shift(log2(BUFFER_SIZE)) {
            _filename = new char[strlen(filename)+1] {0};
            memcpy(_filename, filename, strlen(filename));
            _file = _myFS.open(_filename);
    }
    ~IndexableLittleFSFile() {
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
                __disable_irq();
                _file.close();
                _file = _myFS.open(_filename);
                __enable_irq();
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
            __disable_irq();
            _file.close();
            __enable_irq();
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

private:
    LittleFS &_myFS;
    File _file;
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
};


}



#endif 
