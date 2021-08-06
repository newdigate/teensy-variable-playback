//
// Created by Nicholas Newdigate on 18/07/2020.
//

#include "playarrayresmp.h"

void AudioPlayArrayResmp::begin()
{
    file_size = 0;
    arrayReader.begin();
}

bool AudioPlayArrayResmp::playRaw(int16_t *data, uint32_t numSamples, uint16_t numChannels)
{
    stop();
    bool playing = arrayReader.playRaw(data, numSamples, numChannels);
    return playing;
}

bool AudioPlayArrayResmp::playRaw(const unsigned int *data, uint32_t numSamples, uint16_t numChannels) 
{
    return playRaw((int16_t *) data, numSamples, numChannels);
}

bool AudioPlayArrayResmp::playWav(int16_t *data, uint32_t fileSize)
{
    stop();
    bool playing = arrayReader.playWav(data, fileSize);

    return playing;
}

bool  AudioPlayArrayResmp::playWav(const unsigned int *data, uint32_t fileSize) {
    return playWav((int16_t *) data, fileSize);
}

void AudioPlayArrayResmp::stop()
{
    arrayReader.stop();
}

void AudioPlayArrayResmp::update()
{
    int _numChannels = arrayReader.getNumChannels();
    if (_numChannels == -1)
        return;

    unsigned int i, n;
    audio_block_t *blocks[_numChannels];
    int16_t *data[_numChannels];
    // only update if we're playing
    if (!arrayReader.isPlaying()) return;

    // allocate the audio blocks to transmit
    for (int i=0; i < _numChannels; i++) {
        blocks[i] = allocate();
        if (blocks[i] == nullptr) return;
        data[i] = blocks[i]->data;
    }

    if (arrayReader.available()) {
        // we can read more data from the file...
        n = arrayReader.read((void**)data, AUDIO_BLOCK_SAMPLES);
        for (int channel=0; channel < _numChannels; channel++) {
            for (i=n; i < AUDIO_BLOCK_SAMPLES; i++) {
                blocks[channel]->data[i] = 0;
            }
            transmit(blocks[channel], channel);
        }

        if(_numChannels == 1) {
            transmit(blocks[0], 1);
        }
    } else {
        arrayReader.close();
    }
    for (int channel=0; channel < _numChannels; channel++) {
        release(blocks[channel]);
    }
}

#define B2M (uint32_t)((double)4294967296000.0 / AUDIO_SAMPLE_RATE_EXACT / 2.0) // 97352592

uint32_t AudioPlayArrayResmp::positionMillis()
{
    return ((uint64_t)file_size * B2M) >> 32;
}

uint32_t AudioPlayArrayResmp::lengthMillis()
{
    return ((uint64_t)file_size * B2M) >> 32;
}