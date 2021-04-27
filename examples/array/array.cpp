// Plays a RAW (16-bit signed) PCM audio file at slower or faster rate
// this example requires an uSD-card inserted to teensy 3.6 with a file called DEMO.RAW
#include <Arduino.h>
#include <AudioStream.h>
#include <Audio.h>
#include "../../src/ResamplingArrayReader.h"

ResamplingArrayReader  arrayResampler;     //xy=324,457
int16_t samples[] = {1, 2, 3, 4, 5, 6};

void setup() {
    Serial.println("hello");
    arrayResampler.play(samples, sizeof (samples));
    arrayResampler.setPlaybackRate(0.5);
    Serial.println("setup done");
}

void loop() {
    int16_t buffer[10];
    arrayResampler.read(buffer, 5);
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