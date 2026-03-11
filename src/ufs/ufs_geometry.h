#ifndef _UFSGeometry_H
#define _UFSGeometry_H

#include <QtOpenGL>

#include <iostream>
#include <vector>

#include <stdlib.h>
#include <assert.h>

using namespace std;

class UFSGeometry : public Geometry
{
    public:
        UFSGeometry();
       ~UFSGeometry();

        int  get_nlon() { return _nlon; };
        int  get_nlat() { return _nlat; };
        int  get_nlev() { return _nlev; };
        int  get_ntim() { return _ntim; };

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

        void set_lon(double* v);
        void set_lat(double* v);

    protected:
        string name;

        float* _xSphere;
        float* _ySphere;
        float* _zSphere;

        float _hmax;
        float _hmin;

        float* _xFlat;
        float* _yFlat;

    private:
        void _set_default();
};
#endif

