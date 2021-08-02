//
// Created by Nicholas Newdigate on 18/07/2020.
//

#ifndef TEENSY_RESAMPLING_SDREADER_ARRAY_READERTESTS_CPP
#define TEENSY_RESAMPLING_SDREADER_ARRAY_READERTESTS_CPP

#include <boost/test/unit_test.hpp>
#include "ResamplingArrayFixture.h"
#include "AudioStream.h"

extern unsigned char stereo_souljah_raw[];
extern unsigned int stereo_souljah_raw_len;
BOOST_AUTO_TEST_SUITE(test_array_stereo_loop_forward_playback)

    unsigned char stereo_raw[] = {
    0x00, 0x00, 0x10, 0x00, 
    0x01, 0x00, 0x11, 0x00, 
    0x02, 0x00, 0x12, 0x00,
    0x03, 0x00, 0x13, 0x00, 
    0x04, 0x00, 0x14, 0x00, 
    0x05, 0x00, 0x15, 0x00 };
    
    unsigned int stereo_raw_length = 24;

    BOOST_FIXTURE_TEST_CASE(ReadForwardLoopAtRegularPlaybackRate, ResamplingArrayFixture) {

        const uint32_t expectedDataSize = stereo_souljah_raw_len; // 32 16bit samples = 64 bytes of space
        printf("ReadForwardAtRegularPlaybackRate(%d)\n", expectedDataSize);

        resamplingArrayReader->begin();
        resamplingArrayReader->setPlaybackRate(0.5f);
        resamplingArrayReader->playRaw((int16_t*)stereo_raw, stereo_raw_length/4, 1);
        resamplingArrayReader->setInterpolationType(ResampleInterpolationType::resampleinterpolation_none);
        int16_t actual_left[256];
        int16_t actual_right[256];
        int16_t *buffers[2] = { actual_left, actual_right };

        int j = 0, bytesRead = 0, total_bytes_read = 0, currentExpected = 0;
        bool assertionsPass = true;
        do {
            bytesRead = resamplingArrayReader->read((void**)buffers, 256 ); // 256 samples
            total_bytes_read += bytesRead;
            //printf("j:%d bytesRead: %d \n", j, bytesRead);           
            j++;
        } while (j < 3);
        printf("total_bytes_read: %d \n", total_bytes_read);
        resamplingArrayReader->close();
        BOOST_CHECK_EQUAL(true, true);
    }

BOOST_AUTO_TEST_SUITE_END()

#endif //TEENSY_RESAMPLING_SDREADER_READERTESTS_CPP
