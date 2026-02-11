#ifndef _CoastLine_H
#define _CoastLine_H

#include <QtOpenGL>

#include <iostream>
#include <vector>
#include <string>

#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include <netcdf.h>

#include "nclInterface.h"

using namespace std;

class CoastLine
{
    public:
        CoastLine();
       ~CoastLine();

        void print();

        float*  get_value(char *vn);
        double* get_dv(char *vn);
        int*    get_iv(char *vn);

        void draw();
        void draw(int n);
        void draw(double r, int n);

        void drawONplane();
        void drawONplane(int n);
        void drawONplane(double h, int n);
        void drawONplane2(double h, int n);

        void set_plot_level(int n);
        void set_min_plot_points(int n) { minPlotPoints = n; };

    protected:
        NclFile nclfile;
        NclVar  nclvar;

        int plotLevel;
        int maxPlotLevel;

        int minPlotPoints;

        string flnm[3];

        int ndims[3];
        int nvars[3];
        int natts[3];

        double deg2rad;
        double oneover;

      //path:   /Users/starviewer/test/ncltest/ncl_qt/data/coastline/WDBII_shp/l/WDBII_border_l_L1.shp
      //file global attributes:
      //     layer_name : WDBII_border_l_L1
      //     geometry_type : polyline
      //     geom_segIndex : 0
      //     geom_numSegs : 1
      //     segs_xyzIndex : 0
      //     segs_numPnts : 1

      //dimensions:
      //     geometry = 2
      //     segments = 2
      //     num_features = 450  // unlimited
      //     num_segments = 450
      //     num_points = 7141

      //variables:
      //     integer geometry ( num_features, geometry )
      //     integer segments ( num_segments, segments )
      //     double x ( num_points )
      //     double y ( num_points )
      //     string id ( num_features )
      //     integer level ( num_features )

        string layer_name[3];
        string geometry_type[3];

        int geom_segIndex[3]; // 0
        int geom_numSegs[3];  // 1
        int segs_xyzIndex[3]; // 0
        int segs_numPnts[3];  // 1

        int geometry[3];
        int segments[3];
        int num_features[3];
        int num_segments[3];
        int num_points[3];

        int** geometry_array;
        int** segments_array;
        int** level;

        double** lon;
        double** lat;

        double height;
        double radius;

        int** id;

        void _check_atts(int i);
        void _check_dims(int i);
        void _check_vars(int i);

        void _setup();

        void _lonlat2xy(double lon, double lat, double &x, double &y);
        void _lonlat2xy2(double lon, double lat, double &x, double &y);
        void _lonlat2xyz(double lon, double lat, double &x, double &y, double &z);
};
#endif

