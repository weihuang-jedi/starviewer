#ifndef _MPASGeometry_H
#define _MPASGeometry_H

//$Id: mpasgeometry.h 4833 2013-11-05 15:49:31Z starviewer $

#include <QtOpenGL>

#include <iostream>
#include <vector>

#include <stdlib.h>
#include <assert.h>

#include "geometry.h"
#include "point3d.h"

using namespace std;

struct _point2d
{
    double x;
    double y;
};

typedef struct _point2d pnt2d;

class MPASGeometry : public Geometry
{
    public:
        MPASGeometry();
       ~MPASGeometry();

        void reset();
        void reset_dimension();
        void print();

        void setup();

        int get_nTime() { return _nTime; };
        int get_nCells() { return _nCells; };
        int get_nVertices() { return _nVertices; };
        int get_nVertLevels() { return _nVertLevels; };
        int get_vertexDegree() { return _vertexDegree; };

        void set_cellsOnVertex(int* val) { _cellsOnVertex = val; };
        void set_height(float* val) { _height = val; };
        void set_lonCell(double* val) { _lonCell = val; };
        void set_latCell(double* val) { _latCell = val; };

        int* get_cellsOnVertex() { return _cellsOnVertex; };

        double* get_lonCell() { return _lonCell; };
        double* get_latCell() { return _latCell; };

        void set_nTime(int i) { _nTime = i; };
        void set_nCells(int i) { _nCells = i; };
        void set_nVertices(int i) { _nVertices = i; };
        void set_nVertLevels(int i) { _nVertLevels = i; };
        void set_vertexDegree(int i) { _vertexDegree = i; };

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

        int _nTime;
        int _nCells;
        int _nVertices;
        int _nVertLevels;
        int _vertexDegree;
        int _nBoundaryPoints;

        int* _cellsOnVertex;

        bool* _innerPoints;

        double* _lonCell;
        double* _latCell;

        float* _height;
        float  _hmax, _hmin;
        float  pi;

        vector<int> _boundaryType;
        vector<int> _boundaryPoints;
};
#endif

