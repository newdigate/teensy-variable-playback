// Plays all WAV files from the root directory on the SD card 
#include <Arduino.h>
#include <Audio.h>
#include <SD.h>
#include <TeensyVariablePlayback.h>

// GUItool: begin automatically generated code
AudioPlaySdResmp         playSd1;     //xy=324,457
AudioOutputI2S           i2s2;           //xy=840.8571472167969,445.5714416503906
AudioConnection          patchCord1(playSd1, 0, i2s2, 0);
AudioConnection          patchCord2(playSd1, 1, i2s2, 1);
AudioControlSGTL5000     audioShield;
// GUItool: end automatically generated code

#define A14 10

char** _filenames  = nullptr;
uint16_t _fileIndex = 0;
uint16_t _numWaveFiles = 0;

const int analogInPin = A14;
unsigned long lastSamplePlayed = 0;

double getPlaybackRate(int16_t analog) { //analog: 0..1023
    return  (analog - 512.0) / 512.0;
}

uint16_t getNumWavFilesInDirectory(char *directory);
void populateFilenames(char *directory, char **filenames);

void setup() {
    analogReference(0);
    pinMode(analogInPin, INPUT_DISABLE);   // i.e. Analog

    Serial.begin(57600);

    if (!(SD.begin(BUILTIN_SDCARD))) {
        // stop here if no SD card, but print a message
        while (1) {
            Serial.println("Unable to access the SD card");
            delay(500);
        }
    }
    audioShield.enable();
    audioShield.volume(0.5);

    playSd1.enableInterpolation(true);
    int newsensorValue = analogRead(analogInPin);
    playSd1.setPlaybackRate(getPlaybackRate(newsensorValue));

    _numWaveFiles = getNumWavFilesInDirectory("/");
    Serial.printf("Num wave files: %d\n", _numWaveFiles);
    _filenames = new char*[_numWaveFiles];
    populateFilenames("/", _filenames);

    AudioMemory(24);
}

void loop() {

    int newsensorValue = analogRead(analogInPin);
    playSd1.setPlaybackRate(getPlaybackRate(newsensorValue));
    
    unsigned currentMillis = millis();
    if (currentMillis > lastSamplePlayed + 1000) {
        if (!playSd1.isPlaying()) {
            
            if (playSd1.playWav(_filenames[_fileIndex])) {
              lastSamplePlayed = currentMillis;
              Serial.printf("playing %s\n", _filenames[_fileIndex]);
            }
            _fileIndex++;
            _fileIndex %= _numWaveFiles;
            Serial.print("Memory: ");
            Serial.print(AudioMemoryUsage());
            Serial.print(",");
            Serial.print(AudioMemoryUsageMax());
            Serial.println();
        }
    }
    delay(10);
}

uint16_t getNumWavFilesInDirectory(char *directory) {
  File dir = SD.open(directory);
  uint16_t numWaveFiles = 0;

  while (true) { 

    File files =  dir.openNextFile();
    if (!files) {
      //If no more files, break out.
      break;
    }

    String curfile = files.name(); //put file in string
    
    int m = curfile.lastIndexOf(".WAV");
    int a = curfile.lastIndexOf(".wav");
    int underscore = curfile.indexOf("_");

    // if returned results is more then 0 add them to the list.
    if ((m > 0 || a > 0) && (underscore != 0)) {  
        numWaveFiles++;
    }
    
    files.close();
  }
  // close 
  dir.close();
  return numWaveFiles;
}

void populateFilenames(char *directory, char **filenames) {
  File dir = SD.open(directory);
  uint16_t index = 0;

  while (true) { 

    File files =  dir.openNextFile();
    if (!files) {
      //If no more files, break out.
      break;
    }

    String curfile = files.name(); //put file in string
    
    int m = curfile.lastIndexOf(".WAV");
    int a = curfile.lastIndexOf(".wav");
    int underscore = curfile.indexOf("_");

    // if returned results is more then 0 add them to the list.
    if ((m > 0 || a > 0) && (underscore != 0)) {  
        filenames[index] = new char[curfile.length()+1] {0};
        memcpy(filenames[index], curfile.c_str(), curfile.length());
    } 
    files.close();
  }
  // close 
  dir.close();
}

namespace std {
    void __throw_bad_function_call() {}
    void __throw_length_error(char const*) {}
}