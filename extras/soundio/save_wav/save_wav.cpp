// Plays a RAW (16-bit signed) PCM audio file at slower or faster rate
// this example plays a sample stored in an array
#include <Arduino.h>
#include <Audio.h>
#include "../../../src/playarrayresmp.h"
#include "output_soundio.h"
#include <soundio/soundio.h>
#include <SD.h>
#include <iostream>
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <cassert>

// GUItool: begin automatically generated code
AudioPlayArrayResmp      rraw_a1;        //xy=306,225
AudioRecordQueue         queue1;         //xy=609,267
AudioOutputSoundIO       sio_out1;       //xy=612,224
AudioConnection          patchCord1(rraw_a1, 0, sio_out1, 0);
AudioConnection          patchCord2(rraw_a1, 0, sio_out1, 1);
AudioConnection          patchCord3(rraw_a1, 0, queue1, 0);
// GUItool: end automatically generated code

extern unsigned int mono_souljah_wav_len;
extern unsigned char mono_souljah_wav[];

int16_t buffer[512] = {0};
File frec;

unsigned long lastSamplePlayed = 0;
void my_handler(sig_atomic_t i);
static char stack_body[64*1024];
static stack_t sigseg_stack;
static struct sigaction sigseg_handler;

void crash_handler(sig_atomic_t i);

void setup() {
    Serial.begin(9600);       

    rraw_a1.setPlaybackRate(1.0f);
    rraw_a1.enableInterpolation(true);
    //rraw_a1.play((int16_t*)kick_raw, kick_raw_len/2);    
    Serial.println("setup done");

    if (SD.exists("RECORD.RAW")) {
        // The SD library writes new data to the end of the
        // file, so to start a new recording, the old file
        // must be deleted before new data is written.
        SD.remove("RECORD.RAW");
    }
    frec = SD.open("RECORD.RAW", O_WRITE);

    AudioMemory(120);
    
    if (frec) {
        queue1.begin();
        Serial.println("startRecording");
    } else {
        Serial.println("recording failed...");
    }
}

void loop() {
    unsigned currentMillis = millis();
    if (currentMillis > lastSamplePlayed + 1000) {
        if (!rraw_a1.isPlaying()) {
            rraw_a1.playWav((int16_t *)mono_souljah_wav, mono_souljah_wav_len/2);
            lastSamplePlayed = currentMillis;

            Serial.print("Memory: ");
            Serial.print(AudioMemoryUsage());
            Serial.print(",");
            Serial.print(AudioMemoryUsageMax());
            Serial.println();
        }
    }

    if (queue1.available() >= 1) {   
        int16_t* incomming = queue1.readBuffer();
        if (incomming != NULL) {
            memcpy(buffer, incomming, 256);
            queue1.freeBuffer();
            frec.write((unsigned char *)buffer, 256);
            frec.flush();
        } 
    } 
    delay(1);
}

int main() {
    signal (SIGINT,my_handler);
    signal (SIGSEGV,crash_handler);

    sigseg_stack.ss_sp = stack_body;
    sigseg_stack.ss_flags = SS_ONSTACK;
    sigseg_stack.ss_size = sizeof(stack_body);
 //   assert(!sigaltstack(&sigseg_stack, nullptr));
    sigseg_handler.sa_flags = SA_ONSTACK;
    sigseg_handler.sa_handler = &crash_handler;
 //   assert(!sigaction(SIGSEGV, &sigseg_handler, nullptr));

    initialize_mock_arduino();
    SD.setSDCardFolderPath(".");
    setup();
    while(!arduino_should_exit){
        loop();
    }
    delay(1000);
    frec.close();
}

void my_handler(sig_atomic_t i){
    if ( i== SIGINT) {
        arduino_should_exit = true;
        printf("Caught signal %d\n",i);
    } else 
    {
        cerr << "sig seg fault handler" << endl;
        const int asize = 10;
        void *array[asize];
        size_t size;

        // get void*'s for all entries on the stack
        size = backtrace(array, asize);

        // print out all the frames to stderr
        cerr << "stack trace: " << endl;
        backtrace_symbols_fd(array, size, STDERR_FILENO);
        cerr << "resend SIGSEGV to get core dump" << endl;
        signal(i, SIG_DFL);
        kill(getpid(), i);
    }
}
void crash_handler(sig_atomic_t i){
    cerr << "sig seg fault handler" << endl;
    const int asize = 10;
    void *array[asize];
    size_t size;

    // get void*'s for all entries on the stack
    size = backtrace(array, asize);

    // print out all the frames to stderr
    cerr << "stack trace: " << endl;
    backtrace_symbols_fd(array, size, STDERR_FILENO);
    cerr << "resend SIGSEGV to get core dump" << endl;
    signal(i, SIG_DFL);
    kill(getpid(), i);
}
