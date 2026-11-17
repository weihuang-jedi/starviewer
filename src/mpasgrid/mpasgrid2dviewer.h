#ifndef MPASGrid2DVIEWER_H
#define MPASGrid2DVIEWER_H

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
#include "mpasgridgeometry.h"
#include "mpasgridreader.h"
#include "evaluator.h"
#include "locator.h"
#include "nvoptions.h"

using namespace std;

class MPASGrid2dViewer
{
    public:
        MPASGrid2dViewer(ColorTable* ct, NVOptions* opt);
       ~MPASGrid2dViewer();

        void draw();
        void draw_sphere_grids();
        void draw_plane_grids();

        void reset();
        void setup(string vn, int* var);
        void set_geometry(MPASGridGeometry* gm);
        void set_mpaspartition(MPASGridReader* pg) { mpaspartition = pg; };
        void reset_texture1d(ColorTable *ct);

        void set_tvalue(int n) { _tvalue = n; };

        void set_locator(Locator* l) { locator = l; };

        int get_minval() { return _minval; };
        int get_maxval() { return _maxval; };

    protected:
        Locator* locator;
        Lister* lister;
        Texture1d* texture1d;
        CoastLine* coastline;
        ColorTable* colorTable;
        MPASGridGeometry* geometry;
        MPASGridReader* mpaspartition;
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

        int* _var;

        int  _minval, _workMinVal;
        int  _maxval, _workMaxVal;

        void _initialize();
        void _evaluate(int* var);

        void _flatSubset();
        void _sphereSubset();

        void _flatDisplay();

        void _sphereDisplay();

        void _draw_cross(double radius);
        void _draw_triangle(double radius);

        pnt3d latlon2xyz(double lon, double lat);
        pnt3d latlon2flat(double lon, double lat);
        pnt3d nvop(pnt3d p, double height);
};
#endif

