#ifndef _EAGLELAM_Controller_H
#define _EAGLELAM_Controller_H

//$Id: eaglelam_controller.h 4968 2014-02-13 15:32:52Z starviewer $

#include <QtOpenGL>
#include <vector>
#include <unordered_map>

#include "eaglereader.h"
#include "eaglelam_viewer.h"
#include "eaglelam_geometry.h"
#include "marchingcube.h"
#include "controller.h"
#include "windvector.h"
#include "trajectory.h"
#include "locator.h"
#include "lic.h"

using namespace std;

class EAGLELAM_Controller
{
    public:
        EAGLELAM_Controller(ColorTable *ct, NVOptions* opt,
                            vector<string> datafiles);
       ~EAGLELAM_Controller();

        void setup();
        void draw();
        void setvarname(string vn);
        void set_fileNtime(int nf, int nt);

        void set_locator(Locator* l);

        void setup_vector();
        void unset_vector();

        void set_colorTable(ColorTable* ct) { colorTable = ct; };
        void update_colormap();
        void draw_isosurface();
        void draw_lic();
        void draw_vector();
        void draw_trajectory();

        string* get_timestring();
        string get_varname() { return _varname; };

        int get_nx() { return _nx; };
        int get_ny() { return _ny; };

        int get_numvars() { return _numvars; };
        vector<string> get_varlist() { return varlist; };

        float get_valmax() { return _valmax; };
        float get_valmin() { return _valmin; };

        double* get_xGrid() { return _xGrid; };
        double* get_yGrid() { return _yGrid; };

        EAGLELAM_Geometry* get_geometry() { return geometry; };

    protected:
        EagleReader* ncfile;
        EAGLELAM_Geometry* geometry;
        EAGLELAM_Viewer*  eaglelam_viewer;
        WindVector* windvector;
        Trajectory* trajectory;
        LineIntegralConvolution* lic;
        MarchingCube marchingCube;
        Locator* locator;

        ColorTable* colorTable;
        NVOptions* nvoptions;
        CoastLine* coastline;

        vector<string> eaglelam_timestring;
	unordered_map<string, EagleReader*> var2file;
        vector<string> datafiles;
        vector<string> varlist;

        double* _xGrid;
        double* _yGrid;

        float* lon;
        float* lat;

        float* u;
        float* v;
        float* _value;

        float maxspd;

        float _valmax;
        float _valmin;

        int nxs, nys, nzs;
        int nxp, nyp, nzp;

	int _nx, _ny, _nz, _nt;

	int _numvars;

        bool drawWindVector;

        void _setup4eaglelam();
        void _setup_eaglelam_timestring();

        void _get_vector(int nt);

	string _varname;
	string _title;

	string _grid_mapping_name;
        float _latitude_of_projection_origin;
        float _longitude_of_central_meridian;
        vector<float> _standard_parallel;
};
#endif

