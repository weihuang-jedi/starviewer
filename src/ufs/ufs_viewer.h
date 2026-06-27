#ifndef _UFS_VIEWER_H
#define _UFS_VIEWER_H

#include <iostream>
#include <vector>
#include <memory>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include <QGLWidget>
// #include <QOpenGLWidget>

#include "ufs_geometry.h"
#include "coastline.h"
#include "colorTable.h"
#include "texture1d.h"
#include "util.h"
#include "earth.h"
#include "lister.h"
#include "locator.h"
#include "nvoptions.h"
#include "ncreader.h"
#include "windvector.h"

using namespace std;

#define NEAR_POSITIVE_ZERO	0.0001
#define NEAR_NEGATIVE_ZERO	-NEAR_POSITIVE_ZERO

#define NEAR_NORTH_POLE	0.4999
#define NEAR_SOUTH_POLE	-NEAR_NORTH_POLE

#define NEAR_EAST_BOUNDARY	0.9999
#define NEAR_WEST_BOUNDARY	-NEAR_EAST_BOUNDARY

class UFS2dViewer : public QGLWidget
{

    public:
        UFS2dViewer(ColorTable* ct, NVOptions* opt);
        UFS2dViewer(ColorTable* ct, NVOptions* opt, const char* bmpflnm, ncReader* nchandler);
       ~UFS2dViewer();

        void draw();
        void draw_sphere_grids();
        void draw_plane_grids();

        void reset();
        void setup(string vn, float* var);
        void setup_wind(float* u, float* v);
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
        ncReader* ncfile;
        Earth* earth;
        Lister* lister;
        Locator* locator;
        NVOptions* nvoptions;
	// WindVector* windvector;
	unique_ptr<WindVector> windvector;

        string _varname;

        int previoustimelevel;
        int current_timelevel;

        double* _lon;
        double* _lat;
        float* _lev;

        int _hlon;
        int _nlon;
        int _nlat;
        int _nlev;

        GLuint zcl;
        GLuint ycl;
        GLuint xcl;

        double* _xSphere;
        double* _ySphere;
        double* _zSphere;

        double* _xFlat;
        double* _yFlat;

        float* _var;
        float* pltvar;
        float* _u;
        float* _v;

        float _valmin;
        float _valavg;
        float _valmax;

        double oneover;
        double deg2rad;

        void _initialize();
        void _evaluate(float* var);
        void _adjust_minmax(float* var);

        void _lonlat2xyz(double lon, double lat, double radius,
                         double fact);
        void _lonlat2xyz_texture(double lon, double lat,
			         double radius, double fact);

        void _flatDisplay();
        void _display_Xflat_plane(int xs);
        void _display_Yflat_plane(int ys);

        void _sphereDisplay();
        void _sphereXplane(int xs);
        void _sphereYplane(int ys);

        void _flatBump();
        void _sphereBump();
        void _draw_cross(double radius);
        double _k2h(int k);
        double _k2r(int k);
};
#endif

