#ifndef _CoastLine2_H
#define _CoastLine2_H

#include <QtOpenGL>

#include <iostream>
#include <vector>
#include <string>

#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include <netcdf.h>

#include "nclInterface.h"
#include "nvoptions.h"

using namespace std;

class CoastLine2
{
    public:
        CoastLine2();
        CoastLine2(string idstr);
       ~CoastLine2();

        void print();

        float*  get_fv(char *vn);
        double* get_dv(char *vn);
        int*    get_iv(char *vn);
        double** get_lv(char *vn, int** seg_sizes);

        void draw();
        void draw(double r);

        void drawONplane();
        void drawONplane(double h);
        void drawAregion(double h);
        void drawRegionalGrid(double h);
        void drawFrame(double xleft, double xrite,
                       double ybottom, double ytop, double z);

        void drawLimitedArea(double h);

        void draw_plane_grid(double z);
        void draw_grid(double r);
        void draw_ball(double r);

        void set_base_lon(double lon) { base_lon = lon; };
        void set_base_lat(double lat) { base_lat = lat; };
        void set_scale(double s) { scale = s; };
        void set_grid_lon_space(int n) { grid_lon_space = n; };
        void set_grid_lat_space(int n) { grid_lat_space = n; };
        void set_cntlon(double v) { cntlon = v; };
        void set_nvoptions(NVOptions* opt) { nvoptions = opt; };

    protected:
        NVOptions* nvoptions;

        NclFile nclfile;
        NclVar  nclvar;

        int kind;

        string flnm[3];

        int ndims[3];
        int nvars[3];
        int natts[3];

        double deg2rad;
        double oneover;

        string layer_name[3];
        string geometry_type[3];

        int num_features[3];
        int num_segments[3];
        int num_points[3];

        int** level;
        int** seg_sizes;

        double** area;
        double*** segments;

        double height;
        double radius;

        double cntlon;
        double minlon;
        double maxlon;
        double minlat;
        double maxlat;

        double base_lon, base_lat;
        double scale;

        int grid_lon_space;
        int grid_lat_space;

        double xwest, xeast, xcenter;
        double ysouth, ynorth, ycenter;

        int** id;

        void _check_atts(int i);
        void _check_dims(int i);
        void _check_vars(int i);

        void _setup();
        void _setup(string idstr);
        void _setupRegionInfo();

        void _lonlat2xy(double lon, double lat, double &x, double &y);
        void _lonlat2xyz(double lon, double lat, double &x, double &y, double &z);

        void _drawAregion(double slon, double elon, double slat, double elat, double h);
        void _drawRegionalGrid(double slon, double elon, double slat, double elat, double h);

        void _local_ll2xy(double lon, double lat, double &x, double &y);
};
#endif

