#include "ResamplingArrayReader.h"

unsigned int ResamplingArrayReader::read(void *buf, uint16_t nbyte) {
    if (!_playing) return 0;

    unsigned int count = 0;
    int16_t *index = (int16_t*)buf;
    while (count < nbyte) {

        if (readNextValue(index)){
            count++;
            index++;
        }
        else {
            // we have reached the end of the file

            switch (_loopType) {
                case looptype_repeat:
                {
                    if (_playbackRate >= 0.0) 
                        _bufferPosition = _loop_start;
                    else
                        _bufferPosition = _loop_finish;

                    break;
                }

                case looptype_pingpong:
                {
                    if (_playbackRate >= 0.0) {
                        _bufferPosition = _file_size;
                        //printf("switching to reverse playback...\n");
                    }
                    else {
                        _bufferPosition = 0;
                        //printf("switching to forward playback...\n");
                    }
                    _playbackRate = -_playbackRate;
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

    if (_playbackRate > 0 ) {
        //forward playback
        if (_bufferPosition >= _file_size) {

            if (_bufferPosition >=  _loop_finish )
                return false;

        }
    } else if (_playbackRate < 0) {
        // reverse playback    
        if (_bufferPosition < 0)
            return false;
    }

    int16_t result = _sourceBuffer[_bufferPosition];

    _remainder += _playbackRate;

    auto delta = static_cast<signed int>(_remainder);
    _remainder -= static_cast<double>(delta);

    _bufferPosition +=  delta;
    *value = result;
    return true;
}

void ResamplingArrayReader::begin(void)
{
    _playing = false;
    _bufferPosition = 0;
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
    if (_playbackRate > 0.0) {
        // forward playabck - set _file_offset to first audio block in file
        _bufferPosition = 0;
    } else {
        // reverse playback - forward _file_offset to last audio block in file
        _bufferPosition = _file_size;
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

