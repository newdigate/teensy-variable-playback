//
// Created by Nicholas Newdigate on 10/02/2019.
//

#ifndef TEENSYAUDIOLIBRARY_RESAMPLINGSDREADER_H
#define TEENSYAUDIOLIBRARY_RESAMPLINGSDREADER_H



#ifdef XXXXXX
#include "stdint.h"
#include <SD.h>
#include <AudioStream.h>

#include "../spi_interrupt.h"
#else
#include <cstdint>
#include "SD/SD.h"
#include "spi_interrupt.h"
#define AUDIO_BLOCK_SAMPLES 256
#endif

typedef enum loop_type {
    looptype_none,
    looptype_repeat,
    looptype_pingpong
} loop_type;

class ResamplingSdReader {
public:
    ResamplingSdReader() {
    }

    void begin(void);
    bool play(const char *filename);
    void stop(void);
    bool isPlaying(void) { return _playing; }

    int read(void *buf, uint16_t nbyte);
    bool readNextValue(int16_t *value);

    void setPlaybackRate(double f) {
        _playbackRate = f;
        if (f < 0 && _file_offset == 0) {
            _file.seek(_file_size);
            _file_offset = _file.position();
        }
    }

    float playbackRate() {
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

    void close(void);

private:
    volatile bool _playing = false;
    volatile int32_t _file_offset;
    volatile int32_t _last_read_offset = 0;

    uint32_t _file_size;
    double _playbackRate = 1.0;
    double _remainder = 0.0;
    loop_type _loopType = looptype_none;
     
    int _bufferPosition = 0;

    int16_t _buffer[AUDIO_BLOCK_SAMPLES];
    unsigned int _bufferLength = 0;

    File _file;

    bool updateBuffers(void);

    void StartUsingSPI(){
#if defined(HAS_KINETIS_SDHC)
        if (!(SIM_SCGC3 & SIM_SCGC3_SDHC)) AudioStartUsingSPI();
#else
        AudioStartUsingSPI();
#endif
    }

    void StopUsingSPI() {
#if defined(HAS_KINETIS_SDHC)
        if (!(SIM_SCGC3 & SIM_SCGC3_SDHC)) AudioStopUsingSPI();
#else
        AudioStopUsingSPI();
#endif
    }
};


#endif //PAULSTOFFREGEN_RESAMPLINGSDREADER_H
