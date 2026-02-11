#ifndef MPAS2DVIEWER_H
#define MPAS2DVIEWER_H

//$Id: mpas2dviewer.h 4833 2013-11-05 15:49:31Z starviewer $

#include <iostream>
#include <vector>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "util.h"
#include "lister.h"
#include "colorTable.h"
#include "coastline.h"
#include "texture1d.h"
#include "mpasgeometry.h"
#include "evaluator.h"
#include "locator.h"
#include "nvoptions.h"

using namespace std;

class MPAS2dViewer
{
    public:
        MPAS2dViewer(ColorTable* ct, NVOptions* opt);
       ~MPAS2dViewer();

        void draw();
        void draw_sphere_grids();
        void draw_plane_grids();

        void reset();
        void setup(string vn, double* var);
        void set_geometry(MPASGeometry* gm);
        void reset_texture1d(ColorTable *ct);

        void set_tvalue(int n) { _tvalue = n; };

        void set_locator(Locator* l) { locator = l; };

        double get_minval() { return _minval; };
        double get_maxval() { return _maxval; };

    protected:
        Locator* locator;
        Lister* lister;
        Texture1d* texture1d;
        CoastLine* coastline;
        ColorTable* colorTable;
        MPASGeometry* geometry;
        Evaluator* evaluator;
        NVOptions* nvoptions;

        string _varname;

        GLuint xcl, ycl, zcl;

        int _tvalue;

        int** _zlist;

        bool* _innerPoints;
        int   _nBoundaryPoints;
        vector<int> _boundaryType;
        vector<int> _boundaryPoints;
        vector<int> _subsetVertices;

        double* _var;

        double  _minval, _workMinVal;
        double  _maxval, _workMaxVal;

        void _initialize();
        void _evaluate(double* var);

        void _flatSubset();
        void _sphereSubset();

        void _flatDisplay();
        void _flatDisplayXsec();
        void _flatDisplayYsec();

        void _sphereDisplay();
        void _sphereDisplayXsec();
        void _sphereDisplayYsec();

        void _draw_cross(double radius);
        void _draw_triangle(double radius);

        pnt3d polar2xyz(double lon, double lat, double r);
        pnt3d polar2flat(double lon, double lat, double h);
        pnt3d nvop(pnt3d p, double height);
};
#endif

