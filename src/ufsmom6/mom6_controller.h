#ifndef _MOM6CONTROLLER_H
#define _MOM6CONTROLLER_H

#include <QtOpenGL>

#include <iostream>

#include "ufsmom6reader.h"
#include "mom6_viewer.h"
#include "locator.h"

using namespace std;

#ifndef NAME_LENG
#define NAME_LENG	1024
#endif

class MOM6Controller
{
    public:
        MOM6Controller(ColorTable* ct, NVOptions* opt,
                       const char* fn);
       ~MOM6Controller();

        void setup();

        int getNxh() { return geometry->get_nxh(); };
        int getNyh() { return geometry->get_nyh(); };
        int getNzl() { return geometry->get_nzl(); };
        int getNtime() { return _ntime; };
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
        MOM6Geometry* get_geometry() { return geometry; };

        float get_minval() { return _minval; };
        float get_maxval() { return _maxval; };

        int get_callList();
        void update_file(const char* fn);

        int get_curTime() { return _curTime; };
        int get_ndv(int n);
        int get_nfiles() { return _maxFile; };
        int* get_ntimes() { return _ntimes; };

        vector<string> get_ndvNames(int n);
        void set_fileNtime(int nf, int nt);

    protected:
        UFSMOM6Reader* ncfile;
        MOM6Geometry* geometry;
        ColorTable* colorTable;
        NVOptions* nvoptions;
        CoastLine* coastline;
      //Lister* lister;
        Locator* locator;

        char _flnm[NAME_LENG];

        MOM62dViewer* mom6_viewer;
      //MOM63dViewer* mom6_3dviewer;

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
        int _ntime;

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
};
#endif

