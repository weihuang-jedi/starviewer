#ifndef _HDFCONTROLLER_H
#define _HDFCONTROLLER_H

//$Id: hdfcontroller.h 5247 2014-12-07 17:35:51Z starviewer $

#include <QtOpenGL>

#include "nvFile.h"
#include "colorTable.h"
#include "hdfgeometry.h"
#include "hdf2dviewer.h"
#include "locator.h"

#ifndef NAME_LENG       
#define NAME_LENG       1024
#endif

using namespace std;

class HDFController
{
    public:
        HDFController(ColorTable *ct, NVOptions* opt,
                      const char *fn, bool isList = false);
       ~HDFController();

        int get_nlon() { return geometry->get_nlon(); };
        int get_nlat() { return geometry->get_nlat(); };
        int get_nlev() { return geometry->get_nlev(); };

        int get_nx() { return geometry->get_nlon(); };
        int get_ny() { return geometry->get_nlat(); };
        int get_nz() { return geometry->get_nlev(); };
        int get_nt() { return geometry->get_nTime(); };
        int get_tl() { return _tvalue; };

        double get_minval() { return _valmin; }
        double get_maxval() { return _valmax; }

        void set_colorTable(ColorTable *ct);
        void set2dvarname(string vn);
        void set3dvarname(string vn);

        void draw();

        void set_xsec(int n);
        void set_ysec(int n);
        void set_zlevel(int n);

        void set_tl(int n) { _tvalue = n; };

        string get_varname() { return _varname; };
        string get_title() { return _title; };

      //Evaluator* get_evaluator() { return evaluator; };
        HDFGeometry* get_geometry() { return geometry; };

        int get_curTime() { return _curTime; };
        int get_ndv(int n);
        int get_nfils() { return nvfile->get_nfils(); };
        int* get_ntimes() { return _ntimes; };

        string* get_ndvNames(int n);
        string* get_timestring();

        void set_fileNtime(int nf, int nt);
        void set_locator(Locator* l);

    protected:
        NVFile* nvfile;
        Locator* locator;
        ColorTable* colorTable;
        NVOptions* nvoptions;
        HDFGeometry* geometry;

        char _flnm[NAME_LENG];

        HDF2dViewer *hdf2dviewer;
      //HDF3dViewer *hdf3dviewer;

        int _max_frame;
        int _time_interval;

        string _varname;
        string _timestr;
        string _title;

        int _tvalue;

        bool _initialized;

        int _preFile;
        int _curFile;
        int _curTime;

        int* _ntimes;
        int* _grdsize;
        int* _varsize;

        double* _value;
        double _valmin;
        double _valmax;
	double _fillvalue;

        void _setup();
};
#endif

