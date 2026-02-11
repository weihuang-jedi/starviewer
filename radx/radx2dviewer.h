#ifndef _Radx2DVIEWER_H
#define _Radx2DVIEWER_H

//$Id: radx2dviewer.h 4974 2014-02-13 23:10:57Z starviewer $

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
#include "locator.h"
#include "nvoptions.h"
#include "radxreader.h"

using namespace std;

class Radx2dViewer
{
    public:
        Radx2dViewer(ColorTable* ct, NVOptions* opt);
       ~Radx2dViewer();

        void set_radx(RadxReader* r);

        void draw();

        void reset();
        void setup(string vn);

        void set_locator(Locator* l) { locator = l; };
        void set_colorTable(ColorTable* ct) { colorTable = ct; };

        void get_location(double &blon, double &blat, double &balt);

        double get_minval() { return _minval; };
        double get_maxval() { return _maxval; };

        int get_nsweeps() { return nsweeps; };

        string get_longName() { return _longName; };
        string get_standardName() { return _standardName; };
        string get_units() { return _units; };

    protected:
        Locator* locator;
        Lister* lister;
        ColorTable* colorTable;
        NVOptions* nvoptions;
        RadxReader* radx;

        RadxVol* volume;
        vector<RadxSweep *> sweeps;
        vector<RadxRay *> rays;

        string _varname;
        string _longName;
        string _standardName;
        string _units;

        int** _zlist;

        int isweep;
        int nsweeps;

        int    _colorlen;
        float* _colormap;

        double _missing;
        double _minval;
        double _maxval;

        double _upbound;
        double _lowbound;

        double _base_lon, _base_lat, _base_alt;

        void _initialize();
        void _set_minmax();

        void _draw_cross(double dist);

        void _draw_rect(double x[4], double y[4], double z[4]);
        void _draw_grid();

        void _fill_quad(double x[4], double y[4],
                        double z[4], float value);
        void _display();

        void _set_color(float val);
        void _set_color3d(float val);

        void _fill_cube(double x[8], double y[8],
                        double z[8], float value);
        void _display3d();
};
#endif

