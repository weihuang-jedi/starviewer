#ifndef _Locator_H
#define _Locator_H

// $Id: locator.h 4833 2013-11-05 15:49:31Z starviewer $

#include <stdlib.h>
#include <assert.h>

#include <iostream>

#ifndef NOUNDERSCORE
#define	GCINOUT	gcinout_
#else
#define	GCINOUT	gcinout
#endif

extern "C"
{
    int GCINOUT(double* plat, double* plon,
                double* qlat, double* qlon,
                int* npts, double* work);
}

class Locator
{
    public:
        Locator();
        Locator(int x, int y, int z = 0);
       ~Locator();

        void set_x(int n) { _x = n; };
        void set_y(int n) { _y = n; };
        void set_z(int n) { _z = n; };

        int x() { return _x; };
        int y() { return _y; };
        int z() { return _z; };

        int get_x() { return _x; };
        int get_y() { return _y; };
        int get_z() { return _z; };

        void add_x(int n) { _x += n; };
        void add_y(int n) { _y += n; };
        void add_z(int n) { _z += n; };

        bool on() { return _on; };
        void turnOn() { _on = true; };
        void turnOff() { _on = false; };

        void set_distance(double d) { _distance = d; };
        void set_height(double d) { _height = d; };
        void set_dim(double d) { _dim = d; };
        void set_fovy(double d) { _fovy = d; };
        void set_zfar(double d) { _zfar = d; };
        void set_znear(double d) { _znear = d; };

        double get_distance() { return _distance; };
        double get_height() { return _height; };
        double get_dim() { return _dim; };
        double get_fovy() { return _fovy; };
        double get_zfar() { return _zfar; };
        double get_znear() { return _znear; };

        int pinout(double plat, double plon,
                   double* qlat, double* qlon, int npts);

    protected:
        int _x, _y, _z;

        bool _on;

        double _height;
        double _distance;

        double _fovy;
        double _znear;
        double _zfar;
        double _dim;
};
#endif

