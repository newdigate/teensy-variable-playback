#ifndef TEENSYAUDIOLIBRARY_RESAMPLINGARRAYREADER_H
#define TEENSYAUDIOLIBRARY_RESAMPLINGARRAYREADER_H

#include <Arduino.h>
#include "SD.h"
#include <cstdint>
#include "loop_type.h"
#include "interpolation.h"
#include "ResamplingReader.h"

class ResamplingArrayReader : public ResamplingReader<int16_t> {
public:
    ResamplingArrayReader() :
        ResamplingReader() {
    }
    virtual int16_t getSourceBufferValue(long index) override {
        return _sourceBuffer[index];
    }

    int available(void) {
        return _playing;
    }

    virtual int16_t* createSourceBuffer() override {
        return _sourceBuffer;
    }

    virtual void close(void) override {
        if (_playing) {
            stop();
            deleteInterpolationPoints();
        }
    }

    virtual File open(char *filename) override {
        return File();
    }
protected:
    void StartUsingSPI() override {
    }

    void StopUsingSPI() override {
    }
};


#endif //TEENSYAUDIOLIBRARY_RESAMPLINGARRAYREADER_H
