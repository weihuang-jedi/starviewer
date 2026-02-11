#ifndef _POP2DVIEWER_H
#define _POP2DVIEWER_H

//$Id: pop2dviewer.h 4833 2013-11-05 15:49:31Z starviewer $

#include <iostream>
#include <vector>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "texture1d.h"
#include "coastline.h"
#include "colorTable.h"
#include "nvoptions.h"
#include "popgeometry.h"
#include "locator.h"

using namespace std;

class POP2dViewer
{
    public:
        POP2dViewer(ColorTable* ct, NVOptions* opt);
       ~POP2dViewer();

        void draw();

        void reset();
        void setup(string vn, double* var);
        void set_geometry(POPGeometry* gm);

        void set_xsec(int n) { _xsec = n; };
        void set_ysec(int n) { _ysec = n; };
        void set_zlevel(int n) { _zlevel = n; };

        void reset_texture1d(ColorTable *ct);
        void set_coastline(CoastLine* cl) { coastline = cl; };
        void set_locator(Locator* l) { locator = l; };

        double get_valmin() { return _valmin; };
        double get_valmax() { return _valmax; };

    protected:
        Locator* locator;
      //Lister* lister;
        ColorTable* colorTable;
        NVOptions* nvoptions;
        CoastLine* coastline;
        Texture1d* texture1d;
        POPGeometry* geometry;

        string _varname;

        int _xsec;
        int _ysec;
        int _zlevel;

        int* _xlist;
        int* _ylist;
        int* _zlist;

        int nlon;
        int nlat;
        int nlev;
        int mlon;
        int mlat;
        int mlev;

        double* lon;
        double* lat;
        double* z_w;

        double** _ux2d;
        double** _uy2d;

        double** _tx2d;
        double** _ty2d;

        double* _var;

        double deg2rad;

        double _valmin;
        double _valmax;
        double _vscale;
        double _fillValue;

        void _initialize();
        void _evaluate(double* var);

        void _displayXsphere(int xs);
        void _displayYsphere(int ys);
        void _displayZsphere(int zs);

        void _displayXflat(int xs);
        void _displayYflat(int ys);
        void _displayZflat(int zs);

        void _lonlat2xyz(double lon, double lat, double radius, double f);
        void _lonlat2flat(double lon, double lat, double radius, double f);
};
#endif

