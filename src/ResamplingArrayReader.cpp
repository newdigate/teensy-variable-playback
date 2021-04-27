#include "ResamplingArrayReader.h"

unsigned int ResamplingArrayReader::read(void *buf, uint16_t nbyte) {
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
                        _file_offset = 0 + (_loop_start);
                    else
                        _file_offset = 0 + (_loop_finish) - 512;

                    _bufferLength = 0;
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
                        //printf("switching to forward playback...\n");
                    }
                    _playbackRate = -_playbackRate;
                    _bufferLength = 0;
                    break;
                }            

                case looptype_none:            
                default:
                {
                    //Serial.printf("end of loop...\n");
                    /* no looping - return the number of (resampled) bytes returned... */
                    _playing = false;
                    return count;
                }
            }   
        }
    }
    return count;
}

bool ResamplingArrayReader::readNextValue(int16_t *value) {

    if (_bufferLength == 0)
        if (!updateBuffers()) 
            return false;

    if (_playbackRate > 0 ) {
        //forward playback
        if (_bufferPosition >= _bufferLength) {

            if (_last_read_offset + _bufferPosition >=  _loop_finish )
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

    int16_t result = _sourceBuffer[_bufferPosition/2];
    //Serial.printf("r: %d,", result);

    _remainder += _playbackRate;

    auto delta = static_cast<signed int>(_remainder);
    _remainder -= static_cast<double>(delta);

    _bufferPosition += 2 * delta;
    *value = result;
    return true;
}

void ResamplingArrayReader::begin(void)
{
    _playing = false;
    _file_offset = 0;
    _file_size = 0;
}

bool ResamplingArrayReader::play(int16_t *array, uint32_t length)
{
    _sourceBuffer = array;
    stop();

    _file_size = length;
    _loop_start = 0;
    _loop_finish = _file_size ;

    reset();
    //updateBuffers();
    _playing = true;
    return true;
}

bool ResamplingArrayReader::play()
{
    stop();
    reset();
    _playing = true;
    return true;
}

void ResamplingArrayReader::reset(){
    _bufferLength = 0;
    if (_playbackRate > 0.0) {
        // forward playabck - set _file_offset to first audio block in file
        _file_offset = 0;
    } else {
        // reverse playback - forward _file_offset to last audio block in file
        if (_file_size > AUDIO_BLOCK_SAMPLES)
            _file_offset = _file_size - AUDIO_BLOCK_SAMPLES;
        else
            _file_offset = 0;
    }
}

void ResamplingArrayReader::stop()
{
    if (_playing) {
        _playing = false;
    }
}

int ResamplingArrayReader::available(void) {
    return _playing;
}

void ResamplingArrayReader::close(void) {
    if (_playing)
        stop();
}

bool ResamplingArrayReader::updateBuffers() {
    //printf("begin: file_offset: %d\n", _file_offset);

    bool forward = (_playbackRate >= 0.0);

    uint16_t numberOfBytesToRead = AUDIO_BLOCK_SAMPLES;
    if (!forward) {
        if (_file_offset < 0) {
            // reverse playback, last buffer, only read partial remaining buffer that hasn't already played
            numberOfBytesToRead = _file_offset + AUDIO_BLOCK_SAMPLES;
            if (numberOfBytesToRead == 0)
                return false;

        }
        _bufferPosition = numberOfBytesToRead - 2;
    } else 
    {
        int final_file_offset = _loop_finish - AUDIO_BLOCK_SAMPLES;
        if (_file_offset > final_file_offset) {
            numberOfBytesToRead = _loop_finish  - _file_offset;
        }
        _bufferPosition = 0;
    }
    if (numberOfBytesToRead == 0) return false;

    //Serial.printf("\nreading %d bytes, starting at:%d (into readbuff %d) - _file_offset:%d\n", numberOfBytesToRead, _file.position(), _readBuffer, _file_offset);
    _last_read_offset = _file_offset;
    //__disable_irq();
    if (numberOfBytesToRead +_file_offset > _file_size) {
        numberOfBytesToRead = _file_size - (numberOfBytesToRead +_file_offset );
    }
    if (numberOfBytesToRead == 0) return false;
    memcpy(_destinationBuffer, _sourceBuffer, numberOfBytesToRead);
    int numRead = numberOfBytesToRead;
    //__enable_irq();
    _bufferLength = numRead;
    //Serial.printf("read %d bytes\n", numRead);

    if (_playbackRate < 0) {
        _file_offset -= numRead;     
    } else
        _file_offset += numRead;

    //printf("exit: file_offset: %d\n", _file_offset);
    return true;
}
