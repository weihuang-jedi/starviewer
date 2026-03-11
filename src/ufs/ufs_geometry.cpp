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
    _ntim = 1;
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
    float pi = 3.1415926535897932;
    float arc = pi / 180.0;
    float delt;
    int i, j, n;
    size_t nsqure;

    nsquare = _nlon * _nlat;
    _xSphere = new float[nsquare];
    _ySphere = new float[nsquare];
    _zSphere = new float[nsquare];

    _xFlat = new float[nsquare];
    _yFlat = new float[nsquare];

    _hmax = 0.0;
    _hmin = 10000.0;
    for(n = 0; n < _ufs__ncol; ++n)
    {
        delt = cos(_ufs__lat[n] * arc);
        _xSphere[n] = delt * sin(_lon[n] * arc);
        _ySphere[n] =        sin(_lat[n] * arc);
        _zSphere[n] = delt * cos(_lon[n] * arc);

        if(_ufs__lon[n] > 180)
            _xFlat[n] = _lon[n]/180.0 - 2.0;
        else
            _xFlat[n] = _lon[n]/180.0;
        _yFlat[n] = _lat[n]/180.0;
    }
}

