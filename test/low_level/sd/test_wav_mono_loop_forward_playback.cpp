//
// Created by Nicholas Newdigate on 18/07/2020.
//

#ifndef TEENSY_RESAMPLING_WAVSDREADER_READER_MONO_LOOP_FORWARD_TESTS_CPP
#define TEENSY_RESAMPLING_WAVSDREADER_READER_MONO_LOOP_FORWARD_TESTS_CPP
#include <boost/test/unit_test.hpp>
#include "ResamplingReaderFixture.h"

BOOST_AUTO_TEST_SUITE(test_wav_mono_loop_forward_playback)
    uint16_t test_sndhdrdata_sndhdr_wav[] = {
            0x4952, 0x4646, 0x0038, 0x0000, 0x4157, 0x4556,
            0x6d66, 0x2074, 0x0010, 0x0000, 0x0001, 0x0001,
            0xac44, 0x0000, 0xb110, 0x0002, 0x0004, 0x0010,
            0x6164, 0x6174, 0x0014, 0x0000
    };
    unsigned int test_sndhdrdata_sndhdr_wav_len = 22; // 22 int16_t = 44 bytes = size of wave header

    BOOST_FIXTURE_TEST_CASE(ReadForwardLoopAtRegularPlaybackRate, ResamplingReaderFixture) {

        const uint32_t expectedDataSize = 32;  // = 64 bytes
        test_sndhdrdata_sndhdr_wav[20] = expectedDataSize * 2;
        printf("test_wav_mono_loop_forward_playback::ReadForwardAtRegularPlaybackRate(%d)\n", expectedDataSize);
        int16_t mockFileBytes[expectedDataSize + test_sndhdrdata_sndhdr_wav_len];
        for (int16_t i = 0; i < test_sndhdrdata_sndhdr_wav_len; i++) {
            mockFileBytes[i] = test_sndhdrdata_sndhdr_wav[i];
        }
        for (int16_t i = 0; i < expectedDataSize; i++) {
            mockFileBytes[i + test_sndhdrdata_sndhdr_wav_len] = i;
        }
        SD.setSDCardFileData((char*) mockFileBytes, (expectedDataSize + test_sndhdrdata_sndhdr_wav_len) * 2);

        resamplingSdReader->begin();
        resamplingSdReader->setPlaybackRate(1.0);
        resamplingSdReader->playWav("test2.bin");
        resamplingSdReader->setLoopType(looptype_repeat);
        int16_t actual[256];
        int16_t *buffers[1] = { actual };
        int j = 0, bytesRead = 0, total_bytes_read = 0, currentExpected = 0;
        bool assertionsPass = true;
        do {
            bytesRead = resamplingSdReader->read((void**)buffers, 256 ); // 256 samples
            total_bytes_read += bytesRead;
            //printf("j:%d bytesRead: %d \n", j, bytesRead);

            for (int i=0; i < bytesRead/2; i++) {
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

        BOOST_CHECK_EQUAL( true, assertionsPass);

    }

    BOOST_FIXTURE_TEST_CASE(ReadForwardLoopAtRegularPlaybackRateWithLoopFinish, ResamplingReaderFixture) {

        const uint32_t expectedDataSize = 32;  // = 64 bytes
        test_sndhdrdata_sndhdr_wav[20] = expectedDataSize * 2;

        printf("test_wav_mono_loop_forward_playback::ReadForwardAtRegularPlaybackRate(%d)\n", expectedDataSize);
        int16_t mockFileBytes[expectedDataSize + test_sndhdrdata_sndhdr_wav_len];
        for (int16_t i = 0; i < test_sndhdrdata_sndhdr_wav_len; i++) {
            mockFileBytes[i] = test_sndhdrdata_sndhdr_wav[i];
        }
        for (int16_t i = 0; i < expectedDataSize; i++) {
            mockFileBytes[i + test_sndhdrdata_sndhdr_wav_len] = i;
        }
        SD.setSDCardFileData((char*) mockFileBytes, (expectedDataSize + test_sndhdrdata_sndhdr_wav_len) * 2);

        resamplingSdReader->begin();
        resamplingSdReader->setPlaybackRate(1.0);
        resamplingSdReader->playWav("test2.bin");
        resamplingSdReader->setLoopType(looptype_repeat);
        resamplingSdReader->setLoopFinish(8);

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
                currentExpected %= 8;
            }

            printf("\n");
            j++;
        } while (j < 100);
        printf("total_bytes_read: %d \n", total_bytes_read);
        resamplingSdReader->close();

        BOOST_CHECK_EQUAL( true, assertionsPass);

    }

    BOOST_FIXTURE_TEST_CASE(ReadForwardLoopAtRegularPlaybackRateWithLoopStartAndFinish, ResamplingReaderFixture) {

        const uint32_t expectedDataSize = 32;  // = 64 bytes
        test_sndhdrdata_sndhdr_wav[20] = expectedDataSize * 2;
        printf("test_wav_mono_loop_forward_playback::ReadForwardAtRegularPlaybackRate(%d)\n", expectedDataSize);
        int16_t mockFileBytes[expectedDataSize + test_sndhdrdata_sndhdr_wav_len];
        for (int16_t i = 0; i < test_sndhdrdata_sndhdr_wav_len; i++) {
            mockFileBytes[i] = test_sndhdrdata_sndhdr_wav[i];
        }
        for (int16_t i = 0; i < expectedDataSize; i++) {
            mockFileBytes[i + test_sndhdrdata_sndhdr_wav_len] = i;
        }
        SD.setSDCardFileData((char*) mockFileBytes, (expectedDataSize + test_sndhdrdata_sndhdr_wav_len) * 2);

        resamplingSdReader->begin();
        resamplingSdReader->setPlaybackRate(1.0);
        resamplingSdReader->playWav("test2.bin");
        resamplingSdReader->setLoopType(looptype_repeat);
        resamplingSdReader->setLoopFinish(8);

        int16_t actual[256];
        int16_t *buffers[1] = { actual };
        int j = 0, samplesRead = 0, total_bytes_read = 0, currentExpected = 0;
        bool assertionsPass = true;
        do {
            samplesRead = resamplingSdReader->read((void**)buffers, 256 ); // 256 samples
            total_bytes_read += samplesRead * 2;
            printf("j:%d bytesRead: %d \n", j, total_bytes_read);

            for (int i=0; i < samplesRead; i++) {
                printf("\t\t[%x]:%x", currentExpected, actual[i]);

                if (currentExpected != actual[i]) {
                    assertionsPass = false;
                    BOOST_FAIL("Value not as expected!!!");
                }

                currentExpected++;
                currentExpected %= 8;
            }

            printf("\n");
            j++;
        } while (j < 100);
        printf("total_bytes_read: %d \n", total_bytes_read);
        resamplingSdReader->close();

        BOOST_CHECK_EQUAL( true, assertionsPass);

    }

    BOOST_FIXTURE_TEST_CASE(ReadForwardLoopAtHalfPlaybackRate, ResamplingReaderFixture) {

        const uint32_t size_of_datasource = 800;
        test_sndhdrdata_sndhdr_wav[20] = size_of_datasource * 2;
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
        resamplingSdReader->playWav("test2.bin");
        resamplingSdReader->setLoopType(looptype_none);
        int16_t actual[expectedSize];
        int16_t *buffers[1] = { actual };
        int j = 0, samplesRead = 0, total_bytes_read = 0;
        do {
            samplesRead = resamplingSdReader->read((void**)buffers, 256 );
            total_bytes_read += samplesRead * 2;
            printf("j:%d samplesRead: %d: ", j, samplesRead);
            for (int i=0; i < samplesRead; i++) {
                printf("\t\t[%x]:%x", expected[j * 256 + i], actual[j + i]);
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

#endif //TEENSY_RESAMPLING_WAVSDREADER_READER_MONO_LOOP_FORWARD_TESTS_CPP
