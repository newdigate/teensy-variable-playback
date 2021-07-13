//
// Created by Nicholas Newdigate on 18/07/2020.
//

#include "playsdrawresmp.h"
#include "spi_interrupt.h"

void AudioPlaySdRawResmp::begin()
{
    playing = false;
    file_offset = 0;
    file_size = 0;
    sdReader.begin();
}

bool AudioPlaySdRawResmp::play(const char *filename)
{
    stop();
    playing = sdReader.play(filename);
    return playing;
}

void AudioPlaySdRawResmp::stop()
{
    sdReader.stop();
}

void AudioPlaySdRawResmp::update()
{
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
        n = sdReader.read((void**)data, AUDIO_BLOCK_SAMPLES * 2);
        file_offset += n;
        for (int channel=0; channel < _numChannels; channel++) {
            for (i=n/2; i < AUDIO_BLOCK_SAMPLES; i++) {
                blocks[channel]->data[i] = 0;
            }
            transmit(blocks[channel], channel);
        }
    } else {
        sdReader.close();
        playing = false;
    }
    for (int channel=0; channel < _numChannels; channel++) {
        release(blocks[channel]);
    }
}

#define B2M (uint32_t)((double)4294967296000.0 / AUDIO_SAMPLE_RATE_EXACT / 2.0) // 97352592

uint32_t AudioPlaySdRawResmp::positionMillis()
{
    return ((uint64_t)file_offset * B2M) >> 32;
}

uint32_t AudioPlaySdRawResmp::lengthMillis()
{
    return ((uint64_t)file_size * B2M) >> 32;
}