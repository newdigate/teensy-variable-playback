#ifndef TEENSY_RESAMPLING_INDEXABLESD_FILE_H
#define TEENSY_RESAMPLING_INDEXABLESD_FILE_H

#include <Arduino.h>
#include "IndexableFile.h"
#include <SD.h>
#include <vector>

namespace newdigate {

template<size_t BUFFER_SIZE, size_t MAX_NUM_BUFFERS> // BUFFER_SIZE needs to be a power of two
class IndexableSDFile : public IndexableFile<BUFFER_SIZE, MAX_NUM_BUFFERS,File> {
public:
    static_assert(isPowerOf2(BUFFER_SIZE), "BUFFER_SIZE must be a power of 2");

    IndexableSDFile(const char *filename, SDClass &sd) : 
        IndexableFile<BUFFER_SIZE, MAX_NUM_BUFFERS,File>(filename),
        _sd(sd)
    {
        IndexableFile<BUFFER_SIZE, MAX_NUM_BUFFERS,File>::_file = open(filename);
    }
    
    File open(const char *filename) override {
        return _sd.open(filename);
    }

    virtual ~IndexableSDFile() {
        IndexableFile<BUFFER_SIZE, MAX_NUM_BUFFERS,File>::close();
    }

    int16_t &operator[](int i) {
        return IndexableFile<BUFFER_SIZE, MAX_NUM_BUFFERS,File>::operator[](i);
    }
protected:
    SDClass &_sd;
};

}



#endif 
