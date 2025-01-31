//
// Created by Nicholas Newdigate on 10/02/2019.
//

#ifndef TEENSYAUDIOLIBRARY_RESAMPLINGLFSREADER_H
#define TEENSYAUDIOLIBRARY_RESAMPLINGLFSREADER_H

#include <cstdint>
#include "IndexableLittleFSFile.h"
#include "ResamplingReader.h"
#include "LittleFS.h"

// Settings for LFS buffering
#undef RESAMPLE_BUFFER_SAMPLE_SIZE
#undef RESAMPLE_BUFFER_COUNT
#define RESAMPLE_BUFFER_SAMPLE_SIZE 2048
#define RESAMPLE_BUFFER_COUNT 		  7

namespace newdigate {

class ResamplingLfsReader : public ResamplingReader< IndexableLittleFSFile<RESAMPLE_BUFFER_SAMPLE_SIZE, RESAMPLE_BUFFER_COUNT>, File > {
public:
    ResamplingLfsReader(LittleFS &fs) : 
        ResamplingReader(),
        _myFS(fs) 
    {
    }
    virtual ~ResamplingLfsReader() {
    }

    int16_t getSourceBufferValue(long index) override {
        return (*_sourceBuffer)[index];
    }

    int available(void)
    {
        return _playing;
    }

    File open(char *filename) override {
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

    IndexableLittleFSFile<RESAMPLE_BUFFER_SAMPLE_SIZE, RESAMPLE_BUFFER_COUNT>* createSourceBuffer() override {		
		File f = open(_filename);
        return new IndexableLittleFSFile<RESAMPLE_BUFFER_SAMPLE_SIZE, RESAMPLE_BUFFER_COUNT>(_myFS, _filename, f);
    }

    IndexableLittleFSFile<RESAMPLE_BUFFER_SAMPLE_SIZE, RESAMPLE_BUFFER_COUNT>* createSourceBuffer(File& file) override {
        return new IndexableLittleFSFile<RESAMPLE_BUFFER_SAMPLE_SIZE, RESAMPLE_BUFFER_COUNT>(_myFS, _filename, file);
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
    LittleFS &_myFS;
};

}

#endif //TEENSYAUDIOLIBRARY_RESAMPLINGLFSREADER_H
