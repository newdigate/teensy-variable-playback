// Plays a RAW (16-bit signed) PCM audio file at slower or faster rate
// this example requires an uSD-card inserted to teensy 3.6 with a file called DEMO.RAW
#include <Arduino.h>
#include <AudioStream.h>
#include <Audio.h>
#include "SD.h"
#include "playsdwavresmp.h"

// GUItool: begin automatically generated code
AudioControlSGTL5000     audioShield;
AudioPlaySdWavResmp      playSdWav1;     //xy=324,457
AudioOutputI2S           i2s2;           //xy=840.8571472167969,445.5714416503906
AudioConnection          patchCord1(playSdWav1, 0, i2s2, 0);
AudioConnection          patchCord2(playSdWav1, 0, i2s2, 1);
// GUItool: end automatically generated code

#define A14 10
#define BUILTIN_SDCARD 10

const char* _filename = "DEMO.WAV";
const int analogInPin = A14;

int sensorValue = 0;

void setup() {
    analogReference(0);
    pinMode(analogInPin, INPUT);

    Serial.begin(57600);

    if (!(SD.begin(BUILTIN_SDCARD))) {
        // stop here if no SD card, but print a message
        while (1) {
            Serial.println("Unable to access the SD card");
            delay(500);
        }
    }

    AudioMemory(24);

    audioShield.enable();
    audioShield.volume(0.5);

    playSdWav1.play(_filename);
    playSdWav1.setPlaybackRate(1.0);
    Serial.println("playing...");
}

void loop() {

    int newsensorValue = analogRead(analogInPin);
    if (newsensorValue / 64 != sensorValue / 64) {
        sensorValue = newsensorValue;
        float rate = (sensorValue - 512.0) / 512.0;
        playSdWav1.setPlaybackRate(rate);
        Serial.printf("rate: %f %x\n", rate, sensorValue);
    }

    if (!playSdWav1.isPlaying()) {
        Serial.println("playing...");
        playSdWav1.play(_filename);
    }
}