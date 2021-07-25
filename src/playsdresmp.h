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

class AudioPlaySdResmp : public AudioStream
{
public:
    AudioPlaySdResmp(void) :
            AudioStream(0, NULL),
            sdReader()
    {
        begin();
    }

    void begin(void);
    bool playRaw(const char *filename, uint16_t numChannels);
    bool playWav(const char *filename);
    void stop(void);
    bool isPlaying(void) { return sdReader.isPlaying(); }
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
    ResamplingSdReader sdReader;
  
};


#endif //TEENSY_RESAMPLING_SDREADER_PLAYSDRAWRESMP_H
