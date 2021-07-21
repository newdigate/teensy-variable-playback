//
// Created by Nicholas Newdigate on 24/05/2021.
//
#ifndef TEENSY_RESAMPLING_SDREADER_INTERPOLATION_H
#define TEENSY_RESAMPLING_SDREADER_INTERPOLATION_H

#include <cinttypes>
#include <math.h>


enum ResampleInterpolationType {
    resampleinterpolation_none = 1,
    resampleinterpolation_linear = 2,
    resampleinterpolation_quadratic = 3,
};

struct InterpolationData
{
    int16_t x, y;
};

// https://en.wikipedia.org/wiki/Lagrange_polynomial
// https://www.geeksforgeeks.org/lagranges-interpolation/
// function to interpolate the given data points using Lagrange's formula
// xi corresponds to the new data point whose value is to be obtained
// n represents the number of known data points
int16_t interpolate(InterpolationData *f, double xi, int n);
int16_t fastinterpolate(int16_t d1, int16_t d2, int16_t d3, int16_t d4, float x);
#endif //TEENSY_RESAMPLING_SDREADER_INTERPOLATION_H
