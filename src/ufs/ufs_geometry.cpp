#include "ufs_geometry.h"

UFSGeometry::UFSGeometry()
{
    _set_default();
}

UFSGeometry::~UFSGeometry()
{
    if(NULL != _xSphere)
        delete [] _xSphere;
    if(NULL != _ySphere)
        delete [] _ySphere;
    if(NULL != _zSphere)
        delete [] _zSphere;

    if(NULL != _xFlat)
        delete [] _xFlat;
    if(NULL != _yFlat)
        delete [] _yFlat;

    if(NULL != _lon)
        delete [] _lon;
    if(NULL != _lat)
        delete [] _lat;
    if(NULL != _lev)
        delete [] _lev;
}

void UFSGeometry::_set_default()
{
    if(NULL != _xSphere)
        delete [] _xSphere;
    if(NULL != _ySphere)
        delete [] _ySphere;
    if(NULL != _zSphere)
        delete [] _zSphere;

    if(NULL != _xFlat)
        delete [] _xFlat;
    if(NULL != _yFlat)
        delete [] _yFlat;

    if(NULL != _lon)
        delete [] _lon;
    if(NULL != _lat)
        delete [] _lat;
    if(NULL != _lev)
        delete [] _lev;

    _xSphere = NULL;
    _ySphere = NULL;
    _zSphere = NULL;

    _lon = NULL;
    _lat = NULL;
    _lev = NULL;

    _xFlat = NULL;
    _yFlat = NULL;

    reset();
} 

void UFSGeometry::reset_dimension()
{
    _nlon = 1;
    _nlat = 1;
    _nlev = 1;
}

void UFSGeometry::reset()
{
    reset_dimension();
}

void UFSGeometry::print()
{
   cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
   cout << "Info of <" << name << ">:" << endl;
}

void UFSGeometry::setup()
{
    double pi = 3.1415926535897932;
    double arc = pi / 180.0;
    double delt;
    int i, j;
    size_t n;
    size_t nsquare;

    nsquare = _nlon * _nlat;
    _xSphere = new double[nsquare];
    _ySphere = new double[nsquare];
    _zSphere = new double[nsquare];

    _xFlat = new double[_nlon];
    _yFlat = new double[_nlat];

    _hmax = 0.0;
    _hmin = 10000.0;
    for(j = 0; n < _nlat; ++j) {
        _yFlat[j] = _lat[j]/180.0;
	n = j*_nlon;
        delt = cos(_lat[j] * arc);
        for(i = 0; i < _nlon; ++i)
        {
            _xSphere[n+i] = delt * sin(_lon[i] * arc);
            _ySphere[n+i] =        sin(_lat[j] * arc);
            _zSphere[n+i] = delt * cos(_lon[i] * arc);
        }
    }

    for(i = 0; i < _nlon; ++i)
    {
        if(_lon[i] > 180.0)
            _xFlat[i] = _lon[i]/180.0 - 2.0;
        else
            _xFlat[i] = _lon[i]/180.0;
    }
}

