#ifndef TEENSY_RESAMPLING_SDREADER_PLAYRESMP_H
#define TEENSY_RESAMPLING_SDREADER_PLAYRESMP_H

#include "Arduino.h"
#include "Audio.h"
#include "loop_type.h"

class AudioPlayResmp : public AudioStream
{
    public:
        AudioPlayResmp(void): AudioStream(0, NULL) {}
        virtual ~AudioPlayResmp() {}

        virtual void setPlaybackRate(float f) = 0;
        virtual void setLoopType(loop_type t) = 0;
        virtual void setLoopStart(uint32_t loop_start) = 0;
        virtual void setLoopFinish(uint32_t loop_finish) = 0;
        virtual void begin() = 0;
        virtual void enableInterpolation(bool enable) = 0;
        virtual bool isPlaying(void) = 0;
        virtual void stop() = 0;
};

#endif // TEENSY_RESAMPLING_SDREADER_PLAYRESMP_H