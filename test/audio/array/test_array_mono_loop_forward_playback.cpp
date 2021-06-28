//
// Created by Nicholas Newdigate on 18/07/2020.
//

#ifndef TEENSY_AUDIO_ARRAY_READERTESTS_CPP
#define TEENSY_AUDIO_ARRAY_READERTESTS_CPP

#include <boost/test/unit_test.hpp>
#include "AudioArrayFixture.h"

BOOST_AUTO_TEST_SUITE(test_audio_array_mono_loop_forward_playback)

    unsigned char kick_raw[] = {
  0x99, 0x02, 0xd7, 0x02, 0xfa, 0x02, 0x5f, 0x03, 0xc1, 0x03, 0x2a, 0x04,
  0xad, 0x04, 0xa5, 0x05, 0x76, 0x06, 0x2f, 0x07, 0x9e, 0x07, 0xe2, 0x07,
  0x43, 0x08, 0x92, 0x08, 0xb2, 0x08, 0xe8, 0x08, 0x16, 0x09, 0xda, 0x08,
  0x51, 0x08, 0x01, 0x08, 0x25, 0x08, 0x70, 0x08, 0xc3, 0x08, 0x23, 0x09,
  0x95, 0x09, 0x19, 0x0a, 0x83, 0x0a, 0x7e, 0x0a, 0xd0, 0x0a, 0x65, 0x0b,
  0xf6, 0x0b, 0x89, 0x0c, 0xd1, 0x0c, 0xcf, 0x0c, 0x1a, 0x0d, 0xe5, 0x0d,
  0x5e, 0x0e, 0xbb, 0x0e, 0xec, 0x0e, 0xd9, 0x0e, 0x07, 0x0f, 0xc8, 0x0f,
  0x2a, 0x10, 0x04, 0x10, 0x28, 0x10, 0x54, 0x11, 0x8e, 0x13, 0x4b, 0x16
};
    unsigned int kick_raw_len = 96; // in bytes, divide by 2 to get samples

    BOOST_FIXTURE_TEST_CASE(ReadForwardLoopAtRegularPlaybackRate, AudioArrayFixture) {


        // GUItool: begin automatically generated code
        AudioPlayArrayResmp      memory;        //xy=306,225
        //AudioRecordQueue         queue1;         //xy=609,267
        TestAudioOutput          testout;       //xy=612,224
        AudioConnection          patchCord1(memory, 0, testout, 0);
        AudioConnection          patchCord2(memory, 0, testout, 1);
        // AudioConnection          patchCord3(memory, 0, queue1, 0);
        // GUItool: end automatically generated code

        testout.saveOutputFile("arr_fwd_0_5_quadratic.wav");
        memory.enableInterpolation(true);
        memory.setPlaybackRate(0.5f);
        memory.play((int16_t*)kick_raw, kick_raw_len / 2);
        for (int i=0; i < ((kick_raw_len)/128) + 20; i++) {
            testout.isr();
        }
        testout.closeOutputfile();
        arduino_should_exit = true;
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

#endif //TEENSY_AUDIO_ARRAY_READERTESTS_CPP
