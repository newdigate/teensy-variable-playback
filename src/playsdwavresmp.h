//
// Created by Nicholas Newdigate on 18/07/2020.
//

#ifndef TEENSY_RESAMPLING_SDREADER_PLAYSDWAVRESMP_H
#define TEENSY_RESAMPLING_SDREADER_PLAYSDWAVRESMP_H


#include "Arduino.h"
#include "AudioStream.h"
#include "SD.h"
#include "stdint.h"
#include "ResamplingSdReader.h"
#include "waveheaderparser.h"

class AudioPlaySdWavResmp : public AudioStream
{
public:
    AudioPlaySdWavResmp(void) :
            AudioStream(0, NULL),
            sdReader()
    {
        begin();
    }

    void begin(void);
    bool play(char *filename);
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

    void setNumChannels(uint16_t numChannels) {
        sdReader.setNumChannels(numChannels);
        _numChannels = numChannels;
    }
private:
    wav_header wave_header = wav_header();
    char *_filename = (char*)"";
    uint32_t file_size;
    volatile uint32_t file_offset;
    ResamplingSdReader sdReader;
    volatile bool playing;
    uint16_t _numChannels = 1;
};


#endif //TEENSY_RESAMPLING_SDREADER_PLAYSDWAVRESMP_H
