#include "eagle_controller.h"

EAGLEController::EAGLEController(ColorTable *ct, NVOptions* opt,
                                 const char *fn)
{
    string sfn = string(fn);

    colorTable = ct;
    nvoptions = opt;
    strcpy(_flnm, fn);

    geometry = new EAGLEGeometry();
    geometry->set_name(sfn);

    coastline = new CoastLine();
  
    _maxFile = 1;
    _ntime = 1;

    ncfile = new EagleReader(fn);

    eagle_viewer = NULL;
}

EAGLEController::~EAGLEController()
{
    delete coastline;
    delete ncfile;
    
    if(NULL != eagle_viewer)
        delete eagle_viewer;
    eagle_viewer = NULL;
    
    delete geometry;
} 

template<typename T>
void EAGLEController::_print1d(T* var, int nl)
{
    int i;

    cout << "Enter functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    cout << "nl=" << nl << endl;
    for(i = 0; i < nl; ++i)
    {
        cout << "var[" << i << "] = " << var[i] << endl;
    }
    cout << "Leave functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
} 

void EAGLEController::setup()
{
    int n;
    char bmpflnm[1024];
    const char* path = getenv("STARVIEWERHOME");
    if (path == nullptr) {
        cout << "ERROR: STARVIEWERHOME not set!" << endl;
        throw(errno);
    }
    strcpy(bmpflnm, path);
    strcat(bmpflnm, "/data/earth.bmp");

  //_ntimes = ncfile->get_ntimes();

  //cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
  //cout << "\t_ntimes = " << _ntimes << endl;

  //_maxFile = get_nfiles();
    _maxFile = 1;

  //for(n = 0; n < _maxFile; ++n)
  //    _maxTime += _ntimes[n];
    _maxTime = 1;

  //lister = new Lister[_maxTime];

    _nx = ncfile->getNx();
    _ny = ncfile->getNy();
    _ntime = ncfile->getNtime();
    geometry->set_nx(_nx);
    geometry->set_ny(_ny);
    geometry->set_nlev(1);
    geometry->set_ntime(_ntime);

    geometry->set_lon2d(ncfile->getLongitude());
    geometry->set_lat2d(ncfile->getLatitude());
    geometry->setup();

    geometry->set_has1dLon(false);
    geometry->set_has1dLat(false);

    geometry->set_has2dLon(true);
    geometry->set_has2dLat(true);

    _varname = string("Longitude");

    _sphere = false;
    _ball = false;
    _initialized = false;
    _tvalue = 0;
    _time_interval = 128;
    _curFile = 0;
    _curTime = 0;
    _preFile = _curFile;

    eagle_viewer = new EAGLE2dViewer(colorTable, nvoptions, bmpflnm, ncfile);

  //eagle_viewer->set_lister(&lister[0]);
  //cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
  //cout << "\tvarname:" << _varname << endl;

    _value = ncfile->get_fv(_varname.c_str());
    _title = _varname;

  //cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
  //geometry->print();

    eagle_viewer->set_coastline(coastline);
    eagle_viewer->set_geometry(geometry);
  //eagle_3dviewer->set_geoeagle_(geoeagle_);

    eagle_viewer->setup(_varname, _value);
    _minval = eagle_viewer->get_minval();
    _maxval = eagle_viewer->get_maxval();
}

void EAGLEController::draw()
{
    eagle_viewer->draw();
}

void EAGLEController::set1dvarname(string vn)
{
    _varname = vn;
    _sphere = true;
    _ball = false;

    _tvalue = 0;

    if(_initialized)
        free(_value);

    _value = ncfile->get_fv(vn.c_str());
    _title = vn;

    _initialized = true;

    geometry->set_nlev(1);

  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "setup for <" << vn << ">" << endl;

  //eagle_viewer->set_lister(&lister[0]);
    eagle_viewer->set_geometry(geometry);
    eagle_viewer->setup(vn, _value);

    _minval = eagle_viewer->get_minval();
    _maxval = eagle_viewer->get_maxval();
}

void EAGLEController::set2dvarname(string vn)
{
    _varname = vn;
    _sphere = false;
    _ball = true;

    _tvalue = 0;

    if(_initialized)
        free(_value);

    _value = ncfile->get_fv(vn.c_str());
    _title = vn;

    _initialized = true;

    geometry->set_nlev(1);

  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "setup for <" << vn << ">" << endl;

  //eagle_viewer->set_lister(&lister[0]);
    eagle_viewer->set_geometry(geometry);
    eagle_viewer->setup(vn, _value);

    _minval = eagle_viewer->get_minval();
    _maxval = eagle_viewer->get_maxval();
}

void EAGLEController::set3dvarname(string vn)
{
    _varname = vn;
    _sphere = false;
    _ball = true;

    _tvalue = 0;

    if(_initialized)
        free(_value);

    _value = ncfile->get_fv(vn.c_str());
    _title = vn;

    _initialized = true;

    geometry->set_nx(ncfile->getNy());
    geometry->set_ny(ncfile->getNx());
    geometry->set_nlev(1);

  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "setup for <" << vn << ">" << endl;
  //cout << "nvfile->get_dim_size('lev') = " << nvfile->get_dim_size("lev") << endl;

  //eagle_viewer->set_lister(&lister[0]);
    eagle_viewer->set_geometry(geometry);
    eagle_viewer->setup(vn, _value);

    _minval = eagle_viewer->get_minval();
    _maxval = eagle_viewer->get_maxval();
}

void EAGLEController::set_colorTable(ColorTable *ct)
{
    eagle_viewer->reset_texture1d(ct);
}

int EAGLEController::get_callList()
{
  //cout << "\nFunction: " << __PRETTY_FUNCTION__
  //     << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;

  //return eagle_viewer->get_callList();
    return 0;
}

void EAGLEController::update_file(const char* fn)
{
    cout << "\nFunction: " << __PRETTY_FUNCTION__ 
         << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
    cout << "\tfilename: <" << fn << ">" << endl;
}

int EAGLEController::get_ndv(int n)
{
    int ndv = 0;
    if (1 == n)
        ndv = 4;
    else if (2 == n)
        ndv = ncfile->getNumV2ds();
    else if (3 == n)
        ndv = ncfile->getNumV3ds();
    return ndv;
}

vector<string> EAGLEController::get_ndvNames(int n)
{
    vector<string> varnames;
    varnames = ncfile->getVarNames();
    return varnames;
}

string EAGLEController::get_timestring()
{
    return string("");
}

void EAGLEController::set_fileNtime(int nf, int nt)
{
    size_t gridsize = 1;

  //cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
  //cout << "\tcurFile: " << nf << ", curTime: " << nt << ", varname: <" << _varname << ">" << endl;
  //cout << "\t_preFile: " << _preFile << ", _curFile: " << _curFile << endl;

    _curFile = nf;
    _curTime = nt;

    if(_preFile != _curFile)
    {
        if(_initialized)
            free(_value);

        _value = ncfile->get_fv(_varname.c_str());
        _title = _varname;

        geometry->set_nlev(1);
        geometry->set_ntim(_ntimes[_curFile]);

        _minval = eagle_viewer->get_minval();
        _maxval = eagle_viewer->get_maxval();

        _initialized = true;
    }

    _preFile = _curFile;

    gridsize = _curTime * geometry->get_nx() * geometry->get_ny();

    _set_glbTime();
  //eagle_viewer->set_lister(&lister[_glbTime]);
    eagle_viewer->setup(_varname, &_value[gridsize]);
}

void EAGLEController::_set_glbTime()
{
    int n;

    _glbTime = _curTime;
    for(n = 0; n < _curFile; ++n)
        _glbTime += _ntimes[n];
}

void EAGLEController::set_locator(Locator* l)
{
     locator = l;

     eagle_viewer->set_locator(l);
}

