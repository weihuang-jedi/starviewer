#include "camsegeometry.h"

CAMseGeometry::CAMseGeometry()
{
    _set_default();
}

CAMseGeometry::~CAMseGeometry()
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

void CAMseGeometry::_set_default()
{
    _xSphere = NULL;
    _ySphere = NULL;
    _zSphere = NULL;

    _xFlat = NULL;
    _yFlat = NULL;

    reset();
} 

void CAMseGeometry::reset_dimension()
{
    _nz = 1;

    _camse_lev = 1;
  //_camse_ncol = 1;
}

void CAMseGeometry::reset()
{
    reset_dimension();
}

void CAMseGeometry::print()
{
   cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
   cout << "Info of <" << name << ">:" << endl;
}

void CAMseGeometry::setup_camse()
{
    double pi = 3.1415926535897932;
    double arc = pi / 180.0;
    double delt;
    int n;

    _xSphere = new double[_camse_ncol];
    _ySphere = new double[_camse_ncol];
    _zSphere = new double[_camse_ncol];

    _xFlat = new double[_camse_ncol];
    _yFlat = new double[_camse_ncol];

    _hmax = 0.0;
    _hmin = 10000.0;
    for(n = 0; n < _camse_ncol; ++n)
    {
        delt = cos(_camse_lat[n] * arc);
        _xSphere[n] = delt * sin(_camse_lon[n] * arc);
        _ySphere[n] =        sin(_camse_lat[n] * arc);
        _zSphere[n] = delt * cos(_camse_lon[n] * arc);

        if(_camse_lon[n] > 180)
            _xFlat[n] = _camse_lon[n]/180.0 - 2.0;
        else
            _xFlat[n] = _camse_lon[n]/180.0;
        _yFlat[n] = _camse_lat[n]/180.0;
    }
}

