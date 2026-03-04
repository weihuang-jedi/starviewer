#ifndef _Noise_H
#define _Noise_H

//
// Coherent noise function over 1, 2 or 3 dimensions
// (copyright Ken Perlin)
//

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <assert.h>

using namespace std;

#define MAXB 0x100
#define N 0x1000

class Noise
{
    public:
        Noise();
       ~Noise();

        double* get_noise() { return theNoise; };

        void setup(int ix, int iy, int iz);

    private:
        double* theNoise;
        int    p[MAXB + MAXB + 2];
        double g3[MAXB + MAXB + 2][3];

        bool start;
        int B, BM;

        int nx, ny, nz;

        void InitNoise();
        void normalize3(double v[3]);
        void SetNoiseFrequency(int frequency);
        void set_val(int i, double &t, int &b0, int &b1,
                     double &r0, double &r1, double vec[3]);
        void createNoise3D();

        double noise3(double vec[3]);
        double s_curve(double t);
        double lerp(double t, double a, double b);
        double at3(double rx, double ry, double rz, double q[3]);
};
#endif

