#ifndef _WindVector_H
#define _WindVector_H

#include <iostream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions_3_3_Core>

#include "evaluator.h"
#include "arrow.h"
#include "nvoptions.h"

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

        void draw(int k, double z);
	void drawGPU(int k, double z);

        void setup(int nx, int ny, int nz,
                   float* u, float* v, float*w);
        void setup(int nx, int ny, int nz,
                   float* u, float* v);
        void setup_lonlat(double* lon, double* lat);
        void setup_xyFlat(double* xFlat, double* yFlat);

        void set_colorTable(ColorTable *ct) { colorTable = ct; };

        void set_stepsize(int n) { _stepsize = n; };
        void set_maxspeed(double s) { _maxspeed = s; };

	bool has_w() { return _has_w; };

    protected:
        ColorTable* colorTable;
        NVOptions* nvoptions;
        Arrow* arrow;

        string _varname;

        float* _u;
        float* _v;
        float* _w;

        double* _lon;
        double* _lat;

        double* _xFlat;
        double* _yFlat;

    private:
        int _nx, _ny, _nz;
        int _stepsize, _local_stepsize;
	bool _has_w;

        int    _colorlen;
        float* _colormap;
        double _wings;
        double _maxspeed;
        double _scale, _zScale;

        double axx[3];
        double ayy[3];
        double azz[3];

        void _parameter_setup();

        void _draw_arrow(double x, double y, double z,
                         float u, float v, float w);
        void _draw_arrow(double x, double y, double z,
                         float u, float v);
        void _arrow(double tail[3], double head[3], double w[3]);
        void _cross(double v1[3], double v2[3], double vout[3]);

        double _dist(double vin[3]);

        void _set_color(double spd, float* color);

        // GPU Asset tracking IDs
        GLuint windVBO = 0;
        GLuint texU = 0;
        GLuint texV = 0;
        GLuint texW = 0;
	GLuint colorMapTexture = 0;
	GLsizei windVertexCount = 0;
        QOpenGLShaderProgram* windShader = nullptr;

        void _initWindGPUAssets6lines();
        void _initWindGPUAssets();

	int _colorLen;
	float* _colorMap;
};
#endif

