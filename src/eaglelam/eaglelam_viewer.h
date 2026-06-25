#ifndef _EAGLELAM_Viewer_H
#define _EAGLELAM_Viewer_H

#include <iostream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "evaluator.h"
#include "colorTable.h"
#include "coastline.h"
#include "mapprojection.h"
#include "stateboundary.h"
#include "eaglelam_geometry.h"
#include "nvoptions.h"
#include "locator.h"
#include "niceminmaxstepsize.h"

using namespace std;

class EAGLELAM_Viewer
{
    public:
        EAGLELAM_Viewer(ColorTable *ct, NVOptions* opt);
       ~EAGLELAM_Viewer();

        void draw();
        void scaleNtranslate();

        void reset();
        void setup(string vn, double *var);
        void set_geometry(EAGLELAM_Geometry *geometry);

        void set_opacity(int o);
        void set_colorTable(ColorTable *ct);
        void update_colormap();

        void set_surface(bool s);

        void set_OP(bool v);

        void saveDataset();
        void set_locator(Locator* l) { locator = l; };

        double get_minval() { return vMinimum; };
        double get_maxval() { return vMaximum; };

    protected:
        NVOptions *nvoptions;
        NiceMinMaxStepSize nicemms;
        Locator* locator;
      //Lister* lister;
        ColorTable *colorTable;
        CoastLine* coastline;
        MapProjection* mapprojection;
        StateBoundary* stateboundary;
        EAGLELAM_Geometry *geometry;

        string _varname;

        int  _opacity;
        bool _hasOP;

        double *_var;

        void _display_all();
        void _display_with_bump();
        void _display_on_height_surface();
        void _display_on_sphere();
        void _display_on_map();
        void _display_on_sphere_map();

    private:
        Texture1d*    texture1d;

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

        void _display_Zplane_on_map(int zs);
        void _display_Zplane_on_sphere_map(int zs);

        int nx, ny, nz;
        int nxp, nyp, nzp;

        double deg2rad;
        double _xfactor;
        double oneover;
        float xStart, yStart;
        float xyDelt, zDelt;
        float vMinimum, vMaximum, scale, zScale;
        float niceMin, niceMax;

        double *lon;
        double *lat;
        double *pltvar;

        void _parameter_setup();
        void _get_factor();
        void _planeVertex(double lon, double lat, double height, double f);
        void _sphere1dVertex(float lon, float lat, float r, float f);

        bool first_time;
};
#endif

