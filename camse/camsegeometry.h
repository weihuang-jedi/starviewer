#ifndef _CAMseGeometry_H
#define _CAMseGeometry_H

#include <QtOpenGL>

#include <iostream>
#include <vector>

#include <stdlib.h>
#include <assert.h>

using namespace std;

class CAMseGeometry
{
    public:
        CAMseGeometry();
       ~CAMseGeometry();

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

      //CAMse
        void setup_camse();

        void set_camse_lon(double* v) { _camse_lon = v; };
        void set_camse_lat(double* v) { _camse_lat = v; };
        void set_camse_lev(int n) { _camse_lev = n; };
        void set_camse_ncol(int n) { _camse_ncol = n; };
        void set_camse_ncenters(int n) { _camse_ncenters = n; };
        void set_camse_ncorners(int n) { _camse_ncorners = n; };
        void set_camse_element_corners(int* ecs) { _camse_element_corners = ecs; };

        double* get_camse_lon() { return _camse_lon; };
        double* get_camse_lat() { return _camse_lat; };
        int* get_camse_element_corners() { return _camse_element_corners; };
        int  get_camse_ncenters() { return _camse_ncenters; };
        int  get_camse_ncorners() { return _camse_ncorners; };
        int  get_camse_ncol() { return _camse_ncol; };
        int  get_camse_lev() { return _camse_lev; };

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

      //for CAMse
        double* _camse_lon;
        double* _camse_lat;

        int* _camse_element_corners;
        int  _camse_ncenters;
        int  _camse_ncorners;
        int  _camse_ncol;
        int  _camse_lev;
        
    private:
        void _set_default();
};
#endif

