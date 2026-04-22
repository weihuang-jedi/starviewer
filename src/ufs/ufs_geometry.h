#ifndef _UFSGeometry_H
#define _UFSGeometry_H

#include <QtOpenGL>

#include <iostream>
#include <vector>

#include <stdlib.h>
#include <assert.h>

#include <geometry.h>

using namespace std;

class UFSGeometry : public Geometry {
    public:
        UFSGeometry();
       ~UFSGeometry();


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

      //UFS
        void setup();

	void set_ntim(int v) { _ntim = v; };
	int get_ntim() { return _ntim; };
	int get_hlon() { return _hlon; };

    protected:
        string name;

        double* _xSphere;
        double* _ySphere;
        double* _zSphere;

        double* _xFlat;
        double* _yFlat;

        float _hmax;
        float _hmin;

	int _hlon;

    private:
        void _set_default();

	int _ntim;
};
#endif

