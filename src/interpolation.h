//
// Created by Nicholas Newdigate on 24/05/2021.
//
#ifndef TEENSY_RESAMPLING_SDREADER_INTERPOLATION_H
#define TEENSY_RESAMPLING_SDREADER_INTERPOLATION_H

struct IntepolationData
{
    double x, y;
};

// https://en.wikipedia.org/wiki/Lagrange_polynomial
// https://www.geeksforgeeks.org/lagranges-interpolation/
// function to interpolate the given data points using Lagrange's formula
// xi corresponds to the new data point whose value is to be obtained
// n represents the number of known data points
double interpolate(IntepolationData *f, double xi, int n);
#endif //TEENSY_RESAMPLING_SDREADER_INTERPOLATION_H
