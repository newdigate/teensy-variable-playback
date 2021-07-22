#include <Arduino.h>
#include <Audio.h>
#include <TeensyVariablePlayback.h>
#include <MIDI.h>
#include "flashloader.h"

// GUItool: begin automatically generated code
AudioPlayArrayResmp      rraw_a1;        //xy=321,513
AudioOutputI2S           i2s1;           //xy=675,518
AudioConnection          patchCord1(rraw_a1, 0, i2s1, 0);
AudioConnection          patchCord2(rraw_a1, 0, i2s1, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=521,588
// GUItool: end automatically generated code
#define A14 10

MIDI_CREATE_DEFAULT_INSTANCE();
const int analogInPin = A14;
newdigate::audiosample *sample;

double getPlaybackRate(int16_t analog) { //analog: 0..1023
    return  (analog - 512.0) / 512.0;
}

float calcFrequency(uint8_t note) {
    float result = 440.0 * powf(2.0, (note-69) / 12.0);
    return result;
}

void handleNoteOn(uint8_t channel, uint8_t pitch, uint8_t velocity)
{
    rraw_a1.setLoopType(loop_type::looptype_repeat);
    float freq = calcFrequency(pitch);
    uint32_t numSamples = 44100 / freq;
    rraw_a1.startLoop(numSamples);
}

void handleNoteOff(uint8_t channel, uint8_t pitch, uint8_t velocity)
{
    rraw_a1.setLoopType(loop_type::looptype_none);
}

void setup() {
    analogReference(0);
    pinMode(analogInPin, INPUT_DISABLE);   // i.e. Analog

    Serial.begin(9600);
    while (!SD.begin(BUILTIN_SDCARD)) {
      Serial.println("initialization failed!");
      delay(1000);
    }

    MIDI.setHandleNoteOn(handleNoteOn);  
    MIDI.setHandleNoteOff(handleNoteOff);
    MIDI.begin(MIDI_CHANNEL_OMNI);

    sgtl5000_1.enable();
    sgtl5000_1.volume(0.5f, 0.5f);
    
    rraw_a1.enableInterpolation(true);
    int newsensorValue = analogRead(analogInPin);
    rraw_a1.setPlaybackRate(getPlaybackRate(newsensorValue));
    newdigate::flashloader loader;
    sample = loader.loadSample("MONKS.RAW");

    AudioMemory(50);
    Serial.println("setup done");
}

void loop() {
    int newsensorValue = analogRead(analogInPin);
    rraw_a1.setPlaybackRate(getPlaybackRate(newsensorValue));
    
    unsigned currentMillis = millis();
    if (currentMillis > lastSamplePlayed + 500) {
        if (!rraw_a1.isPlaying()) {
            rraw_a1.play(sample->sampledata, sample->samplesize/2);
            lastSamplePlayed = currentMillis;

            Serial.print("Memory: ");
            Serial.print(AudioMemoryUsage());
            Serial.print(",");
            Serial.print(AudioMemoryUsageMax());
            Serial.println();
        }
    }
    delay(10);
}
