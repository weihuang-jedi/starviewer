//$Id: mpascontroller.cpp 4927 2014-01-23 21:49:37Z starviewer $

#include "mpascontroller.h"

MPASController::MPASController(ColorTable *ct, NVOptions* opt,
                               const char *fn)
{
    string sfn = string(fn);

    colorTable = ct;
    nvoptions = opt;

    strcpy(_flnm, fn);

    cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__
         << ", file: <" << __FILE__ << ">" << endl;
    cout << "\tOpen file: <" << fn << ">" << endl;

    _preFile = -1;
    _curFile = 0;
    _glbTime = 0;
    _curTime = 0;

    _value = NULL;
    mpas2dviewer = NULL;

  //coastline = new CoastLine();

    geometry = new MPASGeometry();
    geometry->set_name(sfn);

    ncfile = new MPASStaticReader(fn);
    _ntimes = new int(1);
    _ntimes[0] = 1;

    geometry->set_nCells(ncfile->get_nCells());
    geometry->set_nVertices(ncfile->get_nVertices());
    geometry->set_vertexDegree(ncfile->get_vertexDegree());
    // geometry->set_nVertLevels(ncfile->get_nVertLevels());
    geometry->set_nVertLevels(1);
    geometry->set_nTime(ncfile->get_nTime());

    geometry->set_mx(360);
    geometry->set_my(180);
    // geometry->set_mz(ncfile->get_dim_size("nVertLevels"));
    geometry->set_mz(1);

    geometry->set_nx(360);
    geometry->set_ny(180);
    // geometry->set_nz(ncfile->get_dim_size("nVertLevels"));
    geometry->set_nz(1);
    geometry->set_nt(ncfile->get_nTime());

    geometry->set_lonCell(ncfile->get_lonCell());
    geometry->set_latCell(ncfile->get_latCell());
#ifdef HASZGRID
    geometry->set_height(ncfile->get_fv("zgrid"));
#endif
    geometry->set_cellsOnVertex(ncfile->get_cellsOnVertex());

    geometry->setup();

    _varname = string("ter");
    // if(nvoptions->get_cb(NV_MPAS_SUBSET))
    //     _varname = string("ter");
    // else
    //     _varname = string("surface_pressure");
}

MPASController::~MPASController()
{
    delete geometry;
    delete ncfile;

    if(NULL != _value)
        free(_value);

    if(NULL != _ntimes)
        free(_ntimes);

    if(NULL != mpas2dviewer)
        delete mpas2dviewer;
} 

void MPASController::setup()
{
    _initialized = false;

    _tvalue = 0;

    cout << "\nEnter <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    cout << "\tMPASController nCells      : " << geometry->get_nCells() << endl;
    cout << "\tMPASController nVertices   : " << geometry->get_nVertices() << endl;
    cout << "\tMPASController nVertLevels : " << geometry->get_nVertLevels() << endl;
    cout << "\tMPASController vertexDegree: " << geometry->get_vertexDegree() << endl;

    mpas2dviewer = new MPAS2dViewer(colorTable, nvoptions);
  //mpas3dviewer = new MPAS3dViewer();

    _value = ncfile->get_fv(_varname.c_str());
    geometry->set_nVertLevels(1);

    mpas2dviewer->set_geometry(geometry);
    mpas2dviewer->setup(_varname, _value);

    _minval = mpas2dviewer->get_minval();
    _maxval = mpas2dviewer->get_maxval();
    cout << "\t_minval = " << _minval << ", _maxval = " << _maxval << endl;
    cout << "Leave <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
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

    _value = ncfile->get_fv(vn.c_str());

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

    _value = ncfile->get_fv(vn.c_str());

    _initialized = true;

    geometry->set_nVertLevels(ncfile->get_nVertLevels());

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
    int ndv = 0;
    if(n == 2)
        ndv = ncfile->getNumV2ds();
    else if(n == 3)
        ndv = ncfile->getNumV3ds();
    return ndv;
}

vector<string> MPASController::get_ndvNames(int n)
{
    vector<string> varnames;
    if(n == 2)
        varnames = ncfile->getV2dNames();
    else if(n == 3)
        varnames = ncfile->getV3dNames();
    return varnames;
}

string* MPASController::get_timestring()
{
    string tsv = string("Unknown");
    string* ts = new string[1];
    ts[0] = tsv;
    // return string(ncfile->get_timestr());
    return ts;
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

        ncfile->select_file(nf);

        _value = ncfile->get_fv(_varname.c_str());
        _title = string("MPAS");
        // _title = ncfile->get_title();

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

