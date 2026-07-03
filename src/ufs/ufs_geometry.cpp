#include "ufs_geometry.h"

UFSGeometry::UFSGeometry()
{
    _hlon = 0;
    _set_default();

    _current_sphere_level = -1;
    _current_flat_level = -1;
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
#if 0
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
#endif
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

  //cout << "Enter functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
  //cout << "_nlon = " << _nlon << endl;
  //cout << "_nlat = " << _nlat << endl;
  //cout << "_nlev = " << _nlev << endl;
    nsquare = _nlon * _nlat;
    _xSphere = new double[nsquare];
    _ySphere = new double[nsquare];
    _zSphere = new double[nsquare];
  //cout << "functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;

    _xFlat = new double[_nlon];
    _yFlat = new double[_nlat];

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
    _hmax = 0.0;
    _hmin = 1000000.0;
    for(j = 0; j < _nlat; ++j) {
        _yFlat[j] = _lat[j]/180.0;
      //cout << "j=" << j << ", lat[j]=" << _lat[j] <<", _yFlat[j]=" << _yFlat[j] << endl;
	n = j*_nlon;
        delt = cos(_lat[j] * arc);
        for(i = 0; i < _nlon; ++i)
        {
            _xSphere[n+i] = delt * sin(_lon[i] * arc);
            _ySphere[n+i] =        sin(_lat[j] * arc);
            _zSphere[n+i] = delt * cos(_lon[i] * arc);
        }
    }

    _hlon = 0;
    for(i = 0; i < _nlon; ++i)
    {
        _xFlat[i] = _lon[i]/180.0;
        if(_xFlat[i] > 1.0) {
           _xFlat[i] -= 2.0;
	   if(0 == _hlon)
	      _hlon = i;
	}
      //cout << "i=" << i << ", _lon=" << _lon[i] << ", _xFlat=" << _xFlat[i] << endl;
    }

    size_t totalRows = _nlat - 1;
    size_t totalVertices = totalRows * (_nlon * 2);

    _flatVertex.resize(totalVertices);

    totalVertices = totalRows * ((_nlon+1) * 2);
    _sphereVertex.resize(totalVertices);

    int k = 0;
    set_sphereVertex(k);
    set_flatVertex(k);
  //cout << "Leave functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
}

double UFSGeometry::_k2h(int k)
{
    double height = 0.5 * ((double) (_nlev-k) / _nlev);
    return height;
}

double UFSGeometry::_k2r(int k)
{
    double radius = 1.0 + _k2h(k);
    return radius;
}

void UFSGeometry::_fillFlatVertex(double x, double y, double z, VertexPoint& vp)
{
    // Position scaled by radius
    vp.x = static_cast<float>(x);
    vp.y = static_cast<float>(y);
    vp.z = static_cast<float>(z);

    // Normals
    vp.nx = static_cast<float>(x);
    vp.ny = static_cast<float>(y);
    vp.nz = static_cast<float>(z);

#if 0
    double alpha = 1.05 * fact;
    if(alpha < 0.1)       alpha = 0.0;
    else if(alpha > 1.0)  alpha = 1.0;
    int cidx = (int) (fact*_colorLen);

    // Color parameters (using 'fact' for grayscale representation as in your original code)
    vp.r = static_cast<float>(_colorMap[3*cidx]);
    vp.g = static_cast<float>(_colorMap[3*cidx+1]);
    vp.b = static_cast<float>(_colorMap[3*cidx+2]);
    vp.a = 1.0;
    // vp.a = static_cast<float>(alpha);
#endif
}

void UFSGeometry::set_flatVertex(int k)
{
    if(k == _current_flat_level)
	return;
    _current_flat_level = k;

    int i, j;
    size_t rowStartIndex;
    size_t localIdx;
    double height = _k2h(k);

    // Use OpenMP to spread the row generation across your supercomputer's CPU cores
    #pragma omp parallel for schedule(static)
    for(j = 1; j < _nlat; ++j)
    {
        // Calculate the exact starting index in the pre-allocated vector for this row
        rowStartIndex = (j - 1) * (_nlon * 2);
        localIdx = 0;

        // First half longitude loop
        for(i = _hlon; i < _nlon; ++i)
        {
            _fillFlatVertex(_xFlat[i], _yFlat[j], height, _flatVertex[rowStartIndex + localIdx++]);
            _fillFlatVertex(_xFlat[i], _yFlat[j-1], height, _flatVertex[rowStartIndex + localIdx++]);
        }

        // Second half longitude loop
        for(i = 0; i < _hlon; ++i)
        {
            _fillFlatVertex(_xFlat[i], _yFlat[j], height, _flatVertex[rowStartIndex + localIdx++]);
            _fillFlatVertex(_xFlat[i], _yFlat[j-1], height, _flatVertex[rowStartIndex + localIdx++]);
        }
    }
}

void UFSGeometry::_fillSphereVertex(double xs, double ys, double zs, double radius, VertexPoint& vp)
{
    // Position scaled by radius
    vp.x = static_cast<float>(xs * radius);
    vp.y = static_cast<float>(ys * radius);
    vp.z = static_cast<float>(zs * radius);

    // Normals
    vp.nx = static_cast<float>(xs);
    vp.ny = static_cast<float>(ys);
    vp.nz = static_cast<float>(zs);

#if 0
    double alpha = 1.05 * fact;
    if(alpha < 0.1)       alpha = 0.0;
    else if(alpha > 1.0)  alpha = 1.0;
    int cidx = (int) (fact*_colorLen);

    // Color parameters (using 'fact' for grayscale representation as in your original code)
    vp.r = static_cast<float>(_colorMap[3*cidx]);
    vp.g = static_cast<float>(_colorMap[3*cidx+1]);
    vp.b = static_cast<float>(_colorMap[3*cidx+2]);
    vp.a = 1.0;
    // vp.a = static_cast<float>(alpha);
#endif
}

void UFSGeometry::set_sphereVertex(int k)
{
    if(k == _current_sphere_level)
	return;
    _current_sphere_level = k;

    int i, j;
    size_t mpos, npos;
    size_t rowStartIndex;
    size_t localIdx;
    double radius = _k2r(k);

    // Use OpenMP to spread the row generation across your supercomputer's CPU cores
    #pragma omp parallel for schedule(static)
    for(j = 1; j < _nlat; ++j)
    {
        // Calculate the exact starting index in the pre-allocated vector for this row
        rowStartIndex = (j - 1) * ((_nlon+1) * 2);
        localIdx = 0;

        mpos = rowStartIndex = (j - 1) * _nlon;
        npos = rowStartIndex = j * _nlon;

        // First half longitude loop
        for(i = 0; i < _nlon; ++i)
        {
            _fillSphereVertex(_xSphere[npos+i], _ySphere[npos+i], _zSphere[npos+i], radius, _sphereVertex[rowStartIndex + localIdx++]);
            _fillSphereVertex(_xSphere[mpos+i], _ySphere[mpos+i], _zSphere[mpos+i], radius, _sphereVertex[rowStartIndex + localIdx++]);
        }

        _fillSphereVertex(_xSphere[npos], _ySphere[npos], _zSphere[npos], radius, _sphereVertex[rowStartIndex + localIdx++]);
        _fillSphereVertex(_xSphere[mpos], _ySphere[mpos], _zSphere[mpos], radius, _sphereVertex[rowStartIndex + localIdx++]);
    }
}
