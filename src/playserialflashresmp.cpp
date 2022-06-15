//
// Created by Nicholas Newdigate on 18/07/2020.
//

#include "playserialflashresmp.h"
#include "waveheaderparser.h"

void AudioPlaySerialFlashResmp::begin()
{
    file_size = 0;
    serialFlashReader.begin();
}

bool AudioPlaySerialFlashResmp::playRaw(const char *filename, uint16_t numChannels)
{
    stop();
    bool playing = serialFlashReader.playRaw(filename, numChannels);
    return playing;
}

bool AudioPlaySerialFlashResmp::playWav(const char *filename)
{
    stop();
    bool playing = serialFlashReader.playWav(filename);
    return playing;
}

void AudioPlaySerialFlashResmp::stop()
{
    serialFlashReader.stop();
}

void AudioPlaySerialFlashResmp::update()
{
    int _numChannels = serialFlashReader.getNumChannels();
    if (_numChannels == -1)
        return;

    unsigned int i, n;
    audio_block_t *blocks[_numChannels];
    int16_t *data[_numChannels];
    // only update if we're playing
    if (!serialFlashReader.isPlaying()) return;

    // allocate the audio blocks to transmit
    for (int i=0; i < _numChannels; i++) {
        blocks[i] = allocate();
        if (blocks[i] == nullptr) return;
        data[i] = blocks[i]->data;
    }

    if (serialFlashReader.available()) {
        // we can read more data from the file...
        n = serialFlashReader.read((void**)data, AUDIO_BLOCK_SAMPLES);
        for (int channel=0; channel < _numChannels; channel++) {
            memset( &blocks[channel]->data[n], 0, (AUDIO_BLOCK_SAMPLES - n) * 2);
            transmit(blocks[channel], channel);
        }

        if(_numChannels == 1) {
            transmit(blocks[0], 1);
        }
    } else {
        serialFlashReader.close();
    }
    for (int channel=0; channel < _numChannels; channel++) {
        release(blocks[channel]);
    }
}

uint32_t AudioPlaySerialFlashResmp::positionMillis()
{
    return serialFlashReader.positionMillis();
}

uint32_t AudioPlaySerialFlashResmp::lengthMillis()
{
    return serialFlashReader.lengthMillis();
}