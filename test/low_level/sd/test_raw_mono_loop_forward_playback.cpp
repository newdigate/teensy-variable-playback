//
// Created by Nicholas Newdigate on 18/07/2020.
//

#ifndef TEENSY_RESAMPLING_SDREADER_READER_MONO_LOOP_TESTS_CPP
#define TEENSY_RESAMPLING_SDREADER_READER_MONO_LOOP_TESTS_CPP
#include <boost/test/unit_test.hpp>
#include "ResamplingReaderFixture.h"

BOOST_AUTO_TEST_SUITE(test_raw_mono_loop_forward_playback)

    BOOST_FIXTURE_TEST_CASE(ReadForwardLoopAtRegularPlaybackRate, ResamplingReaderFixture) {

        const uint32_t expectedDataSize = 32; // 32 16bit samples = 64 bytes of space
        printf("ReadForwardAtRegularPlaybackRate(%d)\n", expectedDataSize);
        int16_t expected[expectedDataSize];
        for (int16_t i = 0; i < expectedDataSize; i++) {
            expected[i] = i;
        }
        SD.setSDCardFileData((char*) expected, expectedDataSize * 2);

        resamplingSdReader->begin();
        resamplingSdReader->setPlaybackRate(1.0);
        resamplingSdReader->playRaw("test2.bin", 1);
        resamplingSdReader->setLoopType(looptype_repeat);
        int16_t actual[256];
        int16_t *buffers[1] = { actual };
        int j = 0, samplesRead = 0, total_bytes_read = 0, currentExpected = 0;
        bool assertionsPass = true;
        do {
            samplesRead = resamplingSdReader->read((void**)buffers, 256 ); // 256 samples
            total_bytes_read += samplesRead * 2;
            printf("j:%d samplesRead: %d \n", j, samplesRead);

            for (int i=0; i < samplesRead; i++) {
                printf("\t\t[%x]:%x", currentExpected, actual[i]);

                if (currentExpected != actual[i]) {
                    assertionsPass = false;
                    BOOST_FAIL("Value not as expected!!!");
                }

                currentExpected++;
                currentExpected %= expectedDataSize;
            }

            printf("\n");
            j++;
        } while (j < 100);
        printf("total_bytes_read: %d \n", total_bytes_read);
        resamplingSdReader->close();

    }

    BOOST_FIXTURE_TEST_CASE(ReadForwardLoopAtHalfPlaybackRate, ResamplingReaderFixture) {

        const uint32_t size_of_datasource = 800;
        printf("ReadForwardAtRegularPlaybackRate(%d)\n", size_of_datasource);
        int16_t dataSource[size_of_datasource];
        for (int16_t i = 0; i < size_of_datasource; i++) {
            dataSource[i] = i;
        }
        SD.setSDCardFileData((char*) dataSource, size_of_datasource * 2);

        const int16_t expectedSize = size_of_datasource * 2;
        int16_t expected[expectedSize];
        for (int16_t i = 0; i < expectedSize; i++) {
            expected[i] = i / 2;
        }

        resamplingSdReader->begin();
        resamplingSdReader->setPlaybackRate(0.5);
        resamplingSdReader->playRaw("test2.bin", 1);
        resamplingSdReader->setLoopType(looptype_none);
        int16_t actual[expectedSize];
        int16_t *buffers[1] = { actual };
        int j = 0, samplesRead = 0, total_bytes_read = 0;
        do {
            samplesRead = resamplingSdReader->read((void**)buffers, 256 );
            total_bytes_read += samplesRead * 2;
            printf("j:%d samplesRead: %d: ", j, samplesRead);
            for (int i=0; i < samplesRead; i++) {
                printf("\t\t[%x]:%x", expected[j * 256 + i], actual[i]);
            }
            printf("\n");

            if (samplesRead != 0)
                BOOST_CHECK_EQUAL_COLLECTIONS(&expected[j * 256], &expected[j * 256 + samplesRead - 1], &actual[0], &actual[samplesRead - 1]);
            j++;
        } while (samplesRead > 0);
        printf("total_bytes_read: %d \n", total_bytes_read);
        resamplingSdReader->close();        
    }

BOOST_AUTO_TEST_SUITE_END()

#endif //TEENSY_RESAMPLING_SDREADER_READER_MONO_LOOP_TESTS_CPP
