#ifndef _UFSCONTROLLER_H
#define _UFSCONTROLLER_H

#include <QtOpenGL>

#include <iostream>

#include "nvFile.h"
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
        UFSController(ColorTable* ct, NVOptions* opt,
                        const char* fn, bool isList = false);
       ~UFSController();

        void setup();

        int get_nlon() { return geometry->get_nlon(); };
        int get_nlat() { return geometry->get_nlat(); };
        int get_nlev() { return geometry->get_nlev(); };
        int get_ntim() { return geometry->get_ntim(); };
        int get_tl() { return _tvalue; };

        void set_mappingFile(string mfnm);

        void set_colorTable(ColorTable* ct);
        void set_locator(Locator* l);

        void set1dvarname(string vn);
        void set2dvarname(string vn);
        void set3dvarname(string vn);

        void draw();

        string get_varname() { return _varname; };
        string get_title() { return _title; };
        string* get_timestring();

      //Evaluator* get_evaluator() { return evaluator; };
        UFSGeometry* get_geometry() { return geometry; };

        double get_minval() { return _minval; };
        double get_maxval() { return _maxval; };

        int get_callList();
        void update_file(const char* fn);

        int get_curTime() { return _curTime; };
        int get_ndv(int n);
        int get_nfils() { return nvfile->get_nfils(); };
        int* get_ntimes() { return _ntimes; };

        string* get_ndvNames(int n);
        void set_fileNtime(int nf, int nt);

    protected:
        ncReader* ncfile;
        NVFile* nvfile;
        NVFile* mappingfile;
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

        int _tvalue;

        bool _sphere;
        bool _ball;
        bool _initialized;

        int* _ntimes;
        int* _grdsize;
        int* _varsize;

        double* _value;
        double _minval;
        double _maxval;

        void _set_glbTime();

        bool _hasMappingFile;
        string _mappingFilename;
};
#endif

