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
        void setup(string vn, double* var);
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

        double* _var;

        void _Vertex(float lon, float lat, float r, float s, float t);

        void _xQuad(float x1, float y1, float z1, float y2, float z2,
                    float s1, float t1, float s2, float t2);

        void _yQuad(float x1, float y1, float z1, float x2, float z2,
                    float s1, float t1, float s2, float t2);

        void _zQuad(float x1, float y1, float x2, float y2, float z2,
                    float s1, float t1, float s2, float t2);

        void _point(float x, float y, float z, float s, float t);

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

