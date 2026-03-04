#ifndef _POPGeometry_H
#define _POPGeometry_H

//$Id: popgeometry.h 4833 2013-11-05 15:49:31Z starviewer $

#include <QtOpenGL>

#include <iostream>
#include <vector>

#include <stdlib.h>
#include <assert.h>

using namespace std;

class POPGeometry
{
    public:
        POPGeometry();
       ~POPGeometry();

        void print();
        void reset();
        void reset_dimension();

        void setup();

        bool hasFillValue() { return _hasFillValue; };

        void set_fillValue(double v) { _fillValue = v; };
        double get_fillValue() { return _fillValue; };

        double* get_pop_ulon() { return _pop_ulon; };
        double* get_pop_ulat() { return _pop_ulat; };
        double* get_pop_tlon() { return _pop_tlon; };
        double* get_pop_tlat() { return _pop_tlat; };
        double* get_pop_z_w()  { return _pop_z_w; };

        int get_pop_nlon() { return _pop_nlon; };
        int get_pop_nlat() { return _pop_nlat; };
        int get_pop_nlev() { return _pop_nlev; };

        void set_pop_ulon(double* v) { _pop_ulon = v; };
        void set_pop_ulat(double* v) { _pop_ulat = v; };
        void set_pop_tlon(double* v) { _pop_tlon = v; };
        void set_pop_tlat(double* v) { _pop_tlat = v; };
        void set_pop_z_w(double* v)  { _pop_z_w = v; };

        void set_pop_nlon(int n) { _pop_nlon = n; };
        void set_pop_nlat(int n) { _pop_nlat = n; };
        void set_pop_nlev(int n) { _pop_nlev = n; };

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

        double* _pop_ulon;
        double* _pop_ulat;
        double* _pop_tlon;
        double* _pop_tlat;
        double* _pop_z_w;

        int _pop_nlon;
        int _pop_nlat;
        int _pop_nlev;

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

