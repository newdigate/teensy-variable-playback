//
// Created by Nicholas Newdigate on 18/07/2020.
//

#include "playarrayresmp.h"

void AudioPlayArrayResmp::begin()
{
    playing = false;
    file_offset = 0;
    file_size = 0;
}

bool AudioPlayArrayResmp::play(int16_t *data, uint32_t numSamples)
{
    stop();
    playing = arrayReader.play(data, numSamples);
    return playing;
}

void AudioPlayArrayResmp::stop()
{
    arrayReader.stop();
}

void AudioPlayArrayResmp::update()
{
    unsigned int i, n;
    audio_block_t *block;

    // only update if we're playing
    if (!playing) return;

    // allocate the audio blocks to transmit
    block = allocate();
    if (block == NULL) return;

    if (arrayReader.available()) {
        // we can read more data from the file...
        n = arrayReader.read(block->data, AUDIO_BLOCK_SAMPLES);
        file_offset += n;
        for (i=n; i < AUDIO_BLOCK_SAMPLES; i++) {
            block->data[i] = 0;
        }
        transmit(block);
    } else {
        arrayReader.close();
        playing = false;
    }
    release(block);
}

#define B2M (uint32_t)((double)4294967296000.0 / AUDIO_SAMPLE_RATE_EXACT / 2.0) // 97352592

uint32_t AudioPlayArrayResmp::positionMillis()
{
    return ((uint64_t)file_offset * B2M) >> 32;
}

uint32_t AudioPlayArrayResmp::lengthMillis()
{
    return ((uint64_t)file_size * B2M) >> 32;
}