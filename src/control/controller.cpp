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
    ncfile = NULL;
    glviewer = NULL;
    nclviewer = NULL;
    pixelviewer = NULL;
    spreadsheet = NULL;
}

Controller::~Controller()
{
    if(NULL != geometry)
        delete geometry;
    if(NULL != ncfile)
        delete ncfile;
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

    ncfile = new ncReader(_flnm.c_str());
    ncfile->exploreFile();
    _grdsize = ncfile->get_grdsize();
    _ntimes = ncfile->get_ntimes();
    _ntiles = ncfile->get_ntiles();

    cout << "\t" << __PRETTY_FUNCTION__ << ">, line: "
         << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    cout << "\t_grdsize[0]: " << _grdsize[0] << endl;
    cout << "\t_ntimes[0]: " << _ntimes[0] << endl;
    cout << "\t_ntiles: " << _ntiles << endl;

    geometry = new Geometry();
    geometry->set_name(_flnm);
    geometry->set_mx(_grdsize[3]);
    geometry->set_my(_grdsize[2]);
    geometry->set_mz(_grdsize[1]);
    geometry->set_nt(_ntimes[0]);

    cout << "\t" << __PRETTY_FUNCTION__ << ">, line: "
         << __LINE__ << ", file: <" << __FILE__ << ">" << endl;

    if (_ntiles == 0)
    {
        geometry->set_has1dLon(true);
        geometry->set_has1dLat(true);
        geometry->set_has1dLev(true);
        geometry->set_lon(ncfile->getLon());
        geometry->set_lat(ncfile->getLat());
        geometry->set_lev(ncfile->getLev());

        geometry->set_nx(ncfile->getNlon());
        geometry->set_ny(ncfile->getNlat());
        geometry->set_nz(ncfile->getNlev());
    }
    else
    {
        geometry->set_has1dLon(false);
        geometry->set_has1dLat(false);
        geometry->set_has1dLev(true);
        geometry->set_lev(ncfile->getLev());
    }

    geometry->set_has2dLon(true);
    geometry->set_lon2d(ncfile->getLon2d());
    geometry->set_has2dLat(true);
    geometry->set_lat2d(ncfile->getLat2d());

    cout << "\t<" << __PRETTY_FUNCTION__ << ">, line: "
         << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    cout << "\t_varname" << _varname << endl;

    _varname = "pressfc";
    _value = ncfile->getFloat(_varname.c_str());
  //_title = ncfile->get_title();

    geometry->set_hasFillValue(false);
  //if(geometry->get_hasFillValue())
  //   geometry->set_fillValue(ncfile->get_fillValue());

  //geometry->print();

    _mx = geometry->get_mx();
    _my = geometry->get_my();
    _mz = geometry->get_mz();

    _nx = geometry->get_nx();
    _ny = geometry->get_ny();
    _nz = geometry->get_nz();
    _nt = geometry->get_nt();
    _nm = 1;

    cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">, line: "
         << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    cout << "\tvariable name: <" << _varname << ">" << endl;
    cout << "\ttitle: <" << _title << ">" << endl;

  //We need to setup 2 viewers, no matter which one is currently used.
  //nclviewer = new NCL_Viewer(colorTable, nvoptions);
  //nclviewer->set_geometry(geometry);
  //nclviewer->setup(_varname, _value);

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

        ncfile->select_file(nf);

        _value = ncfile->getFloat(_varname.c_str());
      //_title = ncfile->get_title();

        geometry->set_nx(ncfile->getNlon());
        geometry->set_ny(ncfile->getNlat());
        geometry->set_nz(ncfile->getNlev());

        geometry->set_nt(_ntimes[_curFile]);

        _minval = glviewer->get_minval();
        _maxval = glviewer->get_maxval();

        _initialized = true;
    }
 
    _preFile = _curFile;

    gridsize = _curTime * geometry->get_nx() *  geometry->get_ny() *  geometry->get_nz();

    pixelviewer->setup(_varname, &_value[gridsize]);
  //nclviewer->setup(_varname, &_value[gridsize]);
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

    _value = ncfile->getFloat(vn.c_str());
    _title = vn;

    geometry->set_nx(ncfile->getNlon());
    geometry->set_ny(ncfile->getNlat());
    geometry->set_nz(1);

    _initialized = true;

    cout << "file: " << __FILE__ << ", line: " << __LINE__ << endl;
    cout << "select var <" << vn << ">" << endl;
    cout << "select geometry->get_nx() = " << geometry->get_nx() << endl;
    cout << "select geometry->get_ny() = " << geometry->get_ny() << endl;
    cout << "select geometry->get_nz() = " << geometry->get_nz() << endl;

   /*
    if(ncfile->get_hasFillValue())
    {
        glviewer->set_hasFillValue(true);
        glviewer->set_fillValue(nvfile->get_fillValue());

        geometry->set_hasFillValue(nvfile->get_hasFillValue());
        geometry->set_fillValue(nvfile->get_fillValue());
    }
   */

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

    _value = ncfile->getFloat(vn.c_str());
    _title = vn;

    _initialized = true;

    geometry->set_nx(ncfile->getNlon());
    geometry->set_ny(ncfile->getNlat());
    geometry->set_nz(ncfile->getNlev());

    cout << "file: " << __FILE__ << ", line: " << __LINE__ << endl;
    cout << "select var <" << vn << ">" << endl;
    cout << "select geometry->get_nx() = " << geometry->get_nx() << endl;
    cout << "select geometry->get_ny() = " << geometry->get_ny() << endl;
    cout << "select geometry->get_nz() = " << geometry->get_nz() << endl;

   /*
    if(nvfile->get_hasFillValue())
    {
        glviewer->set_hasFillValue(true);
        glviewer->set_fillValue(nvfile->get_fillValue());

        geometry->set_hasFillValue(nvfile->get_hasFillValue());
        geometry->set_fillValue(nvfile->get_fillValue());
    }
    */

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
    cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__
         << ", file: <" << __FILE__ << ">" << endl;
  //return nvfile->get_ndv(n);
    return 1;
}

string* Controller::get_ndvNames(int n)
{
    cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__
         << ", file: <" << __FILE__ << ">" << endl;
    string* vn = new string[2];
    return vn;
  //return nvfile->get_ndvNames(n);
}

string* Controller::get_timestring()
{
    cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__
         << ", file: <" << __FILE__ << ">" << endl;
    string* vn = new string[2];
    return vn;
  //return nvfile->get_timestr();
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

float Controller::get_valueAt(int i, int j)
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

