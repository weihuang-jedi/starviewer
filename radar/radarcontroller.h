#ifndef RADARCONTROLLER_H
#define RADARCONTROLLER_H

//$Id: radarcontroller.h 14599 2013-10-21 15:00:21Z starviewer $

#include <QtOpenGL>

#include "colorTable.h"
#include "radarreader.h"
#include "radar2dviewer.h"
#include "radar3dviewer.h"
#include "locator.h"

using namespace std;

#ifndef NAME_LENG
#define NAME_LENG	1024
#endif

class RADARController
{
    public:
        RADARController(ColorTable *ct, NVOptions* opt,
                        string fn, bool isList, string id);
       ~RADARController();

        void set1dvarname(string vn);
        void set2dvarname(string vn);
        void set_colorTable(ColorTable *ct);
        void set_locator(Locator* l);

        void draw();

#if 0
        int get_curTime() { return _curTime; };
        int get_nfils() { return nvfile->get_nfils(); };
        int* get_ntimes() { return _ntimes; };

        void set_fileNtime(int nf, int nt);
#endif

        void get_location(double &blon, double &blat);

        double get_minval() { return _minval; };
        double get_maxval() { return _maxval; };

        string get_varname();
        string get_title();
        string* get_timestring();

        int get_varnumber();
        vector<string> get_varnames();

        int get_nsweeps() { return radar->get_nsweeps(); };
        int get_nrays() { return radar->get_nrays(); };
        int get_nbins() { return radar->get_nbins(); };

    protected:
        Locator* locator;
        ColorTable* colorTable;
        NVOptions* nvoptions;
        RADARReader* radar;

        char _flnm[NAME_LENG];

        RADAR2dViewer *radar2dviewer;
        RADAR3dViewer *radar3dviewer;

        int _max_frame;

        string _varname;
        string _timestr;
        string _title;

        int _preFile;
        int _curFile;
        int _glbTime;
        int _curTime;
        int _tvalue;

        int* _ntimes;

        double _minval;
        double _maxval;

        void _setup();
#if 0
        void _set_glbTime();
#endif
};
#endif

