#ifndef _UFSincrGeometry_H
#define _UFSincrGeometry_H

#include <QtOpenGL>

#include <iostream>
#include <vector>

#include <stdlib.h>
#include <assert.h>

#include <geometry.h>

using namespace std;

class UFSincrGeometry : public Geometry {
    public:
        UFSincrGeometry();
       ~UFSincrGeometry();


        string get_name() { return name; };

        void  set_name(string vn) { name = vn; };

        void print();
        void reset();
        void reset_dimension();

        double* get_xSphere() { return _xSphere; };
        double* get_ySphere() { return _ySphere; };
        double* get_zSphere() { return _zSphere; };

        double* get_xFlat() { return _xFlat; };
        double* get_yFlat() { return _yFlat; };

      //UFSincr
        void setup();

	void set_ntim(int v) { _ntim = v; };
	int get_ntim() { return _ntim; };

    protected:
        string name;

        double* _xSphere;
        double* _ySphere;
        double* _zSphere;

        double* _xFlat;
        double* _yFlat;

        double* _lon;
        double* _lat;
        double* _time;

        float* _pfull;
        float* _phalf;

    private:
        void _set_default();

	int _nx;
	int _ny;
	int _np;
	int _ntim;
	int _ntile;

	int* _tile;
};
#endif

