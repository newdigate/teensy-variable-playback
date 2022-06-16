// Plays a RAW (16-bit signed) PCM audio file at slower or faster rate
// this example requires an uSD-card inserted to teensy 3.6 with a file called DEMO.RAW
#include <Arduino.h>
#include <Audio.h>
#include <SerialFlash.h>
#include <TeensyVariablePlayback.h>

#define CSPIN              6

SerialFlashChip         myfs;

// GUItool: begin automatically generated code
AudioPlaySerialFlashResmp        playSerialFlash1(myfs);     //xy=324,457
AudioOutputI2S           i2s2;                  //xy=840.8571472167969,445.5714416503906
AudioConnection          patchCord1(playSerialFlash1, 0, i2s2, 0);
AudioConnection          patchCord2(playSerialFlash1, 0, i2s2, 1);
AudioControlSGTL5000     audioShield;
// GUItool: end automatically generated code

#define A14 10

const char* _filename = "DEMO.RAW";
const int analogInPin = A14;
unsigned long lastSamplePlayed = 0;
uint32_t diskSize;

double getPlaybackRate(int16_t analog) { //analog: 0..1023
    return  (analog - 512.0) / 512.0;
}

void setup() {
    analogReference(0);
    pinMode(analogInPin, INPUT_DISABLE);   // i.e. Analog


    if (!SerialFlash.begin(CSPIN)) {
        while (1) {
        Serial.println(F("Unable to access SPI Flash chip"));
        delay(1000);
        }
    }
    Serial.println("SerialFlash initialized.");

    audioShield.enable();
    audioShield.volume(0.5);

    playSerialFlash1.enableInterpolation(true);
    int newsensorValue = analogRead(analogInPin);
    playSerialFlash1.setPlaybackRate(getPlaybackRate(newsensorValue));

    AudioMemory(24);
}

void loop() {

    int newsensorValue = analogRead(analogInPin);
    playSerialFlash1.setPlaybackRate(getPlaybackRate(newsensorValue));
    
    unsigned currentMillis = millis();
    if (currentMillis > lastSamplePlayed + 500) {
        if (!playSerialFlash1.isPlaying()) {
            playSerialFlash1.playRaw(_filename, 1);
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


namespace std {
    void __throw_bad_function_call() {}
    void __throw_length_error(char const*) {}
}