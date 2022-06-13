//
// Created by Nicholas Newdigate on 18/07/2020.
//

#include "playlfsresmp.h"
#include "spi_interrupt.h"
#include "waveheaderparser.h"

void AudioPlayLfsResmp::begin()
{
    file_size = 0;
    lfsReader.begin();
}

bool AudioPlayLfsResmp::playRaw(const char *filename, uint16_t numChannels)
{
    stop();
    bool playing = lfsReader.playRaw(filename, numChannels);
    return playing;
}

bool AudioPlayLfsResmp::playWav(const char *filename)
{
    stop();
    bool playing = lfsReader.playWav(filename);
    return playing;
}

void AudioPlayLfsResmp::stop()
{
    lfsReader.stop();
}

void AudioPlayLfsResmp::update()
{
    int _numChannels = lfsReader.getNumChannels();
    if (_numChannels == -1)
        return;

    unsigned int i, n;
    audio_block_t *blocks[_numChannels];
    int16_t *data[_numChannels];
    // only update if we're playing
    if (!lfsReader.isPlaying()) return;

    // allocate the audio blocks to transmit
    for (int i=0; i < _numChannels; i++) {
        blocks[i] = allocate();
        if (blocks[i] == nullptr) return;
        data[i] = blocks[i]->data;
    }

    if (lfsReader.available()) {
        // we can read more data from the file...
        n = lfsReader.read((void**)data, AUDIO_BLOCK_SAMPLES);
        for (int channel=0; channel < _numChannels; channel++) {
            memset( &blocks[channel]->data[n], 0, (AUDIO_BLOCK_SAMPLES - n) * 2);
            transmit(blocks[channel], channel);
        }

        if(_numChannels == 1) {
            transmit(blocks[0], 1);
        }
    } else {
        lfsReader.close();
    }
    for (int channel=0; channel < _numChannels; channel++) {
        release(blocks[channel]);
    }
}

uint32_t AudioPlayLfsResmp::positionMillis()
{
    return lfsReader.positionMillis();
}

uint32_t AudioPlayLfsResmp::lengthMillis()
{
    return lfsReader.lengthMillis();
}