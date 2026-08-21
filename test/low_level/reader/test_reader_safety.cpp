#include <boost/test/unit_test.hpp>
#include <Arduino.h>
#include <Audio.h>
#include "ResamplingSdReader.h"
#include "ResamplingArrayReader.h"
#if !defined(BUILD_FOR_LINUX)
#include "ResamplingLfsReader.h"
#include "ResamplingSerialFlashReader.h"
#endif

using namespace newdigate;

BOOST_AUTO_TEST_SUITE(test_reader_safety_suite)

    BOOST_AUTO_TEST_CASE(test_sd_reader_null_buffer_safety) {
        ResamplingSdReader reader;
        // Reader has not opened any file yet (_sourceBuffer == nullptr)
        BOOST_CHECK_EQUAL(reader.getSourceBufferValue(0), 0);
        BOOST_CHECK_EQUAL(reader.getSourceBufferValue(100), 0);
        reader.close();
    }

    BOOST_AUTO_TEST_CASE(test_array_reader_null_buffer_safety) {
        ResamplingArrayReader reader;
        // Reader has no array set yet (_sourceBuffer == nullptr)
        BOOST_CHECK_EQUAL(reader.getSourceBufferValue(0), 0);
        BOOST_CHECK_EQUAL(reader.getSourceBufferValue(100), 0);
        reader.close();
    }

    BOOST_AUTO_TEST_CASE(test_position_millis_virtual_dispatch) {
        int16_t dummy_audio[1000];
        for (int i = 0; i < 1000; i++) dummy_audio[i] = 0;

        ResamplingArrayReader reader;
        reader.playRaw(dummy_audio, 1000, 1);
        
        ResamplingReader<int16_t, File> *base_ptr = &reader;
        BOOST_CHECK_EQUAL(base_ptr->positionMillis(), 0);
        BOOST_CHECK_GT(base_ptr->lengthMillis(), 0);

        reader.stop();
        reader.close();
    }

    BOOST_AUTO_TEST_CASE(test_play_raw_overloads) {
        int16_t dummy_audio[500];
        for (int i = 0; i < 500; i++) dummy_audio[i] = 0;

        ResamplingArrayReader reader;
        bool ok = reader.playRaw(dummy_audio, 500);
        BOOST_CHECK(ok);
        BOOST_CHECK(reader.isPlaying());
        BOOST_CHECK_EQUAL(reader.getNumChannels(), 1);

        reader.stop();
        reader.close();
    }

BOOST_AUTO_TEST_SUITE_END()
