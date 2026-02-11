//$Id: hdfgeometry.cpp 5246 2014-12-07 16:56:07Z starviewer $

#include "hdfgeometry.h"

HDFGeometry::HDFGeometry()
{
    _lon = NULL;
    _lat = NULL;

    reset_dimension();
}

HDFGeometry::~HDFGeometry()
{
}

void HDFGeometry::reset_dimension()
{
    _upoint = false;
    _tpoint = false;

    _nlon = 1;
    _nlat = 1;
    _nlev = 1;

    _nTime = 1;
}

void HDFGeometry::reset()
{
    reset_dimension();
}

void HDFGeometry::print()
{
   cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
}

void HDFGeometry::setup()
{
    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
    cout << "\nfunction: " << __PRETTY_FUNCTION__ << endl;
}

