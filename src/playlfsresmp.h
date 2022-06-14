//
// Created by Nicholas Newdigate on 18/07/2020.
//

#ifndef TEENSY_RESAMPLING_SDREADER_PLAYLFSRAWRESMP_H
#define TEENSY_RESAMPLING_SDREADER_PLAYLFSRAWRESMP_H


#include "Arduino.h"
#include "AudioStream.h"
#include "SD.h"
#include "stdint.h"
#include "ResamplingLfsReader.h"
#include "playresmp.h"

class AudioPlayLfsResmp : public AudioPlayResmp
{
public:
    AudioPlayLfsResmp(LittleFS &fs) :
            AudioPlayResmp(),
            lfsReader(fs)
    {
        begin();
    }

    void begin(void);
    bool playRaw(const char *filename, uint16_t numChannels);
    bool playWav(const char *filename);
    void stop(void);
    bool isPlaying(void) { return lfsReader.isPlaying(); }
    uint32_t positionMillis(void);
    uint32_t lengthMillis(void);
    virtual void update(void);

    void setPlaybackRate(float f) {
        lfsReader.setPlaybackRate(f);
    }

    void setLoopType(loop_type t) {
        lfsReader.setLoopType(t);
    }

    void setLoopStart(uint32_t loop_start) {
        lfsReader.setLoopStart(loop_start);
    }

    void setLoopFinish(uint32_t loop_finish) {
        lfsReader.setLoopFinish(loop_finish);
    }

    void enableInterpolation(bool enable) {
        if (enable)
            lfsReader.setInterpolationType(ResampleInterpolationType::resampleinterpolation_quadratic);
        else 
            lfsReader.setInterpolationType(ResampleInterpolationType::resampleinterpolation_none);
    }
private:

    uint32_t file_size;
    newdigate::ResamplingLfsReader lfsReader;
  
};


#endif //TEENSY_RESAMPLING_SDREADER_PLAYLFSRAWRESMP_H
