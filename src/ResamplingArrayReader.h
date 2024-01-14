#ifndef TEENSYAUDIOLIBRARY_RESAMPLINGARRAYREADER_H
#define TEENSYAUDIOLIBRARY_RESAMPLINGARRAYREADER_H

#include <Arduino.h>
#include "SD.h"
#include <cstdint>
#include "loop_type.h"
#include "interpolation.h"
#include "ResamplingReader.h"

namespace newdigate {

class ResamplingArrayReader : public ResamplingReader<int16_t, File> {
public:
    ResamplingArrayReader() :
        ResamplingReader() {
    }    
    
    virtual ~ResamplingArrayReader() {
    }

    int16_t getSourceBufferValue(long index) override {
        return _sourceBuffer[index];
    }

    int available(void) {
        return _playing;
    }

    int16_t* createSourceBuffer() override {
        return _sourceBuffer;
    }

    void close(void) override {
        if (_playing) {
            stop();
        }
    }

    File open(char *filename) override {
        return File();
    }
protected:
};

}

#endif //TEENSYAUDIOLIBRARY_RESAMPLINGARRAYREADER_H
