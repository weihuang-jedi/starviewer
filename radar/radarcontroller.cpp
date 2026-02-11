//$Id: radarcontroller.cpp 14610 2013-10-25 21:50:16Z starviewer $

#include "radarcontroller.h"

RADARController::RADARController(ColorTable *ct, NVOptions* opt,
                                 string fn, bool isList, string id)
{
    colorTable = ct;
    nvoptions = opt;

    cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__
         << ", file: <" << __FILE__ << ">" << endl;
    cout << "\tOpen file: <" << fn << ">" << endl;

    radar = new RADARReader(fn, id);
    radar->setup();

  //radar->to_gif();

  //_preFile = -1;
  //_curFile = 0;
  //_glbTime = 0;
  //_curTime = 0;

  //radar2dviewer = new RADAR2dViewer(colorTable, nvoptions);
  //radar2dviewer->set_radar(radar);

    radar3dviewer = new RADAR3dViewer(colorTable, nvoptions);
    radar3dviewer->set_radar(radar);

    _varname = radar->get_varname();
    _setup();
}

RADARController::~RADARController()
{
} 

void RADARController::_setup()
{
    _tvalue = 0;

  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;

    radar3dviewer->setup(_varname);
}

void RADARController::draw()
{
    if(nvoptions->get_zsec() < radar3dviewer->get_nsweeps())
       radar3dviewer->draw();
    else
       radar3dviewer->draw_isosurface();
}

void RADARController::set_colorTable(ColorTable *ct)
{
    radar3dviewer->set_colorTable(ct);
}

#if 0
string* RADARController::get_timestring()
{
    return nvfile->get_timestr();
}

void RADARController::set_fileNtime(int nf, int nt)
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
        _minval = radar3dviewer->get_minval();
        _maxval = radar3dviewer->get_maxval();
    }

    _preFile = _curFile;

    gridsize = _curTime * geometry->get_nVertices() *  geometry->get_nVertLevels();

    radar3dviewer->setup(_varname, &_value[gridsize]);
}

void RADARController::_set_glbTime()
{
    int n;

    _glbTime = _curTime;
    for(n = 0; n < _curFile; ++n)
        _glbTime += _ntimes[n];
}
#endif

void RADARController::set_locator(Locator* l)
{
     locator = l;

     radar3dviewer->set_locator(l);
}

int RADARController::get_varnumber()
{
    return radar->get_varnumber();
}

vector<string> RADARController::get_varnames()
{
    return radar->get_varnames();
}

string RADARController::get_varname()
{
    return radar->get_varname();
}

string RADARController::get_title()
{
    return radar->get_varname();
}

void RADARController::set1dvarname(string vn)
{
    _varname = vn;

    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
    cout << "setup for <" << vn << ">" << endl;
}

void RADARController::set2dvarname(string vn)
{
    _varname = vn;

    _tvalue = 0;

  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\tfunction: " << __PRETTY_FUNCTION__ << endl;
  //cout << "setup for <" << vn << ">" << endl;

    radar3dviewer->setup(vn);

    _curFile = 0;
    _curTime = 0;
    _glbTime = 0;

    _minval = radar3dviewer->get_minval();
    _maxval = radar3dviewer->get_maxval();
}

void RADARController::get_location(double &blon, double &blat)
{
    radar3dviewer->get_location(blon, blat);
}

