//
// Created by Nicholas Newdigate on 18/07/2020.
//

#ifndef TEENSY_RESAMPLING_SDREADER_ARRAY_READERTESTS_CPP
#define TEENSY_RESAMPLING_SDREADER_ARRAY_READERTESTS_CPP

#include <boost/test/unit_test.hpp>
#include "ResamplingArrayFixture.h"

BOOST_AUTO_TEST_SUITE(test_array_mono_loop_forward_playback)

    int16_t kick[11] = {0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20};

    BOOST_FIXTURE_TEST_CASE(ReadForwardLoopAtRegularPlaybackRate, ResamplingArrayFixture) {

        const uint32_t expectedDataSize = 22; // 32 16bit samples = 64 bytes of space
        printf("ReadForwardAtRegularPlaybackRate(%d)\n", expectedDataSize);
        int16_t expected[expectedDataSize];
        for (int16_t i = 0; i < expectedDataSize; i++) {
            expected[i] = ceil(i / 2.0f);
        }
        
        resamplingArrayReader->begin();
        resamplingArrayReader->setPlaybackRate(0.5f);
        resamplingArrayReader->play(kick, 11);
        resamplingArrayReader->enableInterpolation(true);
        int16_t actual[256];

        int j = 0, bytesRead = 0, total_bytes_read = 0, currentExpected = 0;
        bool assertionsPass = true;
        do {
            bytesRead = resamplingArrayReader->read(actual, 512 ); // 256 samples
            total_bytes_read += bytesRead;
            printf("j:%d bytesRead: %d \n", j, bytesRead);

            for (int i=0; i < bytesRead/2; i++) {
                printf("\t\t[%x]:%x", currentExpected, actual[i]);

                if (currentExpected != actual[i]) {
                    assertionsPass = false;
                    //BOOST_FAIL("Value not as expected!!!");
                }

                currentExpected++;
                currentExpected %= expectedDataSize;
            }

            printf("\n");
            j++;
        } while (j < 3);
        printf("total_bytes_read: %d \n", total_bytes_read);
        resamplingArrayReader->close();

    }
/*
    BOOST_FIXTURE_TEST_CASE(ReadForwardLoopAtHalfPlaybackRate, ResamplingArrayFixture) {

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
        resamplingSdReader->play("test2.bin");
        resamplingSdReader->setLoopType(looptype_none);
        int16_t actual[expectedSize];

        int j = 0, bytesRead = 0, total_bytes_read = 0;
        do {
            bytesRead = resamplingSdReader->read(&actual[j * 256], 512 );
            total_bytes_read += bytesRead;
            printf("j:%d bytesRead: %d: ", j, bytesRead);
            for (int i=0; i < bytesRead/2; i++) {
                printf("\t\t[%x]:%x", expected[j * 256 + i], actual[j * 256 + i]);
            }
            printf("\n");
            j++;
        } while (bytesRead > 0);
        printf("total_bytes_read: %d \n", total_bytes_read);
        resamplingSdReader->close();

        BOOST_CHECK_EQUAL_COLLECTIONS(&expected[0], &expected[expectedSize - 1], &actual[0], &actual[(total_bytes_read / 2) - 1]);
    }
*/
BOOST_AUTO_TEST_SUITE_END()

#endif //TEENSY_RESAMPLING_SDREADER_READERTESTS_CPP
