#ifndef _Geometry_H
#define _Geometry_H

//$Id: geometry.h 4833 2013-11-05 15:49:31Z starviewer $

#include <iostream>

#include <stdlib.h>
#include <assert.h>

#include "basegeometry.h"

using namespace std;

class Geometry : public BaseGeometry
{
    public:
        Geometry();
       ~Geometry();

        void printinfo();
        void reset();
        void reset_dimension();

        void set_nlon(int v) { _nlon = v; };
        void set_nlat(int v) { _nlat = v; };
        void set_nlev(int v) { _nlev = v; };

        void set_lon(double* v) { _lon = v; };
        void set_lat(double* v) { _lat = v; };
        void set_lev(float* v) { _lev = v; };

        double* get_lon() { return _lon; };
        double* get_lat() { return _lat; };
        float* get_lev() { return _lev; };

        void set_lon2d(double* v) { _lon2d = v; };
        void set_lat2d(double* v) { _lat2d = v; };

        double* get_lon2d() { return _lon2d; };
        double* get_lat2d() { return _lat2d; };

        void set_has1dLon(bool v) { _has1dLon = v; };
        void set_has1dLat(bool v) { _has1dLat = v; };
        void set_has1dLev(bool v) { _has1dLev = v; };

        bool get_has1dLon() { return _has1dLon; };
        bool get_has1dLat() { return _has1dLat; };
        bool get_has1dLev() { return _has1dLev; };

        void set_has2dLon(bool v) { _has2dLon = v; };
        void set_has2dLat(bool v) { _has2dLat = v; };

        bool get_has2dLon() { return _has2dLon; };
        bool get_has2dLat() { return _has2dLat; };

        int get_nlabels() { return nlabels; };

        float* get_xbvalues() { return xbvalues; }
        float* get_ylvalues() { return ylvalues; }

        char** get_xblabels() { return xblabels; }
        char** get_yllabels() { return yllabels; }

    protected:
        bool _has1dLon;
        bool _has1dLat;
        bool _has1dLev;

        bool _has2dLon;
        bool _has2dLat;

        double* _lon;
        double* _lat;
        float* _lev;

        double* _lon2d;
        double* _lat2d;

        int   _nlon;
        int   _nlat;
        int   _nlev;

        int   nlabels;

        float* xbvalues;
        float* ylvalues;

        char** xblabels;
        char** yllabels;

        void _set_default();
};
#endif

