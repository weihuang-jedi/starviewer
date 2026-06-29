#ifndef _UFSCONTROLLER_H
#define _UFSCONTROLLER_H

#include <QtOpenGL>

#include <iostream>

#include "earth.h"
#include "ncreader.h"
#include "ufs_viewer.h"
#include "locator.h"

using namespace std;

#ifndef NAME_LENG
#define NAME_LENG	1024
#endif

class UFSController
{
    public:
        UFSController(ColorTable* ct, ColorTable *wvct, NVOptions* opt,
                      Earth* earth, CoastLine* cl,
                      const char* fn, bool isList = false);
       ~UFSController();

        void setup();

        int getNlon() { return geometry->get_nlon(); };
        int getNlat() { return geometry->get_nlat(); };
        int getNlev() { return geometry->get_nlev(); };
        int getNtim() { return _ntim; };
        int get_tl() { return _tvalue; };

        void set_colorTable(ColorTable* ct);
        void set_locator(Locator* l);

        void set1dvarname(string vn);
        void set2dvarname(string vn);
        void set3dvarname(string vn);

        void draw();

        string get_varname() { return _varname; };
        string get_title() { return _title; };
        string get_timestring();

      //Evaluator* get_evaluator() { return evaluator; };
        UFSGeometry* get_geometry() { return geometry; };

        float get_minval() { return _minval; };
        float get_maxval() { return _maxval; };

        int get_callList();
        void update_file(const char* fn);

        int get_curTime() { return _curTime; };
        int get_ndv(int n);
      //int get_nfiles() { return ncfile->get_nfiles(); };
        int get_nfiles() { return _maxFile; };
        int* get_ntimes() { return _ntimes; };

        vector<string> get_ndvNames(int n);
        void set_fileNtime(int nf, int nt);

    protected:
        ncReader* ncfile;
        UFSGeometry* geometry;
        ColorTable* colorTable;
        NVOptions* nvoptions;
        CoastLine* coastline;
      //Lister* lister;
        Locator* locator;

        char _flnm[NAME_LENG];

        UFS2dViewer* ufs_viewer;
      //UFS3dViewer* ufs_3dviewer;

        int _max_frame;
        int _time_interval;

        string _varname;
        string _timestr;
        string _title;

        int _preFile;
        int _curFile;
        int _maxFile;

        int _glbTime;
        int _curTime;
        int _maxTime;
        int _ntim;

        int _tvalue;

        bool _sphere;
        bool _initialized;

        int* _ntimes;
        int* _grdsize;
        int* _varsize;

        float* _value;
        float _minval;
        float _maxval;

        void _set_glbTime();
        template<typename T>
        void _print1d(T* var, int nl);
};
#endif

