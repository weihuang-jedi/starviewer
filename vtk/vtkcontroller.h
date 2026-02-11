#ifndef _VTKCONTROLLER_H_
#define _VTKCONTROLLER_H_

//$Id: vtkcontroller.h 4849 2013-12-05 15:43:40Z starviewer $

#include <QtOpenGL>

#include "colorTable.h"
#include "vtkreader.h"
#include "vtk2dviewer.h"
#include "vtk3dviewer.h"
#include "locator.h"

using namespace std;

class VTKController
{
    public:
        VTKController(ColorTable *ct, NVOptions* opt,
                      string fn, bool isList);
       ~VTKController();

        void set1dvarname(string vn);
        void set2dvarname(string vn);
        void set_colorTable(ColorTable *ct);
        void set_locator(Locator* l);

        void draw();

        double get_minval() { return _minval; };
        double get_maxval() { return _maxval; };

        string get_varname();
        string get_title();
        string* get_timestring();

        int get_varnumber();
        vector<string> get_varnames();

    protected:
        Locator* locator;
        ColorTable* colorTable;
        NVOptions* nvoptions;
        VTKReader* reader;

        bool multipleFiles;

        VTK2dViewer *vtk2dviewer;
        VTK3dViewer *vtk3dviewer;

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
};
#endif

