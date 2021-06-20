//
// Created by Nicholas Newdigate on 18/07/2020.
//

#ifndef TEENSY_RESAMPLING_SDREADER_RESAMPLINGREADERFIXTURE_H
#define TEENSY_RESAMPLING_SDREADER_RESAMPLINGREADERFIXTURE_H

#include "ResamplingSdReader.h"

struct ResamplingReaderFixture {

    ResamplingReaderFixture()  {
        resamplingSdReader = new ResamplingSdReader();
    }

    ~ResamplingReaderFixture() {
        delete resamplingSdReader;
    }

    ResamplingSdReader * resamplingSdReader;
};

#endif //TEENSY_RESAMPLING_SDREADER_RESAMPLINGREADERFIXTURE_H
