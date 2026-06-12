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
    cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    cout << "\t_nx: " << _nx << endl;
    cout << "\t_ny: " << _ny << endl;
    cout << "\t_ntime: " << _ntime << endl;
    geometry->set_nx(_nx);
    geometry->set_ny(_ny);
    geometry->set_nlev(1);
    geometry->set_ntime(_ntime);

    geometry->set_longitude(ncfile->getLongitude());
    geometry->set_latitude(ncfile->getLatitude());
    geometry->setup();

    cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    cout << "\t_nx: " << geometry->get_nx() << endl;
    cout << "\t_ny: " << geometry->get_ny() << endl;
    cout << "\t_ntime: " << geometry->get_ntime() << endl;

    _varname = string("longitude");

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
    cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    cout << "\tvarname: " << _varname << endl;

    _value = ncfile->get_fv(_varname.c_str());
    _title = _varname;

    cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    cout << "\t_value[0] = " << _value[0] << endl;
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

void EAGLEController::setvarname(string vn)
{
    _varname = vn;
    _sphere = false;
    _ball = true;

    _tvalue = 0;

    if(_initialized)
        free(_value);

    cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    cout << "\tvarname: " << vn << endl;

    _value = ncfile->get_fv(vn.c_str());
    _title = vn;

    cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    cout << "\t_value[0] = " << _value[0] << endl;

    _initialized = true;

    geometry->set_nlev(1);

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
    ndv = ncfile->getNumVars();
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

        cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
        cout << "\tvarname: " << _varname << endl;

        _value = ncfile->get_fv(_varname.c_str());
        _title = _varname;

        cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
        cout << "\t_value[0] = " << _value[0] << endl;

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

