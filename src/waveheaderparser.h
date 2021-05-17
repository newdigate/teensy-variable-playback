//
// Created by Nicholas Newdigate on 20/07/2020.
//
#ifndef TEENSY_RESAMPLING_SDREADER_WAVEHEADERPARSER_H
#define TEENSY_RESAMPLING_SDREADER_WAVEHEADERPARSER_H

#include <string>
#include <cstdint>
#include "spi_interrupt.h"
#ifndef ARDUINO
#define AUDIO_BLOCK_SAMPLES 256
#endif

using namespace std;

// from https://gist.github.com/Jon-Schneider/8b7c53d27a7a13346a643dac9c19d34f
struct wav_header {
    // RIFF Header
    char riff_header[4]; // Contains "RIFF"
    int header_chunk_size; // Size of the wav portion of the file, which follows the first 8 bytes. File size - 8
    char wave_header[4]; // Contains "WAVE"

    // Format Header
    char fmt_header[4]; // Contains "fmt " (includes trailing space)
    int fmt_chunk_size; // Should be 16 for PCM
    short audio_format; // Should be 1 for PCM. 3 for IEEE Float
    short num_channels;
    int sample_rate;
    int byte_rate;
    short sample_alignment;
    short bit_depth;

    // Data
    char data_header[4];
    int data_bytes;
};

class WaveHeaderParser {
public:
    bool readWaveHeader(const char *filename, wav_header &header) {
        __disable_irq();
        File wavFile = SD.open(filename);
        __enable_irq();
        bool result = readWaveHeader(filename, header, wavFile);
        wavFile.close();
        return result;
    }

    bool readWaveHeader(const char *filename, wav_header &header, File &wavFile) {
        char buffer[44];
        __disable_irq();
        int bytesRead = wavFile.read(buffer, 44);
        __enable_irq();
        if (bytesRead != 44) return false;
        if (buffer[0] != 'R' || buffer[1] != 'I' || buffer[2] != 'F' || buffer[3] != 'F') {
            Serial.printf("expected RIFF (was %s)\n", buffer);
            return false;
        }
        for (int i=0; i < 4; i++)
            header.riff_header[i] = buffer[i];

        auto header_chunk_size = static_cast<unsigned long>(buffer[7] << 24 | buffer[6] << 16 | buffer[5] << 8 | buffer[4]);
        header.header_chunk_size = header_chunk_size;

        for (int i=0; i < 4; i++)
            header.wave_header[i] = buffer[i+8];
        if (buffer[8] != 'W' || buffer[9] != 'A' || buffer[10] != 'V' || buffer[11] != 'E') {
            Serial.printf("expected WAVE (was %s)\n", buffer[8]);
            return false;
        }

        for (int i=0; i < 4; i++)
            header.fmt_header[i] = buffer[i+12];
        if (buffer[12] != 'f' || buffer[13] != 'm' || buffer[14] != 't' || buffer[15] != ' ') {
            Serial.printf("expected 'fmt ' (was %s)\n",  buffer[12]);
            return false;
        }

        auto fmt_chunk_size = static_cast<unsigned long>(buffer[19] << 24 | buffer[18] << 16 | buffer[17] << 8 | buffer[16]);
        header.fmt_chunk_size = fmt_chunk_size;
        if (fmt_chunk_size != 16) {
            Serial.printf("chunk size should be 16 for PCM wave data... (was %d)\n", fmt_chunk_size);
            return false;
        }

        auto audio_format = static_cast<unsigned long>((buffer[21] << 8) | buffer[20]);
        header.audio_format = audio_format;

        auto num_channels = static_cast<unsigned long>((buffer[23] << 8) | buffer[22]);
        header.num_channels = num_channels;

        uint32_t sample_rate = static_cast<uint32_t>((unsigned char)buffer[27] << 24 | (unsigned char)buffer[26] << 16 | (unsigned char)buffer[25] << 8 | (unsigned char)buffer[24]);
        header.sample_rate = sample_rate;

        uint32_t byte_rate = static_cast<uint32_t>((unsigned char)buffer[31] << 24 | (unsigned char)buffer[30] << 16 | (unsigned char)buffer[29] << 8 | (unsigned char)buffer[28]);
        header.byte_rate = byte_rate;

        auto sample_alignment = static_cast<unsigned long>((buffer[33] << 8) | buffer[32]);
        header.sample_alignment = sample_alignment;

        auto bit_depth = static_cast<unsigned long>(buffer[35] << 8 | buffer[34]);
        header.bit_depth = bit_depth;

        for (int i=0; i < 4; i++)
            header.data_header[i] = buffer[i+36];
        if (buffer[36] != 'd' || buffer[37] != 'a' || buffer[38] != 't' || buffer[39] != 'a') {
            Serial.printf("expected data... (was %d)\n", buffer);
            return false;
        }

        auto data_bytes = static_cast<unsigned long>(buffer[43] << 24 | buffer[42] << 16 | buffer[41] << 8 | buffer[40]);
        header.data_bytes = data_bytes;
        return true;
    }
private:
};


#endif //TEENSY_RESAMPLING_SDREADER_WAVEHEADERPARSER_H
