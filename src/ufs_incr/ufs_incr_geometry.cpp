#include "ufs_incr_geometry.h"

UFSincrGeometry::UFSincrGeometry()
{
    _set_default();
}

UFSincrGeometry::~UFSincrGeometry()
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

    if(NULL != _tile)
        delete [] _tile;
}

void UFSincrGeometry::_set_default()
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

void UFSincrGeometry::reset_dimension()
{
    _nlon = 1;
    _nlat = 1;
    _nlev = 1;
    _ntiles = 0;
}

void UFSincrGeometry::reset()
{
    reset_dimension();
}

void UFSincrGeometry::print()
{
   cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
   cout << "Info of <" << name << ">:" << endl;
}

void UFSincrGeometry::setup()
{
    double pi = 3.1415926535897932;
    double arc = pi / 180.0;
    double delt;
    int i, j, k;
    size_t n;
    size_t length;

  //cout << "Enter functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
  //cout << "_nlon = " << _nlon << endl;
  //cout << "_nlat = " << _nlat << endl;
  //cout << "_nlev = " << _nlev << endl;
    length = _nx * _ny * _ntiles;
    _xSphere = new double[length];
    _ySphere = new double[length];
    _zSphere = new double[length];
  //cout << "functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;

    _xFlat = new double[length];
    _yFlat = new double[length];

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

    for(k = 0; k < _ntiles; ++k) {
        for(j = 0; j < _nx; ++j) {
          //cout << "j=" << j << ", lat[j]=" << _lat[j] <<", _yFlat[j]=" << _yFlat[j] << endl;
	    n = (k*_ny + j)*_nx;
            delt = cos(_lat[j] * arc);
            for(i = 0; i < _nlon; ++i)
            {
                _xSphere[n+i] = delt * sin(_lon2d[i] * arc);
                _ySphere[n+i] =        sin(_lat2d[j] * arc);
                _zSphere[n+i] = delt * cos(_lon2d[i] * arc);

                _yFlat[n+i] = _lat2d[n+i]/180.0;
                _xFlat[n+i] = _lon2d[n+i]/180.0;
                if(_xFlat[n+i] > 1.0)
                   _xFlat[n+i] -= 2.0;
            }
        }
    }
  //cout << "Leave functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
}

