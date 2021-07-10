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

#define RESAMPLE_BUFFER_SAMPLE_SIZE 128

class ResamplingSdReader {
public:
    ResamplingSdReader() {
    }

    void begin(void);
    bool play(const char *filename);
    bool play();
    void stop(void);
    bool isPlaying(void) { return _playing; }

    unsigned int read(void **buf, uint16_t nbyte);
    bool readNextValue(int16_t *value, uint16_t channel);

    void setPlaybackRate(double f) {
        _playbackRate = f;
        if (f < 0 && _file_offset == _header_size) {
            //_file.seek(_file_size);
            _file_offset = _file_size;
        }
    }

    double playbackRate() {
        return _playbackRate;
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

    void setHeaderSize(uint32_t headerSize) {
        _header_size = headerSize;
    }

    void setLoopStart(uint32_t loop_start) {
        _loop_start = loop_start;
    }

    void setLoopFinish(uint32_t loop_finish) {
        // sample number, (NOT byte number)
        _loop_finish = loop_finish;
    }

    void setInterpolationType(ResampleInterpolationType interpolationType) {        
        if (interpolationType != _interpolationType) {            
            _interpolationType = interpolationType;
            initializeInterpolationPoints();
        }
    }

    void setNumChannels(uint16_t numChannels) {
        if (_numChannels != numChannels) {            
            numChannels = numChannels;
            initializeInterpolationPoints();
        }
    }

private:
    volatile bool _playing = false;
    volatile int32_t _file_offset;
    volatile int32_t _last_read_offset = 0;

    int32_t _file_size;
    int32_t _header_size = 0;
    double _playbackRate = 1.0;
    double _remainder = 0.0;
    loop_type _loopType = looptype_none;
    char *_filename = (char *)"";
    int _bufferPosition = 0;
    int32_t _loop_start = 0;
    int32_t _loop_finish = 0;
    uint16_t _numChannels = 1;

    int16_t _buffer[RESAMPLE_BUFFER_SAMPLE_SIZE * 2]; // two buffers
    unsigned int _bufferLength = 0;
    unsigned int _numBuffers = 0;
    unsigned int _currentBuffer = 0;
    bool _noMoreBuffersToRead = false;
    unsigned int _nextBufferLength = 0;
    File _file;

    ResampleInterpolationType _interpolationType = ResampleInterpolationType::resampleinterpolation_none;
    unsigned int _numInterpolationPoints = 0;
    IntepolationData **_interpolationPoints = nullptr;
    bool updateBuffers(void);
    void StartUsingSPI(){
        //Serial.printf("start spi: %s\n", _filename);

#if defined(HAS_KINETIS_SDHC)
        if (!(SIM_SCGC3 & SIM_SCGC3_SDHC)) AudioStartUsingSPI();
#else
        AudioStartUsingSPI();
#endif
    }

    void StopUsingSPI() {
        //Serial.printf("stop spi: %s\n", _filename);
#if defined(HAS_KINETIS_SDHC)
        if (!(SIM_SCGC3 & SIM_SCGC3_SDHC)) AudioStopUsingSPI();
#else
        AudioStopUsingSPI();
#endif
    }

    void initializeInterpolationPoints(void);
    void deleteInterpolationPoints(void);
};


#endif //PAULSTOFFREGEN_RESAMPLINGSDREADER_H
