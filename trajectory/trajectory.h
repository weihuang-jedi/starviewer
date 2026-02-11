#ifndef _Trajectory_H
#define _Trajectory_H

//$Id: trajectory.h 5161 2014-07-29 22:35:26Z starviewer $

#include <iostream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "evaluator.h"
#include "colorTable.h"
#include "cone.h"
#include "equalcylinder.h"
#include "position.h"
#include "interp.h"
#include "nvoptions.h"
#include "topography.h"

using namespace std;

#ifndef NV_PI
#define NV_PI   3.1415926535897932
#endif

#ifndef DEG
#define DEG     (180.0 / NV_PI)
#endif

//#define MINIMUM_CYLINDER_HEIGHT	0.001
#define MINIMUM_CYLINDER_HEIGHT	0.0001

class Trajectory
{
    public:
        Trajectory(ColorTable* ct, NVOptions* opt);
        Trajectory(const Trajectory &) { ++instance; };
       ~Trajectory();

        void draw();
        void draw_earth_image(float z);

        void setup(int nx, int ny, int nz,
                   float* u, float* v, float* w, float* h);
        void setup_position(float* lon, float* lat);

        void set_colorTable(ColorTable *ct) { colorTable = ct; };

        void set_dtNdx(float dt, float dx);
        void set_tspan(float t1, float t2);

        void initialize();

    protected:
        ColorTable* colorTable;
        NVOptions* nvoptions;
        Topography topography;
        Cone cone;
        EqualCylinder cylinder;
        Position* position;
        Interp* interpolater;

        string _varname;

        float* _u1;
        float* _v1;
        float* _w1;
        float* _h1;

        float* _u2;
        float* _v2;
        float* _w2;
        float* _h2;

        float* _lon;
        float* _lat;

        float* _tracer;

        float _val;
        float _maxval;

        double _tfac;
        double _dt;
        double _dx;
        double _tspan;

        double _width;

        void _display();
        void _advance();

    private:
        GLuint trajlist;

        long _number_of_points;

        int h_stepsize, v_stepsize;
        int _nx, _ny, _nz;

        int ip, jp, kp;
        int nwsb, nwnb;
        int nwst, nwnt;

        double dtoverdx;
        double h_bot, h_top, hc;
        double z_fac;

        double west, east, south, north, bottom, top;

        bool has_tracer;

        int    _colorlen;
        float* _colormap;
        float color[4];

        float _xStart, _yStart;
        float _xyDelt, _zDelt;

        void _parameter_setup();
        void _set_color(double val);

        double _get_value(long n, float* var1, float* var2);
        void _draw_increment(Position origin, Position adv_pos, bool draw_cone);
        bool _outofbound(Position pos);

        static int instance;
};
#endif

