#include "ResamplingSdReader.h"

int ResamplingSdReader::read(void *buf, uint16_t nbyte) {
    if (!_playing) return 0;

    unsigned int count = 0;
    int16_t *index = (int16_t*)buf;
    while (count < nbyte) {

        if (readNextValue(index)){
            count+=2;
            index++;
        }
        else {
            // we have reached the end of the file

            switch (_loopType) {
                case looptype_repeat:
                {
                    if (_playbackRate >= 0.0) 
                        _file_offset = 0;
                    else
                        _file_offset = _file_size - 512;

                    _bufferLength = 0;
                    _file.seek(_file_offset);
                    break;
                }

                case looptype_pingpong:
                {
                    if (_playbackRate >= 0.0) {
                        _file_offset = _file_size - 512;
                        //printf("switching to reverse playback...\n");
                    }
                    else {
                        _file_offset = 0;
                        _bufferPosition = 0;
                        _file.seek(0);
                        //printf("switching to forward playback...\n");
                    }
                    _playbackRate = -_playbackRate;
                    _bufferLength = 0;
                    break;
                }            

                case looptype_none:            
                default:
                {   
                    /* no looping - return the number of (resampled) bytes returned... */
                    return count;
                    break;
                }
            }   
        }
    }
    return count;
}

bool ResamplingSdReader::readNextValue(int16_t *value) {

    if (_bufferLength == 0)
        if (!updateBuffers()) 
            return false;

    if (_playbackRate > 0 ) {
        //forward playback
        if (_bufferPosition >= _bufferLength) {

            if (_last_read_offset + _bufferPosition >= _file_size)
                return false;

            if (!updateBuffers()) 
                return false;
        }
    } else if (_playbackRate < 0) {
        // reverse playback    
        if (_last_read_offset < 0)
        {  
            if (_bufferPosition < 0)
                return false;
        }
        if (_bufferPosition < 0) {
            if (!updateBuffers()) 
                return false;
        }
    }

    int16_t result = _buffer[_bufferPosition/2];
    //Serial.printf("r: %d,", result);

    _remainder += _playbackRate;

    auto delta = static_cast<signed int>(_remainder);
    _remainder -= static_cast<double>(delta);

    _bufferPosition += 2 * delta;
    *value = result;
    return true;
}

void ResamplingSdReader::begin(void)
{
    _playing = false;
    _file_offset = 0;
    _file_size = 0;
}

bool ResamplingSdReader::play(const char *filename)
{
    stop();

    StartUsingSPI();

    __disable_irq();
    _file = SD.open(filename);
    __enable_irq();

    if (!_file) {
        StopUsingSPI();
        return false;
    }

    __disable_irq();
    _file_size = _file.size();
    __enable_irq();

    if (_playbackRate < 0) {
        // reverse playback - forward _file_offset to last audio block in file
        _file_offset = _file_size - AUDIO_BLOCK_SAMPLES * 2;
    } else {
        // forward playabck - set _file_offset to first audio block in file
        _file_offset = 0;
    }

    _playing = true;
    return true;
}

void ResamplingSdReader::stop()
{
    __disable_irq();
    if (_playing) {
        _playing = false;
        __enable_irq();
        _file.close();
        StopUsingSPI();
    } else {
        __enable_irq();
    }
}

int ResamplingSdReader::available(void) {
    return _file.available() / _playbackRate;
}

void ResamplingSdReader::close(void) {
    if (_playing)
        stop();
    _file.close();
}

bool ResamplingSdReader::updateBuffers() {
    //printf("begin: file_offset: %d\n", _file_offset);

    bool forward = (_playbackRate >= 0.0);

    uint16_t numberOfBytesToRead = AUDIO_BLOCK_SAMPLES * 2;
    if (!forward) {
        if (_file_offset < 0) {
            // reverse playback, last buffer, only read partial remaining buffer that hasn't already played
            numberOfBytesToRead = _file_offset + AUDIO_BLOCK_SAMPLES * 2;
            _file.seek(0);
        
            if (numberOfBytesToRead == 0)
                return false;

        } else 
            _file.seek(_file_offset);

        _bufferPosition = numberOfBytesToRead - 2;
    } else 
    {
        if (_file_offset > _file_size - (AUDIO_BLOCK_SAMPLES * 2)) {
            numberOfBytesToRead = _file_size - _file_offset;
        }
        _bufferPosition = 0;
    }
    //Serial.printf("\nreading %d bytes, starting at:%d (into readbuff %d) - _file_offset:%d\n", numberOfBytesToRead, _file.position(), _readBuffer, _file_offset);
    _last_read_offset = _file_offset;
    int numRead = _file.read(_buffer, numberOfBytesToRead);
    _bufferLength = numRead;
    //Serial.printf("read %d bytes\n", numRead);

    if (_playbackRate < 0) {
        _file_offset -= numRead;     
    } else
        _file_offset += numRead;

    //printf("exit: file_offset: %d\n", _file_offset);
    return true;
}