#ifndef TEENSYAUDIOLIBRARY_RESAMPLINGARRAYREADER_H
#define TEENSYAUDIOLIBRARY_RESAMPLINGARRAYREADER_H

#include <Arduino.h>
#include "SD.h"
#include <cstdint>
#include "loop_type.h"
#include "interpolation.h"

class ResamplingArrayReader {
public:
    ResamplingArrayReader() {

    }

    void begin(void);
    bool playRaw(int16_t *array, uint32_t length, uint16_t numChannels);
    bool playWav(int16_t *array, uint32_t length);
    bool play();
    void stop(void);
    bool isPlaying(void) { return _playing; }

    unsigned int read(void **buf, uint16_t nbyte);
    bool readNextValue(int16_t *value, uint16_t channelNumber);

    void setPlaybackRate(double f) {
        _playbackRate = f;
        if (f < 0.0 && _bufferPosition == 0) {
            //_file.seek(_file_size);
            _bufferPosition = _file_size/2 - _numChannels;
        }
    }

    float playbackRate() {
        return _playbackRate;
    }

    void loop(uint32_t numSamples) {
        __disable_irq();
        _loop_start = _bufferPosition;
        _loop_finish = _bufferPosition + numSamples * _numChannels;
        _loopType = loop_type::looptype_repeat;
        __enable_irq();
    }

    void setLoopType(loop_type loopType)
    {
        _loopType = loopType;
    }

    loop_type getLoopType(){
        return _loopType;    
    }

    int available(void);
    void reset(void);
    void close(void);

    void setLoopStart(uint32_t loop_start) {
        _loop_start = _header_offset + (loop_start * _numChannels);
    }

    void setLoopFinish(uint32_t loop_finish) {
        // sample number, (NOT byte number)
        _loop_finish = _header_offset + (loop_finish * _numChannels);
    }

    void setInterpolationType(ResampleInterpolationType interpolationType) {
        if (interpolationType != _interpolationType) {
            _interpolationType = interpolationType;
            initializeInterpolationPoints();
        }
    }

    int16_t getNumChannels() {
        return _numChannels;
    }

    void setNumChannels(uint16_t numChannels) {
        if (numChannels != _numChannels) {
            _numChannels = numChannels;
            initializeInterpolationPoints();
        }
    }

    void setHeaderSizeInBytes(uint32_t headerSizeInBytes) {
        _header_offset = headerSizeInBytes / 2;
        if (_bufferPosition < _header_offset) {
            if (_playbackRate >= 0) {
                _bufferPosition = _header_offset;
            } else
                _bufferPosition = _loop_finish - _numChannels;
        }
    }

private:
    volatile bool _playing = false;

    int32_t _file_size;
    int32_t _header_offset = 0; // == (header size in bytes ) / 2

    double _playbackRate = 1.0;
    double _remainder = 0.0;
    loop_type _loopType = looptype_none;
    int _bufferPosition = 0;
    int32_t _loop_start = 0;
    int32_t _loop_finish = 0;
    int16_t _numChannels = -1;
    uint16_t _numInterpolationPointsChannels = 0;
    int16_t *_sourceBuffer = nullptr;

    ResampleInterpolationType _interpolationType = ResampleInterpolationType::resampleinterpolation_none;
    unsigned int _numInterpolationPoints = 0;
    InterpolationData **_interpolationPoints = nullptr;
    void initializeInterpolationPoints(void);
    void deleteInterpolationPoints(void);
};


#endif //TEENSYAUDIOLIBRARY_RESAMPLINGARRAYREADER_H
