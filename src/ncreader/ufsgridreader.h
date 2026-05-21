#ifndef _UFSGRIDREADER_H
#define _UFSGRIDREADER_H
 
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>

#include <netcdf.h>

#include "ncbasereader.h"
 
class UFSGridReader : public NCBaseReader
{
  public:
    // declare constructor and deconstructor methods
    UFSGridReader(const char*);
    virtual ~UFSGridReader();
 
    int getNlon() { return _nlon; };
    int getNlat() { return _nlat; };

    float* getGeoLon() { return _geolon; };
    float* getGeoLat() { return _geolat; };

    void exploreFile() override;
    void get_dim_info() override;
    void get_var_info() override;

  protected:
    float* _geolon;
    float* _geolat;

    int _nlon;
    int _nlat;
};
#endif
