#ifndef _NCL_Viewer_H
#define _NCL_Viewer_H

//$Id: ncl_viewer.h 4833 2013-11-05 15:49:31Z starviewer $

#include <iostream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "contour.h"
#include "geometry.h"
#include "util.h"
#include "nvoptions.h"
//#include "earth.h"

using namespace std;

class NCL_Viewer
{
    public:
        NCL_Viewer(ColorTable* ct, NVOptions* opt);
       ~NCL_Viewer();

        void draw();

        void _display_all();
        void _display_with_bump();
        void _display_on_height_surface();
        void _display_on_sphere();
        void _display_Global_on_sphere();

      //void _marchingCubeSetup();

        void reset();
        void setup(string vn, float* var);
        void set_geometry(Geometry* geometry);

        void set_time(int t);
        void set_colorTable(ColorTable* ct);

        void set_opacity(int o);
        void set_OP(bool v);
        void update_colormap() { contour->update_colormap(); }

    private:
        NVOptions *nvoptions;
      //Earth* earth;
        ColorTable* colorTable;

        Geometry* geometry;
        Contour* contour;

        string _varname;

        int  _time;
        int  _opacity;
        bool _hasOP;

        float* _var;

        void _Vertex(double lon, double lat, double r, double s, double t);

        void _xQuad(double x1, double y1, double z1, double y2, double z2,
                    double s1, double t1, double s2, double t2);

        void _yQuad(double x1, double y1, double z1, double x2, double z2,
                    double s1, double t1, double s2, double t2);

        void _zQuad(double x1, double y1, double x2, double y2, double z2,
                    double s1, double t1, double s2, double t2);

        void _point(double x, double y, double z, double s, double t);

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

        bool _first_time;
};
#endif

