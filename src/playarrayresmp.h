//
// Created by Nicholas Newdigate on 18/07/2020.
//

#ifndef TEENSY_RESAMPLING_SDREADER_PLAYARRAYRESMP_H
#define TEENSY_RESAMPLING_SDREADER_PLAYARRAYRESMP_H


#include "Arduino.h"
#include "AudioStream.h"
#include "ResamplingArrayReader.h"
#include "playresmp.h"

class AudioPlayArrayResmp : public AudioPlayResmp<newdigate::ResamplingArrayReader>
{
public:
    AudioPlayArrayResmp(void) :
            AudioPlayResmp<newdigate::ResamplingArrayReader>()
    {
        reader = new newdigate::ResamplingArrayReader();
        begin();
    }

    virtual ~AudioPlayArrayResmp() {
        delete reader;
    }
};


#endif //TEENSY_RESAMPLING_SDREADER_PLAYARRAYRESMP_H
