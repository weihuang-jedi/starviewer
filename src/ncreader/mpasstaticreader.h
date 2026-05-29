#ifndef _MPASGRIDREADER_H
#define _MPASGRIDREADER_H
 
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>

#include <netcdf.h>

#include "ncbasereader.h"
 
class MPASStaticReader : public NCBaseReader
{
  public:
    // declare constructor and deconstructor methods
    MPASStaticReader(const char*);
    virtual ~MPASStaticReader();
 
    int getnCells() { return nCells; };
    int getnVertices() { return _nVertices; };
    int getnEdges() { return _nEdges; };

    double* getlonVertex() { return _lonVertex; };
    double* getlatVertex() { return _latVertex; };

    double* getxCell() { return _xCell; };
    double* getyCell() { return _yCell; };
    double* getzCell() { return _zCell; };

    double* getxVertex() { return _xVertex; };
    double* getyVertex() { return _yVertex; };
    double* getzVertex() { return _zVertex; };

    void exploreFile() override;
    void get_dim_info() override;
    void get_var_info() override;

  protected:
    double* _lonVertex;
    double* _latVertex;

    double* _xCell;
    double* _yCell;
    double* _zCell;

    double* _xVertex;
    double* _yVertex;
    double* _zVertex;

    int* _indexToCellID;
    int* _indexToVertexID;

    int* _cellsOnCell;
    int* _verticesOnCell;
    int* _cellsOnVertex;

    int _nCells;
    int _nVertices;
    int _nEdges;
    int _maxEdges;
    int _maxEdges2;
    int _TWO;
    int _codeLen;
    int _vertexDegree;
};
#endif
