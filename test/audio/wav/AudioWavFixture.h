//
// Created by Nicholas Newdigate on 17/06/2021.
//
#ifndef TEENSY_RESAMPLING_SDREADER_RESAMPLINGWAVFIXTURE_H
#define TEENSY_RESAMPLING_SDREADER_RESAMPLINGWAVFIXTURE_H

#include <Arduino.h>
#include <Audio.h>
#include <SD.h>
#include <iterator>
#include "output_test.h"
#include "playsdresmp.h"

struct AudioWavFixture {

    AudioWavFixture()  {
        AudioMemory(20);
    }

    ~AudioWavFixture() {
        arduino_should_exit = true;
    }
};

#endif //TEENSY_RESAMPLING_SDREADER_RESAMPLINGWAVFIXTURE_H
