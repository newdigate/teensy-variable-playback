//
// Created by Nicholas Newdigate on 18/07/2020.
//

#include "playsdwavresmp.h"
#include "spi_interrupt.h"

void AudioPlaySdWavResmp::begin()
{
    file_offset = 0;
    file_size = 0;
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

        if (wave_header.num_channels != 1) {
            Serial.printf("needs single channel audio, got %d channels\n", wave_header.num_channels);
            return false;
        }

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
    audio_block_t *block;

    // only update if we're playing
    if (!sdReader.isPlaying()) return;

    // allocate the audio blocks to transmit
    block = allocate();
    if (block == NULL) return;

    // we can read more data from the file...
    n = sdReader.read(block->data, AUDIO_BLOCK_SAMPLES*2);
    file_offset += n;
    for (i=n/2; i < AUDIO_BLOCK_SAMPLES; i++) {
        block->data[i] = 0;
    }
    transmit(block);
    release(block);
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