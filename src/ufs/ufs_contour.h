#ifndef _UFS_Contour_H
#define _UFS_Contour_H

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
#include "ufs_geometry.h"
#include "texturekeeper.h"

using namespace std;

class UFS_Contour
{
    public:
        UFS_Contour(ColorTable *ct);	//  Constructor
       ~UFS_Contour();			//  Destructor

        unsigned int get_xid(int n);
        unsigned int get_yid(int n);
        unsigned int get_zid(int n);

        void _tick_setup();
        void _map_setup();
        void _get_viewport_info();

        void set_nx(int n) { _nx = n; };
        void set_ny(int n) { _ny = n; };
        void set_nz(int n) { _nz = n; };

        void setup(string vn, double *var);
        void set_geometry(UFS_Geometry *gm) { geometry = gm; };
        void set_opacity(int opacity) { _opacity = opacity; };
        void set_colorTable(ColorTable *ct) { colorTable = ct; };

        void reset();
        void update_colormap();

        float get_min() { return _minval; }
        float get_max() { return _maxval; }

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

        unsigned int get_textureID();

    protected:
        ColorTable *colorTable;
        UFS_Geometry  *geometry;
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
        double* _value;
        float* pltvar;

        double* lon;
        double* lat;
        double* lev;

        double _minval;
        double _maxval;

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

        int _opacity;

        int gridsizes;

        int    cnMaxLev;
        string cnLevelSelectionMode;
        float cnMinLevelValF;
        float cnMaxLevelValF;
        float cnLevelSpacingF;
};
#endif

