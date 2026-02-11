#ifndef _VTK2DVIEWER_H_
#define _VTK2DVIEWER_H_

//$Id: vtk2dviewer.h 4865 2014-01-01 15:49:23Z starviewer $

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
#include "vtkreader.h"
#include "locator.h"
#include "nvoptions.h"

using namespace std;

class VTK2dViewer
{
    public:
        VTK2dViewer(ColorTable* ct, NVOptions* opt);
        virtual ~VTK2dViewer();

        void draw();

        void reset();
        void setup(string vn);
        void set_reader(VTKReader* r);

        void set_locator(Locator* l) { locator = l; };
        void set_colorTable(ColorTable* ct) { colorTable = ct; };

        float get_minval() { return _minval; };
        float get_maxval() { return _maxval; };

    protected:
        Locator* locator;
        Lister* lister;
        ColorTable* colorTable;
        NVOptions* nvoptions;
        VTKReader* reader;
        vtkPolyData* polyData;

        string _varname;

        int** _zlist;

        int    _colorlen;
        float* _colormap;

        float _minval;
        float _maxval;

        void _initialize();

        void _draw_cross(double dist);

        void _draw_rect(double x[4], double y[4], double z[4]);
        void _draw_grid();

        void _set_minmax();

        void _fill_quad(double x[4], double y[4],
                        double z[4], double v[4]);
        void _display();

        void _set_color(double val);

        void _latlon2polar(double lat, double lon, double r, double &x, double &y, double &z);
        void _latlon2xy(double lat, double lon, double &x, double &y);
};
#endif

