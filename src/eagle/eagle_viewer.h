#ifndef _EAGLE_VIEWER_H
#define _EAGLE_VIEWER_H

#include <iostream>
#include <vector>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include <QGLWidget>
// #include <QOpenGLWidget>

#include "eagle_geometry.h"
#include "coastline.h"
#include "colorTable.h"
#include "texture1d.h"
#include "util.h"
#include "earth.h"
#include "lister.h"
#include "locator.h"
#include "nvoptions.h"
#include "eaglereader.h"

using namespace std;

#define NEAR_POSITIVE_ZERO	0.0001
#define NEAR_NEGATIVE_ZERO	-NEAR_POSITIVE_ZERO

#define NEAR_NORTH_POLE	0.4999
#define NEAR_SOUTH_POLE	-NEAR_NORTH_POLE

#define NEAR_EAST_BOUNDARY	0.9999
#define NEAR_WEST_BOUNDARY	-NEAR_EAST_BOUNDARY

class EAGLE2dViewer : public QGLWidget
{

    public:
        EAGLE2dViewer(ColorTable* ct, NVOptions* opt);
        EAGLE2dViewer(ColorTable* ct, NVOptions* opt, const char* bmpflnm, EagleReader* nchandler);
       ~EAGLE2dViewer();

        void draw();
        void draw_sphere_grids();
        void draw_plane_grids();

        void reset();
        void setup(string vn, float* var);
        void set_geometry(EAGLEGeometry* gm);
        void reset_texture1d(ColorTable *ct);

        void set_coastline(CoastLine* cl) { coastline = cl; };

      //void set_lister(Lister* l) { lister = l; };

        void set_locator(Locator* l) { locator = l; };

        float get_minval() { return _valmin; };
        float get_maxval() { return _valmax; };

    protected:
        ColorTable* colorTable;
        EAGLEGeometry* geometry;
        CoastLine* coastline;
        Texture1d* texture1d;
        EagleReader* ncfile;
        Earth* earth;
        Lister* lister;
        Locator* locator;
        NVOptions* nvoptions;

        string _varname;

        int previoustimelevel;
        int current_timelevel;

        float* _longitude;
        float* _latitude;

        int _nx;
        int _ny;
        int _ntime;

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

        float _valmin;
        float _valavg;
        float _valmax;

        double oneover;
        double deg2rad;

        void _initialize();
        void _evaluate(float* var);
        void _adjust_minmax(float* var);

        void _lonlat2xyz(float lon, float lat, double radius,
                         double fact);
        void _lonlat2xyz_texture(float lon, float lat,
			         double radius, double fact);

        void _flatDisplay();
        void _sphereDisplay();

        void _flatBump();
        void _sphereBump();
        void _draw_cross(double radius);
        double _k2h(int k);
        double _k2r(int k);
};
#endif

