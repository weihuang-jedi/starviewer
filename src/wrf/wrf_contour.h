#ifndef _WRF_Contour_H
#define _WRF_Contour_H

//$Id: wrf_contour.h 4833 2013-11-05 15:49:31Z starviewer $

#include <QtOpenGL>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <string.h>
#include <math.h>
#include <assert.h>

#include <string>
#include <vector>
#include <iostream>

#include <cairo/cairo.h>

#include "colorTable.h"
#include "evaluator.h"
#include "wrf_geometry.h"
#include "texturekeeper.h"

using namespace std;

class WRF_Contour
{
    public:
        WRF_Contour(ColorTable *ct);	//  Constructor
       ~WRF_Contour();			//  Destructor

        unsigned int get_xid(int n);
        unsigned int get_yid(int n);
        unsigned int get_zid(int n);

        void _tick_setup();
        void _map_setup();
        void _get_viewport_info();

        void set_nx(int n) { _nx = n; };
        void set_ny(int n) { _ny = n; };
        void set_nz(int n) { _nz = n; };

        void setup(string vn, float *var);
        void set_geometry(WRF_Geometry *gm) { geometry = gm; };
        void set_opacity(int opacity) { _opacity = opacity; };
        void set_colorTable(ColorTable *ct) { colorTable = ct; };

        void reset();
        void update_colormap();

        float get_min() { return _minval; }
        float get_max() { return _maxval; }

        double get_xStart() { return _xStart; }
        double get_yStart() { return _yStart; }
        double get_xEnd() { return _xEnd; }
        double get_yEnd() { return _yEnd; }

        double get_yStartX() { return _yStartX; }
        double get_zStartX() { return _zStartX; }
        double get_yEndX() { return _yEndX; }
        double get_zEndX() { return _zEndX; }

        double get_xStartY() { return _xStartY; }
        double get_zStartY() { return _zStartY; }
        double get_xEndY() { return _xEndY; }
        double get_zEndY() { return _zEndY; }

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

        unsigned int get_textureID();

    protected:
        ColorTable *colorTable;
        WRF_Geometry  *geometry;
        TextureKeeper* texturekeeper;

        cairo_surface_t *surface;
        cairo_t         *context;

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

        int dims[2];

        string _varname;
        float* _value;
        float* pltvar;

        double* lon;
        double* lat;
        float* lev;

        float _minval;
        float _maxval;

        double _xStart;
        double _yStart;
        double _xEnd;
        double _yEnd;

        double _yStartX;
        double _zStartX;
        double _yEndX;
        double _zEndX;

        double _xStartY;
        double _zStartY;
        double _xEndY;
        double _zEndY;

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

        int _opacity;

        int gridsizes;

        int    cnMaxLev;
        string cnLevelSelectionMode;
        float cnMinLevelValF;
        float cnMaxLevelValF;
        float cnLevelSpacingF;
};
#endif

