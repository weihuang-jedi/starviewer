#ifndef _UFSincrCONTROLLER_H
#define _UFSincrCONTROLLER_H

#include <QtOpenGL>

#include <iostream>

#include "ncreader.h"
#include "locator.h"
// #include "coastline.h"
// #include "colorTable.h"
// #include "nvoptions.h"
#include "ufs_incr_viewer.h"

using namespace std;

#ifndef NAME_LENG
#define NAME_LENG	1024
#endif

class UFSincrController
{
    public:
        UFSincrController(ColorTable* ct, NVOptions* opt,
                          string atmfile, string sfcfile,
                          vector<string> datafiles);
       ~UFSincrController();

        void setup();

        int getNlon() { return incr_geometry->get_nlon(); };
        int getNlat() { return incr_geometry->get_nlat(); };
        int getNlev() { return incr_geometry->get_nlev(); };
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
        UFSincrGeometry* get_incr_geometry() { return incr_geometry; };

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
        UFSincrGeometry* incr_geometry;
        ColorTable* colorTable;
        NVOptions* nvoptions;
        CoastLine* coastline;
      //Lister* lister;
        Locator* locator;

        char _flnm[NAME_LENG];

        UFSincr2dViewer* ufs_incr_viewer;
      //UFSincr3dViewer* ufs_3dviewer;

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
        bool _ball;
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

        string _atmfile;
        string _sfcfile;
        vector<string> _datafiles;
};
#endif

