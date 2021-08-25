//
// Created by Nicholas Newdigate on 18/07/2020.
//

#ifndef TEENSY_AUDIO_SDWAV_STEREO_READERTESTS_CPP
#define TEENSY_AUDIO_SDWAV_STEREO_READERTESTS_CPP

#include <boost/test/unit_test.hpp>
#include "AudioWavFixture.h"
    
BOOST_AUTO_TEST_SUITE(test_audio_wav_tags_in_header)

    const uint16_t numberOfChannels = 2;
    const std::string referencePath = "test/resources/reference/";
    const std::string inputPath = "test/resources/input/";
    const std::string outputPath = "output/";

    BOOST_FIXTURE_TEST_CASE(Wav_with_tags_in_header_1, AudioWavFixture) {

        // GUItool: begin automatically generated code
        AudioPlaySdResmp         wave;        //xy=306,225
        TestAudioOutput          testout;       //xy=612,224
        AudioConnection          patchCord1(wave, 0, testout, 0);
        AudioConnection          patchCord2(wave, 1, testout, 1);
        // GUItool: end automatically generated code

        const double playbackRate = 1.0;
        const std::string testName = "SDTEST1";
        const std::string outputFile = testName+".wav";
        const std::string outputFileName = outputPath + outputFile;
        const std::string referenceFileName = referencePath + testName + ".wav";
        SD.setSDCardFolderPath(inputPath);
        
        if (!testout.saveOutputFile(outputPath.c_str(), outputFile.c_str())) {
            std::cout << "not able to save output file..." << std::endl;
            BOOST_ERROR("not able to save output file (1)...");
        }
        wave.begin();
        wave.enableInterpolation(true);
        wave.setPlaybackRate(playbackRate);
        bool isPlaying = wave.playWav(outputFile.c_str());
        if (!isPlaying) {
            std::cout << "input audio file is not able to play..." << std::endl;
            BOOST_ERROR("input audio file is not able to play (1)...");
        }
        if (!wave.isPlaying()) {
            std::cout << "input audio file is not able to play..." << std::endl;
            BOOST_ERROR("input audio file is not able to play (2)...");
        }

        while (wave.isPlaying()) {
            testout.isr();
        }
        testout.closeOutputfile(numberOfChannels);
        patchCord1.disconnect();
        AudioConnection::reset();
        arduino_should_exit = true;
        
        std::cout << "comparing " << outputFileName << " with " << referenceFileName << ";" << std::endl;

        std::ifstream ifs1(outputFileName);
        std::ifstream ifs2(referenceFileName);
        std::istream_iterator<char> b1(ifs1), e1;
        std::istream_iterator<char> b2(ifs2), e2;

        BOOST_CHECK_EQUAL_COLLECTIONS(b1, e1, b2, e2);
    }
    BOOST_FIXTURE_TEST_CASE(Wav_with_tags_in_header_2, AudioWavFixture) {

        // GUItool: begin automatically generated code
        AudioPlaySdResmp         wave;        //xy=306,225
        TestAudioOutput          testout;       //xy=612,224
        AudioConnection          patchCord1(wave, 0, testout, 0);
        AudioConnection          patchCord2(wave, 1, testout, 1);
        // GUItool: end automatically generated code

        const double playbackRate = 1.0;
        const std::string testName = "SDTEST2";
        const std::string outputFile = testName+".wav";
        const std::string outputFileName = outputPath + outputFile;
        const std::string referenceFileName = referencePath + testName + ".wav";
        SD.setSDCardFolderPath(inputPath);
        
        if (!testout.saveOutputFile(outputPath.c_str(), outputFile.c_str())) {
            std::cout << "not able to save output file..." << std::endl;
            BOOST_CHECK(false);
            return;
        }
        wave.begin();
        wave.enableInterpolation(true);
        wave.setPlaybackRate(playbackRate);
        wave.playWav(outputFile.c_str());

        if (!wave.isPlaying()) {
            std::cout << "input audio file is not able to play..." << std::endl;
            BOOST_CHECK(false);
            return;
        }

        while (wave.isPlaying()) {
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

#endif //TEENSY_AUDIO_SDWAV_STEREO_READERTESTS_CPP
