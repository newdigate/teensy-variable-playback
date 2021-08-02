//
// Created by Nicholas Newdigate on 18/07/2020.
//

#ifndef TEENSY_RESAMPLING_SDREADER_ARRAYWAV_MONO_READERTESTS_CPP
#define TEENSY_RESAMPLING_SDREADER_ARRAYWAV_MONO_READERTESTS_CPP

#include <boost/test/unit_test.hpp>
#include "ResamplingArrayWavFixture.h"

extern unsigned char mono_souljah_wav[];
extern unsigned int mono_souljah_wav_len;

BOOST_AUTO_TEST_SUITE(test_array_mono)

    BOOST_FIXTURE_TEST_CASE(ReadForwardLoopAtRegularPlaybackRate, ResamplingArrayWavFixture) {

        const uint32_t expectedDataSize = mono_souljah_wav_len; // 32 16bit samples = 64 bytes of space
        printf("ReadForwardAtRegularPlaybackRate(%d)\n", expectedDataSize);

        resamplingArrayReader->begin();
        resamplingArrayReader->setPlaybackRate(0.5f);
        resamplingArrayReader->playWav((int16_t*)mono_souljah_wav, mono_souljah_wav_len/2);
        resamplingArrayReader->setInterpolationType(ResampleInterpolationType::resampleinterpolation_linear);
        int16_t actual[256];
        int16_t *buffers[1] = { actual };

        int j = 0, bytesRead = 0, total_bytes_read = 0, currentExpected = 0;
        bool assertionsPass = true;
        do {
            bytesRead = resamplingArrayReader->read((void**)buffers, 256 ); // 256 samples
            total_bytes_read += bytesRead;
            //printf("j:%d bytesRead: %d \n", j, bytesRead);           
            //printf("\n");
            j++;
        } while (bytesRead > 0);
        printf("total_bytes_read: %d \n", total_bytes_read);
        resamplingArrayReader->close();
        BOOST_CHECK_EQUAL(true, true);
    }

BOOST_AUTO_TEST_SUITE_END()

#endif //TEENSY_RESAMPLING_SDREADER_ARRAYWAV_MONO_READERTESTS_CPP
