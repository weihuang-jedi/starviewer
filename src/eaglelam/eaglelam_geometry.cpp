#include "eagle_geometry.h"

EAGLELAM_Geometry::EAGLELAM_Geometry()
{
    _set_default();
}

EAGLELAM_Geometry::~EAGLELAM_Geometry()
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

void EAGLELAM_Geometry::_set_default()
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

void EAGLELAM_Geometry::reset_dimension()
{
    _nx = 1;
    _ny = 1;
    _ntime = 1;
}

void EAGLELAM_Geometry::reset()
{
    reset_dimension();
}

void EAGLELAM_Geometry::print()
{
   cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
   cout << "Info of <" << name << ">:" << endl;
}

void EAGLELAM_Geometry::setup()
{
    double pi = 3.1415926535897932;
    double arc = pi / 180.0;
    double delt;
    int i, j;
    size_t n;
    size_t nsquare;

    // cout << "Enter functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    // cout << "_nx = " << _nx << endl;
    // cout << "_ny = " << _ny << endl;
    // cout << "_ntime = " << _ntime << endl;
    nsquare = _nx * _ny;
    _xSphere = new double[nsquare];
    _ySphere = new double[nsquare];
    _zSphere = new double[nsquare];
    // cout << "functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;

    _xFlat = new double[nsquare];
    _yFlat = new double[nsquare];

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

    for(j = 0; j < _ny; ++j) {
	n = j*_nx;
        for(i = 0; i < _nx; ++i)
        {
            delt = cos(_latitude[n+i] * arc);
            _xSphere[n+i] = delt * sin(_longitude[n+i] * arc);
            _ySphere[n+i] =        sin(_latitude[n+i] * arc);
            _zSphere[n+i] = delt * cos(_longitude[n+i] * arc);
            _yFlat[n+i] = _latitude[n+i]/180.0;
            _xFlat[n+i] = _longitude[n+i]/180.0;
	    if(_xFlat[n+i] > 1.0)
		_xFlat[n+i] -= 2.0;
	}
    }
    // cout << "Leave functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
}

