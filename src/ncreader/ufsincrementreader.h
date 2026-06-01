#ifndef _UFSINCREMENTREADER_H
#define _UFSINCREMENTREADER_H
 
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>

#include <netcdf.h>

#include "ncbasereader.h"
 
class UFSIncrementReader : public NCBaseReader
{
  public:
    // declare constructor and deconstructor methods
    UFSIncrementReader(const char*);
    virtual ~UFSIncrementReader();
 
    int getNx() { return _nx; };
    int getNy() { return _ny; };
    int getNz() { return _nz; };
    int getNt() { return _nt; };

    double* getXaxis() { return _xaxis_1; };
    double* getYaxis() { return _yaxis_1; };
    double* getZaxis() { return _zaxis_1; };
    double* getTime() { return _Time; };

    void exploreFile() override;
    void get_dim_info() override;
    void get_var_info() override;

  protected:
    double* _xaxis_1;
    double* _yaxis_1;
    double* _zaxis_1;
    double* _Time;

    int _nx;
    int _ny;
    int _nz;
    int _nt;
};
#endif
