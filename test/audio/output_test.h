#ifndef output_test_h_
#define output_test_h_

#include <Arduino.h>
#include <AudioStream.h>
#include <thread>
#include <iostream>
#include <fstream>

class TestAudioOutput : public AudioStream
{
public:
    TestAudioOutput(void) : AudioStream(2, inputQueueArray) { begin(); }
	virtual void update(void);
	void begin(void);
	static void isr(void);
	void saveOutputFile(char* filename){
		if (num_inputs == 0) return;
		_outputFile.open(filename);
		if (!_outputFile.is_open()) {
        	Serial.println("couldn't open file for recording...");			
        } else {
			_filename = filename;
			_outputFile.write((char*)test_output_wav_header, 44);
			_saveToFile = true;			
		}
    }
	void closeOutputfile() {
		if (!_saveToFile) return;		
		_saveToFile = false;
		_outputFile.seekp(40, ios_base::beg);
		char buf[4];
		buf[3] = _dataSize >> 24;
		buf[2] = _dataSize >> 16;
		buf[1] = _dataSize >> 8;
		buf[0] = _dataSize;
		_outputFile.write(buf, 4);
		_outputFile.close();
		_filename = nullptr;
	}
protected:
	std::ofstream _outputFile;
	static audio_block_t *block_left_1st;
	static audio_block_t *block_right_1st;
	static bool update_responsibility;
	bool _saveToFile = false;
	bool _dataSize = 0;
	char *_filename;
	static void memcpy_tdm_tx(int16_t *dest, int16_t *src1, int16_t *src2);

private:
	audio_block_t *inputQueueArray[2];
	static unsigned char test_output_wav_header[]; 
};


#endif
