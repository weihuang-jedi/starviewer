//$Id: popgeometry.cpp 4833 2013-11-05 15:49:31Z starviewer $

#include "popgeometry.h"

POPGeometry::POPGeometry()
{
    _pop_ulon = NULL;
    _pop_ulat = NULL;
    _pop_tlon = NULL;
    _pop_tlat = NULL;

    reset_dimension();
}

POPGeometry::~POPGeometry()
{
}

void POPGeometry::reset_dimension()
{
    _upoint = false;
    _tpoint = false;

    _pop_nlon = 1;
    _pop_nlat = 1;
    _pop_nlev = 1;

    _nTime = 1;
}

void POPGeometry::reset()
{
    reset_dimension();
}

void POPGeometry::print()
{
   cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
}

void POPGeometry::setup()
{
    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
    cout << "\nfunction: " << __PRETTY_FUNCTION__ << endl;
}

