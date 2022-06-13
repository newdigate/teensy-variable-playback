// Plays a RAW (16-bit signed) PCM audio file at slower or faster rate
// this example requires an uSD-card inserted to teensy 3.6 with a file called DEMO.RAW
#include <Arduino.h>
#include <Audio.h>
#include <LittleFS.h>
#include <TeensyVariablePlayback.h>

LittleFS_Program         myfs;
#define PROG_FLASH_SIZE 1024 * 1024 * 1 // Specify size to use of onboard Teensy Program Flash chip

// GUItool: begin automatically generated code
AudioPlayLfsResmp        playLfsRaw1(myfs);     //xy=324,457
AudioOutputI2S           i2s2;                  //xy=840.8571472167969,445.5714416503906
AudioConnection          patchCord1(playLfsRaw1, 0, i2s2, 0);
AudioConnection          patchCord2(playLfsRaw1, 0, i2s2, 1);
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

    // see if the Flash is present and can be initialized:
    // lets check to see if the T4 is setup for security first
    #if ARDUINO_TEENSY40
    if ((IOMUXC_GPR_GPR11 & 0x100) == 0x100) {
        //if security is active max disk size is 960x1024
        if (PROG_FLASH_SIZE > 960 * 1024) {
        diskSize = 960 * 1024;
        Serial.printf("Security Enables defaulted to %u bytes\n", diskSize);
        } else {
        diskSize = PROG_FLASH_SIZE;
        Serial.printf("Security Not Enabled using %u bytes\n", diskSize);
        }
    }
    #else
    diskSize = PROG_FLASH_SIZE;
    #endif

    // checks that the LittFS program has started with the disk size specified
    if (!myfs.begin(diskSize)) {
        Serial.printf("Error starting %s\n", "PROGRAM FLASH DISK");
        while (1) {
        // Error, so don't do anything more - stay stuck here
        }
    }
    Serial.println("LittleFS initialized.");

    audioShield.enable();
    audioShield.volume(0.5);

    playLfsRaw1.enableInterpolation(true);
    int newsensorValue = analogRead(analogInPin);
    playLfsRaw1.setPlaybackRate(getPlaybackRate(newsensorValue));

    AudioMemory(24);
}

void loop() {

    int newsensorValue = analogRead(analogInPin);
    playLfsRaw1.setPlaybackRate(getPlaybackRate(newsensorValue));
    
    unsigned currentMillis = millis();
    if (currentMillis > lastSamplePlayed + 500) {
        if (!playLfsRaw1.isPlaying()) {
            playLfsRaw1.playRaw(_filename, 1);
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