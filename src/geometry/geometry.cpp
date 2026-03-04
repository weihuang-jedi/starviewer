//$Id: geometry.cpp 4833 2013-11-05 15:49:31Z starviewer $

#include "geometry.h"

Geometry::Geometry() : BaseGeometry()
{
    BaseGeometry::_set_default();
    _set_default();
}

Geometry::~Geometry()
{
    int n;

    //delete [] xbvalues;
    //delete [] ylvalues;

    for(n = 0; n < nlabels; ++n)
    {
        free(xblabels[n]);
        free(yllabels[n]);
    }

    free(xblabels);
    free(yllabels);
}

void Geometry::reset_dimension()
{
    BaseGeometry::_set_default();
}

void Geometry::reset()
{
    _nx = 1;
    _ny = 1;
    _nz = 1;
    _nt = 1;
    _nm = 1;

    _hasFillValue = false;
    _fillValue = 0.0;
}

void Geometry::printinfo()
{
#if 0
   cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
   cout << "Geo Info of <" << name << ">:" << endl;
   cout << "\tmx = " << _mx << endl;
   cout << "\tmy = " << _my << endl;
   cout << "\tmz = " << _mz << endl;
   cout << "\tnx = " << _nx << endl;
   cout << "\tny = " << _ny << endl;
   cout << "\tnz = " << _nz << endl;
   cout << "\tnt = " << _nt << endl;

   cout << "\thas1dLon = " << _has1dLon << endl;
   cout << "\thas1dLat = " << _has1dLat << endl;

   cout << "\thas2dLon = " << _has2dLon << endl;
   cout << "\thas2dLat = " << _has2dLat << endl;
#endif
}

void Geometry::_set_default()
{
    int n;

    reset();

    _lon = NULL;
    _lat = NULL;
    _lev = NULL;

    _has1dLon = false;
    _has1dLat = false;
    _has1dLev = false;

    _has2dLon = false;
    _has2dLat = false;

    _hasFillValue = false;
    _fillValue = 0.0;

    nlabels = 7;

    xbvalues = new float[nlabels];
    ylvalues = new float[nlabels];

    xbvalues[0] = -180.;
    xbvalues[1] = -120.;
    xbvalues[2] =  -60.;
    xbvalues[3] =    0.;
    xbvalues[4] =   60.;
    xbvalues[5] =  120.;
    xbvalues[6] =  180.;

    ylvalues[0] = -90.;
    ylvalues[1] = -60.;
    ylvalues[2] = -30.;
    ylvalues[3] = 0.;
    ylvalues[4] = 30;
    ylvalues[5] = 60.;
    ylvalues[6] = 90.;

    xblabels = (char **)calloc(nlabels, sizeof(char *));
    assert(xblabels);
    yllabels = (char **)calloc(nlabels, sizeof(char *));
    assert(yllabels);

    for(n = 0; n < nlabels; ++n)
    {
        xblabels[n] = (char *)calloc(6, sizeof(char));
        assert(xblabels[n]);
        yllabels[n] = (char *)calloc(6, sizeof(char));
        assert(yllabels[n]);
    }

    strcpy(xblabels[0], "180W");
    strcpy(xblabels[1], "120W");
    strcpy(xblabels[2],  "60W");
    strcpy(xblabels[3],   "0");
    strcpy(xblabels[4],  "60E");
    strcpy(xblabels[5], "120E");
    strcpy(xblabels[6], "180E");

    strcpy(yllabels[0], "90S");
    strcpy(yllabels[1], "60S");
    strcpy(yllabels[2], "30S");
    strcpy(yllabels[3],  "EQ");
    strcpy(yllabels[4], "30N");
    strcpy(yllabels[5], "60N");
    strcpy(yllabels[6], "90N");
} 

