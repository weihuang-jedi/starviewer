//$Id: vtkcontroller.cpp 4865 2014-01-01 15:49:23Z starviewer $

#include "vtkcontroller.h"

VTKController::VTKController(ColorTable *ct, NVOptions* opt,
                             string fn, bool isList)
{
    colorTable = ct;
    nvoptions = opt;
    multipleFiles = isList;

    cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__
         << ", file: <" << __FILE__ << ">" << endl;
    cout << "\tOpen file: <" << fn << ">" << endl;

    reader = new VTKReader(fn);
  //vtk->setup();

  //_preFile = -1;
  //_curFile = 0;
  //_glbTime = 0;
  //_curTime = 0;

    vtk2dviewer = new VTK2dViewer(colorTable, nvoptions);
    vtk2dviewer->set_reader(reader);

  //vtk3dviewer = new VTK3dViewer(colorTable, nvoptions);
  //vtk3dviewer->set_reader(reader);

    _varname = fn;
    _setup();
}

VTKController::~VTKController()
{
} 

void VTKController::_setup()
{
    _tvalue = 0;

  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;

    vtk2dviewer->setup(_varname);
  //vtk3dviewer->setup(_varname);
}

void VTKController::draw()
{
    vtk2dviewer->draw();
  //vtk3dviewer->draw();
}

void VTKController::set_colorTable(ColorTable *ct)
{
    vtk2dviewer->set_colorTable(ct);
  //vtk3dviewer->set_colorTable(ct);
}

#if 0
string* VTKController::get_timestring()
{
    return nvfile->get_timestr();
}

void VTKController::set_fileNtime(int nf, int nt)
{
    size_t gridsize = 1;

  //cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
  //cout << "\tcurFile: " << nf << ", curTime: " << nt << ", varname: <" << _varname << ">" << endl;

    _curFile = nf;
    _curTime = nt;

    if(_preFile != _curFile)
    {
        nvfile->select_file(nf);

        _value = nvfile->get_dv(_varname);
        _title = nvfile->get_title();

        geometry->set_nt(_ntimes[_curFile]);

        _set_glbTime();
        _minval = vtk2dviewer->get_minval();
        _maxval = vtk2dviewer->get_maxval();
      //_minval = vtk3dviewer->get_minval();
      //_maxval = vtk3dviewer->get_maxval();
    }

    _preFile = _curFile;

    gridsize = _curTime * geometry->get_nVertices() *  geometry->get_nVertLevels();

    vtk2dviewer->setup(_varname, &_value[gridsize]);
  //vtk3dviewer->setup(_varname, &_value[gridsize]);
}

void VTKController::_set_glbTime()
{
    int n;

    _glbTime = _curTime;
    for(n = 0; n < _curFile; ++n)
        _glbTime += _ntimes[n];
}
#endif

void VTKController::set_locator(Locator* l)
{
     locator = l;

     vtk2dviewer->set_locator(l);
   //vtk3dviewer->set_locator(l);
}

int VTKController::get_varnumber()
{
    return 1;
}

vector<string> VTKController::get_varnames()
{
    vector<string> varnames;
    varnames.push_back(_varname);
    return varnames;
}

string VTKController::get_varname()
{
    return _varname;
}

string VTKController::get_title()
{
    return _varname;
}

void VTKController::set1dvarname(string vn)
{
    _varname = vn;

    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
    cout << "setup for <" << vn << ">" << endl;
}

void VTKController::set2dvarname(string vn)
{
    _varname = vn;

    _tvalue = 0;

  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\tfunction: " << __PRETTY_FUNCTION__ << endl;
  //cout << "setup for <" << vn << ">" << endl;

    vtk2dviewer->setup(vn);
  //vtk3dviewer->setup(vn);

    _curFile = 0;
    _curTime = 0;
    _glbTime = 0;

    _minval = vtk2dviewer->get_minval();
    _maxval = vtk2dviewer->get_maxval();
  //_minval = vtk3dviewer->get_minval();
  //_maxval = vtk3dviewer->get_maxval();
}

