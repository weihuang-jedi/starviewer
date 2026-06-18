#ifndef _Arrow_H_
#define _Arrow_H_

//$Id: arrow.h 5161 2014-07-29 22:35:26Z starviewer $

#include <iostream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <math.h>

#include "cone.h"
#include "cylinder.h"
#include "colorTable.h"

using namespace std;

#define NV_PI	3.1415926535897932

#ifndef ARG
#define	ARG	(NV_PI / 180.0)
#endif

#ifndef DEG
#define	DEG	(180.0 / NV_PI)
#endif

#define NUMBEROFCYLINDERS	8

struct _vector_struct
{
    double x;
    double y;
    double z;
    double length;
};

typedef struct _vector_struct vector_struct;

class Arrow
{
    public:
        Arrow(ColorTable* ct);
       ~Arrow();

        void draw();

        void setup(float x, float y, float z,
                   float u, float v, float w);
        void setup(float x, float y, float z,
                   float u, float v);

        void set_maxspeed(double d) { _maxspeed = d; };

        vector_struct get_normalized_vector(vector_struct &vec);
        vector_struct cross(vector_struct v1, vector_struct v2);
        double dotproduct(vector_struct v1, vector_struct v2);

        void longer() { _sh *= 1.10; };
        void shorter() { _sh *= 0.90; };
        void wider() { _sw *= 1.10; };
        void narrower() { _sw *= 0.90; };

    protected:
        Cone* cone;
        Cylinder* cylinder;
        ColorTable* colorTable;

        int _colorlen;
        int _noc;

        float* _colormap;

        float color[4];

        double _x1, _y1, _z1, _u, _v, _w;
        double _x2, _y2, _z2;
        double _dx, _dy, _dz;
        double _rx, _ry, _rz;
        double _sh, _sv, _sw;
        double _speed, _angle;
        double _length, _width;
        double _maxspeed;

        void _draw_arrow(float x, float y, float z,
                         float u, float v, float w);

        void _set_color(double spd);
};
#endif

