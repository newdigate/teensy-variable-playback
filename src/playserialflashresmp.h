//
// Created by Nicholas Newdigate on 18/07/2020.
//

#ifndef TEENSY_RESAMPLING_SDREADER_PLAYSERIALFLASHRAWRESMP_H
#define TEENSY_RESAMPLING_SDREADER_PLAYSERIALFLASHRAWRESMP_H

#include "Arduino.h"
#include "AudioStream.h"
#include "SerialFlash.h"
#include "stdint.h"
#include "ResamplingSerialFlashReader.h"
#include "playresmp.h"

class AudioPlaySerialFlashResmp : public AudioPlayResmp<newdigate::ResamplingSerialFlashReader>
{
public:
    AudioPlaySerialFlashResmp(SerialFlashChip &fs) :
            AudioPlayResmp<newdigate::ResamplingSerialFlashReader>()
    {
        reader = new newdigate::ResamplingSerialFlashReader(fs);
        begin();
    }

    virtual ~AudioPlaySerialFlashResmp() {
        delete reader;
    }
};


#endif //TEENSY_RESAMPLING_SDREADER_PLAYSERIALFLASHRAWRESMP_H
