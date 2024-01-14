//
// Created by Nicholas Newdigate on 10/02/2019.
//

#ifndef TEENSYAUDIOLIBRARY_RESAMPLINGSERIALFLASHREADER_H
#define TEENSYAUDIOLIBRARY_RESAMPLINGSERIALFLASHREADER_H

#include <cstdint>
#include "spi_interrupt.h"
#include "loop_type.h"
#include "interpolation.h"
#include "IndexableSerialFlashFile.h"
#include "ResamplingReader.h"
#include "SerialFlash.h"

#define RESAMPLE_BUFFER_SAMPLE_SIZE 128

#define B2M (uint32_t)((double)4294967296000.0 / AUDIO_SAMPLE_RATE_EXACT / 2.0) // 97352592

namespace newdigate {

class ResamplingSerialFlashReader : public ResamplingReader< IndexableSerialFlashFile<128, 2>, SerialFlashFile > {
public:
    ResamplingSerialFlashReader(SerialFlashChip &fs) : 
        ResamplingReader(),
        _myFS(fs) 
    {
    }

    virtual ~ResamplingSerialFlashReader() {
    }

    int16_t getSourceBufferValue(long index) override {
        return (*_sourceBuffer)[index];
    }

    int available(void)
    {
        return _playing;
    }

    SerialFlashFile open(char *filename) override {
        return _myFS.open(filename);
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
    }

    IndexableSerialFlashFile<128, 2>* createSourceBuffer() override {
        return new IndexableSerialFlashFile<128, 2>(_myFS, _filename);
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
    SerialFlashChip &_myFS;
};

}

#endif //TEENSYAUDIOLIBRARY_RESAMPLINGSERIALFLASHREADER_H
