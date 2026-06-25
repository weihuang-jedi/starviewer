//$Id: eagle_geometry.cpp 4865 2014-01-01 15:49:23Z starviewer $

#include "eagle_geometry.h"

EAGLE_Geometry::EAGLE_Geometry() : Geometry()
{
    Geometry::_set_default();

    _set_default();
}

EAGLE_Geometry::~EAGLE_Geometry()
{
    if(NULL != _lon)
        delete [] _lon;
    if(NULL != _lat)
        delete [] _lat;
}

void EAGLE_Geometry::_set_default()
{
    Xstaggered = false;
    Ystaggered = false;
    Zstaggered = false;

    _nx = 0;
    _ny = 0;
    _nz = 0;
    _nt = 0;
    _nm = 0;

    _lon = NULL;
    _lat = NULL;
    _hgt = NULL;

    _ulon = NULL;
    _ulat = NULL;

    _vlon = NULL;
    _vlat = NULL;

    reset();
} 

void EAGLE_Geometry::reset_dimension()
{
    Xstaggered = false;
    Ystaggered = false;
    Zstaggered = false;

    _nx = 1;
    _ny = 1;
    _nz = 1;
    _nt = 1;
    _nm = 1;
}

void EAGLE_Geometry::reset()
{
    reset_dimension();
}

void EAGLE_Geometry::print()
{
   cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
}

