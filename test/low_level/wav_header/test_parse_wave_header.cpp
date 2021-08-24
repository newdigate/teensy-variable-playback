//
// Created by Nicholas Newdigate on 18/07/2020.
//

#ifndef TEENSY_RESAMPLING_WAVEHEADER_PARSER_TESTS_CPP
#define TEENSY_RESAMPLING_WAVEHEADER_PARSER_TESTS_CPP

#include <boost/test/unit_test.hpp>
#include "WaveHeaderParserFixture.h"

BOOST_AUTO_TEST_SUITE(WaveHeaderParsingTests)

    BOOST_FIXTURE_TEST_CASE(ReadWaveHeader, WaveHeaderParserFixture) {

        unsigned char test_sndhdrdata_sndhdr_wav[] = {
                0x52, 0x49, 0x46, 0x46, 0x38, 0x00, 0x00, 0x00, 0x57, 0x41, 0x56, 0x45,
                0x66, 0x6d, 0x74, 0x20, 0x10, 0x00, 0x00, 0x00, 0x01, 0x00, 0x02, 0x00,
                0x44, 0xac, 0x00, 0x00, 0x10, 0xb1, 0x02, 0x00, 0x04, 0x00, 0x10, 0x00,
                0x64, 0x61, 0x74, 0x61, 0x14, 0x00, 0x00, 0x00
        };
        unsigned int test_sndhdrdata_sndhdr_wav_len = 44;

        SD.setSDCardFileData((char*) test_sndhdrdata_sndhdr_wav, test_sndhdrdata_sndhdr_wav_len);

        wav_header header;
        wav_data_header data_header;
        bool success = waveHeaderParser->readWaveHeader("blah.wav", header, data_header);
        BOOST_CHECK_EQUAL(success, true);
        const char expectedRIFF[5] = "RIFF";
        BOOST_CHECK_EQUAL_COLLECTIONS(&header.riff_header[0], &header.riff_header[3],&expectedRIFF[0], &expectedRIFF[3]);
        BOOST_CHECK_EQUAL(header.header_chunk_size,56);
        const char expectedWave[5] = "WAVE";
        BOOST_CHECK_EQUAL_COLLECTIONS(&header.wave_header[0], &header.wave_header[3],&expectedWave[0], &expectedWave[3]);
        const char expectedfmt[5] = "fmt ";
        BOOST_CHECK_EQUAL_COLLECTIONS(&header.fmt_header[0], &header.fmt_header[3],&expectedfmt[0], &expectedfmt[3]);
        const char expecteddata[5] = "data";
        BOOST_CHECK_EQUAL_COLLECTIONS(&data_header.data_header[0], &data_header.data_header[3],&expecteddata[0], &expecteddata[3]);
        //BOOST_CHECK_EQUAL(File::numOpenFiles,0);
        //BOOST_CHECK_EQUAL(File::numInstances,0);
    }
BOOST_AUTO_TEST_SUITE_END()

#endif //TEENSY_RESAMPLING_WAVEHEADER_PARSER_TESTS_CPP
