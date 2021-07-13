//
// Created by Nicholas Newdigate on 18/07/2020.
//

#ifndef TEENSY_RESAMPLING_SDREADER_PLAYSDRAWRESMP_H
#define TEENSY_RESAMPLING_SDREADER_PLAYSDRAWRESMP_H


#include "Arduino.h"
#include "AudioStream.h"
#include "SD.h"
#include "stdint.h"
#include "ResamplingSdReader.h"

class AudioPlaySdRawResmp : public AudioStream
{
public:
    AudioPlaySdRawResmp(void) :
            AudioStream(0, NULL),
            sdReader()
    {
        begin();
    }

    void begin(void);
    bool play(const char *filename);
    void stop(void);
    bool isPlaying(void) { return playing; }
    uint32_t positionMillis(void);
    uint32_t lengthMillis(void);
    virtual void update(void);

    void setPlaybackRate(float f) {
        sdReader.setPlaybackRate(f);
    }

    void setLoopType(loop_type t) {
        sdReader.setLoopType(t);
    }

    void enableInterpolation(bool enable) {
        if (enable)
            sdReader.setInterpolationType(ResampleInterpolationType::resampleinterpolation_quadratic);
        else 
            sdReader.setInterpolationType(ResampleInterpolationType::resampleinterpolation_none);
    }
private:

    uint32_t file_size;
    volatile uint32_t file_offset;
    volatile bool playing;
    ResamplingSdReader sdReader;
    uint16_t _numChannels = 1;    
};


#endif //TEENSY_RESAMPLING_SDREADER_PLAYSDRAWRESMP_H
