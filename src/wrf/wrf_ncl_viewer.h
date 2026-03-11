#ifndef _WRF_NCL_Viewer_H
#define _WRF_NCL_Viewer_H

//$Id: wrf_ncl_viewer.h 4833 2013-11-05 15:49:31Z starviewer $

#include <iostream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "evaluator.h"
#include "wrf_contour.h"
#include "wrf_geometry.h"
#include "nvoptions.h"
#include "earth.h"

using namespace std;

class WRF_NCL_Viewer
{
    public:
        WRF_NCL_Viewer(ColorTable *ct, NVOptions* opt);
       ~WRF_NCL_Viewer();

        void draw();

        void _display_all();
        void _display_with_bump();
        void _display_on_height_surface();
        void _display_on_sphere();
        void _display_Global_on_sphere();

        void _marchingCubeSetup();

        void reset();
        void setup(string vn, float *var);
        void set_geometry(WRF_Geometry *geometry);

        void set_colorTable(ColorTable *ct);

        void set_opacity(int o);
        void set_OP(bool v);
        void update_colormap() { contour->update_colormap(); }

        float get_min() { return _minval; };
        float get_max() { return _maxval; };

    protected:
        NVOptions *nvoptions;
        ColorTable *colorTable;
        Earth *earth;

        WRF_Contour *contour;
        WRF_Geometry *geometry;

        string _varname;

        int  _opacity;
        bool _hasOP;

        float *_var;

        float _minval;
        float _maxval;

        float _hmin;
        float _hmax;

        void _Vertex(double lon, double lat, double r, double s, double t);

        void _xQuad(double x1, double y1, double z1, double y2, double z2,
                    double s1, double t1, double s2, double t2);

        void _yQuad(double x1, double y1, double z1, double x2, double z2,
                    double s1, double t1, double s2, double t2);

        void _zQuad(double x1, double y1, double x2, double y2, double z2,
                    double s1, double t1, double s2, double t2);

        void _point(double x, double y, double z, double s, double t);

    private:
        int nx, ny, nz;
        int nxp, nyp, nzp;

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

        bool first_time;
};
#endif

