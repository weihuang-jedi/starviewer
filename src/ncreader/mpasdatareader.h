#ifndef _MPASDATAREADER_H
#define _MPASDATAREADER_H
 
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <unordered_map>

#include <netcdf.h>

#include "ncbasereader.h"
 
class MPASDataReader : public NCBaseReader
{
  public:
    // declare constructor and deconstructor methods
    MPASDataReader(const char*);
    virtual ~MPASDataReader();
 
    int get_nCells() { return _nCells; };
    int get_nEdges() { return _nEdges; };
    int get_nTime() { return _nTime; };
    int get_nVertLevels() { return _nVertLevels; };
    int get_nSoilLevels() { return _nSoilLevels; };
    int get_nVertLevelsP1() { return _nVertLevelsP1; };

    char* get_initial_time() { return _initial_time; };
    char* get_xtime() { return _xtime; };

    void exploreFile() override;
    void get_dim_info() override;
    void get_var_info() override;

  protected:
    char* _initial_time;
    char* _xtime;

    int _nTime;
    int _nCells;
    int _nEdges;
    int _Strlen;
    int _nSoilLevels;
    int _nVertLevels;
    int _nVertLevelsP1;

    // Inside your Viewer/Reader class definition or constructor:
    unordered_map<std::string, int*> _dimMap;
};
#endif
