#ifndef _MPASGRIDGeometry_H
#define _MPASGRIDGeometry_H

#include <QtOpenGL>

#include <iostream>
#include <vector>

#include <stdlib.h>
#include <assert.h>

#include "geometry.h"

using namespace std;

class MPASGridGeometry : public Geometry
{
    public:
        MPASGridGeometry();
       ~MPASGridGeometry();

        void reset();
        void reset_dimension();
        void print();

        void setup();

        int get_nCells() { return _nCells; };
        int get_nVertices() { return _nVertices; };
        int get_vertexDegree() { return _vertexDegree; };
        int get_maxEdges() { return _maxEdges; };

        void set_nEdgesOnCell(int* val) { _nEdgesOnCell = val; };
        void set_verticesOnCell(int* val) { _verticesOnCell = val; };
        void set_height(float* val) { _height = val; };
        void set_lonVertex(double* val) { _lonVertex = val; };
        void set_latVertex(double* val) { _latVertex = val; };

        int* get_nEdgesOnCell() { return _nEdgesOnCell; };
        int* get_verticesOnCell() { return _verticesOnCell; };

        double* get_lonVertex() { return _lonVertex; };
        double* get_latVertex() { return _latVertex; };

        void set_nCells(int i) { _nCells = i; };
        void set_nVertices(int i) { _nVertices = i; };
        void set_vertexDegree(int i) { _vertexDegree = i; };
        void set_maxEdges(int i) { _maxEdges = i; };

        void set_name(string nm) { _name = nm; };
        string get_name() { return _name; };

        bool* get_innerPoints() { return _innerPoints; };
        int   get_nBoundaryPoints() { return _nBoundaryPoints; };

        vector<int> get_boundaryType() { return _boundaryType; };
        vector<int> get_boundaryPoints() { return _boundaryPoints; };

        pnt3d polar2xyz(int n, double r);
        pnt3d polar2xyz(int n, int k);
        pnt2d latlon2xy(int n);

    protected:
        string _name;

        int _nCells;
        int _nVertices;
        int _maxEdges;
        int _vertexDegree;
        int _nBoundaryPoints;

        int* _nEdgesOnCell;
        int* _verticesOnCell;

        bool* _innerPoints;

        double* _lonVertex;
        double* _latVertex;

        float* _height;
        float  _hmax, _hmin;
        float  pi;

        vector<int> _boundaryType;
        vector<int> _boundaryPoints;
};
#endif

