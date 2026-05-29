#ifndef MPAS3DVIEWER_H
#define MPAS3DVIEWER_H

#include <iostream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "geometry.h"
#include "texture1d.h"
#include "colorTable.h"
#include "marchTetrahedron.h"

using namespace std;

class MPAS3dViewer
{
    public:
        MPAS3dViewer(ColorTable* ct);
       ~MPAS3dViewer();

        void draw(int tl);

        void reset();

        void setup(string vn, double *var);
        void set_geometry(Geometry* gm);

        void set_tlevel(int n) { _tlevel = n; };
        void set_value(double v) { _value = v; };

    protected:
        Geometry*  geometry;
        Texture1d* texture1d;
        ColorTable* colorTable;
        MarchTetrahedron* marchTetrahedron;

        string _varname;

        int   _tlevel;

        bool *_processed;

        double _valmax;
        double _valmin;
        double _value;

        double *_var;
        double *_evaluate();

        void _initialize();
};
#endif

