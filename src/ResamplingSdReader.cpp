#include "ResamplingSdReader.h"
#include "interpolation.h"

unsigned int ResamplingSdReader::read(void *buf, uint16_t nbyte) {
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
                        _file_offset = _header_size + (_loop_start * 2);
                    else
                        _file_offset = _header_size + (_loop_finish * 2) - 512;

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

bool ResamplingSdReader::readNextValue(int16_t *value) {

    if (_bufferLength == 0)
        if (!updateBuffers()) 
            return false;

    if (_playbackRate > 0 ) {
        //forward playback
        if (_bufferPosition >= _bufferLength) {

            if (_last_read_offset + _bufferPosition >= _header_size + (_loop_finish * 2))
                return false;

            if (!updateBuffers()) 
                return false;
        }
    } else if (_playbackRate < 0) {
        // reverse playback    
        if (_last_read_offset < _header_size)
        {  
            if (_bufferPosition < 0)
                return false;
        }
        if (_bufferPosition < 0) {
            if (!updateBuffers()) 
                return false;
        }
    }

    int samplePosition = _bufferPosition/2;
    double result = _buffer[samplePosition];
    //Serial.printf("r: %d,", result);

    if (_enable_interpolation) {
        double pos =  _remainder + samplePosition;
        if (_remainder > 0.01f) {
            if (_numInterpolationPoints < 4) {
                if (_numInterpolationPoints > 0) {
                    double lastX = _interpolationPoints[3].x - samplePosition;
                    if (lastX >= 0) {
                        double total = 1.0 - ((_remainder - lastX) / (1.0 - lastX));
                        double linearInterpolation =
                                (_interpolationPoints[3].y * (total)) + (_buffer[samplePosition + 1] * (1 - total));
                        result = linearInterpolation;
                    } else {
                        result = _buffer[samplePosition];
                    }

                }
            } else {
                double interpolation = interpolate(_interpolationPoints, pos, 4);
                result = interpolation;
            }
        } else {
            //Serial.printf("[%i, %f]\n", samplePosition, result);
            _interpolationPoints[0] = _interpolationPoints[1];
            _interpolationPoints[1] = _interpolationPoints[2];
            _interpolationPoints[2] = _interpolationPoints[3];
            _interpolationPoints[3].y = result;
            _interpolationPoints[3].x = pos;
            if (_numInterpolationPoints < 4)
                _numInterpolationPoints++;
        }

    }

    _remainder += _playbackRate;
    auto delta = static_cast<signed int>(_remainder);
    _remainder -= static_cast<double>(delta);
    _bufferPosition += 2 * delta;

    *value = round(result);
    return true;
}

void ResamplingSdReader::begin(void)
{
    _playing = false;
    _file_offset = _header_size;
    _file_size = 0;
}

bool ResamplingSdReader::play(const char *filename)
{
    stop();
    if (strcmp(_filename,filename) != 0) {
        if (_file) {
            Serial.printf("closing %s", _filename);
            __disable_irq();
            _file.close();
            __enable_irq();
        }
        _filename = new char[strlen(filename)];
        strncpy(_filename, filename, strlen(filename));
        StartUsingSPI();

        __disable_irq();
        _file = SD.open(filename);
        __enable_irq();

        if (!_file) {
            StopUsingSPI();
            Serial.printf("Not able to open file: %s\n", filename);
            _filename = "";
            return false;
        }

        __disable_irq();
        _file_size = _file.size();
        __enable_irq();
        _loop_start = 0;
        _loop_finish = (_file_size - _header_size) / 2;
        if (_file_size <= _header_size) {
            _playing = false;
            _filename =  "";
            Serial.printf("Wave file contains no samples: %s\n", filename);
            return false;
        }
    }

    reset();
    //updateBuffers();
    _playing = true;
    return true;
}

bool ResamplingSdReader::play()
{
    stop();
    reset();
    //updateBuffers();
    _playing = true;
    return true;
}

void ResamplingSdReader::reset(){
    _bufferLength = 0;
    if (_playbackRate > 0.0) {
        // forward playabck - set _file_offset to first audio block in file
        _file_offset = _header_size;
    } else {
        // reverse playback - forward _file_offset to last audio block in file
        if (_file_size > _header_size + (RESAMPLE_BUFFER_SAMPLE_SIZE * 2))
            _file_offset = _file_size - (RESAMPLE_BUFFER_SAMPLE_SIZE * 2);
        else
            _file_offset = _header_size;
    }
    __disable_irq();
    _file.seek(_file_offset);
    __enable_irq();
    //_playing = true;
}

void ResamplingSdReader::stop()
{
    if (_playing) {
        __disable_irq();
        _playing = false;
        //_file.close();
        __enable_irq();
        //StopUsingSPI();
    }
}

int ResamplingSdReader::available(void) {
    return _playing;
}

void ResamplingSdReader::close(void) {
    if (_playing)
        stop();
    //_file.close();
}

bool ResamplingSdReader::updateBuffers() {
    //printf("begin: file_offset: %d\n", _file_offset);
    if (!_file) return false;

    bool forward = (_playbackRate >= 0.0);

    uint16_t numberOfBytesToRead = RESAMPLE_BUFFER_SAMPLE_SIZE * 2;
    if (!forward) {
        if (_file_offset < _header_size) {
            // reverse playback, last buffer, only read partial remaining buffer that hasn't already played
            numberOfBytesToRead = _file_offset - _header_size + RESAMPLE_BUFFER_SAMPLE_SIZE * 2;
            //__disable_irq();
            _file.seek(_header_size);
            //__enable_irq();
            if (numberOfBytesToRead == 0)
                return false;

        } else {
            //__disable_irq();
            _file.seek(_file_offset);
            //__enable_irq();
        }
        _bufferPosition = numberOfBytesToRead - 2;
    } else 
    {
        int final_file_offset = _header_size + (_loop_finish * 2) - (RESAMPLE_BUFFER_SAMPLE_SIZE * 2);
        if (_file_offset > final_file_offset) {
            numberOfBytesToRead = _header_size + (_loop_finish * 2) - _file_offset;
        }
        _bufferPosition = 0;
    }
    if (numberOfBytesToRead == 0) return false;

    //Serial.printf("\nreading %d bytes, starting at:%d (into readbuff %d) - _file_offset:%d\n", numberOfBytesToRead, _file.position(), _readBuffer, _file_offset);
    _last_read_offset = _file_offset;
    //__disable_irq();
    int numRead = _file.read(_buffer, numberOfBytesToRead);
    if (numRead == 0) return false;
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

