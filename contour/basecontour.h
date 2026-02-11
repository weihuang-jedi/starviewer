#ifndef _BaseContour_H
#define _BaseContour_H

//$Id: basecontour.h 4865 2014-01-01 15:49:23Z starviewer $

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <string.h>
#include <math.h>
#include <assert.h>

#include <string>
#include <vector>
#include <iostream>

#include <QtOpenGL>

#ifdef LINUX
#include <cairo/cairo.h>
#else
#include <cairo.h>
#endif

using namespace std;

#include "colorTable.h"
#include "evaluator.h"
#include "geometry.h"

class BaseContour
{
    public:
        BaseContour(ColorTable *ct);	//  Constructor
        virtual ~BaseContour();			//  Destructor

        virtual unsigned int get_xid(int n) = 0;
        virtual unsigned int get_yid(int n) = 0;
        virtual unsigned int get_zid(int n) = 0;

        virtual void _tick_setup() { };
        virtual void _map_setup() { };
        virtual void _get_viewport_info() { };

        void set_nx(int n);
        void set_ny(int n);
        void set_nz(int n);

        void setup(string vn, float *var);
        void set_geometry(Geometry *gm) { geometry = gm; };
        void set_time(int t) { _time = t; };
        void set_opacity(int opacity) { _opacity = opacity; };
        void set_colorTable(ColorTable *ct) { colorTable = ct; };
        void set_evaluator(Evaluator *ev) { evaluator = ev; };

        void set_nt(int nt) { _nt = nt; };
        void set_nm(int nm) { _nm = nm; };


        void reset();
        void reinitialize();
        void update_colormap();

        float get_xStart() { return _xStart; }
        float get_yStart() { return _yStart; }
        float get_xEnd() { return _xEnd; }
        float get_yEnd() { return _yEnd; }

        float get_yStartX() { return _yStartX; }
        float get_zStartX() { return _zStartX; }
        float get_yEndX() { return _yEndX; }
        float get_zEndX() { return _zEndX; }

        float get_xStartY() { return _xStartY; }
        float get_zStartY() { return _zStartY; }
        float get_xEndY() { return _xEndY; }
        float get_zEndY() { return _zEndY; }

        void initialize();
        void finalize();

        void initialize_context();

        void _general_setup();
        void _viewport_setup();
        void _viewportXsetup();
        void _viewportYsetup();
        void _get_viewportXinfo();
        void _get_viewportYinfo();

        void set_cnMaxLev(int ml) { cnMaxLev = ml; };

        unsigned int get_textureID(unsigned int textureID);

    protected:
        ColorTable *colorTable;
        Evaluator  *evaluator;
        Geometry   *geometry;

        cairo_surface_t *surface;
        cairo_t         *context;

        unsigned int *_z_id;
        unsigned int *_y_id;
        unsigned int *_x_id;

        int iwidth;
        int iheight;
        int clen[2];

        int grlist;
        int srlist;

        int appid;
        int wksid;
        int mapid;
        int dataid;
        int cnid;

        float *_value;
        string _varname;

        float *lon;
        float *lat;
        float *lev;

        float _xStart;
        float _yStart;
        float _xEnd;
        float _yEnd;

        float _yStartX;
        float _zStartX;
        float _yEndX;
        float _zEndX;

        float _xStartY;
        float _zStartY;
        float _xEndY;
        float _zEndY;

        float vpx;
        float vpy;
        float vpw;
        float vph;

        float vpxX;
        float vpyX;
        float vpwX;
        float vphX;

        float vpxY;
        float vpyY;
        float vpwY;
        float vphY;

        int _nx;
        int _ny;
        int _nz;
        int _nt;
        int _nm;

        int _time;
        int _opacity;

        int _onx;
        int _ony;
        int _onz;
        int _ont;
        int _onm;

        int gridsizes;

        int    cnMaxLev;
        string cnLevelSelectionMode;
        float cnMinLevelValF;
        float cnMaxLevelValF;
        float cnLevelSpacingF;
};
#endif

