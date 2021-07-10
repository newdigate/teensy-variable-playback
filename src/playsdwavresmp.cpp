//
// Created by Nicholas Newdigate on 18/07/2020.
//

#include "playsdwavresmp.h"
#include "spi_interrupt.h"

void AudioPlaySdWavResmp::begin()
{
    file_offset = 0;
    file_size = 0;
    sdReader.begin();
}

bool AudioPlaySdWavResmp::play(char *filename)
{
    long startMicros = micros();
    stop();
    if (strcmp(_filename,filename) != 0) {
        _filename = new char[strlen(filename)];
        strcpy( _filename, filename);

        WaveHeaderParser waveHeaderParser;
        bool parseWavHeader = waveHeaderParser.readWaveHeader(filename, wave_header);
        if (parseWavHeader) {
            Serial.printf("parsed header...%s\n", _filename);
            sdReader.setHeaderSize(44);
        } else {
            Serial.printf("failed to parse header...%s\n", _filename);
            return false;
        }
        if (wave_header.bit_depth != 16) {
            Serial.printf("needs 16 bit/sample, got %d bits/sample\n", wave_header.bit_depth);
            return false;
        }

        if (wave_header.audio_format != 1) {
            Serial.printf("needs signed 16 bit audio format (1), got %d\n", wave_header.audio_format);
            return false;
        }

        setNumChannels(wave_header.num_channels);

        bool playing = sdReader.play(filename);
        long stopMicros = micros();
        Serial.printf("play(%d mS)\n", stopMicros - startMicros);
        return playing;
    } else {
        return sdReader.play();
    }
}

void AudioPlaySdWavResmp::stop()
{
    if (sdReader.isPlaying())
        sdReader.stop();
}

void AudioPlaySdWavResmp::update()
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

uint32_t AudioPlaySdWavResmp::positionMillis()
{
    return ((uint64_t)file_offset * B2M) >> 32;
}

uint32_t AudioPlaySdWavResmp::lengthMillis()
{
    return ((uint64_t)file_size * B2M) >> 32;
}