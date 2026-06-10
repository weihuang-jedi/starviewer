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

        int* get_tile() { return _tile; };

      //UFSincr
        void setup();

	void set_nx(int v) { _nx = v; };
	void set_ny(int v) { _ny = v; };
	int get_nx() { return _nx; };
	int get_ny() { return _ny; };

	void set_nlev(int v) { _nlev = v; };
	void set_ntim(int v) { _ntim = v; };
	int get_nlev() { return _nlev; };
	int get_ntim() { return _ntim; };

	void set_ntiles(int v) { _ntiles = v; };
	int get_ntiles() { return _ntiles; };

    protected:
        string name;

        double* _xSphere;
        double* _ySphere;
        double* _zSphere;

        double* _xFlat;
        double* _yFlat;

        double* _lon2d;
        double* _lat2d;
        double* _time;

        float* _pfull;
        float* _phalf;

    private:
        void _set_default();

	int _nx;
	int _ny;
	int _nlev;
	int _ntim;
	int _ntiles;

	int* _tile;
};
#endif

