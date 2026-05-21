#include "ufsincr_geometry.h"

UFSINCRGeometry::UFSINCRGeometry(int nlon, int nlat, float* lon, float* lat)
{
    _nlon = nlon;
    _nlat = nlat;
    _geolon = lon;
    _geolat = lat;

    _setup();
}

UFSINCRGeometry::~UFSINCRGeometry()
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

void UFSINCRGeometry::_setup()
{
    double pi = 3.1415926535897932;
    double arc = pi / 180.0;
    double delt;
    int i, j;
    size_t n;
    size_t nsquare;

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

    _xSphere = NULL;
    _ySphere = NULL;
    _zSphere = NULL;

    _xFlat = NULL;
    _yFlat = NULL;

  //cout << "Enter functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
  //cout << "_nlon = " << _nlon << endl;
  //cout << "_nlat = " << _nlat << endl;
    nsquare = _nlon * _nlat;
    _xSphere = new double[nsquare];
    _ySphere = new double[nsquare];
    _zSphere = new double[nsquare];
    _xFlat = new double[nsquare];
    _yFlat = new double[nsquare];

  //cout << "functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    if(NULL == _geolat) {
	cout << "_geolat in NULL. Stop" << endl;
	throw(errno);
	exit (-1);
    }
    if(NULL == _geolon) {
	cout << "_geolon in NULL. Stop" << endl;
	throw(errno);
	exit (-1);
    }

    for(j = 0; j < _nlat; ++j) {
	n = j*_nlon;
        delt = cos(_geolat[j] * arc);
        for(i = 0; i < _nlon; ++i)
        {
            _xSphere[n+i] = delt * sin(_geolon[i] * arc);
            _ySphere[n+i] =        sin(_geolat[j] * arc);
            _zSphere[n+i] = delt * cos(_geolon[i] * arc);

            _xFlat[n+i] = _geolon[n+i]/180.0;
            if(_xFlat[n+i] > 1.0)
               _xFlat[n+i] -= 2.0;
            _yFlat[n+i] = _geolat[n+i]/180.0;
        }
    }
  //cout << "Leave functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
}

