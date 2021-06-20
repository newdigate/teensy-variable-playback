//
// Created by Nicholas Newdigate on 18/07/2020.
//

#ifndef TEENSY_RESAMPLING_SDREADER_RESAMPLINGREADERFIXTURE_H
#define TEENSY_RESAMPLING_SDREADER_RESAMPLINGREADERFIXTURE_H

#include "waveheaderparser.h"

struct WaveHeaderParserFixture {

    WaveHeaderParserFixture()  {
        waveHeaderParser = new WaveHeaderParser();
    }

    ~WaveHeaderParserFixture() {
        delete waveHeaderParser;
    }

    WaveHeaderParser * waveHeaderParser;
};

#endif //TEENSY_RESAMPLING_SDREADER_RESAMPLINGREADERFIXTURE_H
