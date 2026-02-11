#ifndef _HDFGeometry_H
#define _HDFGeometry_H

//$Id: hdfgeometry.h 5246 2014-12-07 16:56:07Z starviewer $

#include <QtOpenGL>

#include <iostream>
#include <vector>

#include <stdlib.h>
#include <assert.h>

using namespace std;

class HDFGeometry
{
    public:
        HDFGeometry();
       ~HDFGeometry();

        void print();
        void reset();
        void reset_dimension();

        void setup();

        bool hasFillValue() { return _hasFillValue; };

        void set_fillValue(double v) { _fillValue = v; };
        double get_fillValue() { return _fillValue; };

        double* get_lon() { return _lon; };
        double* get_lat() { return _lat; };
        double* get_z()  { return _z; };

        int get_nlon() { return _nlon; };
        int get_nlat() { return _nlat; };
        int get_nlev() { return _nlev; };

        void set_lon(double* v) { _lon = v; };
        void set_lat(double* v) { _lat = v; };
        void set_z(double* v)  { _z = v; };

        void set_nlon(int n) { _nlon = n; };
        void set_nlat(int n) { _nlat = n; };
        void set_nlev(int n) { _nlev = n; };

        int get_nTime() { return _nTime; };
        void set_nTime(int n) { _nTime = n; };

        bool isUpoint() { return _upoint; };
        bool isTpoint() { return _tpoint; };

        void set_name(string nm) { _name = nm; };
        string get_name() { return _name; };

        void set_Upoint() { _upoint = true; };
        void set_Tpoint() { _tpoint = true; };
        void set_hasFillValue(bool v) { _hasFillValue = v; };

    protected:
        string _name;

        double _fillValue;

        int _nTime;

        double* _lon;
        double* _lat;
        double* _z;

        int _nlon;
        int _nlat;
        int _nlev;

        bool _upoint;
        bool _tpoint;
        bool _hasFillValue;

      //int nlabels;

      //float* xbvalues;
      //float* ylvalues;

      //char** xblabels;
      //char** yllabels;
};
#endif

