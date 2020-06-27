#include "ResamplingSdReader.h"

int ResamplingSdReader::read(void *buf, uint16_t nbyte) {
    if (!_playing) return 0;

    unsigned int count = 0;
    int16_t *index = (int16_t*)buf;
    for (int i=0; i< nbyte/2; i++) {

        if (readNextValue(index))
            count+=2;
        else
            return count;

        index++;
    }
    return count;
}

bool ResamplingSdReader::readNextValue(int16_t *value) {

    if (_bufferLength[_playBuffer] == 0)
        return false;

    if (_playbackRate > 0 ) {
        //forward playback

        if (_numBuffers == 1 && _last_read_offset + _bufferPosition >= _file_size)
            return false;

        //Serial.printf("readNextValue: %d/%d \n", _bufferPosition, _bufferLength[_playBuffer]);

        if (_bufferPosition >= _bufferLength[_playBuffer]) {
            bufferIsAvailableForRead[_playBuffer] = true;
            //Serial.printf("\ndiscard buffer: %d\n", _playBuffer);

            if (_numBuffers ==1 && _file_offset >= _file_size)
                return false;

            _numBuffers--;
            //Serial.printf("num  buffers: %d\n", _numBuffers);

            if (_playBuffer < _numBuffers) {
                _readBuffer = _playBuffer;
                //Serial.printf("switch read buffer to: %d\n", _readBuffer);
            }

            _bufferPosition -= _bufferLength[_playBuffer];

            _playBuffer++;
            if (_playBuffer >= ResamplingSdReader_NUM_BUFFERS) {
                _playBuffer = 0;
            }
            //Serial.printf("switch play buffer to: %d\n", _playBuffer);
            updateBuffers();
        }
    } else if (_playbackRate < 0) {

        if (_numBuffers == 1)
            if (_file_offset + (AUDIO_BLOCK_SAMPLES * 2) + _bufferPosition < 0)
                return false;

        // reverse playback
        if (_bufferPosition < 0) {

            if (_numBuffers == 0)
                return false;

            bufferIsAvailableForRead[_playBuffer] = true;
            _readBuffer = _playBuffer;

            _numBuffers--;

            _playBuffer ++;
            if (_playBuffer >= ResamplingSdReader_NUM_BUFFERS) {
                _playBuffer = 0;
            }

            _bufferPosition += _bufferLength[_playBuffer];
            updateBuffers();

        } else if (_bufferPosition > (AUDIO_BLOCK_SAMPLES-1) * 2) {
            _bufferPosition = (AUDIO_BLOCK_SAMPLES-1) * 2;
        }
    }
    int16_t *_buffer = _buffers[_playBuffer];
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


    for (char i=0; i<ResamplingSdReader_NUM_BUFFERS; i++ ) {
        _buffers[i] = (int16_t*)malloc( AUDIO_BLOCK_SAMPLES * 2);
        bufferIsAvailableForRead[i] = true;

    }
    _readBuffer = 0;
    _numBuffers = 0;
    updateBuffers();
    _playBuffer = 0;
    if (_numBuffers == 0)
        return false;

    _playing = true;

    if (_playbackRate < 0) {
        _bufferPosition = _bufferLength[_playBuffer]-2;
    }

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

        for (int i=0; i<ResamplingSdReader_NUM_BUFFERS; i++ ) {
            if (_buffers[i] != NULL) {
                free(_buffers[i]);
                _buffers[i] = NULL;
            }
        }
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

void ResamplingSdReader::updateBuffers() {

    bool forward = (_playbackRate >= 0.0);

    while (_readBuffer >= 0
           && isInRange()
           && bufferIsAvailableForRead[_readBuffer]) {

        //Serial.printf("read buffer: %d (_numBuffers:%d) \n",_readBuffer, _numBuffers);
        int16_t *buffer = _buffers[_readBuffer];
        bufferIsAvailableForRead[_readBuffer] = false;

        _last_read_offset = _file_offset;
        uint16_t numberOfBytesToRead = AUDIO_BLOCK_SAMPLES * 2;
        if (!forward) {
            if (_file_offset < 0) {
                // reverse playback, last buffer, only read partial remaining buffer that hasn't already played
                numberOfBytesToRead = _file_offset + AUDIO_BLOCK_SAMPLES * 2;
            }
            _file.seek(_file_offset < 0? 0 : _file_offset);
        }
        //Serial.printf("\nreading %d bytes, starting at:%d (into readbuff %d) - _file_offset:%d\n", numberOfBytesToRead, _file.position(), _readBuffer, _file_offset);
        int numRead = _file.read(buffer, numberOfBytesToRead);
        _bufferLength[_readBuffer] = numRead;
        //Serial.printf("read %d bytes\n", numRead);

        if (_playbackRate < 0) {
            _file_offset -= numRead;
        } else
            _file_offset += numRead;

        if (numRead > 0) {
            _readBuffer ++;
            if (_readBuffer >= ResamplingSdReader_NUM_BUFFERS)
                _readBuffer = -1;

            _numBuffers++;
        } else {
            _readBuffer = -1;
            //Serial.printf("\n Not able to read anymore buffers for the moment...\n", numRead);
        }
    }
}

bool ResamplingSdReader::isInRange() {
    if (_playbackRate >= 0.0) {
        return _file_offset < _file_size;
    } else {
        return _file_offset + AUDIO_BLOCK_SAMPLES * 2 > 0;
    }
}