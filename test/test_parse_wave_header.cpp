//
// Created by Nicholas Newdigate on 18/07/2020.
//

#ifndef TEENSY_RESAMPLING_WAVEHEADER_PARSER_TESTS_CPP
#define TEENSY_RESAMPLING_WAVEHEADER_PARSER_TESTS_CPP

#include <boost/test/unit_test.hpp>
#include "WaveHeaderParserFixture.h"

BOOST_AUTO_TEST_SUITE(WaveHeaderParsingTests)

    BOOST_FIXTURE_TEST_CASE(ReadForwardAtRegularPlaybackRate, WaveHeaderParserFixture) {

        unsigned char test_sndhdrdata_sndhdr_wav[] = {
                0x52, 0x49, 0x46, 0x46, 0x38, 0x00, 0x00, 0x00, 0x57, 0x41, 0x56, 0x45,
                0x66, 0x6d, 0x74, 0x20, 0x10, 0x00, 0x00, 0x00, 0x01, 0x00, 0x02, 0x00,
                0x44, 0xac, 0x00, 0x00, 0x10, 0xb1, 0x02, 0x00, 0x04, 0x00, 0x10, 0x00,
                0x64, 0x61, 0x74, 0x61, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00
        };
        unsigned int test_sndhdrdata_sndhdr_wav_len = 64;

        SD.setSDCardFileData((char*) test_sndhdrdata_sndhdr_wav, test_sndhdrdata_sndhdr_wav_len);

        wav_header header;
        bool success = waveHeaderParser->readWaveHeader("blah.wav", header);
        BOOST_CHECK_EQUAL(success, true);
   }
BOOST_AUTO_TEST_SUITE_END()

#endif //TEENSY_RESAMPLING_WAVEHEADER_PARSER_TESTS_CPP
