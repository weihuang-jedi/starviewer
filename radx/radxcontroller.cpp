//$Id: radxcontroller.cpp 4970 2014-02-13 16:49:18Z starviewer $

#include "radxcontroller.h"

RadxController::RadxController(ColorTable *ct, NVOptions* opt,
                                 string fn, bool isList)
{
    colorTable = ct;
    nvoptions = opt;

    cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__
         << ", file: <" << __FILE__ << ">" << endl;
    cout << "\tOpen file: <" << fn << ">" << endl;

    radx = new RadxReader(fn);
    radx->setup();

  //_preFile = -1;
  //_curFile = 0;
  //_glbTime = 0;
  //_curTime = 0;

  //radx2dviewer = new Radx2dViewer(colorTable, nvoptions);
  //radx2dviewer->set_radx(radx);

    radx3dviewer = new Radx3dViewer(colorTable, nvoptions);
    radx3dviewer->set_radx(radx);

    _varname = radx->get_varname();
    _setup();
}

RadxController::~RadxController()
{
    delete radx;
    delete radx3dviewer;
} 

void RadxController::_setup()
{
    _tvalue = 0;

  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;

    radx3dviewer->setup(_varname);
}

void RadxController::draw()
{
    if(nvoptions->get_zsec() < radx3dviewer->get_nsweeps())
       radx3dviewer->draw();
    else
       radx3dviewer->draw_isosurface();
}

void RadxController::set_colorTable(ColorTable *ct)
{
    radx3dviewer->set_colorTable(ct);
}

#if 0
string* RadxController::get_timestring()
{
    return nvfile->get_timestr();
}

void RadxController::set_fileNtime(int nf, int nt)
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
        _minval = radx3dviewer->get_minval();
        _maxval = radx3dviewer->get_maxval();
    }

    _preFile = _curFile;

    gridsize = _curTime * geometry->get_nVertices() *  geometry->get_nVertLevels();

    radx3dviewer->setup(_varname, &_value[gridsize]);
}

void RadxController::_set_glbTime()
{
    int n;

    _glbTime = _curTime;
    for(n = 0; n < _curFile; ++n)
        _glbTime += _ntimes[n];
}
#endif

void RadxController::set_locator(Locator* l)
{
     locator = l;

     radx3dviewer->set_locator(l);
}

int RadxController::get_varnumber()
{
    return radx->get_varnumber();
}

vector<string> RadxController::get_varnames()
{
    return radx->get_varnames();
}

string RadxController::get_varname()
{
    return radx->get_varname();
}

string RadxController::get_title()
{
    return radx->get_varname();
}

void RadxController::set1dvarname(string vn)
{
    _varname = vn;

    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
    cout << "setup for <" << vn << ">" << endl;
}

void RadxController::set2dvarname(string vn)
{
    _varname = vn;

    _tvalue = 0;

  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\tfunction: " << __PRETTY_FUNCTION__ << endl;
  //cout << "setup for <" << vn << ">" << endl;

    radx3dviewer->setup(vn);

    _curFile = 0;
    _curTime = 0;
    _glbTime = 0;

    _minval = radx3dviewer->get_minval();
    _maxval = radx3dviewer->get_maxval();
}

void RadxController::get_location(double &blon, double &blat, double &balt)
{
    radx3dviewer->get_location(blon, blat, balt);
}

