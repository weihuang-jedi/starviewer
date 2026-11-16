#include "mpasgridgeometry.h"

MPASGridGeometry::MPASGridGeometry() : Geometry()
{
    pi = 3.1415926535897932;
    reset();
}

MPASGridGeometry::~MPASGridGeometry()
{
    if(NULL != _innerPoints)
        delete [] _innerPoints;
}

void MPASGridGeometry::reset_dimension()
{
    _nCells = 0;
    _nVertices = 0;
    _maxEdges = 0;
}

void MPASGridGeometry::reset()
{
    reset_dimension();

    _innerPoints = NULL;
    _nBoundaryPoints = 0;
} 

void MPASGridGeometry::print()
{
    cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
}

void MPASGridGeometry::setup()
{
    float xWest = -0.5;
    float xEast =  0.5;
#ifdef HASZGRID
    float delt;
    int i;
    size_t m;
#endif
    int n, c, cp1, cp2, v;

    // cout << "\nEnter functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    pnt2d pnt0;
    pnt2d pnt1;
    pnt2d pnt2;

    _innerPoints = new bool[_nVertices];

    // cout << "\t<" << __PRETTY_FUNCTION__ << ">" << endl;
    // cout << "\tline: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    // cout << "\t_hmin = " << _hmin << ", _hmax = " << _hmax << endl;

  //CELLSONVERTEX is 1 based.  Fix that.
    for(n = 0; n < _vertexDegree * _nVertices; ++n)
    {
        _verticesOnCell[n] = _verticesOnCell[n] - 1;
    }

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

        c = _verticesOnCell[v * _vertexDegree];
        cp1 = _verticesOnCell[1 + v * _vertexDegree];
        cp2 = _verticesOnCell[2 + v * _vertexDegree];

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

    // cout << "\t_nBoundaryPoints = " << _nBoundaryPoints << endl;
    // cout << "Leave functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
}

pnt3d MPASGridGeometry::polar2xyz(int n, int k)
{
   pnt3d pnt;
#ifdef HASZGRID
    size_t m = n * (_nVertLevels + 1) + k;
    double delt = _height[m] * cos(_latVertex[n]);
    pnt.y       = _height[m] * sin(_latVertex[n]);
#else
    double delt = cos(_latVertex[n]);
    pnt.y       = sin(_latVertex[n]);
#endif

    pnt.x = delt * sin(_lonVertex[n]);
    pnt.z = delt * cos(_lonVertex[n]);

    return pnt;
} 

pnt3d MPASGridGeometry::polar2xyz(int n, double r)
{
    pnt3d pnt;
    double delt = r * cos(_latVertex[n]);

    pnt.x = delt * sin(_lonVertex[n]);
    pnt.y =    r * sin(_latVertex[n]);
    pnt.z = delt * cos(_lonVertex[n]);

    return pnt;
} 

pnt2d MPASGridGeometry::latlon2xy(int n)
{
    pnt2d pnt;
    pnt.x = _lonVertex[n]/pi;
    if(pnt.x > 1.0)
       pnt.x -= 2.0;
    pnt.y = _latVertex[n]/pi;

    return pnt;
} 

