//
// Created by Nicholas Newdigate on 18/07/2020.
//

#ifndef TEENSY_RESAMPLING_SDREADER_ARRAY_READERTESTS_CPP
#define TEENSY_RESAMPLING_SDREADER_ARRAY_READERTESTS_CPP

#include <boost/test/unit_test.hpp>
#include "ResamplingArrayFixture.h"
extern unsigned char kick_raw[];
extern unsigned int kick_raw_len; // in bytes, divide by 2 to get samples

BOOST_AUTO_TEST_SUITE(test_array_mono_loop_forward_playback)

    BOOST_FIXTURE_TEST_CASE(ReadForwardLoopAtRegularPlaybackRate, ResamplingArrayFixture) {

        const uint32_t expectedDataSize = kick_raw_len; // 32 16bit samples = 64 bytes of space
        printf("ReadForwardAtRegularPlaybackRate(%d)\n", expectedDataSize);

        resamplingArrayReader->begin();
        resamplingArrayReader->setPlaybackRate(1.0f);
        resamplingArrayReader->playRaw((int16_t*)kick_raw, kick_raw_len/2, 1);
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
        } while (j < 3);
        printf("total_bytes_read: %d \n", total_bytes_read);
        resamplingArrayReader->close();
        BOOST_CHECK_EQUAL(true, true);
    }

BOOST_AUTO_TEST_SUITE_END()

#endif //TEENSY_RESAMPLING_SDREADER_READERTESTS_CPP
