#ifndef _MOM6_VIEWER_H
#define _MOM6_VIEWER_H

#include <iostream>
#include <vector>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include <QGLWidget>
// #include <QOpenGLWidget>

#include "mom6_geometry.h"
#include "coastline.h"
#include "colorTable.h"
#include "texture1d.h"
#include "util.h"
#include "earth.h"
#include "lister.h"
#include "locator.h"
#include "nvoptions.h"
#include "ufsmom6reader.h"

using namespace std;

#define NEAR_POSITIVE_ZERO	0.0001
#define NEAR_NEGATIVE_ZERO	-NEAR_POSITIVE_ZERO

#define NEAR_NORTH_POLE	0.4999
#define NEAR_SOUTH_POLE	-NEAR_NORTH_POLE

#define NEAR_EAST_BOUNDARY	0.9999
#define NEAR_WEST_BOUNDARY	-NEAR_EAST_BOUNDARY

class MOM62dViewer : public QGLWidget
{

    public:
        MOM62dViewer(ColorTable* ct, NVOptions* opt);
        MOM62dViewer(ColorTable* ct, NVOptions* opt, const char* bmpflnm, UFSMOM6Reader* nchandler);
       ~MOM62dViewer();

        void draw();
        void draw_sphere_grids();
        void draw_plane_grids();

        void reset();
        void setup(string vn, float* var);
        void set_geometry(MOM6Geometry* gm);
        void reset_texture1d(ColorTable *ct);

        void set_coastline(CoastLine* cl) { coastline = cl; };

      //void set_lister(Lister* l) { lister = l; };

        void set_locator(Locator* l) { locator = l; };

        float get_minval() { return _valmin; };
        float get_maxval() { return _valmax; };

    protected:
        ColorTable* colorTable;
        MOM6Geometry* geometry;
        CoastLine* coastline;
        Texture1d* texture1d;
        UFSMOM6Reader* ncfile;
        Earth* earth;
        Lister* lister;
        Locator* locator;
        NVOptions* nvoptions;

        string _varname;

        int previoustimelevel;
        int current_timelevel;

        double* _geolon;
        double* _geolat;
        double* _zl;

        int _hxh;
        int _nxh;
        int _nyh;
        int _nzl;

        vector<int> _nxsp;

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
        float _missing_value;
        float _half_missing_value;

        double oneover;
        double deg2rad;

        void _initialize();
        void _evaluate(float* var);
        void _adjust_minmax(float* var);

        void _sphere2xyz(double x, double y, double z,
			 double radius, double fact);
        void _sphere2xyz_texture(double x, double y, double z,
			         double radius, double fact);
        void _flat2xyz(double x, double y, double z, double fact);
        void _flat2xyz_texture(double x, double y, double z,
			       double fact);

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

