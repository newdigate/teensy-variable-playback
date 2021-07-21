//
// Created by Nicholas Newdigate on 24/05/2021.
//
#include "interpolation.h"

// https://en.wikipedia.org/wiki/Lagrange_polynomial
// https://www.geeksforgeeks.org/lagranges-interpolation/
// function to interpolate the given data points using Lagrange's formula
// xi corresponds to the new data point whose value is to be obtained
// n represents the number of known data points
int16_t interpolate(InterpolationData *f, double xi, int n) {
    double result = 0.0; // Initialize result
    for (int i=0; i<n; i++)
    {
        // Compute individual terms of above formula
        double term = f[i].y;
        for (int j=0;j<n;j++)
        {
            if (j!=i)
                term = term*(xi - double(f[j].x))/(f[i].x - double(f[j].x));
        }

        // Add current term to result
        result += term;
    }

    int32_t untruncated = round(result);
    if (untruncated < -32768)
      return -32768;
    if (untruncated > 32767)
      return 32767;
    return untruncated;
}


int16_t fastinterpolate(int16_t d1, int16_t d2, int16_t d3, int16_t d4, float x) {
    float x_1 = x * 1000.0;
    float x_2 = x_1 * x_1;
    float x_3 = x_2 * x_1;

    //Serial.printf("%i,%i,%i,%i @ x=%f \n", d1,d2,d3,d4,x);

    return d1 * (x_3  - 6000 * x_2   + 11000000  * x_1  - 6000000000 ) / - 6000000000
         + d2 * (x_3  - 5000 * x_2   +  6000000  * x_1        )     /   2000000000
         + d3 * (x_3  - 4000 * x_2   +  3000000  * x_1        )     / - 2000000000
         + d4 * (x_3  - 3000 * x_2   +  2000000  * x_1        )     /   6000000000;
    
/*
    int32_t untruncated = result;
    if (untruncated < -32768)
      return -32768;
    if (untruncated > 32767)
      return 32767;
    return result;
    */
}