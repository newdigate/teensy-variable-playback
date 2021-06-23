//
// Created by Nicholas Newdigate on 18/07/2020.
//

#ifndef TEENSY_RESAMPLING_SDREADER_PLAYARRAYRESMP_H
#define TEENSY_RESAMPLING_SDREADER_PLAYARRAYRESMP_H


#include "Arduino.h"
#include "AudioStream.h"
#include "ResamplingArrayReader.h"

class AudioPlayArrayResmp : public AudioStream
{
public:
    AudioPlayArrayResmp(void) :
            AudioStream(0, NULL),
            arrayReader()
    {
        begin();
    }

    void begin(void);
    bool play(int16_t *data, uint32_t numSamples);
    void stop(void);
    bool isPlaying(void) { return playing; }
    uint32_t positionMillis(void);
    uint32_t lengthMillis(void);
    virtual void update(void);

    void setPlaybackRate(float f) {
        arrayReader.setPlaybackRate(f);
    }

    void setLoopType(loop_type t) {
        arrayReader.setLoopType(t);
    }

    void enableInterpolation(bool enable) {
        if (enable)
            arrayReader.setInterpolationType(ResampleInterpolationType::resampleinterpolation_quadratic);
        else 
            arrayReader.setInterpolationType(ResampleInterpolationType::resampleinterpolation_none);
    }

private:

    uint32_t file_size;
    volatile uint32_t file_offset;
    volatile bool playing;
    ResamplingArrayReader arrayReader;
};


#endif //TEENSY_RESAMPLING_SDREADER_PLAYARRAYRESMP_H
