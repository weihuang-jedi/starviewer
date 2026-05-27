#ifndef _UFSMOM6READER_H
#define _UFSMOM6READER_H
 
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>

#include <netcdf.h>

#include "ncbasereader.h"
 
class UFSMOM6Reader : public NCBaseReader
{
  public:
    // declare constructor and deconstructor methods
    UFSMOM6Reader(const char*);
    virtual ~UFSMOM6Reader();
 
    int getNxh() { return _nxh; };
    int getNyh() { return _nyh; };
    int getNxg() { return _nxg; };
    int getNyg() { return _nyg; };
    int getNzl() { return _nzl; };
    int getNzi() { return _nzi; };
    int getNnv() { return _nnv; };
    int getNtime() { return _ntime; };

    float* getGeolon() { return _geolon; };
    float* getGeolat() { return _geolat; };

    double* getZl() { return _zl; };

    void exploreFile() override;
    void get_dim_info() override;
    void get_var_info() override;

  protected:
    double* _xh;
    double* _yh;
    double* _xg;
    double* _yg;
    double* _zl;
    double* _zi;
    double* _time;
    double* _nv;

    float* _geolon;
    float* _geolat;

    float* _geolon_c;
    float* _geolat_c;

    float* _geolon_u;
    float* _geolat_u;

    float* _geolon_v;
    float* _geolat_v;

    int _nxh;
    int _nyh;
    int _nxg;
    int _nyg;
    int _nzl;
    int _nzi;
    int _nnv;
    int _ntime;
};
#endif
