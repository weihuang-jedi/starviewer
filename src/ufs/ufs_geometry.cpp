//$Id: ufs_geometry.cpp 4865 2014-01-01 15:49:23Z starviewer $

#include "ufs_geometry.h"

UFS_Geometry::UFS_Geometry() : Geometry()
{
    Geometry::_set_default();

    _set_default();
}

UFS_Geometry::~UFS_Geometry()
{
    if(NULL != _lon)
        delete [] _lon;
    if(NULL != _lat)
        delete [] _lat;
}

void UFS_Geometry::_set_default()
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

void UFS_Geometry::reset_dimension()
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

void UFS_Geometry::reset()
{
    reset_dimension();
}

void UFS_Geometry::print()
{
   cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
}

