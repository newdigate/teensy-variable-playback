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
#include "playresmp.h"

class AudioPlaySdResmp : public AudioPlayResmp
{
public:
    AudioPlaySdResmp(void) :
            AudioPlayResmp(),
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

    void setLoopStart(uint32_t loop_start) {
        sdReader.setLoopStart(loop_start);
    }

    void setLoopFinish(uint32_t loop_finish) {
        sdReader.setLoopFinish(loop_finish);
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
