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
	audio_block_t *block = receiveReadOnly(0);
	audio_block_t *blockRight = receiveReadOnly(1);
	if (block) {

		if (_saveToFile) {
			if (!blockRight){
				_outputFile.write((char*)block->data, 256);
				_dataSize += 256;
			} else {

				int16_t interleaved[256];

				if (blockRight) {
					memcpy_tdm_tx(interleaved, block->data, blockRight->data);
				} else 
					memset(interleaved, 0, 512);
				
				_outputFile.write((char*)interleaved, 512);
				_dataSize += 512;
			}
			
		}

		/*
		Serial.print("Ch1:"); 
	    for (int i=0; i<AUDIO_BLOCK_SAMPLES;i++) {			
			if (block->data[i] < 0)
				Serial.printf("-%04x ", -block->data[i]);
			else 
		        Serial.printf(" %04x ", block->data[i]);
	    }
        Serial.println(); 
		*/
		release(block);

		if (blockRight) {
			/*
			Serial.print("Ch2:"); 
			for (int i=0; i<AUDIO_BLOCK_SAMPLES;i++) {			
				if (blockRight->data[i] < 0)
					Serial.printf("-%04x ", -blockRight->data[i]);
				else 
					Serial.printf(" %04x ", blockRight->data[i]);
			}
			Serial.println(); 
			*/
			release(blockRight);
		}
	} else {        
		//Serial.print(" empty block \n");
	}
}

// Taken from https://github.com/PaulStoffregen/Audio/blob/master/output_tdm.cpp
void TestAudioOutput::memcpy_tdm_tx(int16_t *dest, int16_t *src1, int16_t *src2)
{
	int16_t i, in1, in2, out1, out2;

	for (i=0; i < AUDIO_BLOCK_SAMPLES; i++) {
		*dest = *src1++;
		*(dest + 1) = *src2++;
		dest += 2;
	}
}

unsigned char TestAudioOutput::test_output_wav_header[] = {
		0x52, 0x49, 0x46, 0x46, 0x38, 0x00, 0x00, 0x00, 0x57, 0x41, 0x56, 0x45,
		0x66, 0x6d, 0x74, 0x20, 0x10, 0x00, 0x00, 0x00, 0x01, 0x00, 0x02, 0x00,
		0x44, 0xac, 0x00, 0x00, 0x10, 0xb1, 0x02, 0x00, 0x04, 0x00, 0x10, 0x00,
		0x64, 0x61, 0x74, 0x61, 0x14, 0x00, 0x00, 0x00
};
