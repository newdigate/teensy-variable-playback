//
// Created by Nicholas Newdigate on 18/07/2020.
//

#ifndef TEENSY_RESAMPLING_SDREADER_READER_MONO_FORWARD_TESTS_CPP
#define TEENSY_RESAMPLING_SDREADER_READER_MONO_FORWARD_TESTS_CPP
#define BOOST_AUTO_TEST_MAIN
#define BOOST_TEST_MODULE ResamplingReaderTests
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "ResamplingReaderFixture.h"

BOOST_AUTO_TEST_SUITE(test_raw_mono_noloop_forward_playback)

    BOOST_FIXTURE_TEST_CASE(ReadForwardAtRegularPlaybackRate, ResamplingReaderFixture) {

        const uint32_t expectedDataSize = 1600;
        printf("ReadForwardAtRegularPlaybackRate(%d)\n", expectedDataSize);
        int16_t expected[expectedDataSize];
        for (int16_t i = 0; i < expectedDataSize; i++) {
            expected[i] = i;
        }
        SD.setSDCardFileData((char*) expected, expectedDataSize * 2);

        resamplingSdReader->begin();
        resamplingSdReader->setPlaybackRate(1.0);
        resamplingSdReader->playRaw("test2.bin", 1);
        resamplingSdReader->setLoopType(looptype_none);
        resamplingSdReader->setInterpolationType(ResampleInterpolationType::resampleinterpolation_quadratic);
        int16_t actual[1024];
        int16_t *buffers[1] = { actual };
        int j = 0, samplesRead = 0, total_bytes_read = 0;
        do {
            samplesRead = resamplingSdReader->read((void**)buffers, 256 );
            total_bytes_read += samplesRead * 2;
            printf("j:%d samplesRead: %d \n", j, samplesRead);

            for (int i=0; i < samplesRead; i++) {
                printf("\t\t[%x]:%x", expected[j * 256 + i], actual[j + i]);
            }
            printf("\n");
            j++;
        } while (samplesRead > 0);
        printf("total_bytes_read: %d \n", total_bytes_read);
        resamplingSdReader->close();

        //BOOST_CHECK_EQUAL_COLLECTIONS(&expected[0], &expected[expectedDataSize-1], &actual[0], &actual[(total_bytes_read / 2)-1]);
    }

    BOOST_FIXTURE_TEST_CASE(ReadForwardAtHalfPlaybackRate, ResamplingReaderFixture) {

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
        //resamplingSdReader->setInterpolationType(ResampleInterpolationType::resampleinterpolation_quadratic);

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

#endif //TEENSY_RESAMPLING_SDREADER_READER_MONO_FORWARD_TESTS_CPP
