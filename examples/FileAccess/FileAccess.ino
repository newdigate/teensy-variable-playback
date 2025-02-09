// Plays a WAV (16-bit signed) PCM audio file at slower or faster rate, while
// "simultaneously" accessing the filesystem. For most playback libraries this
// fails badly, due to (usually SD) accesses being made from both foreground
// and interrupt code. Even with EventResponder-based file accesses, we have to
// take precautions against yield() calls within the libraries causing clashes.

#include <Audio.h>
#include <TeensyVariablePlayback.h>

using namespace newdigate;

// Use ONE of SD or array playback - choose here:
#define USE_SD
#define noUSE_ARRAY

// Change code depending on playback source chosen above:
#if defined(USE_SD)
#define AUDIO_PLAYER   AudioPlaySdResmp
#define START_PLAYBACK playWav1.playWav(_filename)
#endif // defined(USE_SD)

#if defined(USE_ARRAY)
#define AUDIO_PLAYER   AudioPlayArrayResmp
#define START_PLAYBACK playWav1.playWav(bufr);
#endif // defined(USE_ARRAY)


// GUItool: begin automatically generated code
AUDIO_PLAYER             playWav1;     //xy=658,608
AudioOutputI2S           i2s2;           //xy=846,600
AudioConnection          patchCord1(playWav1, 0, i2s2, 0);
AudioConnection          patchCord2(playWav1, 0, i2s2, 1);
AudioControlSGTL5000     audioShield;    //xy=853,668
// GUItool: end automatically generated code

//====================================================================================
const char* _filename  = "sweep100-1k.wav";
const char* _filename2 = "sweep-copy.wav";
int16_t* bufr;
size_t flen;
int count;
unsigned long lastSamplePlayed = 0;

//====================================================================================
namespace copystate { typedef enum {idle,copying,complete,exists,error} state; }

void loopCopyUpdate(void)
{
  /* 
   * If you comment / remove these calls then playback and copying 
   * will fail very rapidly. The effect is to prevent yield() calls
   * made within the filesystem library from triggering an audio
   * buffer re-load, so you MUST ensure you have enough buffer
   * allocated for the time taken during your filesystem activity.
   * 
   * It is in any case a good plan to keep the bursts of activity
   * quite short, as is done here.
   * 
   * Buffer sizes and counts are specified at the top of the
   * Resampling<something>Reader.h files
   * 
   * If you use array (in-memory) playback then it is safe to permit
   * event responses during filesystem access, as the array doesn't
   * access the filesystem istelf. But you're then limited to the
   * total size of samples you can play at any one time.
   */
  AudioEventResponder::disableResponse(); 
  
      static copystate::state copystate = copystate::idle;
      static File src, dst;
      char buffer[1200];

      switch (copystate)
      {
        case copystate::idle:
          SD.remove(_filename2);
          src = SD.open(_filename, FILE_READ);
          dst = SD.open(_filename2, FILE_WRITE);
          if (src && dst)
          {
            copystate = copystate::copying;
            Serial.println("Copy started");
            ::count = 0;
          }
          else
            copystate = copystate::error;
          break;

        case copystate::copying:
        {
          int n = random(sizeof buffer - 100)+100;
          int got = src.read(buffer, n);
          if (got > 0)
            dst.write(buffer,got);
          //Serial.printf("%d/%d bytes copied\n", got, n);
          ::count+=got;
          if (got != n)
          {
            src.close();
            copystate = copystate::complete;            
          }
        }
          break;

        case copystate::exists:
          break;     

        case copystate::complete:
          Serial.printf("Copy complete: %d bytes\n", ::count);
          copystate = copystate::idle;
          break;     

        case copystate::error:
          Serial.println("Copy error");
          src.close();
          dst.close();
          copystate = copystate::idle;
          break;     
      }
  
  AudioEventResponder::enableResponse(); 
}

//====================================================================================
void setup()
{
  while (!Serial)
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

  AudioMemory(24);

  // audioShield.setAddress(HIGH);
  audioShield.enable();
  audioShield.volume(0.1);

  playWav1.enableInterpolation(true);
  //    playWav1.setBufferInPSRAM(true);
  playWav1.setUseDualPlaybackHead(true);
  playWav1.setCrossfadeDurationInSamples(500);

  // copy file to RAM for use with array playback
  File f = SD.open(_filename);
  if (f)
  {
    flen = f.size();
    bufr = (int16_t*) malloc(flen);
    f.read(bufr, flen);
    f.close();
  }
  else
    Serial.print("NOT ");
  Serial.println("playing...");
}

float rates[] = {1.0f, 1.5f, -1.0f, 2.0f};
int rate;

void loop()
{
  unsigned currentMillis = millis();
  if (currentMillis > lastSamplePlayed + 50)
  {
    if (!playWav1.isPlaying())
    {
      rate++;
      if (rate > 3)
        rate = 0;
      playWav1.setPlaybackRate(rates[rate]);
      if (0 == rate)
        Serial.println();
      Serial.printf("Rate %d (%.1fx)\n", rate, rates[rate]);

      playWav1.setLoopType(looptype_none);
      START_PLAYBACK; // method depends on playback object
      playWav1.setLoopType(looptype_repeat);
      playWav1.setLoopStart(22050);

      lastSamplePlayed = currentMillis;
    }
  }
  delay(10);
  loopCopyUpdate();
  if (currentMillis > lastSamplePlayed + 1900)
    playWav1.stop();
}
