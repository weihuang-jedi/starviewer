#include "mom6_geometry.h"

MOM6Geometry::MOM6Geometry()
{
    _hlon = 0;
    _set_default();
}

MOM6Geometry::~MOM6Geometry()
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

    if(NULL != _geolon)
        delete [] _geolon;
    if(NULL != _geolat)
        delete [] _geolat;
}

void MOM6Geometry::_set_default()
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

    if(NULL != _geolon)
        delete [] _geolon;
    if(NULL != _geolat)
        delete [] _geolat;

    _xSphere = NULL;
    _ySphere = NULL;
    _zSphere = NULL;

    _xFlat = NULL;
    _yFlat = NULL;

    reset();
} 

void MOM6Geometry::reset_dimension()
{
    _nxh = 1;
    _nyh = 1;
    _nzl = 1;
}

void MOM6Geometry::reset()
{
    reset_dimension();
}

void MOM6Geometry::print()
{
   cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
   cout << "Info of <" << name << ">:" << endl;
}

void MOM6Geometry::setup()
{
    double pi = 3.1415926535897932;
    double arc = pi / 180.0;
    double delt;
    int i, j;
    size_t n;
    size_t nsquare;

  //cout << "Enter functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
  //cout << "_nxh = " << _nxh << endl;
  //cout << "_nyh = " << _nyh << endl;
    nsquare = _nxh * _nyh;
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
    _hlon = 0;
    for(j = 0; j < _nxh; ++j) {
	n = j*_nlon;
        for(i = 0; i < _nyh; ++i) {
            delt = cos(_geolat[n+i] * arc);
            _xSphere[n+i] = delt * sin(_geolon[n+i] * arc);
            _ySphere[n+i] =        sin(_geolat[n+i] * arc);
            _zSphere[n+i] = delt * cos(_geolon[n+i] * arc);

            _yFlat[n+i] = _geolat[n+i]/180.0;
            _xFlat[n+i] = _geolon[n+i]/180.0;
            if(_xFlat[n+i] > 1.0) {
               _xFlat[n+i] -= 2.0;
	       if(0 == _hlon)
	            _hlon = i;
	    }
	}
    }
  //cout << "Leave functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
}

