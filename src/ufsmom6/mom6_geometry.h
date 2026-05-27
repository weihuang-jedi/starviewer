#ifndef _MOM6Geometry_H
#define _MOM6Geometry_H

#include <QtOpenGL>

#include <iostream>
#include <vector>

#include <stdlib.h>
#include <assert.h>

#include <geometry.h>

using namespace std;

class MOM6Geometry : public Geometry {
    public:
        MOM6Geometry();
       ~MOM6Geometry();

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

      //MOM6
        void setup();

	void set_ntime(int v) { _ntime = v; };
	void set_nxh(int v) { _nxh = v; };
	void set_nyh(int v) { _nyh = v; };
	void set_nxg(int v) { _nxg = v; };
	void set_nyg(int v) { _nyg = v; };
	void set_nzl(int v) { _nzl = v; };
	void set_nzi(int v) { _nzi = v; };

	void set_geolon(float* v) { _geolon = v; };
	void set_geolat(float* v) { _geolat = v; };
	void set_zl(double* v) { _zl = v; };

	// void set_z_l(double* v) { _z_l = v; };
	// void set_z_i(double* v) { _z_i = v; };

	int get_ntime() { return _ntime; };
	int get_nxh() { return _nxh; };
	int get_nyh() { return _nyh; };
	int get_nxg() { return _nxg; };
	int get_nyg() { return _nyg; };
	int get_nzl() { return _nzl; };
	int get_nzi() { return _nzi; };
	int get_hlon() { return _hlon; };

    protected:
        string name;

        double* _xSphere;
        double* _ySphere;
        double* _zSphere;

        double* _xFlat;
        double* _yFlat;

        double* _zl;

        float* _geolon;
        float* _geolat;

	int _hlon;

    private:
        void _set_default();

	int _ntime;
	int _nxh;
	int _nyh;
	int _nxg;
	int _nyg;
	int _nzl;
	int _nzi;
};
#endif

