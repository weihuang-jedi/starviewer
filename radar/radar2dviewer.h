#ifndef _RADAR2DVIEWER_H
#define _RADAR2DVIEWER_H

//$Id: radar2dviewer.h 14599 2013-10-21 15:00:21Z starviewer $

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
#include "radarreader.h"
#include "locator.h"
#include "nvoptions.h"

using namespace std;

class RADAR2dViewer
{
    public:
        RADAR2dViewer(ColorTable* ct, NVOptions* opt);
       ~RADAR2dViewer();

        void set_radar(RADARReader* r);

        void draw();

        void reset();
        void setup(string vn);

        void set_locator(Locator* l) { locator = l; };
        void set_colorTable(ColorTable* ct) { colorTable = ct; };

        void get_location(double &blon, double &blat);

        float get_minval() { return _minval; };
        float get_maxval() { return _maxval; };

        int get_nsweeps() { return nsweeps; };

    protected:
        Locator* locator;
        Lister* lister;
        ColorTable* colorTable;
        NVOptions* nvoptions;
        RADARReader* radar;
        Volume* volume;
        Sweep* sweep;
        Ray* ray;

        string _varname;

        int** _zlist;

        int isweep;
        int nsweeps;

        int    _colorlen;
        float* _colormap;

        float _minval;
        float _maxval;

        float _upbound;
        float _lowbound;

        double _base_lon, _base_lat;

        void _initialize();

        void _draw_cross(double dist);

        void _draw_rect(double x[4], double y[4], double z[4]);
        void _draw_grid();

        void _set_minmax();

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

