//$Id: mpasgeometry.cpp 4833 2013-11-05 15:49:31Z starviewer $

#include "mpasgeometry.h"

MPASGeometry::MPASGeometry() : Geometry()
{
    pi = 3.1415926535897932;
    reset();
}

MPASGeometry::~MPASGeometry()
{
    if(NULL != _innerPoints)
        delete [] _innerPoints;
}

void MPASGeometry::reset_dimension()
{
    _nTime = 1;
    _nCells = 0;
  //_nVertices = 0;
    _nVertLevels = 1;
}

void MPASGeometry::reset()
{
    reset_dimension();

    _innerPoints = NULL;
    _nBoundaryPoints = 0;
} 

void MPASGeometry::print()
{
    cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
}

void MPASGeometry::setup()
{
    float xWest = -0.5;
    float xEast =  0.5;
#ifdef HASZGRID
    float delt;
    int i;
    size_t m;
#endif
    int n, c, cp1, cp2, v;

    cout << "\nEnter functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    pnt2d pnt0;
    pnt2d pnt1;
    pnt2d pnt2;

    _innerPoints = new bool[_nVertices];

    _hmax = 1.0;
    _hmin = 10000.0;
#if 0
    for(n = 0; n < _nCells; ++n)
    {
#ifdef HASZGRID
        m = n * (_nVertLevels + 1);
        for(i = 0; i < _nVertLevels; ++i)
        {
             _height[m+i] = 0.5 * (_height[m+i] + _height[m+i+1]);
             if(_hmax < _height[m+i])
                _hmax = _height[m+i];
             if(_hmin > _height[m+i])
                _hmin = _height[m+i];
        }
#endif
    }
#endif

    cout << "\t<" << __PRETTY_FUNCTION__ << ">" << endl;
    cout << "\tline: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    cout << "\t_hmin = " << _hmin << ", _hmax = " << _hmax << endl;

  //CELLSONVERTEX is 1 based.  Fix that.
    for(n = 0; n < _vertexDegree * _nVertices; ++n)
    {
        _cellsOnVertex[n] = _cellsOnVertex[n] - 1;
    }

#if 0
#ifdef HASZGRID
  //applys a factor of 0.125 to height, and then divides by _hmax
    delt = 0.125 / _hmax;

    for(n = 0; n < _nCells; ++n)
    {
        m = n * (_nVertLevels + 1);
        for(i = 0; i < _nVertLevels; ++i)
        {
             _height[m+i] = 1.0 + delt * _height[m+i];
        }
    }
#endif
#endif

  //Check if triangle across longitude 0.
  //  _boundaryType            _boundaryType
  //         west                        west
  //   -2    lon 0               -1      lon 0
  //        .  |                           |  .
  //        |\ |                           | /|
  //        | \|                           |/ |
  //        |  +                           +  |
  //        |  |\                         /|  |
  //        |  | .                       . |  |
  //        |  |/                         \|  |
  //        |  +                           +  |
  //        | /|                           |\ |
  //        |/ |                           | \|
  //        .  |                           |  .
  //      2                      1
  //         east                        east

    for(v = 0; v < _nVertices; ++v)
    {
        _innerPoints[v] = true;

        c = _cellsOnVertex[v * _vertexDegree];
        cp1 = _cellsOnVertex[1 + v * _vertexDegree];
        cp2 = _cellsOnVertex[2 + v * _vertexDegree];

        pnt0 = latlon2xy(c);
        pnt1 = latlon2xy(cp1);
        pnt2 = latlon2xy(cp2);

        if(pnt0.x > xEast)
        {
            if(pnt1.x > xEast)
            {
                if(pnt2.x > xWest)
                   continue;
            
                _innerPoints[v] = false;
                ++_nBoundaryPoints;

                _boundaryType.push_back(2);

                _boundaryPoints.push_back(c);
                _boundaryPoints.push_back(cp1);
                _boundaryPoints.push_back(cp2);

                continue;
            }

            if(pnt1.x < xWest)
            {
                if(pnt2.x > xEast)
                {
                    _boundaryType.push_back(2);
                    _innerPoints[v] = false;
                    ++_nBoundaryPoints;

                    _boundaryPoints.push_back(c);
                    _boundaryPoints.push_back(cp2);
                    _boundaryPoints.push_back(cp1);

                    continue;
                }

                if(pnt2.x < xWest)
                {
                    _boundaryType.push_back(1);
                    _innerPoints[v] = false;
                    ++_nBoundaryPoints;

                    _boundaryPoints.push_back(c);
                    _boundaryPoints.push_back(cp1);
                    _boundaryPoints.push_back(cp2);

                    continue;
                }
            }
        }

        if(pnt0.x < xWest)
        {
            if(pnt1.x < xWest)
            {
                if(pnt2.x < xEast)
                    continue;

                ++_nBoundaryPoints;
  
                _boundaryType.push_back(-1);

                _boundaryPoints.push_back(c);
                _boundaryPoints.push_back(cp1);
                _boundaryPoints.push_back(cp2);

                _innerPoints[v] = false;
                continue;
            }

            if(pnt1.x > xEast)
            {
                if(pnt2.x < xWest)
                {
                    _boundaryType.push_back(-1);
                    _innerPoints[v] = false;
                    ++_nBoundaryPoints;

                    _boundaryPoints.push_back(c);
                    _boundaryPoints.push_back(cp2);
                    _boundaryPoints.push_back(cp1);

                    continue;
                }

                if(pnt2.x > xEast)
                {
                    _boundaryType.push_back(-2);
                    _innerPoints[v] = false;
                    ++_nBoundaryPoints;
         
                    _boundaryPoints.push_back(cp1);
                    _boundaryPoints.push_back(cp2);
                    _boundaryPoints.push_back(c);

                    _innerPoints[v] = false;
                }
            }
        }
    }

    cout << "\t_nBoundaryPoints = " << _nBoundaryPoints << endl;
    cout << "Leave functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
}

pnt3d MPASGeometry::polar2xyz(int n, int k)
{
   pnt3d pnt;
#ifdef HASZGRID
    size_t m = n * (_nVertLevels + 1) + k;
    double delt = _height[m] * cos(_latCell[n]);
    pnt.y       = _height[m] * sin(_latCell[n]);
#else
    double delt = cos(_latCell[n]);
    pnt.y       = sin(_latCell[n]);
#endif

    pnt.x = delt * sin(_lonCell[n]);
    pnt.z = delt * cos(_lonCell[n]);

    return pnt;
} 

pnt3d MPASGeometry::polar2xyz(int n, double r)
{
    pnt3d pnt;
    double delt = r * cos(_latCell[n]);

    pnt.x = delt * sin(_lonCell[n]);
    pnt.y =    r * sin(_latCell[n]);
    pnt.z = delt * cos(_lonCell[n]);

    return pnt;
} 

pnt2d MPASGeometry::latlon2xy(int n)
{
    pnt2d pnt;
    pnt.x = _lonCell[n]/pi - 1.0;
    pnt.y = _latCell[n]/pi;

    return pnt;
} 

