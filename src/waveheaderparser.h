//
// Created by Nicholas Newdigate on 20/07/2020.
//
#ifndef TEENSY_RESAMPLING_SDREADER_WAVEHEADERPARSER_H
#define TEENSY_RESAMPLING_SDREADER_WAVEHEADERPARSER_H

#include <string>
#include <cstdint>
#include <SD.h>
#include "spi_interrupt.h"

// from https://gist.github.com/Jon-Schneider/8b7c53d27a7a13346a643dac9c19d34f
struct wav_header {
    // RIFF Header
    char riff_header[4] = {0,0,0,0};    // 00 - 03 - Contains "RIFF"
    int header_chunk_size = 0;  // 04 - 07 - Size of the wav portion of the file, which follows the first 8 bytes. File size - 8
    char wave_header[4] = {0,0,0,0};    // 08 - 11 - Contains "WAVE"

    // Format Header
    char fmt_header[4] = {0,0,0,0};     // 12 - 15 - Contains "fmt " (includes trailing space)
    int fmt_chunk_size = 0;     // 16 - 19 - Should be 16 for PCM
    short audio_format = 0;     // 20 - 21 - Should be 1 for PCM. 3 for IEEE Float
    short num_channels = 0;     // 22 - 23
    int sample_rate = 0;        // 24 - 27
    int byte_rate = 0;          // 28 - 31
    short sample_alignment = 0; // 32 - 33
    short bit_depth  = 0;        // 34 - 35
};

struct wav_data_header {
    // Data
    char data_header[4] = {0,0,0,0};    // 36 - 39
    unsigned int data_bytes = 0;// 40 - 43
};

class WaveHeaderParser {
public:
    bool readWaveHeader(const char *filename, wav_header &header, wav_data_header &wav_data_header) {
        File wavFile = SD.open(filename);
        if (!wavFile) {
            Serial.printf("Not able to open wave file... %s\n", filename);
            return false;
        }
        bool result = readWaveHeader(filename, header, wavFile);
        if (result) {
            wavFile.seek(36);
            unsigned char buffer[8];
            size_t bytesRead = wavFile.read(buffer, 8);
            if (bytesRead != 8) {
                Serial.printf("Not able to read header... %s\n", filename);
                result = false;
            }

            if (result) {
                unsigned infoTagsSize;
                result = readInfoTags(buffer, 0, infoTagsSize);

                if (result) {
                    wavFile.seek(36 + infoTagsSize);
                    bytesRead = wavFile.read(buffer, 8);
                    if (bytesRead != 8) {
                        Serial.printf("Not able to read header... %s\n", filename);
                        return false;
                    }

                    result = readDataHeader(buffer, 0, wav_data_header);
                }
            }
        }
        wavFile.close();
        return result;
    }

    bool readWaveHeader(const char *filename, wav_header &header, File &wavFile) {
        char buffer[36];
        int bytesRead = wavFile.read(buffer, 36);
        if (bytesRead != 36) {
            Serial.printf("expected 36 bytes (was %d)\n", bytesRead);
            return false;
        }
        return readWaveHeaderFromBuffer(buffer, header);
    }

    bool readWaveHeaderFromBuffer(const char *buffer, wav_header &header) {
        if (buffer[0] != 'R' || buffer[1] != 'I' || buffer[2] != 'F' || buffer[3] != 'F') {
            Serial.printf("expected RIFF (was %s)\n", buffer);
            return false;
        }
        for (int i=0; i < 4; i++)
            header.riff_header[i] = buffer[i];

        unsigned char *b = (unsigned char*)buffer;

        auto header_chunk_size = static_cast<unsigned long>(b[7] << 24 | b[6] << 16 | b[5] << 8 | b[4]);
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

        auto fmt_chunk_size = static_cast<unsigned long>(b[19] << 24 | b[18] << 16 | b[17] << 8 | b[16]);
        header.fmt_chunk_size = fmt_chunk_size;
        if (fmt_chunk_size != 16) {
            Serial.printf("chunk size should be 16 for PCM wave data... (was %d)\n", fmt_chunk_size);
            return false;
        }

        auto audio_format = static_cast<unsigned long>((b[21] << 8) | b[20]);
        header.audio_format = audio_format;

        auto num_channels = static_cast<unsigned long>((b[23] << 8) | b[22]);
        header.num_channels = num_channels;

        uint32_t sample_rate = static_cast<uint32_t>(b[27] << 24 | b[26] << 16 | b[25] << 8 | b[24]);
        header.sample_rate = sample_rate;

        uint32_t byte_rate = static_cast<uint32_t>(b[31] << 24 | b[30] << 16 | b[29] << 8 | b[28]);
        header.byte_rate = byte_rate;

        auto sample_alignment = static_cast<unsigned long>((b[33] << 8) | b[32]);
        header.sample_alignment = sample_alignment;

        auto bit_depth = static_cast<unsigned long>(b[35] << 8 | b[34]);
        header.bit_depth = bit_depth;

        return true;
    }

    bool readInfoTags(unsigned char *buffer, size_t offset, unsigned &infoTagsSize) {
        if (    buffer[offset+0] == 'L' 
             && buffer[offset+1] == 'I' 
             && buffer[offset+2] == 'S' 
             && buffer[offset+3] == 'T') {
            infoTagsSize = static_cast<uint32_t>(buffer[offset+7] << 24 | buffer[offset+6] << 16 | buffer[offset+5] << 8 | buffer[offset+4]);    
            infoTagsSize += 8;
            return true;
        }

        if (    buffer[offset+0] == 'd' 
             && buffer[offset+1] == 'a' 
             && buffer[offset+2] == 't' 
             && buffer[offset+3] == 'a') {
            infoTagsSize = 0;
            return true;
        }

        Serial.println("expected 'data' or 'LIST'...");
        return false;
    }

    bool readDataHeader(unsigned char *buffer, size_t offset, wav_data_header &data_header) {

      for (int i=0; i < 4; i++)
            data_header.data_header[i] = buffer[i+offset];

        if (buffer[offset+0] != 'd' || buffer[offset+1] != 'a' || buffer[offset+2] != 't' || buffer[offset+3] != 'a') {
            Serial.printf("expected data... (was %d)\n", buffer);
            return false;
        }

        auto data_bytes = static_cast<unsigned long>(buffer[offset+7] << 24 | buffer[offset+6] << 16 | buffer[offset+5] << 8 | buffer[offset+4]);
        data_header.data_bytes = data_bytes;
        return true;
    }
private:
};


#endif //TEENSY_RESAMPLING_SDREADER_WAVEHEADERPARSER_H
