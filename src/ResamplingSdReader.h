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

// Settings for SD card buffering
#undef RESAMPLE_BUFFER_SAMPLE_SIZE
#undef RESAMPLE_BUFFER_COUNT
#define RESAMPLE_BUFFER_SAMPLE_SIZE 512
#define RESAMPLE_BUFFER_COUNT 		  7

#define B2M (uint32_t)((double)4294967296000.0 / AUDIO_SAMPLE_RATE_EXACT / 2.0) // 97352592

namespace newdigate {

class ResamplingSdReader : public ResamplingReader< IndexableSDFile<RESAMPLE_BUFFER_SAMPLE_SIZE, RESAMPLE_BUFFER_COUNT>, File > {
public:
    ResamplingSdReader(SDClass &sd = SD) : 
        ResamplingReader(),
        _sd(sd)
    {
    }
    
    virtual ~ResamplingSdReader() {
    }

    int16_t getSourceBufferValue(long index) override {
        return (_sourceBuffer == nullptr)
					?0
					:(*_sourceBuffer)[index];
    }

    int available(void)
    {
        return _playing;
    }

    File open(char *filename) override {
        return _sd.open(filename);
    }

    void close(void) override
    {
        if (_playing)
            stop();
        if (_sourceBuffer != nullptr) {
            _sourceBuffer->close();
            delete _sourceBuffer;
            _sourceBuffer = nullptr;
        }
        if (_filename != nullptr) {
            delete [] _filename;
            _filename = nullptr;
        }
        deleteInterpolationPoints();
    }

    IndexableSDFile<RESAMPLE_BUFFER_SAMPLE_SIZE, RESAMPLE_BUFFER_COUNT>* createSourceBuffer() override {
        return new IndexableSDFile<RESAMPLE_BUFFER_SAMPLE_SIZE, RESAMPLE_BUFFER_COUNT>(_filename, _sd);
    }

    uint32_t positionMillis(void) {
        if (_file_size == 0) return 0;
        if (!_useDualPlaybackHead) {
            return (uint32_t) (( (double)_bufferPosition1 * lengthMillis() ) / (double)(_file_size/2));
        } else 
        {
            if (_crossfade < 0.5)
                return (uint32_t) (( (double)_bufferPosition1 * lengthMillis() ) / (double)(_file_size/2));
            else
                return (uint32_t) (( (double)_bufferPosition2 * lengthMillis() ) / (double)(_file_size/2));
        }
    }

    uint32_t lengthMillis(void) {
        return ((uint64_t)_file_size * B2M) >> 32;
    }
    
protected:    
     SDClass &_sd;
};

}

#endif //TEENSYAUDIOLIBRARY_RESAMPLINGSDREADER_H
