// Plays a RAW (16-bit signed) PCM audio file at slower or faster rate
// this example requires an uSD-card inserted to teensy 3.6 with a file called DEMO.RAW
#include <Arduino.h>
#include <AudioStream.h>
#include <Audio.h>
#include "../../src/playarrayresmp.h"

AudioPlayArrayResmp  playArrayResampler;     //xy=324,457
AudioOutputI2S  i2s;     //xy=324,457

int16_t samples[] = {1, 2, 3, 4, 5, 6};

AudioConnection connection(playArrayResampler, 0, i2s, 0);

void setup() {
    AudioMemory(100);
    playArrayResampler.play(samples, sizeof (samples) /sizeof(int16_t));
    playArrayResampler.setPlaybackRate(0.5);
    playArrayResampler.setLoopType(looptype_repeat);
}

void loop() {
    #ifdef BUILD_FOR_LINUX
    software_isr();
    #endif
}

#ifdef BUILD_FOR_LINUX
int main() {
    initialize_mock_arduino();
    setup();
    while(true){
        loop();
    }
}
#endif