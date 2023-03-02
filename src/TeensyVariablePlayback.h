//
// Created by Nicholas Newdigate on 21/07/2020.
//

#ifndef TEENSY_RESAMPLING_ARDUINO_SAMPLER_H
#define TEENSY_RESAMPLING_ARDUINO_SAMPLER_H

#include "waveheaderparser.h"
#include "ResamplingSdReader.h"
#include "ResamplingArrayReader.h"
#include "playsdresmp.h"
#include "playarrayresmp.h"
#ifndef BUILD_FOR_LINUX
#include "ResamplingLfsReader.h"
#include "ResamplingSerialFlashReader.h"
#include "playlfsresmp.h"
#include "playserialflashresmp.h"
#endif
#endif //TEENSY_RESAMPLING_ARDUINO_SAMPLER_H
