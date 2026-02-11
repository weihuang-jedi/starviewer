//$Id: basegeometry.cpp 4833 2013-11-05 15:49:31Z starviewer $

#include "basegeometry.h"

BaseGeometry::BaseGeometry()
{
    _set_default();
}

BaseGeometry::~BaseGeometry()
{
}

void BaseGeometry::_set_default()
{
    _mx = 1;
    _my = 1;
    _mz = 1;

    _nx = 1;
    _ny = 1;
    _nz = 1;
    _nt = 1;
    _nm = 1;
} 

