#include <QtOpenGL>
#include <string.h>
#include <math.h>
#include "mapprojection.h"

MapProjection::MapProjection()
{
    rad_per_deg = 4.0*atan(1.0)/180.0;

    _code = -1;
    _lat1 = -999.9;
    _lon1 = -999.9;
    _stdlon = -999.9;
    _truelat1 = -999.9;
    _truelat2 = -999.9;

    _dx = -999.9;
    _hemi = 1.0;
    _cone = -999.9;
    _polei = -999.9;
    _polej = -999.9;
    _rsw = -999.9;
    _rebydx = -999.9;
   
    _init = false;

    xStart = 0.0;
    yStart = 0.0;
    xyDelt = 1.0;

  //print();
}

MapProjection::~MapProjection()
{
} 

void MapProjection::setup(int code, double lon1, double lat1,
                          double dx, double stdlon, double truelat1, double truelat2)
{
    _code = code;
    _stdlon = stdlon;
    _truelat1 = truelat1;
    _truelat2 = truelat2;

    _dx = dx;
    _rebydx = EARTH_RADIUS / dx;
    _lon1 = lon1;
    _lat1 = lat1;

  //cout << "\nfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
  //cout << "\t_code = " << _code << ", PROJ_LAMBERT_CONFORMAL = " << PROJ_LAMBERT_CONFORMAL << endl;

  //Convert truelat1 to radian and compute cos for later use
    _cos_truelat1 = cos(_truelat1 * rad_per_deg);
   
    switch(_code)
    {
        case PROJ_LAMBERT_CONFORMAL:
            set_LambertConformal();
            _init = true;
            break;
        default:
            _init = false;
    }
}

void MapProjection::set_LambertConformal_cone()
{
  /*
   * Purpose: compute the cone factor of a Lambert Conformal projection

   * First, see if this is a secant or tangent projection.  For tangent
   * projections, truelat1 = truelat2 and the cone is tangent to the
   * Earth's surface at this latitude.  For secant projections, the cone
   * intersects the Earth's surface at each of the distinctly different
   * latitudes
   */

   if (fabs(_truelat1 - _truelat2) > 0.1)
   {
      _cone = log(cos(_truelat1*rad_per_deg)) - log(cos(_truelat2*rad_per_deg));
      _cone = _cone /(log(tan((45.0 - fabs(_truelat1)/2.0) * rad_per_deg))
                    - log(tan((45.0 - fabs(_truelat2)/2.0) * rad_per_deg)));
   }
   else
      _cone = sin(fabs(_truelat1)*rad_per_deg);
}

double MapProjection::get_lc_cdlon(double lon)
{
    double arg;
    double deltalon;

    deltalon = lon - _stdlon;
    if (deltalon > +180.0) deltalon -= 360.0;
    if (deltalon < -180.0) deltalon += 360.0;

    arg = _cone*deltalon*rad_per_deg;

    return arg;
}

double MapProjection::get_lc_rd(double lat)
{
    double arg, rd;

  //Radius to desired point
    arg = tan(0.5*(90.0*_hemi-lat)*rad_per_deg)
        / tan(0.5*(90.0*_hemi-_truelat1)*rad_per_deg);
    rd = _rebydx * (_cos_truelat1/_cone) * pow(arg, _cone);

    return rd;
}

void MapProjection::set_LambertConformal()
{
  //Purpose: Initialize the remaining items in the proj structure for a lambert conformal grid.

    double arg;

  //cout << "\nfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;

  //Compute cone factor
    set_LambertConformal_cone();

  //Compute the radius to our known lower-left (SW) corner
    _rsw = get_lc_rd(_lat1);

  //cout << "\t_cone = " << _cone << ", _rsw = " << _rsw << endl;

  //Find pole point
    arg = get_lc_cdlon(_lon1);
    _polei = - _hemi * _rsw * sin(arg);
    _polej = _rsw * cos(arg);

  //cout << "\t_polei = " << _polei << ", _polej = " << _polej << endl;
}

void MapProjection::lc_llxy(double lon, double lat, double &x, double &y)
{
  //Purpose: compute the geographical latitude and longitude values
  //to the cartesian x/y on a Lambert Conformal projection.

    double arg, rd;

    rd = get_lc_rd(lat);
    arg = get_lc_cdlon(lon);

    x = _polei + _hemi * rd * sin(arg);
    y = _polej - rd * cos(arg);

  //Finally, if we are in the southern hemisphere, flip the i/j
  //values to a coordinate system where (1,1) is the SW corner
  //(what we assume) which is different than the original NCEP
  //algorithms which used the NE corner as the origin in the
  //southern hemisphere (left-hand vs. right-hand coordinate?)
    if(_hemi < 0.0)
    {
        x = 1.0 - x;
        y = 1.0 - y;
    }

    if(x < xmin)
        x = -99999.9;
    if(x > xmax)
        x = 99999.9;
    if(y < ymin)
        y = -99999.9;
    if(y > ymax)
        y = 99999.9;

    x *= xyDelt;
    y *= xyDelt;

    x += xStart;
    y += yStart;
}

void MapProjection::lc_llxy(double *lon, double *lat, double *x, double *y, int n)
{
  //Purpose: compute the geographical latitude and longitude values
  //to the cartesian x/y on a Lambert Conformal projection.

    double arg, rd;
    int i;

    for(i = 0; i < n; ++i)
    {
      //Radius to desired point
        rd = get_lc_rd(lat[i]);
        arg = get_lc_cdlon(lon[i]);

        x[i] = _polei + _hemi * rd * sin(arg);
        y[i] = _polej - rd * cos(arg);
    }

  //Finally, if we are in the southern hemisphere, flip the i/j
  //values to a coordinate system where (1,1) is the SW corner
  //(what we assume) which is different than the original NCEP
  //algorithms which used the NE corner as the origin in the
  //southern hemisphere (left-hand vs. right-hand coordinate?)
    if(_hemi < 0.0)
    {
        for(i = 0; i < n; ++i)
        {
            x[i] = 2.0 - x[i];
            y[i] = 2.0 - y[i];
        }
    }

    for(i = 0; i < n; ++i)
    {
        x[i] *= xyDelt;
        y[i] *= xyDelt;

        x[i] += xStart;
        y[i] += yStart;
    }
}

void MapProjection::set_dimension(int nx, int ny)
{
    _nx = nx;
    _ny = ny;

    xmin = 0.0;
    xmax = double(_nx - 1.0);

    ymin = 0.0;
    ymax = double(_ny - 1.0);

    if(nx > ny)
    {
        xyDelt = 1.0 / (nx - 1.0);
        yStart = 0.5 * (1.0 - (ny - 1.0) * xyDelt);
        xStart = 0.0;
    }
    else
    {
        xyDelt = 1.0 / (ny - 1.0);
        xStart = 0.5 * (1.0 - (nx - 1.0) * xyDelt);
        yStart = 0.0;
    }
}

