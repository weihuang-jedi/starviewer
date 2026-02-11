#ifndef _GL_Viewer_H
#define _GL_Viewer_H

//$Id: gl_viewer.h 4833 2013-11-05 15:49:31Z starviewer $

#include <iostream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "colorTable.h"
#include "texture1d.h"
#include "coastline.h"
#include "leveler.h"
//#include "earth.h"
#include "evaluator.h"
#include "nvoptions.h"
#include "geometry.h"

using namespace std;

class GL_Viewer
{
    public:
        GL_Viewer(ColorTable* ct, NVOptions* opt);
       ~GL_Viewer();

        void draw();

        void _display_all();
        void _display_with_bump();
        void _display_on_height_surface();
        void _display_on_sphere();
        void _display_on_map();
        void _display_on_sphere_map();

      //void _marchingCubeSetup();

        void reset();
        void setup(string vn, double* var);
        void set_geometry(Geometry* geometry);
        void set_colorTable(ColorTable* ct);

        void update_colormap();

        void set_selecting(bool s);
        void set_opacity(int o);
        void set_OP(bool v);

        double get_minval() { return vMinimum; };
        double get_maxval() { return vMaximum; };

        void set_fillValue(double v) { fillValue = v; };
        void set_hasFillValue(bool v) { hasFillValue = v; };

        double get_fillValue() { return fillValue; };
        bool  get_hasFillValue() { return hasFillValue; };

    protected:
        NVOptions *nvoptions;
        Leveler*   leveler;
        Texture1d* texture1d;
        CoastLine* coastline;

        void _display_Xplane(int xs);
        void _display_Yplane(int ys);
        void _display_Zplane(int zs);

        void _display_Xplane_with_bump(int xs);
        void _display_Yplane_with_bump(int ys);
        void _display_Zplane_with_bump(int zs);

        void _display_Xplane_on_height_surface(int xs);
        void _display_Yplane_on_height_surface(int ys);
        void _display_Zplane_on_height_surface(int zs);

        void _display_Xplane_on_sphere(int xs);
        void _display_Yplane_on_sphere(int ys);
        void _display_Zplane_on_sphere(int zs);

        void _display_Zplane_on_map_1d(int zs);
        void _display_Zplane_on_map_2d(int zs);

        void _display_Zplane_on_sphere_map_1d(int zs);
        void _display_Zplane_on_sphere_map_2d(int zs);

        int nx, ny, nz;

        double deg2rad;
        double oneover;
        double xStart, yStart;
        double xyDelt, zDelt;
        double vMinimum, vMaximum, scale, zScale;

        double* xa;
        double* xb;
        double* ya;

        double* lon;
        double* lat;
        double* lev;

        double* pltvar;
        double fillValue;
        bool  hasFillValue;

        void _parameter_setup();
        void _sphere1dVertex(double lon, double lat, double r, double f);
        void _planeVertex(double lon, double lat, double h, double f);

        void _get_factor();

      //Earth* earth;
        ColorTable* colorTable;
        Evaluator* evaluator;
        Geometry* geometry;

        string _varname;

        int _opacity;

        bool _selectingOn;

        bool _hasOP;
        bool _hasGeoInfo;
        bool need_adjust;

        double* _var;
        double _xfactor;

        int* _xlist;
        int* _ylist;
        int* _zlist;

        bool _first_time;
};
#endif

