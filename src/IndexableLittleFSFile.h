#ifndef TEENSY_RESAMPLING_INDEXABLELITTLEFS_FILE_H
#define TEENSY_RESAMPLING_INDEXABLELITTLEFS_FILE_H

#include <Arduino.h>
#include "IndexableFile.h"
#include <LittleFS.h>
#include <vector>

namespace newdigate {

template<size_t BUFFER_SIZE, size_t MAX_NUM_BUFFERS> // BUFFER_SIZE needs to be a power of two
class IndexableLittleFSFile : public IndexableFile<BUFFER_SIZE, MAX_NUM_BUFFERS, File> {
public:
    static_assert(isPowerOf2(BUFFER_SIZE), "BUFFER_SIZE must be a power of 2");

    IndexableLittleFSFile(LittleFS &fs, const char *filename) : 
        IndexableFile<BUFFER_SIZE, MAX_NUM_BUFFERS, File>(filename),
        _myFS(fs) 
    {
        IndexableFile<BUFFER_SIZE, MAX_NUM_BUFFERS,File>::_file = _myFS.open(filename);
    }
    
    File open(const char *filename) override {
        return _myFS.open(filename);
    }

    virtual ~IndexableLittleFSFile() {
        IndexableFile<BUFFER_SIZE, MAX_NUM_BUFFERS,File>::close();
    }

    int16_t &operator[](int i) {
        return IndexableFile<BUFFER_SIZE, MAX_NUM_BUFFERS,File>::operator[](i);
    }

private:
    LittleFS &_myFS;
};

}



#endif //TEENSY_RESAMPLING_INDEXABLELITTLEFS_FILE_H
