#include "eagle_geometry.h"

EAGLEGeometry::EAGLEGeometry()
{
    _set_default();
}

EAGLEGeometry::~EAGLEGeometry()
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

    if(NULL != _lon2d)
        delete [] _lon2d;
    if(NULL != _lat2d)
        delete [] _lat2d;
}

void EAGLEGeometry::_set_default()
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

    if(NULL != _lon2d)
        delete [] _lon2d;
    if(NULL != _lat2d)
        delete [] _lat2d;

    _xSphere = NULL;
    _ySphere = NULL;
    _zSphere = NULL;

    _lon2d = NULL;
    _lat2d = NULL;

    _xFlat = NULL;
    _yFlat = NULL;

    reset();
} 

void EAGLEGeometry::reset_dimension()
{
    _nx = 1;
    _ny = 1;
    _ntime = 1;
}

void EAGLEGeometry::reset()
{
    reset_dimension();
}

void EAGLEGeometry::print()
{
   cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
   cout << "Info of <" << name << ">:" << endl;
}

void EAGLEGeometry::setup()
{
    double pi = 3.1415926535897932;
    double arc = pi / 180.0;
    double delt;
    int i, j;
    size_t n;
    size_t nsquare;

    cout << "Enter functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    cout << "_nx = " << _nx << endl;
    cout << "_ny = " << _ny << endl;
    cout << "_ntime = " << _ntime << endl;
    nsquare = _nx * _ny;
    _xSphere = new double[nsquare];
    _ySphere = new double[nsquare];
    _zSphere = new double[nsquare];
  //cout << "functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;

    _xFlat = new double[nsquare];
    _yFlat = new double[nsquare];

  //cout << "functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    if(NULL == _lat) {
	cout << "_lat in NULL. Stop" << endl;
	throw(errno);
	exit (-1);
    }
    if(NULL == _lon) {
	cout << "_lon in NULL. Stop" << endl;
	throw(errno);
	exit (-1);
    }

    for(j = 0; j < _ny; ++j) {
	n = j*_nx;
        for(i = 0; i < _nx; ++i)
        {
            delt = cos(_lat2d[n+i] * arc);
            _xSphere[n+i] = delt * sin(_lon2d[n+i] * arc);
            _ySphere[n+i] =        sin(_lat2d[n+i] * arc);
            _zSphere[n+i] = delt * cos(_lon2d[n+i] * arc);
            _yFlat[n+i] = _lat2d[n+i]/180.0;
            _xFlat[n+i] = _lon2d[n+i]/180.0;
	}
    }
  //cout << "Leave functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
}

