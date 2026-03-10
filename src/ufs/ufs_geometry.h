#ifndef _UFSGeometry_H
#define _UFSGeometry_H

#include <QtOpenGL>

#include <iostream>
#include <vector>

#include <stdlib.h>
#include <assert.h>

using namespace std;

class UFSGeometry
{
    public:
        UFSGeometry();
       ~UFSGeometry();

        int  get_nz() { return _nz; };
        int  get_nt() { return _nt; };

        void  set_nz(int nz) { _nz = nz; };
        void  set_nt(int nt) { _nt = nt; };

        string get_name() { return name; };

        void  set_name(string vn) { name = vn; };

        void print();
        void reset();
        void reset_dimension();

        float* get_xSphere() { return _xSphere; };
        float* get_ySphere() { return _ySphere; };
        float* get_zSphere() { return _zSphere; };

        float* get_xFlat() { return _xFlat; };
        float* get_yFlat() { return _yFlat; };

      //UFS
        void setup();

        void set_lon(float* v) { _lon = v; };
        void set_lat(float* v) { _lat = v; };
        void set_lev(float* v) { _lev = v; };

        float* get_lon() { return _lon; };
        float* get_lat() { return _lat; };
        float* get_lev() { return _lev; };

    protected:
        string name;

        float* _xSphere;
        float* _ySphere;
        float* _zSphere;

        float _hmax;
        float _hmin;

        float* _xFlat;
        float* _yFlat;

        float* _lon;
        float* _lat;
        float* _lev;

        int _nlon;
        int _nlat;
        int _ulev;
        int _ntim;
        
    private:
        void _set_default();
};
#endif

