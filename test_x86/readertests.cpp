//
// Created by Nicholas Newdigate on 18/07/2020.
//

#ifndef TEENSY_RESAMPLING_SDREADER_READERTESTS_CPP
#define TEENSY_RESAMPLING_SDREADER_READERTESTS_CPP

#define BOOST_TEST_MODULE My Test
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>
#include "ResamplingReaderFixture.h"

BOOST_AUTO_TEST_SUITE(AbsoluteDateCheckSuite)

    BOOST_FIXTURE_TEST_CASE(ReadForwardAtRegularPlaybackRate, ResamplingReaderFixture) {

        const uint32_t testFileSize = 800;
        printf("test_resampling_forward(%d)\n", testFileSize);

        int16_t testbuffer[testFileSize];
        for (int16_t i = 0; i < testFileSize; i++) {
            testbuffer[i] = i;
        }

        SD.setSDCardFileData( (char*) testbuffer, testFileSize * 2);

        resamplingSdReader->begin();
        resamplingSdReader->setPlaybackRate(1.0);
        resamplingSdReader->play("test2.bin");
        resamplingSdReader->setLoopType(looptype_none);
        int16_t buffer[256];

        int j = 0, bytesRead = 0, total_bytes_read = 0;
        do {
            bytesRead = resamplingSdReader->read( buffer, 512 );
            total_bytes_read += bytesRead;
            printf("j:%d bytesRead: %d \n", j, bytesRead);

            for (int i=0; i < bytesRead/2; i++) {
                printf("\t\t[%x]:%x", i, buffer[i]);
            }
            //char c = cin.get();
            printf("\n");
            j++;
        } while (bytesRead > 0);
        printf("total_bytes_read: %d \n", total_bytes_read);


        resamplingSdReader->close();
        int i = 1;
        BOOST_CHECK_EQUAL(i,1);
    }

BOOST_AUTO_TEST_SUITE_END()

#endif //TEENSY_RESAMPLING_SDREADER_READERTESTS_CPP
