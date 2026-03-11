#ifndef _WindVector_H
#define _WindVector_H

//$Id: windvector.h 4875 2014-01-05 19:48:41Z starviewer $

#include <iostream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "evaluator.h"
#include "arrow.h"
#include "nvoptions.h"
//#include "earth.h"
#include "topography.h"

//axes:
#define X_DIRECTION	1
#define Y_DIRECTION	2
#define Z_DIRECTION	3

using namespace std;

class WindVector
{
    public:
        WindVector(ColorTable* ct, NVOptions* opt);
       ~WindVector();

        void draw();
        void draw_earth_image(float z);

        void setup(int nx, int ny, int nz,
                   float* u, float* v, float*w);
        void setup_position(double* lon, double* lat);

        void set_colorTable(ColorTable *ct) { colorTable = ct; };

        void set_stepsize(int n) { _stepsize = n; };
        void set_maxspeed(float s) { _maxspeed = s; };

    protected:
        ColorTable* colorTable;
        NVOptions* nvoptions;
        Arrow* arrow;
      //Earth* earth;
        Topography* topography;

        string _varname;

        float* _u;
        float* _v;
        float* _w;

        double* _lon;
        double* _lat;

        void _display_all();

    private:
        void _display_Xplane();
        void _display_Yplane();
        void _display_Zplane();
        void _display_arrow_onZplane();

        int _nx, _ny, _nz;
        int _stepsize, _local_stepsize;

        int    _colorlen;
        float* _colormap;
        float _wings;
        float _maxspeed;
        float _scale, _zScale;
        float _xStart, _yStart;
        float _xyDelt, _zDelt;

        float axx[3];
        float ayy[3];
        float azz[3];

        void _parameter_setup();

        void _draw_arrow(float x, float y, float z,
                         float u, float v, float w);
        void _arrow(float tail[3], float head[3], float w[3]);
        void _cross(float v1[3], float v2[3], float vout[3]);

        float _dist(float vin[3]);

        void _set_color(float spd, float* color);
};
#endif

