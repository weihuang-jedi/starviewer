#ifndef _Controller_H
#define _Controller_H

//$Id: controller.h 4865 2014-01-01 15:49:23Z starviewer $

#include <QtOpenGL>

#include <iostream>
#include <string>

#include "ncreader.h"
#include "nvoptions.h"
#include "gl_viewer.h"
#include "spreadsheet.h"
#include "pixel_viewer.h"

using namespace std;

class Controller
{
    public:
        Controller(ColorTable* ct, NVOptions* opt,
                   string fn, bool isList = false);
        virtual ~Controller();

        virtual void setup();
        virtual void draw();
        virtual void set2dvarname(string vn);
        virtual void set3dvarname(string vn);
        virtual void set_fileNtime(int nf, int nt);

        virtual void set_selecting(bool b);

        virtual void update_colormap();

        virtual void setup_vector();
        virtual void unset_vector();

        virtual string* get_timestring();

        virtual void set_colorTable(ColorTable* ct);

        int get_mx() { return _mx; };
        int get_my() { return _my; };
        int get_mz() { return _mz; };

        int get_nx() { return _nx; };
        int get_ny() { return _ny; };
        int get_nz() { return _nz; };
        int get_nt() { return _nt; };

        int get_curTime() { return _curTime; };
      //int get_nfiles() { return ncfile->get_nfiles(); };
        int get_nfiles() { return 1; };
        int* get_ntimes() { return _ntimes; };
        int  get_ntiles() { return _ntiles; };

        virtual int get_ndv(int n);
        virtual string* get_ndvNames(int n);

        string get_varname() { return _varname; };

        Geometry* get_geometry() { return geometry; };

        float get_minval() { return _minval; };
        float get_maxval() { return _maxval; };

        string get_title() { return _title; };

        float get_valueAt(int i, int j);

        void setup_spreadsheet();
        void update_spreadsheet(int n);

    protected:
        ncReader*   ncfile;
        Geometry*   geometry;
        ColorTable* colorTable;
        GL_Viewer*  glviewer;
        NVOptions*  nvoptions;
        SpreadSheet* spreadsheet;
        Pixel_Viewer* pixelviewer;

        string _flnm;
        string _varname;
        string _timestr;
        string _title;

        int _preFile;
        int _curFile;

        int _preTime;
        int _curTime;

        bool _isFileList;
        bool _initialized;

        bool _selectingOn;

        int _mx;
        int _my;
        int _mz;

        int _nx;
        int _ny;
        int _nz;
        int _nt;
        int _nm;

        int  _ntiles;
        int* _ntimes;
        int* _grdsize;
        int* _varsize;

        float* _value;
        float _minval;
        float _maxval;
};
#endif

