#ifndef _MapProjection_H
#define _MapProjection_H
/*
  ! Define data structures to define various projections

   type proj_info
      integer          :: code     ! integer code for projection type
      real             :: lat1     ! SW latitude (1,1) in degrees (-90->90N)
      real             :: lon1     ! SW longitude (1,1) in degrees (-180->180E)
      real             :: dx       ! Grid spacing in meters at truelats, used
                                   ! only for ps, lc, and merc projections
      real             :: stdlon   ! Longitude parallel to y-axis (-180->180E)
      real             :: truelat1 ! First true latitude (all projections)
      real             :: truelat2 ! Second true lat (LC only)
      real             :: hemi     ! 1 for NH, -1 for SH
      real             :: cone     ! Cone factor for LC projections
      real             :: polei    ! Computed i-location of pole point
      real             :: polej    ! Computed j-location of pole point
      real             :: rsw      ! Computed radius to SW corner
      real             :: rebydx   ! Earth radius divided by dx
      logical          :: init     ! Flag to indicate if this struct is ready for use
   end type proj_info
  */

//$Id$

#include <iostream>

//Projection codes for proj_info structure:
#define PROJ_LATLON	0
#define PROJ_LAMBERT_CONFORMAL 1
#define PROJ_MERCATER	3
#define PROJ_POLAR_STEREOGRAPHIC		5

//https://en.wikipedia.org/wiki/Earth_radius
//equatorial radius 6378.1370
//polar radius 6356.7523
#define EARTH_RADIUS	6371008.8

using namespace std;

class MapProjection
{
    public:
        MapProjection();
       ~MapProjection();

        int get_code() { return _code; };
        double get_lat1() { return _lat1; };
        double get_lon1() { return _lon1; };
        double get_stdlon() { return _stdlon; };
        double get_truelat1() { return _truelat1; };
        double get_truelat2() { return _truelat2; };
        double get_dx() { return _dx; };
        double get_hemi() { return _hemi; };

        void set_code(int val) { _code = val; };
        void set_lat1(double val) { _lat1 = val; };
        void set_lon1(double val) { _lon1 = val; };
        void set_stdlon(double val) { _stdlon = val; };
        void set_truelat1(double val) { _truelat1 = val; };
        void set_truelat2(double val) { _truelat2 = val; };
        void set_dx(double val) { _dx = val; };
        void set_hemi(double val) { _hemi = val; };

        void setup(int code, double lon1, double lat1,
                   double dx, double stdlon, double truelat1, double truelat2);
        void lc_llxy(double lon, double lat, double &x, double &y);
        void lc_llxy(double *lon, double *lat, double *x, double *y, int n);

        void set_dimension(int nx, int ny);

    private:
        double rad_per_deg;

        int _code;
        double _lat1, _lon1;
        double _stdlon, _truelat1, _truelat2;
        double _dx, _rebydx;
        double _hemi, _cone, _polei, _polej, _rsw;
        bool _init;

        double _cos_truelat1;

        void set_LambertConformal_cone();
        void set_LambertConformal();

        int _nx, _ny;
        double xmin, xmax, ymin, ymax;
        double xStart, yStart, xyDelt;

        double get_lc_cdlon(double lon);
        double get_lc_rd(double lat);
};
#endif

