//
// Created by Nicholas Newdigate on 24/05/2021.
//
#ifndef TEENSY_RESAMPLING_SDREADER_INTERPOLATION_H
#define TEENSY_RESAMPLING_SDREADER_INTERPOLATION_H

#include <cinttypes>
#include <cmath>
#include <math.h>


struct InterpolationData
{
    uint32_t x;
	int16_t y;
};

// https://en.wikipedia.org/wiki/Lagrange_polynomial
// https://www.geeksforgeeks.org/lagranges-interpolation/
// function to interpolate the given data points using Lagrange's formula
// xi corresponds to the new data point whose value is to be obtained
// n represents the number of known data points
int16_t interpolate(InterpolationData *f, double xi, int n);

inline int16_t fastinterpolate(int16_t d1, int16_t d2, int16_t d3, int16_t d4, float t) {
    const float a = (1.0f / 6.0f) * (-static_cast<float>(d1) + 3.0f * (static_cast<float>(d2) - static_cast<float>(d3)) + static_cast<float>(d4));
    const float b = 0.5f * (static_cast<float>(d1) - 2.0f * static_cast<float>(d2) + static_cast<float>(d3));
    const float c = (1.0f / 6.0f) * (-2.0f * static_cast<float>(d1) - 3.0f * static_cast<float>(d2) + 6.0f * static_cast<float>(d3) - static_cast<float>(d4));
    const float d = static_cast<float>(d2);

    const float result = ((a * t + b) * t + c) * t + d;
    int32_t rounded = static_cast<int32_t>(roundf(result));
    if (rounded < -32768) return -32768;
    if (rounded > 32767) return 32767;
    return static_cast<int16_t>(rounded);
}

#endif //TEENSY_RESAMPLING_SDREADER_INTERPOLATION_H
