//$Id: mpascontroller.cpp 4927 2014-01-23 21:49:37Z starviewer $

#include "mpascontroller.h"

MPASController::MPASController(ColorTable *ct, NVOptions* opt,
                               const char *fn, bool isList)
{
    string sfn = string(fn);

    colorTable = ct;
    nvoptions = opt;

    strcpy(_flnm, fn);

  //cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__
  //     << ", file: <" << __FILE__ << ">" << endl;
  //cout << "\tOpen file: <" << fn << ">" << endl;

    _preFile = -1;
    _curFile = 0;
    _glbTime = 0;
    _curTime = 0;

    _value = NULL;
    mpas2dviewer = NULL;

  //coastline = new CoastLine();

    geometry = new MPASGeometry();
    geometry->set_name(sfn);

    nvfile = new NVFile(fn, isList, true);
    _ntimes = nvfile->get_ntimes();

    geometry->set_nCells(nvfile->get_dim_size("nCells"));
    geometry->set_nVertices(nvfile->get_dim_size("nVertices"));
    geometry->set_vertexDegree(nvfile->get_dim_size("vertexDegree"));
    geometry->set_nVertLevels(nvfile->get_dim_size("nVertLevels"));
    geometry->set_nTime(nvfile->get_dim_size("Time"));

    geometry->set_mx(360);
    geometry->set_my(180);
    geometry->set_mz(nvfile->get_dim_size("nVertLevels"));

    geometry->set_nx(360);
    geometry->set_ny(180);
    geometry->set_nz(nvfile->get_dim_size("nVertLevels"));
    geometry->set_nt(nvfile->get_dim_size("Time"));

    geometry->set_lonCell(nvfile->get_dv("lonCell"));
    geometry->set_latCell(nvfile->get_dv("latCell"));
#ifdef HASZGRID
    geometry->set_height(nvfile->get_dv("zgrid"));
#endif
    geometry->set_cellsOnVertex(nvfile->get_iv("cellsOnVertex"));

    geometry->setup();

  //_varname = string("sst");
    if(nvoptions->get_cb(NV_MPAS_SUBSET))
        _varname = string("t2m");
    else
        _varname = string("surface_pressure");

    _setup();
}

MPASController::~MPASController()
{
    delete geometry;
    delete nvfile;

    if(NULL != _value)
        free(_value);

    if(NULL != mpas2dviewer)
        delete mpas2dviewer;
} 

void MPASController::_setup()
{
    _initialized = false;

    _tvalue = 0;

  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\tMPASController nCells      : " << geometry->get_nCells() << endl;
  //cout << "\tMPASController nVertices   : " << geometry->get_nVertices() << endl;
  //cout << "\tMPASController nVertLevels : " << geometry->get_nVertLevels() << endl;
  //cout << "\tMPASController Time        : " << geometry->get_nTime() << endl;
  //cout << "\tMPASController vertexDegree: " << geometry->get_vertexDegree() << endl;

    mpas2dviewer = new MPAS2dViewer(colorTable, nvoptions);
  //mpas3dviewer = new MPAS3dViewer();

    _value = nvfile->get_dv(_varname);
    geometry->set_nVertLevels(1);

    mpas2dviewer->set_geometry(geometry);
    mpas2dviewer->setup(_varname, _value);

    _minval = mpas2dviewer->get_minval();
    _maxval = mpas2dviewer->get_maxval();
}

void MPASController::draw()
{
    mpas2dviewer->draw();
}

void MPASController::set1dvarname(string vn)
{
    _varname = vn;

    _tvalue = 0;

    if(_initialized && (NULL != _value))
        free(_value);

    _value = nvfile->get_dv(vn);

    _initialized = true;

  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "setup for <" << vn << ">" << endl;

    geometry->set_nVertLevels(1);

    mpas2dviewer->set_geometry(geometry);
    mpas2dviewer->setup(vn, _value);

    _curFile = 0;
    _curTime = 0;
    _glbTime = 0;
    geometry->set_nt(_ntimes[_curFile]);

    _minval = mpas2dviewer->get_minval();
    _maxval = mpas2dviewer->get_maxval();
}

void MPASController::set2dvarname(string vn)
{
    _varname = vn;

    _tvalue = 0;

    if(_initialized && (NULL != _value))
        free(_value);

    _value = nvfile->get_dv(vn);

    _initialized = true;

    geometry->set_nVertLevels(nvfile->get_dim_size("nVertLevels"));

  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "setup for <" << vn << ">" << endl;

    mpas2dviewer->set_geometry(geometry);
    mpas2dviewer->setup(vn, _value);
  //mpas3dviewer->setup(vn, _value);

    _curFile = 0;
    _curTime = 0;
    _glbTime = 0;
    geometry->set_nt(_ntimes[_curFile]);

    _minval = mpas2dviewer->get_minval();
    _maxval = mpas2dviewer->get_maxval();
}

void MPASController::set_colorTable(ColorTable *ct)
{
    mpas2dviewer->reset_texture1d(ct);
}

int MPASController::get_ndv(int n)
{
    return nvfile->get_ndv(n);
}

string* MPASController::get_ndvNames(int n)
{
    string* varnames = nvfile->get_ndvNames(n);
    return varnames;
}

string* MPASController::get_timestring()
{
    return nvfile->get_timestr();
}

void MPASController::set_fileNtime(int nf, int nt)
{
    size_t gridsize = 1;

  //cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
  //cout << "\tcurFile: " << nf << ", curTime: " << nt << ", varname: <" << _varname << ">" << endl;

    _curFile = nf;
    _curTime = nt;

    if(_preFile != _curFile)
    {
        if(_initialized && (NULL != _value))
            free(_value);

        nvfile->select_file(nf);

        _value = nvfile->get_dv(_varname);
        _title = nvfile->get_title();

        geometry->set_nt(_ntimes[_curFile]);

        _set_glbTime();
        _minval = mpas2dviewer->get_minval();
        _maxval = mpas2dviewer->get_maxval();

        _initialized = true;
    }

    _preFile = _curFile;

    gridsize = _curTime * geometry->get_nVertices() *  geometry->get_nVertLevels();

    mpas2dviewer->setup(_varname, &_value[gridsize]);
}

void MPASController::_set_glbTime()
{
    int n;

    _glbTime = _curTime;
    for(n = 0; n < _curFile; ++n)
        _glbTime += _ntimes[n];
}

void MPASController::set_locator(Locator* l)
{
     locator = l;

     mpas2dviewer->set_locator(l);
}

void MPASController::set_tl(int n)
{
      _tvalue = n;
     mpas2dviewer->set_tvalue(n);
}

