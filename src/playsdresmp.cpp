//
// Created by Nicholas Newdigate on 18/07/2020.
//

#include "playsdresmp.h"
#include "spi_interrupt.h"
#include "waveheaderparser.h"

void AudioPlaySdResmp::begin()
{
    file_size = 0;
    sdReader.begin();
}

bool AudioPlaySdResmp::playRaw(const char *filename, uint16_t numChannels)
{
    stop();
    bool playing = sdReader.playRaw(filename, numChannels);
    return playing;
}

bool AudioPlaySdResmp::playWav(const char *filename)
{
    stop();
    bool playing = sdReader.playWav(filename);
    return playing;

}

void AudioPlaySdResmp::stop()
{
    sdReader.stop();
}

void AudioPlaySdResmp::update()
{
    int _numChannels = sdReader.getNumChannels();
    if (_numChannels == -1)
        return;

    unsigned int i, n;
    audio_block_t *blocks[_numChannels];
    int16_t *data[_numChannels];
    // only update if we're playing
    if (!sdReader.isPlaying()) return;

    // allocate the audio blocks to transmit
    for (int i=0; i < _numChannels; i++) {
        blocks[i] = allocate();
        if (blocks[i] == nullptr) return;
        data[i] = blocks[i]->data;
    }

    if (sdReader.available()) {
        // we can read more data from the file...
        n = sdReader.read((void**)data, AUDIO_BLOCK_SAMPLES);
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
        sdReader.close();
    }
    for (int channel=0; channel < _numChannels; channel++) {
        release(blocks[channel]);
    }
}

#define B2M (uint32_t)((double)4294967296000.0 / AUDIO_SAMPLE_RATE_EXACT / 2.0) // 97352592

uint32_t AudioPlaySdResmp::positionMillis()
{
    return ((uint64_t)file_size * B2M) >> 32;
}

uint32_t AudioPlaySdResmp::lengthMillis()
{
    return ((uint64_t)file_size * B2M) >> 32;
}