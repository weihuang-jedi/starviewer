//$Id: controller.cpp 4927 2014-01-23 21:49:37Z starviewer $

#include <iostream>
#include <string>

#include "controller.h"

Controller::Controller(ColorTable *ct, NVOptions* opt,
                       string fn, bool isList)
{
  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\tfilename: <" << fn << ">, isList = " << isList << endl;

    colorTable = ct;
    nvoptions = opt;

    _flnm = fn;
    _isFileList = isList;

    _initialized = false;

    _preFile = -1;
    _curFile = 0;

    _preTime = -1;
    _curTime = 0;

    geometry = NULL;
    nvfile = NULL;
    glviewer = NULL;
    nclviewer = NULL;
    pixelviewer = NULL;
    spreadsheet = NULL;
}

Controller::~Controller()
{
    if(NULL != geometry)
        delete geometry;
    if(NULL != nvfile)
        delete nvfile;
    if(NULL != glviewer)
        delete glviewer;
    if(NULL != nclviewer)
        delete nclviewer;
    if(NULL != pixelviewer)
        delete pixelviewer;
    if(NULL != spreadsheet)
        delete spreadsheet;
} 

void Controller::setup()
{
  //cout << "\nEnter functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__
  //     << ", file: <" << __FILE__ << ">" << endl;

    nvfile = new NVFile(_flnm, _isFileList);
    _grdsize = nvfile->get_grdsize();
    _ntimes = nvfile->get_ntimes();

    geometry = new Geometry();
    geometry->set_name(_flnm);
    geometry->set_mx(_grdsize[2]);
    geometry->set_my(_grdsize[1]);
    geometry->set_mz(_grdsize[0]);
    geometry->set_nt(_ntimes[0]);

    geometry->set_has1dLon(nvfile->get_has1dLon());
    if(nvfile->get_has1dLon())
       geometry->set_lon(nvfile->get_lon_value());
    geometry->set_has1dLat(nvfile->get_has1dLat());
    if(nvfile->get_has1dLat())
       geometry->set_lat(nvfile->get_lat_value());
    geometry->set_has1dLev(nvfile->get_has1dLev());
    if(nvfile->get_has1dLev())
       geometry->set_lev(nvfile->get_lev_value());

    geometry->set_has2dLon(nvfile->get_has2dLon());
    if(nvfile->get_has2dLon())
       geometry->set_lon(nvfile->get_lon_value());
    geometry->set_has2dLat(nvfile->get_has2dLat());
    if(nvfile->get_has2dLat())
       geometry->set_lat(nvfile->get_lat_value());

    _varname = nvfile->get_varname(2);

  //cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">, line: "
  //     << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
  //cout << "\tnvfile->get_has2dLon() name: <" << nvfile->get_has2dLon() << ">" << endl;
  //cout << "\tnvfile->get_has2dLat() name: <" << nvfile->get_has2dLon() << ">" << endl;

    if(0 == _varname.compare("unknown"))
    {
        _varname = nvfile->get_varname(3);

      //cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">, line: "
      //     << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
      //cout << "\tvariable name: <" << _varname << ">" << endl;
    }

  //cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">, line: "
  //     << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
  //cout << "\tvariable name: <" << _varname << ">" << endl;

    _value = nvfile->get_dv(_varname);
    _title = nvfile->get_title();

    geometry->set_hasFillValue(nvfile->get_hasFillValue());
    if(geometry->get_hasFillValue())
       geometry->set_fillValue(nvfile->get_fillValue());

    _varsize = nvfile->get_varsize();
    geometry->set_nx(_varsize[2]);
    geometry->set_ny(_varsize[1]);
    geometry->set_nz(_varsize[0]);

  //geometry->print();

    _mx = geometry->get_mx();
    _my = geometry->get_my();
    _mz = geometry->get_mz();

    _nx = geometry->get_nx();
    _ny = geometry->get_ny();
    _nz = geometry->get_nz();
    _nt = geometry->get_nt();
    _nm = 1;

  //cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">, line: "
  //     << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
  //cout << "\tvariable name: <" << _varname << ">" << endl;
  //cout << "\ttitle: <" << _title << ">" << endl;

  //We need to setup 2 viewers, no matter which one is currently used.
    nclviewer = new NCL_Viewer(colorTable, nvoptions);
    nclviewer->set_geometry(geometry);
    nclviewer->setup(_varname, _value);

    glviewer = new GL_Viewer(colorTable, nvoptions);
    glviewer->set_geometry(geometry);
    glviewer->setup(_varname, _value);

    pixelviewer = new Pixel_Viewer(nvoptions);
    pixelviewer->set_colorTable(colorTable);
    pixelviewer->set_geometry(geometry);
    pixelviewer->setup(_varname, _value);

    _minval = glviewer->get_minval();
    _maxval = glviewer->get_maxval();
}

void Controller::setup_spreadsheet()
{
    spreadsheet = new SpreadSheet();
    spreadsheet->resize(1200, 900);
    spreadsheet->set_level(0);
    spreadsheet->set_geometry(geometry);
    spreadsheet->setup(_varname, _value);
    spreadsheet->show();
}

void Controller::update_spreadsheet(int n)
{
    spreadsheet->set_level(n);
    spreadsheet->setup(_varname, _value);
}

void Controller::set_fileNtime(int nf, int nt)
{
    size_t gridsize = 1;

  //cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
  //cout << "\tcurFile: " << nf << ", curTime: " << nt << ", varname: <" << _varname << ">" << endl;

    _curFile = nf;
    _curTime = nt;

    if(_preFile != _curFile)
    {
        if(_initialized)
            free(_value);

        nvfile->select_file(nf);

        _value = nvfile->get_dv(_varname);
        _title = nvfile->get_title();

        _varsize = nvfile->get_varsize();
        geometry->set_nx(_varsize[2]);
        geometry->set_ny(_varsize[1]);
        geometry->set_nz(_varsize[0]);

        geometry->set_nt(_ntimes[_curFile]);

        _minval = glviewer->get_minval();
        _maxval = glviewer->get_maxval();

        _initialized = true;
    }
 
    _preFile = _curFile;

    gridsize = _curTime * geometry->get_nx() *  geometry->get_ny() *  geometry->get_nz();

    pixelviewer->setup(_varname, &_value[gridsize]);
    nclviewer->setup(_varname, &_value[gridsize]);
    glviewer->setup(_varname, &_value[gridsize]);
}

void Controller::draw()
{
  //cout << "\nEnter functions: <" << __PRETTY_FUNCTION__
  //     << ">, line: " << __LINE__
  //     << ", file: <" << __FILE__ << ">" << endl;
  //cout << "\tnvoptions->get_cb(NV_USENCL) = " << nvoptions->get_cb(NV_USENCL) << endl;
  //cout << "\tnvoptions->get_cb(NV_PIXELON) = " << nvoptions->get_cb(NV_PIXELON) << endl;

    if(nvoptions->get_cb(NV_DATAVIEWON))
    {
        if(nvoptions->get_cb(NV_CLEARDATAVIEW))
        {
            spreadsheet->clear();
            nvoptions->set_cb(NV_CLEARDATAVIEW, false);

            update_spreadsheet(nvoptions->get_zsec());
        }
      
      //spreadsheet->draw();
    }

    if(nvoptions->get_cb(NV_PIXELON))
    {
        pixelviewer->draw();
    }
    else
    {
        if(nvoptions->get_cb(NV_USENCL))
            nclviewer->draw();
        else
            glviewer->draw();
    }

  //cout << "Leave functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__
  //     << ", file: <" << __FILE__ << ">" << endl << endl;
}

void Controller::set_selecting(bool b)
{
    cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__
         << ", file: <" << __FILE__ << ">" << endl;
    cout << "\tSelecting is set to " << b << endl;

    _selectingOn = b;
    glviewer->set_selecting(b);
}

void Controller::set2dvarname(string vn)
{
    _varname = vn;

    _curFile = 0;
    _curTime = 0;

    if(_initialized)
        free(_value);

    _value = nvfile->get_dv(vn);
    _title = nvfile->get_title();

    _varsize = nvfile->get_varsize();
    geometry->set_nx(_varsize[2]);
    geometry->set_ny(_varsize[1]);
    geometry->set_nz(_varsize[0]);

    _initialized = true;

    cout << "file: " << __FILE__ << ", line: " << __LINE__ << endl;
    cout << "select var <" << vn << ">" << endl;
    cout << "select geometry->get_nx() = " << geometry->get_nx() << endl;
    cout << "select geometry->get_ny() = " << geometry->get_ny() << endl;
    cout << "select geometry->get_nz() = " << geometry->get_nz() << endl;

    if(nvfile->get_hasFillValue())
    {
        glviewer->set_hasFillValue(true);
        glviewer->set_fillValue(nvfile->get_fillValue());

        geometry->set_hasFillValue(nvfile->get_hasFillValue());
        geometry->set_fillValue(nvfile->get_fillValue());
    }

    pixelviewer->setup(vn, _value);
    nclviewer->setup(vn, _value);
    glviewer->setup(vn, _value);

    _minval = glviewer->get_minval();
    _maxval = glviewer->get_maxval();

    if(nvoptions->get_cb(NV_DATAVIEWON))
    {
        spreadsheet->clear();
        update_spreadsheet(0);
    }
}

void Controller::set3dvarname(string vn)
{
    _varname = vn;

    _curFile = 0;
    _curTime = 0;

    if(_initialized)
        free(_value);

    _value = nvfile->get_dv(vn);
    _title = nvfile->get_title();

    _initialized = true;

    _varsize = nvfile->get_varsize();
    geometry->set_nx(_varsize[2]);
    geometry->set_ny(_varsize[1]);
    geometry->set_nz(_varsize[0]);

    cout << "file: " << __FILE__ << ", line: " << __LINE__ << endl;
    cout << "select var <" << vn << ">" << endl;
    cout << "select geometry->get_nx() = " << geometry->get_nx() << endl;
    cout << "select geometry->get_ny() = " << geometry->get_ny() << endl;
    cout << "select geometry->get_nz() = " << geometry->get_nz() << endl;

    if(nvfile->get_hasFillValue())
    {
        glviewer->set_hasFillValue(true);
        glviewer->set_fillValue(nvfile->get_fillValue());

        geometry->set_hasFillValue(nvfile->get_hasFillValue());
        geometry->set_fillValue(nvfile->get_fillValue());
    }

    pixelviewer->setup(vn, _value);
    nclviewer->setup(vn, _value);
    glviewer->setup(vn, _value);

    _minval = glviewer->get_minval();
    _maxval = glviewer->get_maxval();

    if(nvoptions->get_cb(NV_DATAVIEWON))
    {
        spreadsheet->clear();
        update_spreadsheet(0);
    }
}

void Controller::update_colormap()
{
    nclviewer->update_colormap();
    glviewer->update_colormap();
}

int Controller::get_ndv(int n)
{
    return nvfile->get_ndv(n);
}

string* Controller::get_ndvNames(int n)
{
    return nvfile->get_ndvNames(n);
}

string* Controller::get_timestring()
{
    return nvfile->get_timestr();
} 

void Controller::setup_vector()
{
    cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__
         << ", file: <" << __FILE__ << ">" << endl;
}

void Controller::unset_vector()
{
    cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__
         << ", file: <" << __FILE__ << ">" << endl;
}

double Controller::get_valueAt(int i, int j)
{
  //size_t n = i + (j + nvoptions->get_zsec() * _ny) * _nx;
    size_t n = i + j * _nx;
    return _value[n];
}

void Controller::set_colorTable(ColorTable* ct)
{
    colorTable = ct;
    update_colormap();
}

