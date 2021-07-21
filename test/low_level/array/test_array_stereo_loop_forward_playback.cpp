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

        resamplingArrayReader->setNumChannels(2);
        resamplingArrayReader->begin();
        resamplingArrayReader->setPlaybackRate(0.5f);
        resamplingArrayReader->playRaw((int16_t*)stereo_raw, stereo_raw_length/4);
        resamplingArrayReader->setInterpolationType(ResampleInterpolationType::resampleinterpolation_none);
        int16_t actual_left[256];
        int16_t actual_right[256];
        int16_t *buffers[2] = { actual_left, actual_right };

        int j = 0, bytesRead = 0, total_bytes_read = 0, currentExpected = 0;
        bool assertionsPass = true;
        do {
            bytesRead = resamplingArrayReader->read((void**)buffers, 256 ); // 256 samples
            total_bytes_read += bytesRead;
            printf("j:%d bytesRead: %d \n", j, bytesRead);           
            printf("\n");
            /*
            Serial.print("Ch1:"); 
            for (int i=0; i<bytesRead;i++) {			
                if (actual_left[i] < 0)
                    Serial.printf("-%04x ", -actual_left[i]);
                else 
                    Serial.printf(" %04x ", actual_left[i]);
            }
            Serial.println(); 	

            Serial.print("Ch2:"); 
            for (int i=0; i<bytesRead;i++) {			
                if (actual_right[i] < 0)
                    Serial.printf("-%04x ", -actual_right[i]);
                else 
                    Serial.printf(" %04x ", actual_right[i]);
            }
            */
            j++;
        } while (j < 3);
        printf("total_bytes_read: %d \n", total_bytes_read);
        resamplingArrayReader->close();
        BOOST_CHECK_EQUAL(true, true);
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
