// Plays a subset of piano samples at various rates
// to give a full chromatic scale.

#include <Audio.h>
#include <TeensyVariablePlayback.h>

// GUItool: begin automatically generated code
AudioPlaySdResmp           playSdWav1;     //xy=114,255
AudioPlaySdResmp           playSdWav2; //xy=117,293
AudioPlaySdResmp           playSdWav3; //xy=118,332
AudioPlaySdResmp           playSdWav4; //xy=121,372
AudioPlaySdResmp           playSdWav5; //xy=125,413
AudioPlaySdResmp           playSdWav6; //xy=131,453
AudioPlaySdResmp           playSdWav7; //xy=135,492
AudioPlaySdResmp           playSdWav8; //xy=138,532
AudioEffectEnvelope      envL1;      //xy=371,128
AudioEffectEnvelope      envL2; //xy=374,161
AudioEffectEnvelope      envL3; //xy=376,194
AudioEffectEnvelope      envL4; //xy=379,227
AudioEffectEnvelope      envL5; //xy=384,261
AudioEffectEnvelope      envL6; //xy=385,297
AudioEffectEnvelope      envL7; //xy=388,331
AudioEffectEnvelope      envL8; //xy=392,365
AudioEffectEnvelope      envR1; //xy=394,420
AudioEffectEnvelope      envR2; //xy=398,453
AudioEffectEnvelope      envR3; //xy=401,489
AudioEffectEnvelope      envR4; //xy=402,524
AudioEffectEnvelope      envR5; //xy=405,559
AudioEffectEnvelope      envR6; //xy=406,596
AudioEffectEnvelope      envR7; //xy=406,633
AudioEffectEnvelope      envR8; //xy=409,670
AudioMixer4              mixerL1;         //xy=609,206
AudioMixer4              mixerL2; //xy=612,281
AudioMixer4              mixerR1; //xy=621,506
AudioMixer4              mixerR2; //xy=625,578
AudioMixer4              mixerL; //xy=786,258
AudioMixer4              mixerR; //xy=798,553
AudioOutputI2S           i2sOut;           //xy=961,408

AudioConnection          patchCord1(playSdWav1, 0, envL1, 0);
AudioConnection          patchCord2(playSdWav1, 1, envR1, 0);
AudioConnection          patchCord3(playSdWav2, 0, envL2, 0);
AudioConnection          patchCord4(playSdWav2, 1, envR2, 0);
AudioConnection          patchCord5(playSdWav3, 0, envL3, 0);
AudioConnection          patchCord6(playSdWav3, 1, envR3, 0);
AudioConnection          patchCord7(playSdWav4, 0, envL4, 0);
AudioConnection          patchCord8(playSdWav4, 1, envR4, 0);
AudioConnection          patchCord9(playSdWav5, 0, envL5, 0);
AudioConnection          patchCord10(playSdWav5, 1, envR5, 0);
AudioConnection          patchCord11(playSdWav6, 0, envL6, 0);
AudioConnection          patchCord12(playSdWav6, 1, envR6, 0);
AudioConnection          patchCord13(playSdWav7, 0, envL7, 0);
AudioConnection          patchCord14(playSdWav7, 1, envR7, 0);
AudioConnection          patchCord15(playSdWav8, 0, envL8, 0);
AudioConnection          patchCord16(playSdWav8, 1, envR8, 0);
AudioConnection          patchCord17(envL1, 0, mixerL1, 0);
AudioConnection          patchCord18(envL2, 0, mixerL1, 1);
AudioConnection          patchCord19(envL3, 0, mixerL1, 2);
AudioConnection          patchCord20(envL4, 0, mixerL1, 3);
AudioConnection          patchCord21(envL5, 0, mixerL2, 0);
AudioConnection          patchCord22(envL6, 0, mixerL2, 1);
AudioConnection          patchCord23(envL7, 0, mixerL2, 2);
AudioConnection          patchCord24(envL8, 0, mixerL2, 3);
AudioConnection          patchCord25(envR1, 0, mixerR1, 0);
AudioConnection          patchCord26(envR2, 0, mixerR1, 1);
AudioConnection          patchCord27(envR3, 0, mixerR1, 2);
AudioConnection          patchCord28(envR4, 0, mixerR1, 3);
AudioConnection          patchCord29(envR5, 0, mixerR2, 0);
AudioConnection          patchCord30(envR6, 0, mixerR2, 1);
AudioConnection          patchCord31(envR7, 0, mixerR2, 2);
AudioConnection          patchCord32(envR8, 0, mixerR2, 3);
AudioConnection          patchCord33(mixerL1, 0, mixerL, 0);
AudioConnection          patchCord34(mixerL2, 0, mixerL, 1);
AudioConnection          patchCord35(mixerR1, 0, mixerR, 0);
AudioConnection          patchCord36(mixerR2, 0, mixerR, 1);
AudioConnection          patchCord37(mixerL, 0, i2sOut, 0);
AudioConnection          patchCord38(mixerR, 0, i2sOut, 1);

AudioControlSGTL5000     sgtl5000;     //xy=994,461
// GUItool: end automatically generated code
//====================================================================================
/*
 * Samples at 6-semitone intervals: we allow ourselves to change
 * playback speed to flatten by 3 semitones, or sharpen by 2
 */
const char* samples[] = 
{
  "/piano/C2.wav", // 36
  "/piano/F#2.wav",
  "/piano/C3.wav",
  "/piano/F#3.wav",
  "/piano/C4.wav", // MIDI note 60, middle C, 261.6Hz
  "/piano/F#4.wav",
  "/piano/C5.wav",
  "/piano/F#5.wav",
  "/piano/C6.wav", // 84

};

const int LOWEST_NOTE = 36-3, HIGHEST_NOTE = 84+2; // available range using the above samples
const double semitone = pow(2.0, 1.0/12);          // multiply by this to change frequency one semitone
extern const unsigned char score[];
#define COUNT_OF(a) (int)(sizeof a / sizeof a[0])

//====================================================================================
const float velocity2amplitude[127] = {
0.01778,0.01966,0.02164,0.02371,0.02588,0.02814,0.03049,0.03294,0.03549,0.03812,
0.04086,0.04369,0.04661,0.04963,0.05274,0.05594,0.05924,0.06264,0.06613,0.06972,
0.07340,0.07717,0.08104,0.08500,0.08906,0.09321,0.09746,0.10180,0.10624,0.11077,
0.11539,0.12011,0.12493,0.12984,0.13484,0.13994,0.14513,0.15042,0.15581,0.16128,
0.16685,0.17252,0.17828,0.18414,0.19009,0.19613,0.20227,0.20851,0.21484,0.22126,
0.22778,0.23439,0.24110,0.24790,0.25480,0.26179,0.26887,0.27605,0.28333,0.29070,
0.29816,0.30572,0.31337,0.32112,0.32896,0.33690,0.34493,0.35306,0.36128,0.36960,
0.37801,0.38651,0.39511,0.40381,0.41260,0.42148,0.43046,0.43953,0.44870,0.45796,
0.46732,0.47677,0.48631,0.49595,0.50569,0.51552,0.52544,0.53546,0.54557,0.55578,
0.56609,0.57648,0.58697,0.59756,0.60824,0.61902,0.62989,0.64085,0.65191,0.66307,
0.67432,0.68566,0.69710,0.70863,0.72026,0.73198,0.74380,0.75571,0.76771,0.77981,
0.79201,0.80430,0.81668,0.82916,0.84174,0.85440,0.86717,0.88003,0.89298,0.90602,
0.91917,0.93240,0.94573,0.95916,0.97268,0.98629,1.00000
};

//====================================================================================
class Voice
{
  AudioPlaySdResmp& playWav;
  AudioEffectEnvelope& envL, &envR;
  AudioMixer4& mixerL, &mixerR;
  int mixCh;
  bool active;
  
  void initEnv(AudioEffectEnvelope& e)
  {
    e.delay(0.0f);
    e.attack(0.0f);
    e.hold(0.0f);
    e.decay(0.0f);
    e.sustain(1.0f);
    e.release(200.0f); // just use release to smooth note end
  }
public:
  Voice(AudioPlaySdResmp& p, AudioEffectEnvelope& el, AudioEffectEnvelope& er, 
        AudioMixer4& ml, AudioMixer4& mr, int ch);
  bool noteOn(int note, int velocity);
  void noteOff(void);
  void poll(void);
};


Voice::Voice(AudioPlaySdResmp& p, 
             AudioEffectEnvelope& el, AudioEffectEnvelope& er, 
             AudioMixer4& ml, AudioMixer4& mr, 
             int ch)
  : playWav(p), envL(el), envR(er), mixerL(ml), mixerR(mr), mixCh(ch)
{
  initEnv(envL);
  initEnv(envR);  
}


bool Voice::noteOn(int note, int velocity)
{
  bool result = false;
  if (note >= LOWEST_NOTE && note <= HIGHEST_NOTE && velocity > 0)
  {
    // pick sample to start with
    note -= LOWEST_NOTE;
    int idx = note / 6; 

    // change playback rate as needed
    double rate = pow(semitone, note % 6 - 3);
    
    playWav.playWav(samples[idx]);
    playWav.setPlaybackRate(rate);
    envL.noteOn();
    envR.noteOn();

    velocity -= 1;
    mixerL.gain(mixCh, velocity2amplitude[velocity]);
    mixerR.gain(mixCh, velocity2amplitude[velocity]);
    
    active = true;
    result = true;
  }
  return result;
}


void Voice::noteOff(void)
{
    envL.noteOff();
    envR.noteOff();  
}


// poll to stop streaming sample once the envelopes are inactive
void Voice::poll(void)
{
    if (active && !envL.isActive() && !envR.isActive())
    {
      playWav.stop();
      active = false;
    }
}


//====================================================================================
Voice voices[] = {
  Voice{playSdWav1,envL1,envR1,mixerL1,mixerR1,0},
  Voice{playSdWav2,envL2,envR2,mixerL1,mixerR1,1},
  Voice{playSdWav3,envL3,envR3,mixerL1,mixerR1,2},
  Voice{playSdWav4,envL4,envR4,mixerL1,mixerR1,3},
  Voice{playSdWav5,envL5,envR5,mixerL2,mixerR2,0},
  Voice{playSdWav6,envL6,envR6,mixerL2,mixerR2,1},
  Voice{playSdWav7,envL7,envR7,mixerL2,mixerR2,2},
  Voice{playSdWav8,envL8,envR8,mixerL2,mixerR2,3},
};

//====================================================================================
// Based on PlaySynthMusic example:
#define CMD_PLAYNOTE  0x90  /* play a note: low nibble is generator #, note is next byte */
#define CMD_STOPNOTE  0x80  /* stop a note: low nibble is generator # */
#define CMD_RESTART 0xe0  /* restart the score from the beginning */
#define CMD_STOP  0xf0  /* stop playing */

void updatePlayer(void)
{
  static uint32_t next;
  static const unsigned char* p = score;
  unsigned char cmd, ch;

  if (millis() >= next)
  {
    cmd = *p++;
    ch = cmd & 0x0F;
    
    switch (cmd & 0xF0)
    {
      default: // delay
        next = millis() + ((cmd<<8) | *p++);
        break;

      case CMD_STOP:
        for (int i=0;i<COUNT_OF(voices);i++)
          voices[i].noteOff();
        next = 0xFFFFFFFF;
        p--;
        Serial.println("Stopped");
        break;

      case CMD_PLAYNOTE:
      {
        int note = *p++, velocity = *p++;
        voices[ch].noteOn(note, velocity);
      }
        break;
                
      case CMD_STOPNOTE:
        voices[ch].noteOff();
        break; 
    }
  }
}

//====================================================================================
void setup() 
{
    while(!Serial)
      ;
    if (CrashReport)
      Serial.print(CrashReport);

    while (!(SD.begin(BUILTIN_SDCARD))) 
    {
        // stop here if no SD card, but print a message
        Serial.println("Unable to access the SD card");
        delay(500);
    }
    Serial.println("SD card initialised");

    AudioMemory(20);

    // sgtl5000.setAddress(HIGH);
    sgtl5000.enable();
    sgtl5000.volume(0.2);

    Serial.println("playing...");
}


void loop()
{
  static uint32_t last;
  
  updatePlayer(); // play the sequence

  // turn off silent voices
  for (int i=0;i<COUNT_OF(voices);i++)
    voices[i].poll();

  // show CPU and audio block usage
  if (millis() - last >= 2000)
  {
    last = millis();
    Serial.printf("Max CPU: %.2f%%; max blocks: %d\n",
                  AudioProcessorUsageMax(),
                  AudioMemoryUsageMax());
    AudioProcessorUsageMaxReset();
    AudioMemoryUsageMaxReset();
  }
}
