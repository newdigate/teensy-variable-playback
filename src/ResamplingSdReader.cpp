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
                    _bufferPosition = 0;
                    _numBuffers = 0;
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

    if (_numBuffers == 0)
        if (!updateBuffers()) 
            return false;

    if (_playbackRate > 0 ) {
        //forward playback
        
        if (_numBuffers < 2 // only if we need another buffer
            && _file_offset < _file_size // don't try to load past the end of the file
            && _bufferPosition > _bufferLength/2) {
            // double buffered: we've reached halfway through the current buffer, load the next buffer 
            updateBuffers();
        }

        if (_bufferPosition >= _bufferLength) {
            if (_numBuffers <= 1 && _noMoreBuffersToRead && _nextBufferLength == 0)
                return false;
            if (_numBuffers <= 1 && _last_read_offset + _bufferPosition + _header_size >= _file_size)
                return false;

            _currentBuffer = (_currentBuffer + 1) % 2;
            _bufferLength = _nextBufferLength;
            _bufferPosition = _bufferPosition % RESAMPLE_BUFFER_SAMPLE_SIZE; 
            _numBuffers--;
        }
    } else if (_playbackRate < 0) {
        // reverse playback    
        if (_last_read_offset < _header_size && _numBuffers==1)
        {  
            if (_bufferPosition < 0)
                return false;
        }

        if (_numBuffers < 2 // only if we need another buffer
            && _file_offset < _file_size // don't try to load past the end of the file
            && _bufferPosition < _bufferLength/2) {
            // double buffered: we've reached halfway through the current buffer, load the next buffer 
            updateBuffers();
        }

        if (_bufferPosition < 0) {
            if (_numBuffers <= 1 && _last_read_offset + _bufferPosition + _header_size < 0)
                return false;

            _currentBuffer = (_currentBuffer + 1) % 2;
            _bufferLength = _nextBufferLength;
            _bufferPosition = (_nextBufferLength + _bufferPosition); 
            _numBuffers--;
        }
    }

    int samplePosition = (_bufferPosition/2) + (_currentBuffer * RESAMPLE_BUFFER_SAMPLE_SIZE);
    int16_t result = _buffer[samplePosition];
    //Serial.printf("r: %d,", result);

    if (_interpolationType == ResampleInterpolationType::resampleinterpolation_linear) {

        double abs_remainder = abs(_remainder);
        if (abs_remainder > 0.0) {

            if (_playbackRate > 0) {
                if (_remainder - _playbackRate < 0.0){
                    // we crossed over a whole number, make sure we update the samples for interpolation

                    if (_playbackRate > 1.0) {
                        // need to update last sample
                        _interpolationPoints[1].y = _buffer[samplePosition-1];
                    }

                    _interpolationPoints[0].y = _interpolationPoints[1].y;
                    _interpolationPoints[1].y = result;
                    if (_numInterpolationPoints < 2)
                        _numInterpolationPoints++;
                }
            } 
            else if (_playbackRate < 0) {
                if (_remainder - _playbackRate > 0.0){
                    // we crossed over a whole number, make sure we update the samples for interpolation

                    if (_playbackRate < -1.0) {
                        // need to update last sample
                        _interpolationPoints[1].y = _buffer[samplePosition+1];
                    }

                    _interpolationPoints[0].y = _interpolationPoints[1].y;
                    _interpolationPoints[1].y = result;
                    if (_numInterpolationPoints < 2)
                        _numInterpolationPoints++;
                }
            }

            if (_numInterpolationPoints > 1) {
                result = abs_remainder * _interpolationPoints[1].y + (1.0 - abs_remainder) * _interpolationPoints[0].y;
                //Serial.printf("[%f]\n", interpolation);
            }
        } else {
            _interpolationPoints[0].y = _interpolationPoints[1].y;
            _interpolationPoints[1].y = result;
            if (_numInterpolationPoints < 2)
                _numInterpolationPoints++;

            result =_interpolationPoints[0].y;
            //Serial.printf("%f\n", result);
        }
    } 
    else if (_interpolationType == ResampleInterpolationType::resampleinterpolation_quadratic) {
        double abs_remainder = abs(_remainder);
        if (abs_remainder > 0.0) {
            if (_playbackRate > 0) {                
                if (_remainder - _playbackRate < 0.0){
                    // we crossed over a whole number, make sure we update the samples for interpolation
                    int numberOfSamplesToUpdate = - floor(_remainder - _playbackRate);
                    if (numberOfSamplesToUpdate > 4) 
                        numberOfSamplesToUpdate = 4; // if playbackrate > 4, only need to pop last 4 samples
                    int currentBufferOffset = RESAMPLE_BUFFER_SAMPLE_SIZE * _currentBuffer;
                    for (int i=numberOfSamplesToUpdate; i > 0; i--) {
                        _interpolationPoints[0].y = _interpolationPoints[1].y;
                        _interpolationPoints[1].y = _interpolationPoints[2].y;
                        _interpolationPoints[2].y = _interpolationPoints[3].y;
                        long currBuffPos = samplePosition - i + 1 - currentBufferOffset;
                        if (currBuffPos < 0) {
                            if (_numBuffers > 0) {
                                int prevBufferOffset = RESAMPLE_BUFFER_SAMPLE_SIZE * ((_currentBuffer+1) % 2);
                                int prevSamplePos = RESAMPLE_BUFFER_SAMPLE_SIZE + currBuffPos;
                                _interpolationPoints[3].y =  _buffer[prevBufferOffset + prevSamplePos];
                                //Serial.printf("---[%i] %i: %i\n",  _interpolationPoints[3].y, prevBufferOffset + prevSamplePos, _bufferPosition);
                            }
                        } else 
                        if (currBuffPos >= (_bufferLength / 2) ) {
                            if (_numBuffers > 1) {
                                int nextBufferOffset = RESAMPLE_BUFFER_SAMPLE_SIZE * ((_currentBuffer+1) % 2);
                                int nextSamplePos = currBuffPos % (_bufferLength / 2);
                                _interpolationPoints[3].y =  _buffer[nextBufferOffset + nextSamplePos];
                                //Serial.printf("---[%i] %i: %i\n",  _interpolationPoints[3].y, nextBufferOffset + nextSamplePos, _bufferPosition);
                            }
                        } else {
                            _interpolationPoints[3].y =  _buffer[samplePosition-i+1];
                            //Serial.printf("---[%i] %i: %i\n",  _interpolationPoints[3].y, samplePosition-i+1, _bufferPosition);
                        }
                        if (_numInterpolationPoints < 4) _numInterpolationPoints++;
                    }
                }
            } 
            else if (_playbackRate < 0) {                
                if (_remainder - _playbackRate > 0.0){
                    // we crossed over a whole number, make sure we update the samples for interpolation
                    int numberOfSamplesToUpdate =  ceil(_remainder - _playbackRate);
                    if (numberOfSamplesToUpdate > 4) 
                        numberOfSamplesToUpdate = 4; // if playbackrate > 4, only need to pop last 4 samples
                    int currentBufferOffset = RESAMPLE_BUFFER_SAMPLE_SIZE * _currentBuffer;
                    for (int i=numberOfSamplesToUpdate; i > 0; i--) {
                        _interpolationPoints[0].y = _interpolationPoints[1].y;
                        _interpolationPoints[1].y = _interpolationPoints[2].y;
                        _interpolationPoints[2].y = _interpolationPoints[3].y;
                        long currBuffPos = samplePosition + i -1 - currentBufferOffset;
                        if (currBuffPos < 0) {
                            if (_numBuffers > 0) {
                                int prevBufferOffset = RESAMPLE_BUFFER_SAMPLE_SIZE * ((_currentBuffer+1) % 2);
                                int prevSamplePos = RESAMPLE_BUFFER_SAMPLE_SIZE + (currBuffPos);
                                _interpolationPoints[3].y =  _buffer[prevBufferOffset + prevSamplePos];
                                //Serial.printf("---[%i] %i: %i\n",  _buffer[prevBufferOffset + prevSamplePos], prevBufferOffset + prevSamplePos, _bufferPosition);
                            }
                        } else 
                        if (currBuffPos >= (_bufferLength / 2) ) {
                            if (_numBuffers >= 1) {
                                int nextBufferOffset = RESAMPLE_BUFFER_SAMPLE_SIZE * ((_currentBuffer+1) % 2);
                                int nextSamplePos = currBuffPos % (_bufferLength / 2);
                                _interpolationPoints[3].y =  _buffer[nextBufferOffset + nextSamplePos];
                                //Serial.printf("---[%i] %i: %i\n",  _interpolationPoints[3].y, nextBufferOffset + nextSamplePos, _bufferPosition);
                            }
                        }
                        else {
                            _interpolationPoints[3].y =  _buffer[samplePosition+i-1];
                            //Serial.printf("---[%i] %i: %i\n",  _interpolationPoints[3].y, samplePosition+i-1, _bufferPosition);
                        }
                        if (_numInterpolationPoints < 4) _numInterpolationPoints++;
                    }
                }
            }
            
            if (_numInterpolationPoints >= 4) {
                //int16_t interpolation = interpolate(_interpolationPoints, 1.0 + abs_remainder, 4);
                int16_t interpolation 
                    = fastinterpolate(
                        _interpolationPoints[0].y, 
                        _interpolationPoints[1].y, 
                        _interpolationPoints[2].y, 
                        _interpolationPoints[3].y, 
                        1.0 + abs_remainder); 
                result = interpolation;
                //Serial.printf("[%f]\n", interpolation);
            } else 
                result = 0;
        } else {
            _interpolationPoints[0].y = _interpolationPoints[1].y;
            _interpolationPoints[1].y = _interpolationPoints[2].y;
            _interpolationPoints[2].y = _interpolationPoints[3].y;
            _interpolationPoints[3].y = result;
            if (_numInterpolationPoints < 4) {
                _numInterpolationPoints++;
                result = 0;
            } else 
                result = _interpolationPoints[1].y;
            //Serial.printf("%f\n", result);
        }
    }

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
            _filename = (char *)"";
            return false;
        }

        __disable_irq();
        _file_size = _file.size();
        __enable_irq();
        _loop_start = 0;
        _loop_finish = (_file_size - _header_size) / 2;
        if (_file_size <= _header_size) {
            _playing = false;
            _filename =  (char *)"";
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
    _numBuffers = 0;
    _numInterpolationPoints = 0;
    _noMoreBuffersToRead = false;
    _last_read_offset = 0;
    _remainder = 0;
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
    _file.seek(_file_offset);
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

    if (_numBuffers > 1) return true; // we've already got enough buffers ?

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
        if (_numBuffers == 0) {
            _bufferPosition = numberOfBytesToRead - 2;
        }
    } else 
    {
        int final_file_offset = _header_size + (_loop_finish * 2) - (RESAMPLE_BUFFER_SAMPLE_SIZE * 2);
        if (_file_offset > final_file_offset) {
            numberOfBytesToRead = _header_size + (_loop_finish * 2) - _file_offset;
        }
        if (_numBuffers == 0)
            _bufferPosition = 0;
    }
    if (numberOfBytesToRead == 0) return false;

    unsigned int bufferOffset = 0;
    if (_numBuffers == 1){
        bufferOffset = (_currentBuffer == 0)? RESAMPLE_BUFFER_SAMPLE_SIZE : 0;
    }

    //Serial.printf("\nreading %d bytes, starting at:%d (into readbuff %d) - _file_offset:%d\n", numberOfBytesToRead, _file.position(), _readBuffer, _file_offset);
    _last_read_offset = _file_offset;
    //__disable_irq();
    int numRead = _file.read(_buffer + bufferOffset, numberOfBytesToRead);
    if (numRead == 0) {
        _noMoreBuffersToRead = true;
        return false;
    }

    if (numRead < RESAMPLE_BUFFER_SAMPLE_SIZE * 2)
        _noMoreBuffersToRead = true;        
    //__enable_irq();
    if (_numBuffers == 0)
        _bufferLength = numRead;
    else
        _nextBufferLength = numRead;
    //Serial.printf("read %d bytes\n", numRead);

    _numBuffers++;

    if (_playbackRate < 0) {
        _file_offset -= numRead;     
    } else
        _file_offset += numRead;

    //printf("exit: file_offset: %d\n", _file_offset);
    return true;
}

