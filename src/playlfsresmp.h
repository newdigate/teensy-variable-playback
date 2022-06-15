//
// Created by Nicholas Newdigate on 18/07/2020.
//

#ifndef TEENSY_RESAMPLING_SDREADER_PLAYLFSRAWRESMP_H
#define TEENSY_RESAMPLING_SDREADER_PLAYLFSRAWRESMP_H

#include "ResamplingLfsReader.h"

class AudioPlayLfsResmp : public AudioPlayResmp<newdigate::ResamplingLfsReader>
{
public:
    AudioPlayLfsResmp(LittleFS &fs) :
            AudioPlayResmp<newdigate::ResamplingLfsReader>()
    {
        reader = new newdigate::ResamplingLfsReader(fs);
        begin();
    }

    virtual ~AudioPlayLfsResmp() {
        delete reader;
    }
};


#endif //TEENSY_RESAMPLING_SDREADER_PLAYLFSRAWRESMP_H
