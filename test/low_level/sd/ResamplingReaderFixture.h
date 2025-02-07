//
// Created by Nicholas Newdigate on 18/07/2020.
//

#ifndef TEENSY_RESAMPLING_SDREADER_RESAMPLINGREADERFIXTURE_H
#define TEENSY_RESAMPLING_SDREADER_RESAMPLINGREADERFIXTURE_H

#include "Audio.h"
#include "ResamplingSdReader.h"

class ResamplingReaderFixture {

public:
    class TestResamplingSdReader : public newdigate::ResamplingSdReader
    {
        public:
            void forceTriggerReload(float pbd = 1.0) {_sourceBuffer->triggerReload(pbd);}
    };
    ResamplingReaderFixture()  {
        resamplingSdReader = new TestResamplingSdReader;
    }

    ~ResamplingReaderFixture() {
        delete resamplingSdReader;
    }

    TestResamplingSdReader* resamplingSdReader;
};

#endif //TEENSY_RESAMPLING_SDREADER_RESAMPLINGREADERFIXTURE_H
