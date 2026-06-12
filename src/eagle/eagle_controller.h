#ifndef _EAGLECONTROLLER_H
#define _EAGLECONTROLLER_H

#include <QtOpenGL>

#include <iostream>

#include "eaglereader.h"
#include "eagle_viewer.h"
#include "locator.h"

using namespace std;

#ifndef NAME_LENG
#define NAME_LENG	1024
#endif

class EAGLEController
{
    public:
        EAGLEController(ColorTable* ct, NVOptions* opt, const char* fn);
       ~EAGLEController();

        void setup();

        int getNx() { return geometry->get_nx(); };
        int getNy() { return geometry->get_nx(); };
        int getNtime() { return _ntime; };

        void set_colorTable(ColorTable* ct);
        void set_locator(Locator* l);

        void setvarname(string vn);

        void draw();

        string get_varname() { return _varname; };
        string get_title() { return _title; };
        string get_timestring();

      //Evaluator* get_evaluator() { return evaluator; };
        EAGLEGeometry* get_geometry() { return geometry; };

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
        EagleReader* ncfile;
        EAGLEGeometry* geometry;
        ColorTable* colorTable;
        NVOptions* nvoptions;
        CoastLine* coastline;
      //Lister* lister;
        Locator* locator;

        char _flnm[NAME_LENG];

        EAGLE2dViewer* eagle_viewer;

        int _max_frame;
        int _time_interval;

        string _varname;
        string _timestr;
        string _title;

        int _preFile;
        int _curFile;
        int _maxFile;

        int _nx;
        int _ny;
        int _ntime;
        int _glbTime;
        int _curTime;
        int _maxTime;

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

