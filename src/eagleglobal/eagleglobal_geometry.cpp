#include "eagleglobal_geometry.h"

EagleGlobalGeometry::EagleGlobalGeometry()
{
    _set_default();
}

EagleGlobalGeometry::~EagleGlobalGeometry()
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

    if(NULL != _longitude)
        delete [] _longitude;
    if(NULL != _latitude)
        delete [] _latitude;
}

void EagleGlobalGeometry::_set_default()
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

    if(NULL != _longitude)
        delete [] _lon2d;
    if(NULL != _latitude)
        delete [] _latitude;

    _xSphere = NULL;
    _ySphere = NULL;
    _zSphere = NULL;

    _longitude = NULL;
    _latitude = NULL;

    _xFlat = NULL;
    _yFlat = NULL;

    reset();
} 

void EagleGlobalGeometry::reset_dimension()
{
    _hlon = -1;
    _nlon = 1;
    _nlat = 1;
    _ntime = 1;
}

void EagleGlobalGeometry::reset()
{
    reset_dimension();
}

void EagleGlobalGeometry::print()
{
   cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
   cout << "Info of <" << name << ">:" << endl;
}

void EagleGlobalGeometry::setup()
{
    double pi = 3.1415926535897932;
    double arc = pi / 180.0;
    double delt;
    int i, j;
    size_t n;
    size_t nsquare;

    // cout << "Enter functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    // cout << "_nlon = " << _nlon << endl;
    // cout << "_nlat = " << _nlat << endl;
    // cout << "_ntime = " << _ntime << endl;
    nsquare = _nlon * _nlat;
    _xSphere = new double[nsquare];
    _ySphere = new double[nsquare];
    _zSphere = new double[nsquare];
    // cout << "functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;

    _xFlat = new double[_nlon];
    _yFlat = new double[_nlat];

    // cout << "functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    if(NULL == _latitude) {
	cout << "_latitude is NULL. Stop" << endl;
	throw(errno);
	exit (-1);
    }
    if(NULL == _longitude) {
	cout << "_longitude is NULL. Stop" << endl;
	throw(errno);
	exit (-1);
    }

    for(j = 0; j < _nlat; ++j) {
	n = j*_nlon;
        _yFlat[j] = _latitude[j]/180.0;
        delt = cos(_latitude[j] * arc);
        for(i = 0; i < _nlon; ++i)
        {
            _xSphere[n+i] = delt * sin(_longitude[i] * arc);
            _ySphere[n+i] =        sin(_latitude[j] * arc);
            _zSphere[n+i] = delt * cos(_longitude[i] * arc);
	}
    }
    _hlon = -1;
    for(i = 0; i < _nlon; ++i)
    {
        _xFlat[i] = _longitude[i]/180.0;
        if(_xFlat[i] >= 1.0)
        {
	    if(_hlon < 0)
		_hlon = i;
            _xFlat[i] -= 2.0;
	}
    }
    // cout << "Leave functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
}

