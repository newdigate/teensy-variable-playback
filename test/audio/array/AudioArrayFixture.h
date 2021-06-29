//
// Created by Nicholas Newdigate on 17/06/2021.
//
#ifndef TEENSY_RESAMPLING_SDREADER_RESAMPLINGARRAYFIXTURE_H
#define TEENSY_RESAMPLING_SDREADER_RESAMPLINGARRAYFIXTURE_H

#include <Arduino.h>
#include <Audio.h>
#include <iterator>
#include "output_test.h"
#include "playarrayresmp.h"

struct AudioArrayFixture {

    AudioArrayFixture()  {
        AudioMemory(20);
    }

    ~AudioArrayFixture() {
        arduino_should_exit = true;
    }
};

#endif //TEENSY_RESAMPLING_SDREADER_RESAMPLINGARRAYFIXTURE_H
