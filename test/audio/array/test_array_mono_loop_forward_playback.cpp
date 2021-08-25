//
// Created by Nicholas Newdigate on 18/07/2020.
//

#ifndef TEENSY_AUDIO_ARRAY_READERTESTS_CPP
#define TEENSY_AUDIO_ARRAY_READERTESTS_CPP

#include <boost/test/unit_test.hpp>
#include "AudioArrayFixture.h"

extern unsigned char kick_raw[];
extern unsigned int kick_raw_len; // in bytes, divide by 2 to get samples

BOOST_AUTO_TEST_SUITE(test_audio_array_mono_loop_forward_playback)

    const uint16_t numberOfChannels = 1;

    const std::string referencePath = "test/resources/reference/";
    const std::string inputPath = "test/resources/input/";
    const std::string outputPath = "output/";

    BOOST_FIXTURE_TEST_CASE(Array_fwd_1_0000_quadratic_mono_noloop, AudioArrayFixture) {

        // GUItool: begin automatically generated code
        AudioPlayArrayResmp      memory;        //xy=306,225
        TestAudioOutput          testout;       //xy=612,224
        AudioConnection          patchCord1(memory, 0, testout, 0);
        // GUItool: end automatically generated code

        const double playbackRate = 1.0;
        const std::string testName = "Array_fwd_1_0000_quadratic_mono_noloop";
        const std::string outputFile = testName+".wav";
        const std::string outputFileName = outputPath + outputFile;
        const std::string referenceFileName = referencePath+testName+".wav";
 
        testout.saveOutputFile(outputPath.c_str(), outputFile.c_str());
        memory.begin();
        memory.enableInterpolation(true);
        memory.setPlaybackRate(playbackRate);
        memory.playRaw((int16_t*)kick_raw, kick_raw_len / 2, numberOfChannels);
        for (int i=0; i < ((kick_raw_len)/128) + 20; i++) {
            testout.isr();
        }
        testout.closeOutputfile(numberOfChannels);
        patchCord1.disconnect();
        AudioConnection::reset();
        arduino_should_exit = true;
        
        std::ifstream ifs1(outputFileName);
        std::ifstream ifs2(referenceFileName);
        std::istream_iterator<char> b1(ifs1), e1;
        std::istream_iterator<char> b2(ifs2), e2;

        BOOST_CHECK_EQUAL_COLLECTIONS(b1, e1, b2, e2);
    }

    BOOST_FIXTURE_TEST_CASE(Array_fwd_0_5000_quadratic_mono_noloop, AudioArrayFixture) {

        // GUItool: begin automatically generated code
        AudioPlayArrayResmp      memory;        //xy=306,225
        TestAudioOutput          testout;       //xy=612,224
        AudioConnection          patchCord1(memory, 0, testout, 0);
        // GUItool: end automatically generated code
        const double playbackRate = 0.5;
        const std::string testName = "Array_fwd_0_5000_quadratic_mono_noloop";
        const std::string outputFile = testName+".wav";
        const std::string outputFileName = outputPath + outputFile;
        const std::string referenceFileName = referencePath+testName+".wav";

        testout.saveOutputFile(outputPath.c_str(), outputFile.c_str());
        memory.begin();
        memory.enableInterpolation(true);
        memory.setPlaybackRate(playbackRate);
        memory.playRaw((int16_t*)kick_raw, kick_raw_len / 2, numberOfChannels);
        for (int i=0; i < ((kick_raw_len)/128) + 20; i++) {
            testout.isr();
        }
        testout.closeOutputfile(numberOfChannels);
        patchCord1.disconnect();
        AudioConnection::reset();
        arduino_should_exit = true;

        std::ifstream ifs1(outputFileName);
        std::ifstream ifs2(referenceFileName);
        std::istream_iterator<char> b1(ifs1), e1;
        std::istream_iterator<char> b2(ifs2), e2;

        BOOST_CHECK_EQUAL_COLLECTIONS(b1, e1, b2, e2);
    }

    BOOST_FIXTURE_TEST_CASE(Array_fwd_2_0000_quadratic_mono_noloop, AudioArrayFixture) {

        // GUItool: begin automatically generated code
        AudioPlayArrayResmp      memory;        //xy=306,225
        TestAudioOutput          testout;       //xy=612,224
        AudioConnection          patchCord1(memory, 0, testout, 0);
        // GUItool: end automatically generated code

        const double playbackRate = 2.0;
        const std::string testName = "Array_fwd_2_0000_quadratic_mono_noloop";
        const std::string outputFile = testName+".wav";
        const std::string outputFileName = outputPath + outputFile;
        const std::string referenceFileName = referencePath+testName+".wav";

        testout.saveOutputFile(outputPath.c_str(), outputFile.c_str());     
        memory.begin();           
        memory.enableInterpolation(true);
        memory.setPlaybackRate(playbackRate);
        memory.playRaw((int16_t*)kick_raw, kick_raw_len / 2, numberOfChannels);
        for (int i=0; i < ((kick_raw_len)/128) + 20; i++) {
            testout.isr();
        }
        testout.closeOutputfile(numberOfChannels);
        patchCord1.disconnect();
        AudioConnection::reset();
        arduino_should_exit = true;

        std::ifstream ifs1(outputFileName);
        std::ifstream ifs2(referenceFileName);
        std::istream_iterator<char> b1(ifs1), e1;
        std::istream_iterator<char> b2(ifs2), e2;

        BOOST_CHECK_EQUAL_COLLECTIONS(b1, e1, b2, e2);
    }

    BOOST_FIXTURE_TEST_CASE(Array_fwd_0_7437_quadratic_mono_noloop, AudioArrayFixture) {

        // GUItool: begin automatically generated code
        AudioPlayArrayResmp      memory;        //xy=306,225
        TestAudioOutput          testout;       //xy=612,224
        AudioConnection          patchCord1(memory, 0, testout, 0);
        // GUItool: end automatically generated code

        const double playbackRate = 0.7437;
        const std::string testName = "Array_fwd_0_7437_quadratic_mono_noloop";
        const std::string outputFile = testName+".wav";
        const std::string outputFileName = outputPath + outputFile;
        const std::string referenceFileName = referencePath+testName+".wav";

        testout.saveOutputFile(outputPath.c_str(), outputFile.c_str());
        memory.begin();
        memory.enableInterpolation(true);
        memory.setPlaybackRate(playbackRate);
        memory.playRaw((int16_t*)kick_raw, kick_raw_len / 2, numberOfChannels);
        for (int i=0; i < ((kick_raw_len)/128) + 20; i++) {
            testout.isr();
        }
        testout.closeOutputfile(numberOfChannels);
        patchCord1.disconnect();
        AudioConnection::reset();
        arduino_should_exit = true;

        std::ifstream ifs1(outputFileName);
        std::ifstream ifs2(referenceFileName);
        std::istream_iterator<char> b1(ifs1), e1;
        std::istream_iterator<char> b2(ifs2), e2;

        BOOST_CHECK_EQUAL_COLLECTIONS(b1, e1, b2, e2);

    }

    BOOST_FIXTURE_TEST_CASE(Array_fwd_1_7437_quadratic_mono_noloop, AudioArrayFixture) {

        // GUItool: begin automatically generated code
        AudioPlayArrayResmp      memory;        //xy=306,225
        TestAudioOutput          testout;       //xy=612,224
        AudioConnection          patchCord1(memory, 0, testout, 0);
        // GUItool: end automatically generated code

        const double playbackRate = 1.7437;
        const std::string testName = "Array_fwd_1_7437_quadratic_mono_noloop";
        const std::string outputFile = testName+".wav";
        const std::string outputFileName = outputPath + outputFile;
        const std::string referenceFileName = referencePath+testName+".wav";

        testout.saveOutputFile(outputPath.c_str(), outputFile.c_str());
        memory.begin();
        memory.enableInterpolation(true);
        memory.setPlaybackRate(playbackRate);
        memory.playRaw((int16_t*)kick_raw, kick_raw_len / 2, numberOfChannels);
        for (int i=0; i < ((kick_raw_len)/128) + 20; i++) {
            testout.isr();
        }
        testout.closeOutputfile(numberOfChannels);
        patchCord1.disconnect();
        AudioConnection::reset();
        arduino_should_exit = true;

        std::ifstream ifs1(outputFileName);
        std::ifstream ifs2(referenceFileName);
        std::istream_iterator<char> b1(ifs1), e1;
        std::istream_iterator<char> b2(ifs2), e2;

        BOOST_CHECK_EQUAL_COLLECTIONS(b1, e1, b2, e2);
    }

    BOOST_FIXTURE_TEST_CASE(Array_fwd_8_7437_quadratic_mono_noloop, AudioArrayFixture) {

        // GUItool: begin automatically generated code
        AudioPlayArrayResmp      memory;        //xy=306,225
        TestAudioOutput          testout;       //xy=612,224
        AudioConnection          patchCord1(memory, 0, testout, 0);
        // GUItool: end automatically generated code

        const double playbackRate = 8.7437;
        const std::string testName = "Array_fwd_8_7437_quadratic_mono_noloop";
        const std::string outputFile = testName+".wav";
        const std::string outputFileName = outputPath + outputFile;
        const std::string referenceFileName = referencePath+testName+".wav";

        testout.saveOutputFile(outputPath.c_str(), outputFile.c_str());
        memory.begin();        
        memory.enableInterpolation(true);
        memory.setPlaybackRate(playbackRate);
        memory.playRaw((int16_t*)kick_raw, kick_raw_len / 2, numberOfChannels);
        for (int i=0; i < ((kick_raw_len)/128) + 20; i++) {
            testout.isr();
        }
        testout.closeOutputfile(numberOfChannels);
        patchCord1.disconnect();
        AudioConnection::reset();
        arduino_should_exit = true;

        std::ifstream ifs1(outputFileName);
        std::ifstream ifs2(referenceFileName);
        std::istream_iterator<char> b1(ifs1), e1;
        std::istream_iterator<char> b2(ifs2), e2;

        BOOST_CHECK_EQUAL_COLLECTIONS(b1, e1, b2, e2);
    }

BOOST_AUTO_TEST_SUITE_END()

#endif //TEENSY_AUDIO_ARRAY_READERTESTS_CPP
