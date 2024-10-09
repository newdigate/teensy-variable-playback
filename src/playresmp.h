#ifndef TEENSY_RESAMPLING_SDREADER_PLAYRESMP_H
#define TEENSY_RESAMPLING_SDREADER_PLAYRESMP_H

#include "Arduino.h"
#include "Audio.h"
#include "loop_type.h"
#include "AudioEventResponder.h"

extern void readerClose(void);

template <class TResamplingReader>
class AudioPlayResmp : public AudioStream, public newdigate::AudioEventResponder
{
		enum {evNothing,evReload,evClose};
    public:
        AudioPlayResmp(): AudioStream(0, NULL), reader(nullptr)
        {
        }

        virtual ~AudioPlayResmp() {
        }

		static void event_response(EventResponderRef evRef)
		{
// digitalWriteFast(33,1);
			AudioPlayResmp<TResamplingReader>* player = (AudioPlayResmp*) evRef.getData();
			TResamplingReader* reader = (TResamplingReader*) player->reader;
			int status = evRef.getStatus();
			
			disableResponse();
			if (nullptr != reader)
				switch (status)
				{
					case evReload:
						reader->triggerReload();
						break;
						
					case evClose:
						reader->close();
						break;
				}
			enableResponse();
// digitalWriteFast(33,0);
		}
		
        void begin(void)
        {
            reader->begin();
        }

        bool playRaw(const char *filename, uint16_t numChannels)
        {
			disableResponse();
            stop();
			if (getForceResponse())
				attachPolled(event_response);
			else
				attach(event_response);
			updateResponse();
            bool result = reader->play(filename, false, numChannels);
			enableResponse();
			return result;
        }

        bool playWav(const char *filename)
        {
			disableResponse();
            stop();
			if (getForceResponse())
				attachPolled(event_response);
			else
				attach(event_response);
			updateResponse();
            bool result = reader->play(filename, true, 0);
			enableResponse();
			return result;
        }
        
        bool playRaw(int16_t *data, uint32_t numSamples, uint16_t numChannels)
        {
            stop();
            return reader->playRaw(data, numSamples, numChannels);
        }

        bool playRaw(const unsigned int *data, uint32_t numSamples, uint16_t numChannels) 
        {
            return playRaw((int16_t *) data, numSamples, numChannels);
        }

        bool playWav(int16_t *data, uint32_t fileSize)
        {
            stop();
            return reader->playWav(data, fileSize);
        }

        bool playWav(const unsigned int *data, uint32_t fileSize) {
            return playWav((int16_t *) data, fileSize);
        }

        bool playWav(int16_t *data)
        {
            stop();
            return reader->playWav(data);
        }

        void setPlaybackRate(float f) {
            reader->setPlaybackRate(f);
        }

        void setLoopType(loop_type t) {
            reader->setLoopType(t);
        }

        loop_type getLoopType(void) {
            return reader->getLoopType();
        }

        void setLoopStart(uint32_t loop_start) {
            reader->setLoopStart(loop_start);
        }

        void setLoopFinish(uint32_t loop_finish) {
            reader->setLoopFinish(loop_finish);
        }

        void setUseDualPlaybackHead(bool useDualPlaybackHead) {
            reader->setUseDualPlaybackHead(useDualPlaybackHead);
        }

        void setCrossfadeDurationInSamples(unsigned int crossfadeDurationInSamples) {
            reader->setCrossfadeDurationInSamples(crossfadeDurationInSamples);
        }

        void setPlayStart(play_start start, uint32_t playback_start = 0) {
            reader->setPlayStart(start, playback_start);
        }

        void enableInterpolation(bool enable) {
            if (enable)
                reader->setInterpolationType(ResampleInterpolationType::resampleinterpolation_quadratic);
            else 
                reader->setInterpolationType(ResampleInterpolationType::resampleinterpolation_none);
        }

        bool isPlaying(void) {
            return reader->isPlaying();
        }

        bool getBufferInPSRAM(void) {
            return reader->getBufferInPSRAM();
        }

        void setBufferInPSRAM(bool flag) {
            reader->setBufferInPSRAM(flag);
        }

        void stop() {
			disableResponse();
			clearEvent();
            reader->stop();
			enableResponse();
        }
		
		size_t getBufferSize(void) { return reader->getBufferSize(); }
		void getStatus(char* buf)  { return reader->getStatus(buf); }
		void triggerReload()  { return reader->triggerReload(this); }

        void update()
        {
			bool gotBlocks = true;
            int _numChannels = reader->getNumChannels();
            if (_numChannels == -1)
                return;

            unsigned int n;
            audio_block_t *blocks[_numChannels];
            int16_t *data[_numChannels];
            // only update if we're playing
            if (!reader->isPlaying()) return;

            // allocate the audio blocks to transmit
            for (int i=0; i < _numChannels; i++) {
                blocks[i] = allocate();
                if (blocks[i] == nullptr) 
					gotBlocks = false;
				else
					data[i] = blocks[i]->data;
            }

			if (gotBlocks) // enough blocks, do the transmit
			{
				if (reader->available()) {
					// we can read more data from the file...
					n = reader->read((void**)data, AUDIO_BLOCK_SAMPLES);
					for (int channel=0; channel < _numChannels; channel++) {
						memset( &blocks[channel]->data[n], 0, (AUDIO_BLOCK_SAMPLES - n) * 2);
						transmit(blocks[channel], channel);
					}

					if(_numChannels == 1) {
						transmit(blocks[0], 1);
					}
					
					if (AUDIO_BLOCK_SAMPLES == n) // got enough samples...
						triggerEvent(evReload,this); // ...load more if needed
					else
						triggerEvent(evClose,this); // ...end of file, finish playing
				} else {
					triggerEvent(evClose,this);
				}
			}
			
			// release all allocated blocks, even if there
			// weren't enough and we couldn't transmit
            for (int channel=0; channel < _numChannels; channel++) 
			{
				if (nullptr != blocks[channel]) // only release if allocated!
					release(blocks[channel]);
            }
        }
		
        uint32_t positionMillis()
        {
            return reader->positionMillis();
        }

        uint32_t lengthMillis()
        {
            return reader->lengthMillis();
        }

        double getCrossFade() {
            return reader->getCrossfade();
        }

        int getBufferPosition1() {
            return reader->getBufferPosition1();
        }

        int getBufferPosition2() {
            return reader->getBufferPosition2();
        }

        bool getUseDualPlaybackHead() {
            return reader->getUseDualPlaybackHead();
        }

        int32_t getLoopFinish() {
            return reader->getLoopFinish();
        }

    protected:
        TResamplingReader *reader;
};

#endif // TEENSY_RESAMPLING_SDREADER_PLAYRESMP_H