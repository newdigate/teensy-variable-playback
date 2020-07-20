//
// Created by Nicholas Newdigate on 20/07/2020.
//

#ifndef TEENSY_RESAMPLING_SDREADER_WAVEHEADERPARSER_H
#define TEENSY_RESAMPLING_SDREADER_WAVEHEADERPARSER_H

#include <string>
#ifdef ARDUINO
#include "stdint.h"
#include <SD.h>
#include <AudioStream.h>
#include "../spi_interrupt.h"
#else
#include <cstdint>
#include "SD/SD.h"
#include "spi_interrupt.h"
#define AUDIO_BLOCK_SAMPLES 256
#endif

using namespace std;

struct wav_header {
    // RIFF Header
    char riff_header[4]; // Contains "RIFF"
    int wav_size; // Size of the wav portion of the file, which follows the first 8 bytes. File size - 8
    char wave_header[4]; // Contains "WAVE"

    // Format Header
    char fmt_header[4]; // Contains "fmt " (includes trailing space)
    int fmt_chunk_size; // Should be 16 for PCM
    short audio_format; // Should be 1 for PCM. 3 for IEEE Float
    short num_channels;
    int sample_rate;
    int byte_rate; // Number of bytes per second. sample_rate * num_channels * Bytes Per Sample
    short sample_alignment; // num_channels * Bytes Per Sample
    short bit_depth; // Number of bits per sample

    // Data
    char data_header[4]; // Contains "data"
    int data_bytes; // Number of bytes in data. Number of samples * num_channels * sample byte size
    // uint8_t bytes[]; // Remainder of wave file is bytes
};

class WaveHeaderParser {
public:
    bool readWaveHeader(const char *filename, wav_header &header) {
        File wavFile = SD.open(filename);
        char buffer[4] = {0, 0, 0, 0};
        int bytesRead = wavFile.read(buffer, 4);
        if (bytesRead != 4) return false;
        if (buffer[0] != 'R' || buffer[1] != 'I' || buffer[2] != 'F' || buffer[3] != 'F')
            return false;

        bytesRead = wavFile.read(buffer, 4);
        if (bytesRead != 4) return false;
        auto chuckSize = static_cast<unsigned long>(buffer[3] << 24 | buffer[2] << 16 | buffer[1] << 8 | buffer[0]);

        bytesRead = wavFile.read(buffer, 4);
        if (bytesRead != 4) return false;
        if (buffer[0] != 'W' || buffer[1] != 'A' || buffer[2] != 'V' || buffer[3] != 'E')
            return false;

        bytesRead = wavFile.read(buffer, 4);
        return true;
    }
private:

};


#endif //TEENSY_RESAMPLING_SDREADER_WAVEHEADERPARSER_H
