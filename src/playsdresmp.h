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

class AudioPlaySdResmp : public AudioPlayResmp<newdigate::ResamplingSdReader>
{
public:
    AudioPlaySdResmp(SDClass &sd = SD) :
        AudioPlayResmp<newdigate::ResamplingSdReader>()
    {
        reader = new newdigate::ResamplingSdReader(sd);
        begin();
    }
    

    virtual ~AudioPlaySdResmp() {
        delete reader;
    }
};


#endif //TEENSY_RESAMPLING_SDREADER_PLAYSDRAWRESMP_H
