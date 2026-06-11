#ifndef _EAGLEGeometry_H
#define _EAGLEGeometry_H

#include <QtOpenGL>

#include <iostream>
#include <vector>

#include <stdlib.h>
#include <assert.h>

#include <geometry.h>

using namespace std;

class EAGLEGeometry : public Geometry {
    public:
        EAGLEGeometry();
       ~EAGLEGeometry();


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

      //EAGLE
        void setup();

	void set_ntime(int v) { _ntime = v; };
	int get_ntime() { return _ntime; };

    protected:
        string name;

        double* _xSphere;
        double* _ySphere;
        double* _zSphere;

        double* _xFlat;
        double* _yFlat;

    private:
        void _set_default();

	int _nx;
	int _ny;
	int _ntime;
};
#endif

