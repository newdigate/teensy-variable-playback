#ifndef TEENSY_RESAMPLING_INDEXABLESERIALFLASH_FILE_H
#define TEENSY_RESAMPLING_INDEXABLESERIALFLASH_FILE_H

#include <Arduino.h>
#include "IndexableFile.h"
#include <SerialFlash.h>
#include <vector>

namespace newdigate {

template<size_t BUFFER_SIZE, size_t MAX_NUM_BUFFERS> // BUFFER_SIZE needs to be a power of two
class IndexableSerialFlashFile : public IndexableFile<BUFFER_SIZE, MAX_NUM_BUFFERS,SerialFlashFile> {
public:
    static_assert(isPowerOf2(BUFFER_SIZE), "BUFFER_SIZE must be a power of 2");

    IndexableSerialFlashFile(SerialFlashChip &fs, const char *filename) : 
        IndexableFile<BUFFER_SIZE, MAX_NUM_BUFFERS,SerialFlashFile>(filename),
        _myFS(fs) 
    {
        IndexableFile<BUFFER_SIZE, MAX_NUM_BUFFERS,SerialFlashFile>::_file = _myFS.open(filename);
    }
    
    SerialFlashFile open(const char *filename) override {
        return _myFS.open(filename);
    }

    virtual ~IndexableSerialFlashFile() {
        IndexableFile<BUFFER_SIZE, MAX_NUM_BUFFERS,SerialFlashFile>::close();
    }

    int16_t &operator[](int i) {
        return IndexableFile<BUFFER_SIZE, MAX_NUM_BUFFERS,SerialFlashFile>::operator[](i);
    }

private:
    SerialFlashChip &_myFS;
};

}



#endif //TEENSY_RESAMPLING_INDEXABLESERIALFLASH_FILE_H
