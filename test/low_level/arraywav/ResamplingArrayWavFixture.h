//
// Created by Nicholas Newdigate on 17/06/2021.
//

#ifndef TEENSY_RESAMPLING_SDREADER_RESAMPLINGARRAYWAVFIXTURE_H
#define TEENSY_RESAMPLING_SDREADER_RESAMPLINGARRAYWAVFIXTURE_H

#include "ResamplingArrayReader.h"

struct ResamplingArrayWavFixture {

    ResamplingArrayWavFixture()  {
        resamplingArrayReader = new ResamplingArrayReader();
    }

    ~ResamplingArrayWavFixture() {
        delete resamplingArrayReader;
    }

    ResamplingArrayReader * resamplingArrayReader;
};

#endif //TEENSY_RESAMPLING_SDREADER_RESAMPLINGARRAYWAVFIXTURE_H
