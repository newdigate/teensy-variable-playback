//
// Created by Nicholas Newdigate on 18/07/2020.
//

#ifndef TEENSY_RESAMPLING_SDREADER_READER_MONO_NOLOOP_TESTS_CPP
#define TEENSY_RESAMPLING_SDREADER_READER_MONO_NOLOOP_TESTS_CPP
#include <boost/test/unit_test.hpp>
#include "ResamplingReaderFixture.h"
#include <cmath>

BOOST_AUTO_TEST_SUITE(test_raw_mono_noloop_forward_double_rate_playback)
    const double playBackRate = 2.0;

    void populateDataSourceAndSetSDCardMockData(const uint32_t size_of_datasource, int16_t dataSource[]) {
        for (int16_t i = 0; i < size_of_datasource; i++) {
            dataSource[i] = i;
        }
        SD.setSDCardFileData((char *) dataSource, size_of_datasource * 2);
    }

    void testReadForwardAtDoublePlaybackRate(const uint32_t size_of_datasource, ResamplingSdReader *resamplingSdReader) {
        printf("test_raw_mono_noloop_forward_double_rate_playback::testReadForwardAtDoublePlaybackRate(rate:%.2f\tsamples:%d)\n", playBackRate, size_of_datasource);

        int16_t dataSource[size_of_datasource];
        populateDataSourceAndSetSDCardMockData(size_of_datasource, dataSource);

        const int16_t expectedSize = ceil(size_of_datasource / 2.0);
        int16_t expected[expectedSize];
        for (int16_t i = 0; i < expectedSize; i++) {
            expected[i] = i * 2;
        }

        resamplingSdReader->begin();
        resamplingSdReader->setPlaybackRate(playBackRate);
        resamplingSdReader->playRaw("test2.bin", 1);
        resamplingSdReader->setLoopType(looptype_none);
        int16_t actual[256];
        int16_t *buffers[1] = { actual };

        int j = 0, samplesRead = 0, total_bytes_read = 0;
        do {
            samplesRead = resamplingSdReader->read((void**)buffers, 256);
            total_bytes_read += samplesRead * 2;
            printf("j:%d samplesRead: %d: ", j, samplesRead);
            for (int i = 0; i < samplesRead; i++) {
                printf("\t\t[%x]:%x", expected[j * 256 + i], actual[i]);
            }
            printf("\n");
            if (samplesRead != 0)
                BOOST_CHECK_EQUAL_COLLECTIONS(&expected[j * 256], &expected[j * 256 + samplesRead - 1], &actual[0], &actual[samplesRead - 1]);
            j++;
        } while (samplesRead > 0);
        printf("actual: bytes read: %d; samples read: %d \t\texpected: bytes read: %d; samples read:%d\n",
               total_bytes_read, total_bytes_read / 2, expectedSize * 2, expectedSize);
        resamplingSdReader->close();

        BOOST_CHECK_EQUAL(expectedSize * 2, total_bytes_read );
    }

    BOOST_FIXTURE_TEST_CASE(RawFileHas0Samples, ResamplingReaderFixture)
    {
        testReadForwardAtDoublePlaybackRate(0, resamplingSdReader);
    }

    BOOST_FIXTURE_TEST_CASE(RawFileHas1Sample, ResamplingReaderFixture) {
        testReadForwardAtDoublePlaybackRate(1, resamplingSdReader);
    }

    BOOST_FIXTURE_TEST_CASE(RawFileHas2Samples, ResamplingReaderFixture) {
        testReadForwardAtDoublePlaybackRate(2, resamplingSdReader);
    }

    BOOST_FIXTURE_TEST_CASE(RawFileHas4Samples, ResamplingReaderFixture) {
        testReadForwardAtDoublePlaybackRate(2, resamplingSdReader);
    }

    BOOST_FIXTURE_TEST_CASE(RawFileHas255Samples, ResamplingReaderFixture) {
        testReadForwardAtDoublePlaybackRate(255, resamplingSdReader);
    }

    BOOST_FIXTURE_TEST_CASE(RawFileHas256Samples, ResamplingReaderFixture) {
        testReadForwardAtDoublePlaybackRate(256, resamplingSdReader);
    }

    BOOST_FIXTURE_TEST_CASE(RawFileHas257Samples, ResamplingReaderFixture) {
        testReadForwardAtDoublePlaybackRate(257, resamplingSdReader);
    }

    BOOST_FIXTURE_TEST_CASE(RawFileHas500Samples, ResamplingReaderFixture) {
        testReadForwardAtDoublePlaybackRate(500, resamplingSdReader);
    }

    BOOST_FIXTURE_TEST_CASE(RawFileHas512Samples, ResamplingReaderFixture) {
        testReadForwardAtDoublePlaybackRate(512, resamplingSdReader);
    }

    BOOST_FIXTURE_TEST_CASE(RawFileHas800Samples, ResamplingReaderFixture) {
        testReadForwardAtDoublePlaybackRate(800, resamplingSdReader);
    }

    BOOST_FIXTURE_TEST_CASE(RawFileHas1023Samples, ResamplingReaderFixture) {
        testReadForwardAtDoublePlaybackRate(1023, resamplingSdReader);
    }

    BOOST_FIXTURE_TEST_CASE(RawFileHas1024Samples, ResamplingReaderFixture) {
        testReadForwardAtDoublePlaybackRate(1024, resamplingSdReader);
    }

    BOOST_FIXTURE_TEST_CASE(RawFileHas1025Samples, ResamplingReaderFixture) {
        testReadForwardAtDoublePlaybackRate(1025, resamplingSdReader);
    }

BOOST_AUTO_TEST_SUITE_END()

#endif //TEENSY_RESAMPLING_SDREADER_READER_MONO_NOLOOP_TESTS_CPP