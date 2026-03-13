#ifndef _VIEWER_H
#define _VIEWER_H

#include <iostream>
#include <vector>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "ufs_geometry.h"
#include "coastline.h"
#include "colorTable.h"
#include "texture1d.h"
#include "util.h"
#include "earth.h"
#include "lister.h"
#include "locator.h"
#include "nvoptions.h"

using namespace std;

#define NEAR_POSITIVE_ZERO	0.0001
#define NEAR_NEGATIVE_ZERO	-NEAR_POSITIVE_ZERO

#define NEAR_NORTH_POLE	0.4999
#define NEAR_SOUTH_POLE	-NEAR_NORTH_POLE

#define NEAR_EAST_BOUNDARY	0.9999
#define NEAR_WEST_BOUNDARY	-NEAR_EAST_BOUNDARY

class UFS2dViewer
{
    public:
        UFS2dViewer(ColorTable* ct, NVOptions* opt);
       ~UFS2dViewer();

        void draw();
        void draw_sphere_grids();
        void draw_plane_grids();

        void reset();
        void setup(string vn, float* var);
        void set_geometry(UFSGeometry* gm);
        void reset_texture1d(ColorTable *ct);

        void set_coastline(CoastLine* cl) { coastline = cl; };

      //void set_lister(Lister* l) { lister = l; };

        void set_locator(Locator* l) { locator = l; };

        float get_minval() { return _valmin; };
        float get_maxval() { return _valmax; };

    protected:
        ColorTable* colorTable;
        UFSGeometry* geometry;
        CoastLine* coastline;
        Texture1d* texture1d;
        Earth* earth;
        Lister* lister;
        Locator* locator;
        NVOptions* nvoptions;

        string _varname;

        int previoustimelevel;
        int current_timelevel;

        double _deltlon;
        double _deltlat;

        double* _lon;
        double* _lat;
        float* _lev;

        int _nlon;
        int _nlat;
        int _nlev;

        GLuint zcl;
        GLuint ycl;
        GLuint xcl;

        bool* _innerPoints;
        int   _nBoundaryPoints;

        double* _xSphere;
        double* _ySphere;
        double* _zSphere;

        double* _xFlat;
        double* _yFlat;

        float* _var;
        float* pltvar;

        float _valmin;
        float _valmax;

        double oneover;
        double deg2rad;

        int num_south_pole_cols;
        int num_north_pole_cols;
        int num_boundary_cols;

        vector<int> south_pole_cols;
        vector<int> north_pole_cols;
        vector<int> boundary_cols;

        void _initialize();
        void _evaluate(float* var);

        void _lonlat2xyz(double lon, double lat, double radius,
                         double fact);

        void _flatDisplay();
        void _display_Xflat_plane(int xs);
        void _display_Yflat_plane(int ys);

        void _sphereDisplay();
        void _sphereXplane(int xs);
        void _sphereYplane(int ys);

        void handle_selectedXsphere_quad(int nlonc, double sv, double* radius);
        void handle_selectedYsphere_quad(int jlatc, double sv, double* radius);

        void handle_selectedXflat_quad(int ilonc, double sv, double* height);
        void handle_selectedYflat_quad(int jlatc, double sv, double* height);

        void _flatBump();
        void _sphereBump();
        void _draw_cross(double radius);
};
#endif

