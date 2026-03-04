//$Id: popcontroller.cpp 4927 2014-01-23 21:49:37Z starviewer $

#include "popcontroller.h"

POPController::POPController(ColorTable *ct, NVOptions* opt,
                             const char *fn, bool isList)
{
    string sfn = string(fn);

    colorTable = ct;
    nvoptions = opt;

    strcpy(_flnm, fn);

    cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__
         << ", file: <" << __FILE__ << ">" << endl;
    cout << "\tOpen file: <" << fn << ">" << endl;

    geometry = new POPGeometry();

    nvfile = new NVFile(fn, isList);
    _grdsize = nvfile->get_grdsize();
    _ntimes = nvfile->get_ntimes();

    geometry->set_name(sfn);
    geometry->set_pop_nlon(nvfile->get_dim_size("nlon"));
    geometry->set_pop_nlat(nvfile->get_dim_size("nlat"));
    geometry->set_pop_nlev(nvfile->get_dim_size("z_w"));
  //geometry->set_nt(_ntimes[0]);

    geometry->set_pop_ulon(nvfile->get_dv("ULONG"));
    geometry->set_pop_ulat(nvfile->get_dv("ULAT"));
    geometry->set_pop_tlon(nvfile->get_dv("TLONG"));
    geometry->set_pop_tlat(nvfile->get_dv("TLAT"));
    geometry->set_pop_z_w(nvfile->get_dv("z_w"));

    _varname = string("TEMP");

    _setup();
}

POPController::~POPController()
{
    delete geometry;
    delete nvfile;
    delete pop2dviewer;
} 

void POPController::_setup()
{
    _initialized = false;

    _tvalue = 0;
    _time_interval = 128;

    pop2dviewer = new POP2dViewer(colorTable, nvoptions);
    pop2dviewer->set_geometry(geometry);

    _value = nvfile->get_dv(_varname);

  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\tPOPController nlon : " << geometry->get_pop_nlon() << endl;
  //cout << "\tPOPController nlat : " << geometry->get_pop_nlat() << endl;
  //cout << "\tPOPController nlev : " << geometry->get_pop_nlev() << endl;

  //cout << "\n_setup, file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //geometry->print();

    pop2dviewer->setup(_varname, _value);

    _valmin = pop2dviewer->get_valmin();
    _valmax = pop2dviewer->get_valmax();
}

void POPController::draw()
{
  //if(_sphere)
  //{
      //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
      //cout << "2d draw" << endl;
        pop2dviewer->draw();
      //geometry->draw_triangle();
  //}
  //else
  //{
  //    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
  //    cout << "need to draw a ball" << endl;
      //pop3dviewer->draw(_tvalue);
  //}
}

void POPController::set2dvarname(string vn)
{
    _varname = vn;

    _tvalue = 0;

    if(_initialized)
        free(_value);

    _value = nvfile->get_dv(vn);

    _initialized = true;

    geometry->set_pop_nlon(nvfile->get_dim_size("nlon"));
    geometry->set_pop_nlat(nvfile->get_dim_size("nlat"));
    geometry->set_pop_nlev(1);

    if(nvfile->get_hasFillValue())
    {
        geometry->set_hasFillValue(true);
        geometry->set_fillValue((double) nvfile->get_fillValue());
    }

  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "setup for <" << vn << ">" << endl;

    pop2dviewer->set_geometry(geometry);
    pop2dviewer->setup(vn, _value);

    _valmin = pop2dviewer->get_valmin();
    _valmax = pop2dviewer->get_valmax();
}

void POPController::set3dvarname(string vn)
{
    _varname = vn;

    _tvalue = 0;

    if(_initialized)
        free(_value);

    _value = nvfile->get_dv(vn);

    _initialized = true;

    geometry->set_pop_nlon(nvfile->get_dim_size("nlon"));
    geometry->set_pop_nlat(nvfile->get_dim_size("nlat"));
    geometry->set_pop_nlev(nvfile->get_dim_size("z_w"));

    if(nvfile->get_hasFillValue())
    {
        geometry->set_hasFillValue(true);
        geometry->set_fillValue((double) nvfile->get_fillValue());
    }

  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "setup for <" << vn << ">" << endl;

    pop2dviewer->set_geometry(geometry);
    pop2dviewer->setup(vn, _value);

    _valmin = pop2dviewer->get_valmin();
    _valmax = pop2dviewer->get_valmax();
}

void POPController::set_xsec(int n)
{
  //cout << "\nset_zlevel, file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\tn = " << n << endl;

    pop2dviewer->set_xsec(n);
}

void POPController::set_ysec(int n)
{
  //cout << "\nset_zlevel, file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\tn = " << n << endl;

    pop2dviewer->set_ysec(n);
}

void POPController::set_zlevel(int n)
{
  //cout << "\nset_zlevel, file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\tn = " << n << endl;

    pop2dviewer->set_zlevel(n);
}

void POPController::set_colorTable(ColorTable *ct)
{
    pop2dviewer->reset_texture1d(ct);
}

int POPController::get_ndv(int n)
{
    return nvfile->get_ndv(n);
}

string* POPController::get_ndvNames(int n)
{
    string* varnames = nvfile->get_ndvNames(n);
    return varnames;
}

string* POPController::get_timestring()
{
    return nvfile->get_timestr();
}

void POPController::set_fileNtime(int nf, int nt)
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

      //_minval = pop2dviewer->get_minval();
      //_maxval = pop2dviewer->get_maxval();

        _initialized = true;
    }

    _preFile = _curFile;

    gridsize = _curTime * geometry->get_pop_nlon()
                        * geometry->get_pop_nlat()
                        * geometry->get_pop_nlev();

    pop2dviewer->setup(_varname, &_value[gridsize]);
}

void POPController::set_locator(Locator* l)
{
     locator = l;

     pop2dviewer->set_locator(l);
}

