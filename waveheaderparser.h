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
#include "spi_interrupt.h"
#else
#include <cstdint>
#include "teensy/SD/SD.h"
#include "teensy/spi_interrupt.h"
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
        File wavFile = SD.open(filename);
        bool result = readWaveHeader(filename, header, wavFile);
        wavFile.close();
        return result;
    }

    bool readWaveHeader(const char *filename, wav_header &header, File &wavFile) {
        char buffer[5] = {0, 0, 0, 0, 0};
        int bytesRead = wavFile.read(buffer, 4);
        if (bytesRead != 4) return false;
        if (buffer[0] != 'R' || buffer[1] != 'I' || buffer[2] != 'F' || buffer[3] != 'F') {
            Serial.printf("expected RIFF (was %s)\n", buffer);
            return false;
        }
        for (int i=0; i < 4; i++)
            header.riff_header[i] = buffer[i];

        bytesRead = wavFile.read(buffer, 4);
        if (bytesRead != 4) return false;
        auto header_chunk_size = static_cast<unsigned long>(buffer[3] << 24 | buffer[2] << 16 | buffer[1] << 8 | buffer[0]);
        header.header_chunk_size = header_chunk_size;

        bytesRead = wavFile.read(buffer, 4);
        if (bytesRead != 4) return false;
        for (int i=0; i < 4; i++)
            header.wave_header[i] = buffer[i];
        if (buffer[0] != 'W' || buffer[1] != 'A' || buffer[2] != 'V' || buffer[3] != 'E') {
            Serial.printf("expected WAVE (was %d)\n", buffer);
            return false;
        }


        bytesRead = wavFile.read(buffer, 4);
        for (int i=0; i < 4; i++)
            header.fmt_header[i] = buffer[i];
        if (bytesRead != 4) return false;
        if (buffer[0] != 'f' || buffer[1] != 'm' || buffer[2] != 't' || buffer[3] != ' ') {
            Serial.printf("expected 'fmt ' (was %d)\n", buffer);
            return false;
        }

        bytesRead = wavFile.read(buffer, 4);
        if (bytesRead != 4) return false;
        auto fmt_chunk_size = static_cast<unsigned long>(buffer[3] << 24 | buffer[2] << 16 | buffer[1] << 8 | buffer[0]);
        header.fmt_chunk_size = fmt_chunk_size;
        if (fmt_chunk_size != 16) {
            Serial.printf("chunk size should be 16 for PCM wave data... (was %d)\n", fmt_chunk_size);
            return false;
        }

        bytesRead = wavFile.read(buffer, 2);
        if (bytesRead != 2) return false;
        auto audio_format = static_cast<unsigned long>((buffer[1] << 8) | buffer[0]);
        header.audio_format = audio_format;

        bytesRead = wavFile.read(buffer, 2);
        if (bytesRead != 2) return false;
        auto num_channels = static_cast<unsigned long>((buffer[1] << 8) | buffer[0]);
        header.num_channels = num_channels;

        bytesRead = wavFile.read(buffer, 4);
        if (bytesRead != 4) return false;
        uint32_t sample_rate = static_cast<uint32_t>((unsigned char)buffer[3] << 24 | (unsigned char)buffer[2] << 16 | (unsigned char)buffer[1] << 8 | (unsigned char)buffer[0]);
        header.sample_rate = sample_rate;

        bytesRead = wavFile.read(buffer, 4);
        if (bytesRead != 4) return false;
        uint32_t byte_rate = static_cast<uint32_t>((unsigned char)buffer[3] << 24 | (unsigned char)buffer[2] << 16 | (unsigned char)buffer[1] << 8 | (unsigned char)buffer[0]);
        header.byte_rate = byte_rate;

        bytesRead = wavFile.read(buffer, 2);
        if (bytesRead != 2) return false;
        auto sample_alignment = static_cast<unsigned long>((buffer[1] << 8) | buffer[0]);
        header.sample_alignment = sample_alignment;

        bytesRead = wavFile.read(buffer, 2);
        if (bytesRead != 2) return false;
        auto bit_depth = static_cast<unsigned long>(buffer[1] << 8 | buffer[0]);
        header.bit_depth = bit_depth;

        bytesRead = wavFile.read(buffer, 4);
        if (bytesRead != 4) return false;
        for (int i=0; i < 4; i++)
            header.data_header[i] = buffer[i];
        if (buffer[0] != 'd' || buffer[1] != 'a' || buffer[2] != 't' || buffer[3] != 'a') {
            Serial.printf("expected data... (was %d)\n", buffer);
            return false;
        }

        bytesRead = wavFile.read(buffer, 4);
        if (bytesRead != 4) return false;
        auto data_bytes = static_cast<unsigned long>(buffer[3] << 24 | buffer[2] << 16 | buffer[1] << 8 | buffer[0]);
        header.data_bytes = data_bytes;
        return true;
    }
private:
};


#endif //TEENSY_RESAMPLING_SDREADER_WAVEHEADERPARSER_H
