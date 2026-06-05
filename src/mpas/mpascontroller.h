#ifndef MPASCONTROLLER_H
#define MPASCONTROLLER_H

#include <QtOpenGL>

#include "colorTable.h"
#include "coastline.h"
#include "mpasstaticreader.h"
#include "mpasdatareader.h"
#include "mpasgeometry.h"
#include "mpas2dviewer.h"
#include "locator.h"

using namespace std;

class MPASController
{
    public:
        MPASController(ColorTable *ct, NVOptions* opt,
		       string static_flnm, string data_flnm);
       ~MPASController();

        int get_nCells() { return geometry->get_nCells(); };
        int get_nVertices() { return geometry->get_nVertices(); };

        int get_nz() { return geometry->get_nVertLevels(); };
        int get_nt() { return geometry->get_nTime(); };
        int get_tl() { return _tvalue; };

        void set_colorTable(ColorTable *ct);
        void set_locator(Locator* l);

        void set1dvarname(string vn);
        void set2dvarname(string vn);
        void set3dvarname(string vn);

        void draw();

        void set_tl(int n);

        string get_varname() { return _varname; };
        string get_title() { return _title; };
        string* get_timestring();

      //MPASEvaluator* get_evaluator() { return evaluator; };
        MPASGeometry* get_geometry() { return geometry; };

        int get_curTime() { return _curTime; };
        int get_ndv(int n);
        int get_nfiles() { return 1; };
        int* get_ntimes() { return _ntimes; };

        vector<string> get_ndvNames(int n);
        void set_fileNtime(int nf, int nt);
        void setup();

        float get_minval() { return _minval; };
        float get_maxval() { return _maxval; };

	bool is2d(string vn);
	bool is3d(string vn);

    protected:
        MPASStaticReader* ncstatic;
        MPASDataReader* ncdata;
        Locator* locator;
        ColorTable* colorTable;
        NVOptions* nvoptions;
        MPASGeometry* geometry;

        MPAS2dViewer *mpas2dviewer;
      //MPAS3dViewer *mpas3dviewer;

        int _max_frame;

        vector<string> _list2dvars;
        vector<string> _list3dvars;

        string _varname;
        string _timestr;
        string _title;

        int _preFile;
        int _curFile;
        int _glbTime;
        int _curTime;
        int _tvalue;

        bool _initialized;

        int* _grdsize;
        int* _varsize;
        int* _ntimes;

        float* _value;
        float _minval;
        float _maxval;

        void _set_glbTime();
};
#endif

