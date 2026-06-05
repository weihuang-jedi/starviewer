#ifndef _MPASSTATICREADER_H
#define _MPASSTATICREADER_H
 
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <unordered_map>

#include <netcdf.h>

#include "ncbasereader.h"
 
class MPASStaticReader : public NCBaseReader
{
  public:
    // declare constructor and deconstructor methods
    MPASStaticReader(const char*);
    virtual ~MPASStaticReader();
 
    int get_nCells() { return _nCells; };
    int get_nVertices() { return _nVertices; };
    int get_nEdges() { return _nEdges; };
    int get_nTime() { return _nTime; };

    int get_vertexDegree() { return _vertexDegree; };
    int get_nVertLevels() { return _nVertLevels; };

    double* get_lonCell() { return _lonCell; };
    double* get_latCell() { return _latCell; };

    double* get_xCell() { return _xCell; };
    double* get_yCell() { return _yCell; };
    double* get_zCell() { return _zCell; };

    double* get_lonVertex() { return _lonVertex; };
    double* get_latVertex() { return _latVertex; };

    double* get_xVertex() { return _xVertex; };
    double* get_yVertex() { return _yVertex; };
    double* get_zVertex() { return _zVertex; };

    int* get_indexToCellID() { return _indexToCellID; };
    int* get_indexToVertexID() { return _indexToVertexID; };
    int* get_cellsOnCell() { return _cellsOnCell; };
    int* get_verticesOnCell() { return _verticesOnCell; };
    int* get_cellsOnVertex() { return _cellsOnVertex; };

    char* get_xtime() { return _xtime; };

    void exploreFile() override;
    void get_dim_info() override;
    void get_var_info() override;

  protected:
    double* _lonCell;
    double* _latCell;

    double* _xCell;
    double* _yCell;
    double* _zCell;

    double* _lonVertex;
    double* _latVertex;

    double* _xVertex;
    double* _yVertex;
    double* _zVertex;

    int* _indexToCellID;
    int* _indexToVertexID;

    int* _cellsOnCell;
    int* _verticesOnCell;
    int* _cellsOnVertex;

    char* _xtime;

    int _nTime;
    int _nCells;
    int _nVertices;
    int _nEdges;
    int _maxEdges;
    int _maxEdges2;
    int _TWO;
    int _vertexDegree;
    int _Strlen;
    int _R3;
    int _nMonth;
    int _FIFTEEN;
    int _TWENTYONE;

    int _nVertLevels;

    // Inside your Viewer/Reader class definition or constructor:
    unordered_map<std::string, int*> _dimMap;
};
#endif
