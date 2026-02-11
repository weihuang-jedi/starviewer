#ifndef MARCHTETRAHEDRON_H
#define MARCHTETRAHEDRON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include <math.h>

#include <QtOpenGL>

#include "colorTable.h"

typedef struct _point3d_ pt3d;

struct _point3d_
{
    float fX;
    float fY;
    float fZ;
};

using namespace std;

//
// This program is modified from:
//
// Marching Cubes Example Program 
// by Cory Bloyd (corysama@yahoo.com)
//
// A simple, portable and complete implementation of the Marching Cubes
// and Marching Tetrahedrons algorithms in a single source file.
// There are many ways that this code could be made faster, but the 
// intent is for the code to be easy to understand.
//
// For a description of the algorithm go to
// http://astronomy.swin.edu.au/pbourke/modelling/polygonise/
//
// This code is public domain.
//

class MarchTetrahedron
{
    public:
        MarchTetrahedron();
       ~MarchTetrahedron();

        void set_isolevel(float *v, int il) { isovalue = v; isolevel = il; }
        void set_colormap(float **cm, int cl) { colormap = cm; colorlen = cl; }

        void vTetrahedron(pt3d *pasTetrahedronPosition,
                          float *pafTetrahedronValue);

    private:
        float* isovalue;
        int isolevel;

        float** colormap;
        int colorlen;

        void _runTetrahedron(pt3d *pasTetrahedronPosition,
                             float *pafTetrahedronValue,
                             float fTargetValue);
};
#endif

