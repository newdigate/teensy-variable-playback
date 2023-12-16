#ifndef TEENSY_RESAMPLING_SDREADER_PLAYRESMP_H
#define TEENSY_RESAMPLING_SDREADER_PLAYRESMP_H

#include "Arduino.h"
#include "Audio.h"
#include "loop_type.h"

template <class TResamplingReader>
class AudioPlayResmp : public AudioStream
{
    public:
        AudioPlayResmp(): AudioStream(0, NULL), reader(nullptr)
        {
        }

        virtual ~AudioPlayResmp() {
        }

        void begin(void)
        {
            reader->begin();
        }

        bool playRaw(const char *filename, uint16_t numChannels)
        {
            stop();
            return reader->play(filename, false, numChannels);
        }

        bool playWav(const char *filename)
        {
            stop();
            return reader->play(filename, true, 0);
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

        void setPlayStart(play_start start) {
            reader->setPlayStart(start);
        }

        void enableInterpolation(bool enable) {
            if (enable)
                reader->setInterpolationType(ResampleInterpolationType::resampleinterpolation_quadratic);
            else 
                reader->setInterpolationType(ResampleInterpolationType::resampleinterpolation_none);
        }

        bool isPlaying(void) {
            return reader->isPlaying();
        };

        void stop() {
            reader->stop();
        }

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

				} else {
					reader->close();
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