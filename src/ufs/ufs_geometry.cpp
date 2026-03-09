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
}

void UFSGeometry::_set_default()
{
    _xSphere = NULL;
    _ySphere = NULL;
    _zSphere = NULL;

    _xFlat = NULL;
    _yFlat = NULL;

    reset();
} 

void UFSGeometry::reset_dimension()
{
    _nz = 1;

    _ufs__lev = 1;
  //_ufs__ncol = 1;
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

void UFSGeometry::setup_ufs_()
{
    double pi = 3.1415926535897932;
    double arc = pi / 180.0;
    double delt;
    int n;

    _xSphere = new double[_ufs__ncol];
    _ySphere = new double[_ufs__ncol];
    _zSphere = new double[_ufs__ncol];

    _xFlat = new double[_ufs__ncol];
    _yFlat = new double[_ufs__ncol];

    _hmax = 0.0;
    _hmin = 10000.0;
    for(n = 0; n < _ufs__ncol; ++n)
    {
        delt = cos(_ufs__lat[n] * arc);
        _xSphere[n] = delt * sin(_ufs__lon[n] * arc);
        _ySphere[n] =        sin(_ufs__lat[n] * arc);
        _zSphere[n] = delt * cos(_ufs__lon[n] * arc);

        if(_ufs__lon[n] > 180)
            _xFlat[n] = _ufs__lon[n]/180.0 - 2.0;
        else
            _xFlat[n] = _ufs__lon[n]/180.0;
        _yFlat[n] = _ufs__lat[n]/180.0;
    }
}

