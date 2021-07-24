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
    bool playRaw(int16_t *data, uint32_t numSamples, uint16_t numChannels);
    bool playRaw(const unsigned int *data, uint32_t numSamples, uint16_t numChannels);
    
    bool playWav(int16_t *data, uint32_t fileSize);
    bool playWav(const unsigned int *data, uint32_t fileSize);

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
    
    void startLoop(uint32_t samples) {
        arrayReader.loop(samples);
    }

    void enableInterpolation(bool enable) {
        if (enable)
            arrayReader.setInterpolationType(ResampleInterpolationType::resampleinterpolation_quadratic);
        else 
            arrayReader.setInterpolationType(ResampleInterpolationType::resampleinterpolation_none);
    }



private:

    uint32_t file_size;

    volatile bool playing;
    ResamplingArrayReader arrayReader;
    uint16_t _numChannels = 1;

    void setNumChannels(uint16_t numChannels) {
        arrayReader.setNumChannels(numChannels);
        _numChannels = numChannels;
    }
};


#endif //TEENSY_RESAMPLING_SDREADER_PLAYARRAYRESMP_H
