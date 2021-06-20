#include "output_test.h"

audio_block_t * TestAudioOutput::block_left_1st = NULL;
audio_block_t * TestAudioOutput::block_right_1st = NULL;
bool TestAudioOutput::update_responsibility = false;

void TestAudioOutput::begin(void)
{
	update_responsibility = update_setup();
    active = true;
}

void TestAudioOutput::isr(void)
{
    if (TestAudioOutput::update_responsibility) AudioStream::update_all();
}

void TestAudioOutput::update(void)
{
	// null audio device: discard all incoming data
	if (!active) return;
	audio_block_t *block = receiveReadOnly();
	if (block) {
	    for (int i=0; i<AUDIO_BLOCK_SAMPLES;i++) {			
			if (block->data[i] < 0)
				Serial.printf("-%04x ", -block->data[i]);
			else 
		        Serial.printf(" %04x ", block->data[i]);
	    }
        Serial.println(); 
		release(block);
	} else {        
		Serial.printf(" empty block \n");
	}
}