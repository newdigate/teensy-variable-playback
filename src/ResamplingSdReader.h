//
// Created by Nicholas Newdigate on 10/02/2019.
//

#ifndef TEENSYAUDIOLIBRARY_RESAMPLINGSDREADER_H
#define TEENSYAUDIOLIBRARY_RESAMPLINGSDREADER_H

#include "SD.h"
#include <cstdint>
#include "spi_interrupt.h"
#include "loop_type.h"
#include "interpolation.h"
#include "IndexableSDFile.h"
#include "ResamplingReader.h"

#define RESAMPLE_BUFFER_SAMPLE_SIZE 128

#define B2M (uint32_t)((double)4294967296000.0 / AUDIO_SAMPLE_RATE_EXACT / 2.0) // 97352592

class ResamplingSdReader : public ResamplingReader< newdigate::IndexableSDFile<128, 2> > {
public:
    ResamplingSdReader() : 
        ResamplingReader() 
    {
    }

    int16_t getSourceBufferValue(long index) override {
        return (*_sourceBuffer)[index];
    }

    int available(void)
    {
        return _playing;
    }

    File open(char *filename) override {
        return SD.open(filename);
    }

    void close(void) override
    {
        if (_playing)
            stop();
        if (_sourceBuffer != nullptr) {
            _sourceBuffer->close();
            delete _sourceBuffer;
            _sourceBuffer = nullptr;
            StopUsingSPI();
        }
        if (_filename != nullptr) {
            delete [] _filename;
            _filename = nullptr;
        }
        deleteInterpolationPoints();
    }

    virtual newdigate::IndexableSDFile<128, 2>* createSourceBuffer() override {
        return new newdigate::IndexableSDFile<128, 2>(_filename);
    }

    uint32_t positionMillis(void) {
        if (_file_size == 0) return 0;

        return (uint32_t) (( (double)_bufferPosition * lengthMillis() ) / (double)(_file_size/2));
    }

    uint32_t lengthMillis(void) {
        return ((uint64_t)_file_size * B2M) >> 32;
    }
    
protected:    
     
    void StartUsingSPI() override {
    }

    void StopUsingSPI() override {
    }
};


#endif //TEENSYAUDIOLIBRARY_RESAMPLINGSDREADER_H
