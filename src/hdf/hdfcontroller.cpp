//$Id: hdfcontroller.cpp 5248 2014-12-08 15:46:55Z starviewer $

#include "hdfcontroller.h"

HDFController::HDFController(ColorTable *ct, NVOptions* opt,
                             const char *fn, bool isList)
{
    string sfn = string(fn);

    colorTable = ct;
    nvoptions = opt;

    strcpy(_flnm, fn);

    cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__
         << ", file: <" << __FILE__ << ">" << endl;
    cout << "\tOpen file: <" << fn << ">" << endl;

  //_fillvalue = -9999.0;
    _fillvalue = -5555.0;
    geometry = new HDFGeometry();

    nvfile = new NVFile(fn, isList);
    _grdsize = nvfile->get_grdsize();
    _ntimes = nvfile->get_ntimes();

    geometry->set_name(sfn);
    geometry->set_nlat(nvfile->get_dim_size("DIM_000"));
    geometry->set_nlon(nvfile->get_dim_size("DIM_001"));
    geometry->set_nlev(1);
  //geometry->set_nlev(nvfile->get_dim_size("z_w"));
  //geometry->set_nt(_ntimes[0]);

    cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__
         << ", file: <" << __FILE__ << ">" << endl;
    cout << "nlev = " << geometry->get_nlev() << endl;
    cout << "nlon = " << geometry->get_nlat() << endl;
    cout << "nlat = " << geometry->get_nlon() << endl;

    geometry->set_lon(nvfile->get_dv("Longitude"));
    geometry->set_lat(nvfile->get_dv("Latitude"));
  //geometry->set_z(nvfile->get_dv("z_w"));
    geometry->set_fillValue(_fillvalue);

    _varname = string("liquidPrecipFraction");

    _setup();
}

HDFController::~HDFController()
{
    delete geometry;
    delete nvfile;
    delete hdf2dviewer;
} 

void HDFController::_setup()
{
    _initialized = false;

    _tvalue = 0;
    _time_interval = 128;

    hdf2dviewer = new HDF2dViewer(colorTable, nvoptions);
    hdf2dviewer->set_geometry(geometry);

    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
    cout << "\t_varname: " << _varname << endl;
    cout << "\tHDFController nlon : " << geometry->get_nlon() << endl;
    cout << "\tHDFController nlat : " << geometry->get_nlat() << endl;
    cout << "\tHDFController nlev : " << geometry->get_nlev() << endl;

    _value = nvfile->get_dv(_varname);

    cout << "\n_setup, file: " << __FILE__ << ", line: " << __LINE__ << endl;
    geometry->print();

    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "hasFillValue: " << nvfile->get_hasFillValue() << endl;
  //cout << "FillValue: " << nvfile->get_fillValue() << endl;
    geometry->set_fillValue(_fillvalue);
    cout << "FillValue: " << geometry->get_fillValue() << endl;

    hdf2dviewer->setup(_varname, _value);

    _valmin = hdf2dviewer->get_valmin();
    _valmax = hdf2dviewer->get_valmax();

    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
    cout << "_valmin = " << _valmin << ", _valmax = " << _valmax << endl;
}

void HDFController::draw()
{
  //if(_sphere)
  //{
      //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
      //cout << "2d draw" << endl;
        hdf2dviewer->draw();
      //geometry->draw_triangle();
  //}
  //else
  //{
  //    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
  //    cout << "need to draw a ball" << endl;
      //hdf3dviewer->draw(_tvalue);
  //}
}

void HDFController::set2dvarname(string vn)
{
    _varname = vn;

    _tvalue = 0;

    if(_initialized)
        free(_value);

    _value = nvfile->get_dv(vn);

    _initialized = true;

    geometry->set_nlat(nvfile->get_dim_size("DIM_000"));
    geometry->set_nlon(nvfile->get_dim_size("DIM_001"));
    geometry->set_nlev(1);

    if(nvfile->get_hasFillValue())
    {
        geometry->set_hasFillValue(true);
        geometry->set_fillValue((double) nvfile->get_fillValue());
    }
    geometry->set_fillValue(_fillvalue);

    hdf2dviewer->set_geometry(geometry);
    hdf2dviewer->setup(vn, _value);

    _valmin = hdf2dviewer->get_valmin();
    _valmax = hdf2dviewer->get_valmax();
}

void HDFController::set3dvarname(string vn)
{
    _varname = vn;

    _tvalue = 0;

    if(_initialized)
        free(_value);

    _value = nvfile->get_dv(vn);

    _initialized = true;

    geometry->set_nlat(nvfile->get_dim_size("DIM_000"));
    geometry->set_nlon(nvfile->get_dim_size("DIM_001"));
    geometry->set_nlev(nvfile->get_dim_size("z_w"));

    if(nvfile->get_hasFillValue())
    {
        geometry->set_hasFillValue(true);
        geometry->set_fillValue((double) nvfile->get_fillValue());
    }

  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "setup for <" << vn << ">" << endl;

    hdf2dviewer->set_geometry(geometry);
    hdf2dviewer->setup(vn, _value);

    _valmin = hdf2dviewer->get_valmin();
    _valmax = hdf2dviewer->get_valmax();
}

void HDFController::set_xsec(int n)
{
  //cout << "\nset_zlevel, file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\tn = " << n << endl;

    hdf2dviewer->set_xsec(n);
}

void HDFController::set_ysec(int n)
{
  //cout << "\nset_zlevel, file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\tn = " << n << endl;

    hdf2dviewer->set_ysec(n);
}

void HDFController::set_zlevel(int n)
{
  //cout << "\nset_zlevel, file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\tn = " << n << endl;

    hdf2dviewer->set_zlevel(n);
}

void HDFController::set_colorTable(ColorTable *ct)
{
    hdf2dviewer->reset_texture1d(ct);
}

int HDFController::get_ndv(int n)
{
    return nvfile->get_ndv(n);
}

string* HDFController::get_ndvNames(int n)
{
    string* varnames = nvfile->get_ndvNames(n);
    return varnames;
}

string* HDFController::get_timestring()
{
    return nvfile->get_timestr();
}

void HDFController::set_fileNtime(int nf, int nt)
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

      //_varsize = nvfile->get_varsize();
      //geometry->set_nx(_varsize[2]);
      //geometry->set_ny(_varsize[1]);
      //geometry->set_nz(_varsize[0]);
      //geometry->set_nt(_ntimes[_curFile]);

      //_minval = hdf2dviewer->get_minval();
      //_maxval = hdf2dviewer->get_maxval();

        _initialized = true;
    }

    _preFile = _curFile;

    gridsize = _curTime * geometry->get_nlon()
                        * geometry->get_nlat()
                        * geometry->get_nlev();

    hdf2dviewer->setup(_varname, &_value[gridsize]);
}

void HDFController::set_locator(Locator* l)
{
     locator = l;

     hdf2dviewer->set_locator(l);
}

