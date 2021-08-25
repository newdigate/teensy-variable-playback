#ifndef output_test_h_
#define output_test_h_

#include <Arduino.h>
#include <AudioStream.h>
#include <thread>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <unistd.h>

#ifdef __APPLE__ 
#define __filesystem std::__fs::filesystem
#else
#define __filesystem std::filesystem
#endif
class TestAudioOutput : public AudioStream
{
public:
    TestAudioOutput(void) : AudioStream(2, inputQueueArray) { begin(); }
	virtual void update(void);
	void begin(void);
	static void isr(void);
	bool saveOutputFile(const char * path, const char* filename){
		if (num_inputs == 0) return false;
		char cwd[500];
		if (getcwd(cwd, sizeof(cwd)) != NULL) {
			printf("Current working dir: %s\n", cwd);
		} else {
			perror("getcwd() error");
		}
		string outputPath = string(cwd) + "/" + string(path);
		__filesystem::path p(outputPath);
		if (! __filesystem::exists(p) )
			__filesystem::create_directories(outputPath);
		
		_filePath = outputPath + string(filename);
		std::cout << "saving output audio .wav file to " << _filePath << std::endl;
		_outputFile.open(_filePath, ios_base::trunc | ios_base::out);
		if (!_outputFile.is_open()) {
        	Serial.printf("couldn't open file for recording...%s\n", _filePath.c_str());
			return false;	
        } else {
			_filename = filename;
			_outputFile.write((char*)test_output_wav_header, 44);
			_saveToFile = true;	
			return true;		
		}
    }
	void closeOutputfile(uint16_t numChannels) {
		if (!_saveToFile) return;
		if (_outputFile.is_open()) {
			_saveToFile = false;
			char buf[4];
			buf[1] = numChannels >> 8;
			buf[0] = numChannels;
			_outputFile.seekp(22, ios_base::beg);
			_outputFile.write(buf, 2);

			long bytespersecond = numChannels * 2 * 44100;
			buf[3] = bytespersecond >> 24;
			buf[2] = bytespersecond >> 16;
			buf[1] = bytespersecond >> 8;
			buf[0] = bytespersecond;
			_outputFile.seekp(28, ios_base::beg);
			_outputFile.write(buf, 4);

			short bytespersampleframe = numChannels * 2;
			buf[1] = bytespersampleframe >> 8;
			buf[0] = bytespersampleframe;
			_outputFile.seekp(32, ios_base::beg);
			_outputFile.write(buf, 2);

			buf[3] = _dataSize >> 24;
			buf[2] = _dataSize >> 16;
			buf[1] = _dataSize >> 8;
			buf[0] = _dataSize;
			_outputFile.seekp(40, ios_base::beg);
			_outputFile.write(buf, 4);
			_outputFile.close();
			_filename = nullptr;
		}
	}
protected:
	std::ofstream _outputFile;
	std::string _filePath;
	static audio_block_t *block_left_1st;
	static audio_block_t *block_right_1st;
	static bool update_responsibility;
	bool _saveToFile = false;
	unsigned int _dataSize = 0;
	const char *_filename;
	static void memcpy_tdm_tx(int16_t *dest, int16_t *src1, int16_t *src2);

private:
	audio_block_t *inputQueueArray[2];
	static unsigned char test_output_wav_header[]; 
};


#endif
