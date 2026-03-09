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

        double* get_xSphere() { return _xSphere; };
        double* get_ySphere() { return _ySphere; };
        double* get_zSphere() { return _zSphere; };

        double* get_xFlat() { return _xFlat; };
        double* get_yFlat() { return _yFlat; };

      //UFS
        void setup_ufs_();

        void set_ufs__lon(double* v) { _ufs__lon = v; };
        void set_ufs__lat(double* v) { _ufs__lat = v; };
        void set_ufs__lev(int n) { _ufs__lev = n; };
        void set_ufs__ncol(int n) { _ufs__ncol = n; };
        void set_ufs__ncenters(int n) { _ufs__ncenters = n; };
        void set_ufs__ncorners(int n) { _ufs__ncorners = n; };
        void set_ufs__element_corners(int* ecs) { _ufs__element_corners = ecs; };

        double* get_ufs__lon() { return _ufs__lon; };
        double* get_ufs__lat() { return _ufs__lat; };
        int* get_ufs__element_corners() { return _ufs__element_corners; };
        int  get_ufs__ncenters() { return _ufs__ncenters; };
        int  get_ufs__ncorners() { return _ufs__ncorners; };
        int  get_ufs__ncol() { return _ufs__ncol; };
        int  get_ufs__lev() { return _ufs__lev; };

    protected:
        string name;

        int  _nz;
        int  _nt;
        int  _nm;
        int  _nTime;

        double* _xSphere;
        double* _ySphere;
        double* _zSphere;

        double _hmax;
        double _hmin;

        double* _xFlat;
        double* _yFlat;

      //for UFS
        double* _ufs__lon;
        double* _ufs__lat;

        int* _ufs__element_corners;
        int  _ufs__ncenters;
        int  _ufs__ncorners;
        int  _ufs__ncol;
        int  _ufs__lev;
        
    private:
        void _set_default();
};
#endif

