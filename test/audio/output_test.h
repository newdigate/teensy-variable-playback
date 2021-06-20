#ifndef output_test_h_
#define output_test_h_

#include <Arduino.h>
#include <AudioStream.h>
#include <thread>
class TestAudioOutput : public AudioStream
{
public:
    TestAudioOutput(void) : AudioStream(2, inputQueueArray) { begin(); }
	virtual void update(void);
	void begin(void);
	static void isr(void);
protected:
	static audio_block_t *block_left_1st;
	static audio_block_t *block_right_1st;
	static bool update_responsibility;

private:
	audio_block_t *inputQueueArray[2];
};


#endif
