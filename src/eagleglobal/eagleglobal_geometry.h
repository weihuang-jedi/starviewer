#ifndef _EagleGlobalGeometry_H
#define _EagleGlobalGeometry_H

#include <QtOpenGL>

#include <iostream>
#include <vector>

#include <stdlib.h>
#include <assert.h>

#include <geometry.h>

using namespace std;

class EagleGlobalGeometry : public Geometry {
    public:
        EagleGlobalGeometry();
       ~EagleGlobalGeometry();

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

        double* get_longitude() { return _longitude; };
        double* get_latitude() { return _latitude; };

	void set_longitude(double *v) { _longitude = v; };
	void set_latitude(double *v) { _latitude = v; };

      //EagleGlobal
        void setup();

	void set_ntime(int v) { _ntime = v; };
	int get_ntime() { return _ntime; };

	int get_hlon() { return _hlon; };

    protected:
        string name;

        double* _xSphere;
        double* _ySphere;
        double* _zSphere;

        double* _xFlat;
        double* _yFlat;

        double* _longitude;
        double* _latitude;

	int _hlon;

    private:
        void _set_default();

	int _ntime;
	int _nforecast_reference_time;
};
#endif

