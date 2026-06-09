#ifndef _UFSINCR_VIEWER_H
#define _UFSINCR_VIEWER_H

#include <iostream>
#include <vector>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include <QGLWidget>
#include <QtOpenGL>
#include <QOpenGLWidget>
#include <vector>

#include "coastline.h"
#include "colorTable.h"
#include "texture1d.h"
#include "util.h"
#include "earth.h"
#include "lister.h"
#include "locator.h"
#include "nvoptions.h"
#include "ufsincr_geometry.h"
#include "ufsincrementreader.h"

using namespace std;

#define NEAR_POSITIVE_ZERO	0.0001
#define NEAR_NEGATIVE_ZERO	-NEAR_POSITIVE_ZERO

#define NEAR_NORTH_POLE	0.4999
#define NEAR_SOUTH_POLE	-NEAR_NORTH_POLE

#define NEAR_EAST_BOUNDARY	0.9999
#define NEAR_WEST_BOUNDARY	-NEAR_EAST_BOUNDARY

// class UFSINCR2dViewer : public QGLWidget, protected QOpenGLFunctions
// class UFSINCR2dViewer : public QGLWidget
class UFSINCR2dViewer : public QObject
{
    public:
        UFSINCR2dViewer(ColorTable* ct, NVOptions* opt);
        UFSINCR2dViewer(ColorTable* ct, NVOptions* opt,
			const char* bmpflnm,
			vector<UFSIncrementReader*> nchandler);
       ~UFSINCR2dViewer();

        void draw();
        void draw_sphere_grids();
        void draw_plane_grids();

	// void initializeGL() override;

        void reset();
        void setup(string vn, vector<float*> var);
        void set_geometry(vector<UFSINCRGeometry*> gm);
        void reset_texture1d(ColorTable *ct);
        void set_coastline(CoastLine* cl) { coastline = cl; };

      //void set_lister(Lister* l) { lister = l; };

        void set_locator(Locator* l) { locator = l; };

        // void renderBitmapString(float x, float y, void *font, const string &str);

        float get_minval() { return _valmin; };
        float get_maxval() { return _valmax; };

    protected:
        ColorTable* colorTable;
        vector<UFSINCRGeometry*> geometry;
        CoastLine* coastline = NULL;
        Texture1d* texture1d = NULL;
        vector<UFSIncrementReader*> ncfile;
        Earth* earth = NULL;
        Lister* lister = NULL;
        Locator* locator = NULL;
        NVOptions* nvoptions;

        string _varname;

        int previoustimelevel;
        int current_timelevel;

        vector<float*> _lon;
        vector<float*> _lat;
        vector<float*> _lev;

        int _hlon;
        int _nlon;
        int _nlat;
        int _nlev;
        int _ntiles;

        GLuint zcl;
        GLuint ycl;
        GLuint xcl;

        vector<double*> _xSphere;
        vector<double*> _ySphere;
        vector<double*> _zSphere;

        vector<double*> _xFlat;
        vector<double*> _yFlat;

        vector<float*> _var;
        vector<float*> pltvar;

        float _valmin;
        float _valavg;
        float _valmax;
        float _scalelength;

        double oneover;
        double deg2rad;

        void _initialize();
        void _evaluate(vector<float*> var);
        void _adjust_minmax(vector<float*> var);

        void _sphereVertex(double x, double y, double z,
			   double radius, double fact);
        void _sphereVertex_texture(double x, double y, double z,
			           double radius, double fact);

        void _flatVertex(double x, double y, double z,
			 double fact);
        void _flatVertex_texture(double x, double y, double z,
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

	char _bmpflnm[1024];
	bool _needActivateEarth;
};
#endif

