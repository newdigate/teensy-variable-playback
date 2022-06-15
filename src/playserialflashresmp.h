//
// Created by Nicholas Newdigate on 18/07/2020.
//

#ifndef TEENSY_RESAMPLING_SDREADER_PLAYSERIALFLASHRAWRESMP_H
#define TEENSY_RESAMPLING_SDREADER_PLAYSERIALFLASHRAWRESMP_H

#include "Arduino.h"
#include "AudioStream.h"
#include "SerialFlash.h"
#include "stdint.h"
#include "ResamplingSerialFlashReader.h"
#include "playresmp.h"

class AudioPlaySerialFlashResmp : public AudioPlayResmp
{
public:
    AudioPlaySerialFlashResmp(SerialFlashChip &fs) :
            AudioPlayResmp(),
            serialFlashReader(fs)
    {
        begin();
    }

    void begin(void);
    bool playRaw(const char *filename, uint16_t numChannels);
    bool playWav(const char *filename);
    void stop(void);
    bool isPlaying(void) { return serialFlashReader.isPlaying(); }
    uint32_t positionMillis(void);
    uint32_t lengthMillis(void);
    virtual void update(void);

    void setPlaybackRate(float f) {
        serialFlashReader.setPlaybackRate(f);
    }

    void setLoopType(loop_type t) {
        serialFlashReader.setLoopType(t);
    }

    void setLoopStart(uint32_t loop_start) {
        serialFlashReader.setLoopStart(loop_start);
    }

    void setLoopFinish(uint32_t loop_finish) {
        serialFlashReader.setLoopFinish(loop_finish);
    }

    void enableInterpolation(bool enable) {
        if (enable)
            serialFlashReader.setInterpolationType(ResampleInterpolationType::resampleinterpolation_quadratic);
        else 
            serialFlashReader.setInterpolationType(ResampleInterpolationType::resampleinterpolation_none);
    }
private:

    uint32_t file_size;
    newdigate::ResamplingSerialFlashReader serialFlashReader;
  
};


#endif //TEENSY_RESAMPLING_SDREADER_PLAYSERIALFLASHRAWRESMP_H
