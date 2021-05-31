//
// Created by Nicholas Newdigate on 24/05/2021.
//
#include "interpolation.h"

// https://en.wikipedia.org/wiki/Lagrange_polynomial
// https://www.geeksforgeeks.org/lagranges-interpolation/
// function to interpolate the given data points using Lagrange's formula
// xi corresponds to the new data point whose value is to be obtained
// n represents the number of known data points
double interpolate(IntepolationData *f, double xi, int n) {
    double result = 0; // Initialize result
    for (int i=0; i<n; i++)
    {
        // Compute individual terms of above formula
        double term = f[i].y;
        for (int j=0;j<n;j++)
        {
            if (j!=i)
                term = term*(xi - f[j].x)/(f[i].x - f[j].x);
        }

        // Add current term to result
        result += term;
    }
    return result;
}