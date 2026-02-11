#ifndef MARCHINGCUBE_H
#define MARCHINGCUBE_H

//$Id: marchingcube.h 4946 2014-02-03 17:04:17Z starviewer $

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include <QtOpenGL>

#include "colorTable.h"
#include "nvoptions.h"

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

#include "math.h"

float fGetOffset(float fValue1, float fValue2, float fValueDesired);

struct _vector
{
    float fX;
    float fY;
    float fZ;     
};

typedef struct _vector mc_vector;

class MarchingCube
{
    public:
        MarchingCube();
        ~MarchingCube();

        void set_colorTable(ColorTable *ct) { colorTable = ct; };

        void setup(int ix, int iy, int iz,
                   double* var, double vmin, double vmax);

        void set_descend() { _descend = 1.0; }
        void set_ascend() { _descend = -1.0; }

        void reset();

        void display();

        void set_varname(string n) { _varname = n; };
        void set_curtime(int n) { _curtime = n; };
        void set_nvoptions(NVOptions* opt) { nvoptions = opt; };

        string get_varname() { return _varname; };
        int    get_curtime() { return _curtime; };

    private:
        ColorTable* colorTable;
        NVOptions* nvoptions;

        string _varname;

        void _compile_display_list();
        void _march_this_cube(int is, int js, int ks,
                              float fTargetValue);

        int _display_layers;
        int _display_list;
        int _curtime;

        double* _var;
        float  _isolevel[21];
        float  _isoalpha[21];

        float _descend;
        float _minval, _maxval;

        int   nx, ny, nz;
        float x, y, z;
        float xDelt, yDelt, zDelt;
        float xStart, yStart, zStart;

        float *_desired_level;

        void _vMarchTetrahedron(mc_vector *pasTetrahedronPosition,
                                float *pafTetrahedronValue,
                                float fTargetValue);

        float get_x(int i) { return (xStart + i * xDelt); };
        float get_y(int j) { return (yStart + j * yDelt); };
        float get_z(int k) { return (zStart + k * zDelt); };
};

#endif

