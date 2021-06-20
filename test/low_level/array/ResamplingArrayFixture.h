//
// Created by Nicholas Newdigate on 17/06/2021.
//

#ifndef TEENSY_RESAMPLING_SDREADER_RESAMPLINGARRAYFIXTURE_H
#define TEENSY_RESAMPLING_SDREADER_RESAMPLINGARRAYFIXTURE_H

#include "ResamplingArrayReader.h"

struct ResamplingArrayFixture {

    ResamplingArrayFixture()  {
        resamplingArrayReader = new ResamplingArrayReader();
    }

    ~ResamplingArrayFixture() {
        delete resamplingArrayReader;
    }

    ResamplingArrayReader * resamplingArrayReader;
};

#endif //TEENSY_RESAMPLING_SDREADER_RESAMPLINGARRAYFIXTURE_H
