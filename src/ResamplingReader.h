#ifndef TEENSYAUDIOLIBRARY_RESAMPLINGREADER_H
#define TEENSYAUDIOLIBRARY_RESAMPLINGREADER_H

#include <Arduino.h>
#include <cstdint>
#include "loop_type.h"
#include "interpolation.h"
#include "waveheaderparser.h"

namespace newdigate {

template<class TArray, class TFile>
class ResamplingReader {
public:
    ResamplingReader() {
    }
    virtual ~ResamplingReader() {       
    }

    virtual TFile open(char *filename) = 0;
    virtual TArray* createSourceBuffer() = 0;
    virtual int16_t getSourceBufferValue(long index) = 0;
    virtual void close(void) = 0;
    int _lastInterpolationPosition[8] = {0};

    void begin(void) 
    {
        if (_interpolationType != ResampleInterpolationType::resampleinterpolation_none) {
            initializeInterpolationPoints();
        }
        _playing = false;
        _crossfade = 0.0;
        if (_play_start == play_start::play_start_sample)
            _bufferPosition1 = _header_offset;
        else
            _bufferPosition1 = _loop_start;
        _file_size = 0;
    }

    bool playRaw(TArray *array, uint32_t length, uint16_t numChannels)
    {
        _sourceBuffer = array;
        stop();

        _header_offset = 0;
        _file_size = length * 2;
        _loop_start = 0;
        _loop_finish = length;
        setNumChannels(numChannels);

        reset();
        _playing = true;
        return true;
    }

    bool playRaw(TArray *array, uint16_t numChannels) {
        return playRaw(array, false, numChannels); 
    }

    bool playWav(TArray *array, uint32_t length) {
        return playRaw(array, true); 
    }

    bool playWav(TArray *array) {
        bool result = false;
        
        do
        {
            wav_header hdr;
            unsigned infoTagsSize;
            wav_data_header wav_data_hdr;           
            WaveHeaderParser parser;
            
            if (!parser.readWaveHeaderFromBuffer((char*) array, hdr))
                break;
            
            // make unwarranted assumptions about the header format
            // by using a magic number...
            if (!parser.readInfoTags((unsigned char*) array, 36, infoTagsSize))
                break;
            
            if (!parser.readDataHeader((unsigned char*) array, 36 + infoTagsSize, wav_data_hdr))
                break;
                        
            result = playRaw((TArray*)((char*) array + (36 + infoTagsSize + sizeof wav_data_hdr)), 
                             wav_data_hdr.data_bytes / 2, 
                             hdr.num_channels); 
        } while (0);
        
        return result;
    }

    bool play(const char *filename, bool isWave, uint16_t numChannelsIfRaw = 0)
    {
        close();

        if (!isWave) // if raw file, then hardcode the numChannels as per the parameter
            setNumChannels(numChannelsIfRaw);

        _filename = new char[strlen(filename)+1] {0};
        memcpy(_filename, filename, strlen(filename) + 1);

        TFile file = open(_filename);
        if (!file) {
            Serial.printf("Not able to open file: %s\n", _filename);
            if (_filename) delete [] _filename;
            _filename = nullptr;
            return false;
        }

        _file_size = file.size();
        if (isWave) {
            wav_header wav_header;
            wav_data_header data_header;

            WaveHeaderParser wavHeaderParser;
            char buffer[36];
            size_t bytesRead = file.read(buffer, 36);
            
            wavHeaderParser.readWaveHeaderFromBuffer((const char *) buffer, wav_header);
            if (wav_header.bit_depth != 16) {
                Serial.printf("Needs 16 bit audio! Aborting.... (got %d)", wav_header.bit_depth);
                return false;
            }
            setNumChannels(wav_header.num_channels);
            
            bytesRead = file.read(buffer, 8);
            unsigned infoTagsSize;
            if (!wavHeaderParser.readInfoTags((unsigned char *)buffer, 0, infoTagsSize))
            {
                Serial.println("Not able to read header! Aborting...");
                return false;
            }

            file.seek(36 + infoTagsSize);
            bytesRead = file.read(buffer, 8);

            if (!wavHeaderParser.readDataHeader((unsigned char *)buffer, 0, data_header)) {
                Serial.println("Not able to read header! Aborting...");
                return false;
            }

            _header_offset = (44 + infoTagsSize) / 2;
            _loop_finish = ((data_header.data_bytes) / 2) + _header_offset; 
        } else 
            _loop_finish = _file_size / 2;
        
        file.close();

        if (_file_size <= _header_offset * sizeof(int16_t)) {
            _playing = false;
            if (_filename) delete [] _filename;
            _filename =  nullptr;
            Serial.printf("Wave file contains no samples: %s\n", filename);
            return false;
        }

        _sourceBuffer = createSourceBuffer();
        _loop_start = _header_offset;

        reset();
        _playing = true;
        return true;
    }

    bool playRaw(const char *filename, uint16_t numChannelsIfRaw){
        return play(filename, false, numChannelsIfRaw);
    }
    
    bool playWav(const char *filename){
        return play(filename, true);
    }

    bool play()
    {
        stop();
        reset();
        _playing = true;
        return true;
    }

    void stop(void)
    {
        if (_playing) {   
            _playing = false;
        }
    }           

    bool isPlaying(void) { return _playing; }

    unsigned int read(void **buf, uint16_t nsamples) {
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
                            _crossfade = 0.0;
                            if (_playbackRate >= 0.0) 
                                _bufferPosition1 = _loop_start;
                            else
                                _bufferPosition1 = _loop_finish - _numChannels;

                            break;
                        }

                        case looptype_pingpong:
                        {
                            if (_playbackRate >= 0.0) {
                                _bufferPosition1 = _loop_finish - _numChannels;
                            }
                            else {
                                if (_play_start == play_start::play_start_sample)
                                    _bufferPosition1 = _header_offset;
                                else
                                    _bufferPosition1 = _loop_start;
                            }
                            _playbackRate = -_playbackRate;
                            break;
                        }            

                        case looptype_none:            
                        default:
                        {
                            //Serial.printf("end of loop...\n");
                            /* no looping - return the number of (resampled) bytes returned... */
                            close();
                            return count;
                        }
                    }   
                }
            }
        }
        return count;
    }

    // read the sample value for given channel and store it at the location pointed to by the pointer 'value'
    bool readNextValue(int16_t *value, uint16_t channel) {
        if (!_useDualPlaybackHead) {
            if (_playbackRate >= 0 ) {
                //forward playback
                if (_bufferPosition1 >=  _loop_finish )
                    return false;
            } else if (_playbackRate < 0) {
                // reverse playback
                if (_play_start == play_start::play_start_sample) {
                    if (_bufferPosition1 < _header_offset)
                        return false;
                } else {
                    if (_bufferPosition1 < _loop_start)
                        return false;    
                }
            }
        } else {
            if (_playbackRate >= 0.0) {
                if (_crossfade == 0.0 && _bufferPosition1 > int32_t((_loop_finish - _numChannels) - _crossfadeDurationInSamples)) {
                    _bufferPosition2 = _loop_start;
                    _crossfade = 1.0 - (( (_loop_finish-_numChannels) - _bufferPosition1 ) / static_cast<double>(_crossfadeDurationInSamples));
                    _crossfadeState = 1;
                } else if (_crossfade == 1.0 && _bufferPosition2 > int32_t((_loop_finish - _numChannels)- _crossfadeDurationInSamples)) {
                    _bufferPosition1 = _loop_start;
                    _crossfade = ((_loop_finish - _numChannels) - _bufferPosition2) / static_cast<double>(_crossfadeDurationInSamples);
                    _crossfadeState = 2;
                } else if (_crossfadeState == 1) {
                    _crossfade = 1.0 - (((_loop_finish - _numChannels) - _bufferPosition1) / static_cast<double>(_crossfadeDurationInSamples));
                    if (_crossfade >= 1.0) {
                        _crossfadeState = 0;
                        _crossfade = 1.0;
                    }
                } else if (_crossfadeState == 2) {
                    _crossfade = ( (_loop_finish - _numChannels) - _bufferPosition2 ) / static_cast<double>(_crossfadeDurationInSamples);
                    if (_crossfade <= 0.0) {
                        _crossfadeState = 0;
                        _crossfade = 0.0;
                    }
                }
            } else {
                if (_crossfade == 0.0 && _bufferPosition1 < int32_t(_crossfadeDurationInSamples + _header_offset)) {
                    _bufferPosition2 = _loop_finish - _numChannels;
                    _crossfade = 1.0 - (_bufferPosition1 - _header_offset) / static_cast<double>(_crossfadeDurationInSamples);
                    _crossfadeState = 1;
                } else if (_crossfade == 1.0 && _bufferPosition2 < int32_t(_crossfadeDurationInSamples + _header_offset)) {
                    _bufferPosition1 = _loop_finish - _numChannels;
                    _crossfade = (_bufferPosition2 - _header_offset) / static_cast<double>(_crossfadeDurationInSamples);
                    _crossfadeState = 2;
                } else if (_crossfadeState == 1) {
                    _crossfade = 1.0 - (_bufferPosition1 - _header_offset) / static_cast<double>(_crossfadeDurationInSamples);
                    if (_crossfade >= 1.0) {
                        _crossfadeState = 0;
                        _crossfade = 1.0;
                    }
                } else if (_crossfadeState == 2) {
                    _crossfade = (_bufferPosition2 - _header_offset) / static_cast<double>(_crossfadeDurationInSamples);
                    if (_crossfade <= 0.0) {
                        _crossfadeState = 0;
                        _crossfade = 0.0;
                    }
                }
            }
        }

        int16_t result = 0;
        if (!_useDualPlaybackHead || _crossfade == 0.0) {
            result =  getSourceBufferValue(_bufferPosition1 + channel);
        } else if (_crossfade == 1.0){
            result =  getSourceBufferValue(_bufferPosition2 + channel);
        } else{
            int result1 =  getSourceBufferValue(_bufferPosition1 + channel);
            int result2 =  getSourceBufferValue(_bufferPosition2 + channel);
            result = ((1.0 - _crossfade ) * result1) + ((_crossfade) * result2);
        }

        if (_interpolationType == ResampleInterpolationType::resampleinterpolation_linear) {
            double abs_remainder = abs(_remainder);
            if (abs_remainder > 0.0) {

                if (_playbackRate > 0) {
                    if (_remainder - _playbackRate < 0.0){
                        // we crossed over a whole number, make sure we update the samples for interpolation
                        if (!_useDualPlaybackHead) {
                            if ( _numInterpolationPoints[channel] < 2 &&_playbackRate > 1.0 && _bufferPosition1 - _numChannels > _header_offset * 2 ) {
                                // need to update last sample
                                _interpolationPoints[channel][1].y = getSourceBufferValue(_bufferPosition1 - _numChannels);
                            }
                        }
                        _interpolationPoints[channel][0].y = _interpolationPoints[channel][1].y;
                        _interpolationPoints[channel][1].y = result;
                        if (_numInterpolationPoints[channel] < 2)
                            _numInterpolationPoints[channel]++;
                    }
                } 
                else if (_playbackRate < 0) {
                    if (_remainder - _playbackRate > 0.0){
                        // we crossed over a whole number, make sure we update the samples for interpolation
                        if (!_useDualPlaybackHead) {
                            if (_numInterpolationPoints[channel] < 2  && _playbackRate < -1.0) {
                                // need to update last sample
                                _interpolationPoints[channel][1].y = getSourceBufferValue(_bufferPosition1 + _numChannels);
                            }
                        }
                        _interpolationPoints[channel][0].y = _interpolationPoints[channel][1].y;
                        _interpolationPoints[channel][1].y = result;
                        if (_numInterpolationPoints[channel] < 2)
                            _numInterpolationPoints[channel]++;
                    }
                }

                if (_numInterpolationPoints[channel] > 1) {
                    result = abs_remainder * _interpolationPoints[channel][1].y + (1.0 - abs_remainder) * _interpolationPoints[channel][0].y;
                }
            } else {
                _interpolationPoints[channel][0].y = _interpolationPoints[channel][1].y;
                _interpolationPoints[channel][1].y = result;
                if (_numInterpolationPoints[channel] < 2)
                    _numInterpolationPoints[channel]++;

                result =_interpolationPoints[channel][0].y;
            }
        } 
        else if (_interpolationType == ResampleInterpolationType::resampleinterpolation_quadratic) {
            int delta = abs(_bufferPosition1/_numChannels - _lastInterpolationPosition[channel]);
            //Serial.printf("delta %d %d:\n", delta, _bufferPosition1/_numChannels - _lastInterpolationPosition[channel]);
            if (_numInterpolationPoints[channel] == 0){
                 _interpolationPoints[channel][3].y = result;
                 _numInterpolationPoints[channel] = 1;
            }
            if (delta > 0) {
                if (_playbackRate > 0) {                
                        // we crossed over a whole number, make sure we update the samples for interpolation
                        int numberOfSamplesToUpdate = delta;
                        if (numberOfSamplesToUpdate > 4) 
                            numberOfSamplesToUpdate = 4; // if playbackrate > 4, only need to pop last 4 samples
                        for (int i=numberOfSamplesToUpdate; i > 0; i--) {
                            _interpolationPoints[channel][0].y = _interpolationPoints[channel][1].y;
                            _interpolationPoints[channel][1].y = _interpolationPoints[channel][2].y;
                            _interpolationPoints[channel][2].y = _interpolationPoints[channel][3].y;
                            if (!_useDualPlaybackHead) {
                                _interpolationPoints[channel][3].y = getSourceBufferValue(_bufferPosition1-(i*_numChannels)+1+channel);
                            } else 
                            {
                                _interpolationPoints[channel][3].y = 
                                    getSourceBufferValue(_bufferPosition1-(i*_numChannels)+1+channel) * _crossfade 
                                    +
                                    getSourceBufferValue(_bufferPosition2-(i*_numChannels)+1+channel) * (1.0 -_crossfade);
                            }
                            if (_numInterpolationPoints[channel] < 4) _numInterpolationPoints[channel]++;
                        }
                        _lastInterpolationPosition[channel] = _bufferPosition1 / _numChannels;
                } 
                else if (_playbackRate < 0) {                
                        // we crossed over a whole number, make sure we update the samples for interpolation
                        int numberOfSamplesToUpdate = delta;
                        if (numberOfSamplesToUpdate > 4) 
                            numberOfSamplesToUpdate = 4; // if playbackrate > 4, only need to pop last 4 samples
                        for (int i=numberOfSamplesToUpdate; i > 0; i--) {
                            _interpolationPoints[channel][0].y = _interpolationPoints[channel][1].y;
                            _interpolationPoints[channel][1].y = _interpolationPoints[channel][2].y;
                            _interpolationPoints[channel][2].y = _interpolationPoints[channel][3].y;
                            if (!_useDualPlaybackHead) {
                                _interpolationPoints[channel][3].y = getSourceBufferValue(_bufferPosition1+(i*_numChannels)-1+channel);
                            } else 
                            {
                                _interpolationPoints[channel][3].y = 
                                    getSourceBufferValue(_bufferPosition1+(i*_numChannels)-1+channel) * _crossfade 
                                    +
                                    getSourceBufferValue(_bufferPosition2+(i*_numChannels)-1+channel) * (1.0 - _crossfade); 
                            }
                            if (_numInterpolationPoints[channel] < 4) _numInterpolationPoints[channel]++;
                            _lastInterpolationPosition[channel] = _bufferPosition1 / _numChannels;
                        }
                }          
            }
            if (_remainder != 0.0) {
                    if (_numInterpolationPoints[channel] >= 4) {
                        //result = interpolate(_interpolationPoints[channel], _interpolationPoints[channel][0].x + ((_bufferPosition1 - _interpolationPoints[channel][0].x) * abs_remainder), 4);
                        int16_t interpolation 
                            = fastinterpolate(
                                _interpolationPoints[channel][0].y, 
                                _interpolationPoints[channel][1].y, 
                                _interpolationPoints[channel][2].y, 
                                _interpolationPoints[channel][3].y, 
                                1.0 + abs(_remainder)); 
                        /*
                        Serial.printf("[%d %d %d %d] @ %f = %d \n",
                                _interpolationPoints[channel][0].y, 
                                _interpolationPoints[channel][1].y, 
                                _interpolationPoints[channel][2].y, 
                                _interpolationPoints[channel][3].y, 
                                1.0 + abs(_remainder),
                                interpolation
                        );
                        */
                        result = interpolation;
                    } else 
                        result = 0;
            } else {
                if (_numInterpolationPoints[channel] < 4) {
                    result = 0;
                } else 
                    result = _interpolationPoints[channel][1].y;
            }
        }

        if (channel == _numChannels - 1) {
            _remainder += _playbackRate;

            auto delta = static_cast<signed int>(_remainder);
            _remainder -= static_cast<double>(delta);
            if (!_useDualPlaybackHead) {
                _bufferPosition1 += (delta * _numChannels);
            } else {
                if (_crossfade < 1.0)
                    _bufferPosition1 += (delta * _numChannels);

                if (_crossfade > 0.0)
                    _bufferPosition2 += (delta * _numChannels);
                }
        }
        //Serial.printf("ch: %d, index: %d, value: %d\n", channel, _bufferPosition1, result);
        *value = result;
        return true;
    }

    void setPlaybackRate(double f) {
        _playbackRate = f;
        if (!_useDualPlaybackHead) {
            if (f < 0.0) {
                if  (_bufferPosition1 <= _header_offset) {
                    if (_play_start == play_start::play_start_sample)
                        _bufferPosition1 = _file_size/2 - _numChannels;
                    else
                        _bufferPosition1 = _loop_finish - _numChannels;
                } 
            } else {
                if (f >= 0.0 && _bufferPosition1 < _header_offset) {
                    if (_play_start == play_start::play_start_sample) 
                        _bufferPosition1 = _header_offset;
                    else
                        _bufferPosition1 = _loop_start;
                }
            }
        } else { 
            // _useDualPlaybackHead == true 
            if (_crossfade == 0.0) {
                if (f < 0.0) { 
                    if( _bufferPosition1 <= _header_offset) {
                        if (_play_start == play_start::play_start_sample)
                            _bufferPosition1 = _file_size/2 - _numChannels;
                        else
                            _bufferPosition1 = _loop_finish - _numChannels;
                    }
                } else {
                    if (f >= 0.0 && _bufferPosition1 < _header_offset) {
                        _bufferPosition1 = _header_offset;
                    }
                }
            }
        }
    }

    float playbackRate() {
        return _playbackRate;
    }

    void loop(uint32_t numSamples) {
        int bufferPosition = (_crossfade < 1.0)? _bufferPosition1 : _bufferPosition2;
        _loop_start = bufferPosition;
        _loop_finish = bufferPosition + numSamples * _numChannels;
        _loopType = loop_type::looptype_repeat;
    }

    void setLoopType(loop_type loopType)
    {
        _loopType = loopType;
    }

    loop_type getLoopType() {
        return _loopType;    
    }

    int available(void) {
        return _playing;
    }

    void reset(void) {
        if (_interpolationType != ResampleInterpolationType::resampleinterpolation_none) {
            initializeInterpolationPoints();
        }

        if (_playbackRate > 0.0) {
            // forward playabck - set _file_offset to first audio block in file
            if (_play_start == play_start::play_start_sample)
                _bufferPosition1 = _header_offset;
            else
                _bufferPosition1 = _loop_start;
        } else {
            // reverse playback - forward _file_offset to last audio block in file
            if (_play_start == play_start::play_start_sample)
                _bufferPosition1 = _file_size/2 - _numChannels;
            else
                _bufferPosition1 = _loop_finish - _numChannels;
        }
        
        for(int i=0;i<8;i++) {
            _numInterpolationPoints[i] = 0;
             if (_playbackRate > 0.0)
                _lastInterpolationPosition[i] = _bufferPosition1 / _numChannels;
            else 
                _lastInterpolationPosition[i] = (_bufferPosition1 / _numChannels) + 1;
        }

        _crossfade = 0.0;
    }

    void setLoopStart(uint32_t loop_start) {
        _loop_start = _header_offset + (loop_start * _numChannels);
    }

    void setLoopFinish(uint32_t loop_finish) {
        // sample number, (NOT byte number)
        _loop_finish = _header_offset + (loop_finish * _numChannels);
    }

    void setUseDualPlaybackHead(bool useDualPlaybackHead) {
        _useDualPlaybackHead = useDualPlaybackHead;
    }

    void setCrossfadeDurationInSamples(unsigned int crossfadeDurationInSamples) {
        _crossfadeDurationInSamples = crossfadeDurationInSamples;
    }

    void setInterpolationType(ResampleInterpolationType interpolationType) {
        if (interpolationType != _interpolationType) {
            _interpolationType = interpolationType;
            initializeInterpolationPoints();
        }
    }

    int16_t getNumChannels() {
        return _numChannels;
    }

    void setNumChannels(uint16_t numChannels) {
        if (numChannels != _numChannels) {
            _numChannels = numChannels;
            initializeInterpolationPoints();
        }
    }

    void setHeaderSizeInBytes(uint32_t headerSizeInBytes) {
        _header_offset = headerSizeInBytes / 2;
    }
    
    void setPlayStart(play_start start) {
        _play_start = start;
    }

    #define B2M (uint32_t)((double)4294967296000.0 / AUDIO_SAMPLE_RATE_EXACT / 2.0) // 97352592
    uint32_t positionMillis()
    {
        return ((uint64_t)_file_size * B2M) >> 32;
    }

    uint32_t lengthMillis()
    {
        return ((uint64_t)_file_size * B2M) >> 32;
    }

    int getBufferPosition1() {
        return _bufferPosition1;
    }

    int getBufferPosition2() {
        return _bufferPosition2;
    }

    double getCrossfade() {
        return _crossfade;
    }

    bool getUseDualPlaybackHead(){
        return _useDualPlaybackHead;
    }
    
    unsigned int getCrossfadeDurationInSamples() {
        return _crossfadeDurationInSamples;
    } 

    int32_t getLooptStart() {
        return  _loop_start / _numChannels - _header_offset;
    }

    int32_t getLoopFinish() {
        return  _loop_finish / _numChannels - _header_offset;
    }
    
protected:
    volatile bool _playing = false;

    int32_t _file_size;
    int32_t _header_offset = 0; // == (header size in bytes ) / 2

    double _playbackRate = 1.0;
    double _remainder = 0.0;
    loop_type _loopType = loop_type::looptype_none;
    play_start _play_start = play_start::play_start_sample;
    int32_t _bufferPosition1 = 0;
    int32_t _bufferPosition2 = 0;
    double _crossfade = 0.0;
    bool _useDualPlaybackHead = false;
    unsigned int _crossfadeDurationInSamples = 256; 
    int _crossfadeState = 0;
    int32_t _loop_start = 0;
    int32_t _loop_finish = 0;
    int16_t _numChannels = -1;
    uint16_t _numInterpolationPointsChannels = 0;
    char *_filename = nullptr;
    TArray *_sourceBuffer = nullptr;

    ResampleInterpolationType _interpolationType = ResampleInterpolationType::resampleinterpolation_none;
    unsigned int _numInterpolationPoints[8] = {0};
    InterpolationData _interpolationPoints[8][4] = {0};
    
    void initializeInterpolationPoints(void) {
        if (_numChannels < 0)
            return;
            
        _numInterpolationPointsChannels = 0;
    }

};

}

#endif //TEENSYAUDIOLIBRARY_RESAMPLINGREADER_H
