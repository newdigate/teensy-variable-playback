#include "ResamplingArrayReader.h"
#include "interpolation.h"
#include "waveheaderparser.h"

// read n samples into each buffer (1 buffer per channel)
unsigned int ResamplingArrayReader::read(void **buf, uint16_t nsamples) {
    if (!_playing) return 0;

    int16_t *index[_numChannels];
    unsigned int count = 0;
    for (int channel=0; channel < _numChannels; channel++) {
        index[channel] = (int16_t*)buf[channel];
    }

    while (count < nsamples) {

        for (int channel=0; channel < _numChannels; channel++) {
            if (readNextValue(index[channel], channel)) {
                if (channel == _numChannels - 1)
                    count++;
                index[channel]++;
            }
            else {
                // we have reached the end of the file

                switch (_loopType) {
                    case looptype_repeat:
                    {
                        if (_playbackRate >= 0.0) 
                            _bufferPosition = _loop_start;
                        else
                            _bufferPosition = _loop_finish - _numChannels;

                        break;
                    }

                    case looptype_pingpong:
                    {
                        if (_playbackRate >= 0.0) {
                            _bufferPosition = _loop_finish - _numChannels;
                            //printf("switching to reverse playback...\n");
                        }
                        else {
                            _bufferPosition = _header_offset;
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
    }
    return count;
}

// read the sample value for given channel and store it at the location pointed to by the pointer 'value'
bool ResamplingArrayReader::readNextValue(int16_t *value, uint16_t channel) {

    if (_playbackRate >= 0 ) {
        //forward playback
        if (_bufferPosition >=  _loop_finish )
            return false;

    } else if (_playbackRate < 0) {
        // reverse playback    
        if (_bufferPosition < _header_offset)
            return false;
    }

    int16_t result = _sourceBuffer[_bufferPosition + channel];
    if (_interpolationType == ResampleInterpolationType::resampleinterpolation_linear) {

        double abs_remainder = abs(_remainder);
        if (abs_remainder > 0.0) {

            if (_playbackRate > 0) {
                if (_remainder - _playbackRate < 0.0){
                    // we crossed over a whole number, make sure we update the samples for interpolation

                    if (_playbackRate > 1.0) {
                        // need to update last sample
                        _interpolationPoints[channel][1].y = _sourceBuffer[_bufferPosition-_numChannels];
                    }

                    _interpolationPoints[channel][0].y = _interpolationPoints[channel][1].y;
                    _interpolationPoints[channel][1].y = result;
                    if (_numInterpolationPoints < 2)
                        _numInterpolationPoints++;
                }
            } 
            else if (_playbackRate < 0) {
                if (_remainder - _playbackRate > 0.0){
                    // we crossed over a whole number, make sure we update the samples for interpolation

                    if (_playbackRate < -1.0) {
                        // need to update last sample
                        _interpolationPoints[channel][1].y = _sourceBuffer[_bufferPosition+_numChannels];
                    }

                    _interpolationPoints[channel][0].y = _interpolationPoints[channel][1].y;
                    _interpolationPoints[channel][1].y = result;
                    if (_numInterpolationPoints < 2)
                        _numInterpolationPoints++;
                }
            }

            if (_numInterpolationPoints > 1) {
                result = abs_remainder * _interpolationPoints[channel][1].y + (1.0 - abs_remainder) * _interpolationPoints[channel][0].y;
                //Serial.printf("[%f]\n", interpolation);
            }
        } else {
            _interpolationPoints[channel][0].y = _interpolationPoints[channel][1].y;
            _interpolationPoints[channel][1].y = result;
            if (_numInterpolationPoints < 2)
                _numInterpolationPoints++;

            result =_interpolationPoints[channel][0].y;
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
                    for (int i=numberOfSamplesToUpdate; i > 0; i--) {
                        _interpolationPoints[channel][0].y = _interpolationPoints[channel][1].y;
                        _interpolationPoints[channel][1].y = _interpolationPoints[channel][2].y;
                        _interpolationPoints[channel][2].y = _interpolationPoints[channel][3].y;
                        _interpolationPoints[channel][3].y =  _sourceBuffer[_bufferPosition-(i*_numChannels)+1+channel];
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
                    for (int i=numberOfSamplesToUpdate; i > 0; i--) {
                        _interpolationPoints[channel][0].y = _interpolationPoints[channel][1].y;
                        _interpolationPoints[channel][1].y = _interpolationPoints[channel][2].y;
                        _interpolationPoints[channel][2].y = _interpolationPoints[channel][3].y;
                        _interpolationPoints[channel][3].y =  _sourceBuffer[_bufferPosition+(i*_numChannels)-1+channel];
                        if (_numInterpolationPoints < 4) _numInterpolationPoints++;
                    }
                }
            }
            
            if (_numInterpolationPoints >= 4) {
                //int16_t interpolation = interpolate(_interpolationPoints, 1.0 + abs_remainder, 4);
                int16_t interpolation 
                    = fastinterpolate(
                        _interpolationPoints[channel][0].y, 
                        _interpolationPoints[channel][1].y, 
                        _interpolationPoints[channel][2].y, 
                        _interpolationPoints[channel][3].y, 
                        1.0 + abs_remainder); 
                result = interpolation;
                //Serial.printf("[%f]\n", interpolation);
            } else 
                result = 0;
        } else {
            _interpolationPoints[channel][0].y = _interpolationPoints[channel][1].y;
            _interpolationPoints[channel][1].y = _interpolationPoints[channel][2].y;
            _interpolationPoints[channel][2].y = _interpolationPoints[channel][3].y;
            _interpolationPoints[channel][3].y = result;
            if (_numInterpolationPoints < 4) {
                _numInterpolationPoints++;
                result = 0;
            } else 
                result = _interpolationPoints[channel][1].y;
            //Serial.printf("%f\n", result);
        }
    }

    if (channel == _numChannels - 1) {
        _remainder += _playbackRate;

        auto delta = static_cast<signed int>(_remainder);
        _remainder -= static_cast<double>(delta);
        _bufferPosition +=  (delta * _numChannels);
    }

    *value = result;
    return true;
}

void ResamplingArrayReader::initializeInterpolationPoints(void) {
    if (_numChannels < 0)
        return;
        
    deleteInterpolationPoints();
    _interpolationPoints = new InterpolationData*[_numChannels];
    for (int channel=0; channel < _numChannels; channel++) {        
        InterpolationData *interpolation = new InterpolationData[4];
        interpolation[0].y = 0.0;
        interpolation[1].y = 0.0;    
        interpolation[2].y = 0.0;    
        interpolation[3].y = 0.0;
        _interpolationPoints[channel] = interpolation ;
    }
    _numInterpolationPointsChannels = _numChannels;
}

void ResamplingArrayReader::deleteInterpolationPoints(void) {
    if (!_interpolationPoints) return;
    for (int i=0; i<_numInterpolationPointsChannels; i++) {
        delete [] _interpolationPoints[i];
    }
     delete [] _interpolationPoints;
     _interpolationPoints = nullptr;
     _numInterpolationPointsChannels = 0;
}

void ResamplingArrayReader::begin(void)
{
    if (_interpolationType != ResampleInterpolationType::resampleinterpolation_none) {
        initializeInterpolationPoints();
    }
    _playing = false;
    _bufferPosition = _header_offset;
    _file_size = 0;
}

bool ResamplingArrayReader::playRaw(int16_t *array, uint32_t length, uint16_t numChannels)
{
    _sourceBuffer = array;
    stop();

    _header_offset = 0;
    _file_size = length * 2;
    _loop_start = 0;
    _loop_finish = length;
    setNumChannels(numChannels);

    reset();
    //updateBuffers();
    _playing = true;
    return true;
}

bool ResamplingArrayReader::playWav(int16_t *array, uint32_t length)                // length == total number of 16-bit samples for all channels, including header
{
    _sourceBuffer = array;
    stop();

    wav_header wav_header;
    wav_data_header data_header;

    WaveHeaderParser wavHeaderParser;
    if (!wavHeaderParser.readWaveHeaderFromBuffer((const char *) array, wav_header)) {
        Serial.println("Not able to read header! Aborting.... ");
        return false;
    }

    if (wav_header.bit_depth != 16) {
        Serial.printf("Needs 16 bit audio! Aborting.... (got %d)\n", wav_header.bit_depth);
        return false;
    }
    setNumChannels(wav_header.num_channels);
    unsigned infoTagsSize;
    if (!wavHeaderParser.readInfoTags((unsigned char *)array, 36, infoTagsSize))
    {
        Serial.println("Not able to read header! Aborting...");
        return false;
    }

    if (!wavHeaderParser.readDataHeader((unsigned char *)array, 36 + infoTagsSize, data_header)) {
        Serial.println("Not able to read header! Aborting...");
        return false;
    }


    _header_offset = (44 + infoTagsSize) / 2;
    _file_size = data_header.data_bytes + 44 + infoTagsSize; //2 bytes per sample
    if (_file_size > length * 2){
        Serial.printf("TeensyVariablePlayback: warning: length of array in bytes (%d) is smaller than the file data size in bytes (%d) according to the header - defaulting length to filesize...", length * 2, _file_size);
        _loop_finish = length;
    } else
        _loop_finish = _file_size / 2;
    _loop_start = _header_offset;

    reset();
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
    _numInterpolationPoints = 0;
    if (_playbackRate > 0.0) {
        // forward playabck - set _file_offset to first audio block in file
        _bufferPosition = _header_offset;
    } else {
        // reverse playback - forward _file_offset to last audio block in file
        _bufferPosition = _loop_finish - _numChannels;
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
    if (_playing) {
        stop();
        deleteInterpolationPoints();
    }
}

