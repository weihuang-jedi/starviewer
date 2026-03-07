#ifndef _UFS_Controller_H
#define _UFS_Controller_H

//$Id: ufs_controller.h 4968 2014-02-13 15:32:52Z starviewer $

#include <QtOpenGL>
#include <vector>

#include "ufs_gl_viewer.h"
#include "ufs_ncl_viewer.h"
#include "ufs_geometry.h"
#include "marchingcube.h"
#include "controller.h"
#include "windvector.h"
#include "trajectory.h"
#include "locator.h"
#include "lic.h"

using namespace std;

class UFS_Controller : public Controller
{
    public:
        UFS_Controller(ColorTable *ct, NVOptions* opt,
                       const char *fn, bool isList = false);
       ~UFS_Controller();

        void setup();
        void draw();
        void set2dvarname(string vn);
        void set3dvarname(string vn);
        void set_fileNtime(int nf, int nt);

        void set_locator(Locator* l);

        void setup_vector();
        void unset_vector();

        void update_colormap();
        void draw_isosurface();
        void draw_lic();
        void draw_vector();
        void draw_trajectory();

        string* get_timestring();

        int get_ndv(int n);
        string* get_ndvNames(int n);

        float get_maxval() { return _maxval; };
        float get_minval() { return _minval; };

    protected:
        UFS_Geometry* ufs_geometry;
        UFS_NCL_Viewer* ufs_nclviewer;
        UFS_GL_Viewer*  ufs_glviewer;
        WindVector* windvector;
        Trajectory* trajectory;
        LineIntegralConvolution* lic;
        MarchingCube marchingCube;
        Locator* locator;

        vector<string> ufs_timestring;

        float* lon;
        float* lat;

        float* u1;
        float* v1;
        float* w1;
        float* h1;

        float* u2;
        float* v2;
        float* w2;
        float* h2;

        float* ua;
        float* va;
        float* wa;
        float* ha;

        float maxspd;

        int nxs, nys, nzs;
        int nxp, nyp, nzp;

        bool drawWindVector;

        void _setup4ufs();
        void _setup_ufs_timestring();

        void _get_vector(int nt);
};
#endif

