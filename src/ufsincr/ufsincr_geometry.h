#ifndef _UFSINCR_Geometry_H
#define _UFSINCR_Geometry_H

#include <QtOpenGL>

#include <iostream>
#include <vector>

#include <stdlib.h>
#include <assert.h>

using namespace std;

class UFSINCRGeometry
{
    public:
        UFSINCRGeometry(int nlon, int nlat, float* lon, float* lat);
       ~UFSINCRGeometry();

        double* get_xSphere() { return _xSphere; };
        double* get_ySphere() { return _ySphere; };
        double* get_zSphere() { return _zSphere; };

        double* get_xFlat() { return _xFlat; };
        double* get_yFlat() { return _yFlat; };

        float* get_geolon() { return _geolon; };
        float* get_geolat() { return _geolat; };

	int getNlon() { return _nlon; };
	int getNlat() { return _nlat; };

    protected:
        void _setup();

        double* _xSphere;
        double* _ySphere;
        double* _zSphere;

        double* _xFlat;
        double* _yFlat;

        float* _geolon;
        float* _geolat;

	int _nlon;
	int _nlat;
};
#endif

