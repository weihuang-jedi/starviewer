#ifndef _UFSINCR_CONTROLLER_H
#define _UFSINCR_CONTROLLER_H

#include <iostream>

#include <QWidget>
#include <QGLWidget>

#include "ufsgridreader.h"
#include "ufsincrementreader.h"
#include "ufsincr_viewer.h"
#include "locator.h"

using namespace std;

#ifndef NAME_LENG
#define NAME_LENG	1024
#endif

class UFSINCRController
{
    public:
        UFSINCRController(ColorTable* ct, NVOptions* opt,
                          vector<string> gridflnm, vector<string> incrflnm);
       ~UFSINCRController();

        void setup();

        int getNlon() { return _nlon; };
        int getNlat() { return _nlat; };
        int getNlev() { return _nlev; };
        int getNtim() { return _ntim; };
        int getNtiles() { return _ntiles; };
        int get_tl() { return _tvalue; };

        void set_colorTable(ColorTable* ct);
        void set_locator(Locator* l);

        void set1dvarname(string vn);
        void set2dvarname(string vn);
        void set3dvarname(string vn);

        void draw();

        string get_varname() { return _varname; };
        string get_title() { return _title; };

      //Evaluator* get_evaluator() { return evaluator; };
        vector<UFSINCRGeometry*> get_geometry() { return geometry; };

        float get_minval() { return _minval; };
        float get_maxval() { return _maxval; };

        int get_callList();
        void update_file(const char* fn);

        int get_curTime() { return _curTime; };
        int get_ndv(int n);
      //int get_nfiles() { return ncfile->get_nfiles(); };
        int get_nfiles() { return _maxFile; };

        int get_nlon() { return _nlon; };
        int get_nlat() { return _nlat; };
        int get_nlev() { return _nlev; };

        vector<string> get_ndvNames(int n);

	QWidget* get_viewer() { return dynamic_cast<QWidget*>(ufsincr_viewer); };

    protected:
        vector<UFSGridReader*> ncgridfile;
        vector<UFSIncrementReader*> ncincrfile;
        vector<UFSINCRGeometry*> geometry;
        ColorTable* colorTable;
        NVOptions* nvoptions;
        CoastLine* coastline;
      //Lister* lister;
        Locator* locator;

        vector<string> _gridfilenames;
        vector<string> _incrementfilenames;

        UFSINCR2dViewer* ufsincr_viewer;
      //UFSINCR3dViewer* ufsincr_3dviewer;

        int _max_frame;

        string _varname;
        string _title;

        int _preFile;
        int _curFile;
        int _maxFile;

        int _glbTime;
        int _curTime;
        int _maxTime;
        int _ntiles;
        int _ntim;
        int _nlon;
        int _nlat;
        int _nlev;

        int _tvalue;

        bool _sphere;
        bool _ball;
        bool _initialized;

        int* _grdsize;
        int* _varsize;

        vector<float*> _value;

        float _minval;
        float _maxval;

        template<typename T>
        void _print1d(T* var, int nl);
        void _update_value(const char* vn);
};
#endif

