#include "ufsincr_controller.h"

UFSINCRController::UFSINCRController(ColorTable *ct, NVOptions* opt,
                                 const char *fn, bool isList)
{
    string sfn = string(fn);

    colorTable = ct;
    nvoptions = opt;
    strcpy(_flnm, fn);

    geometry = new UFSINCRGeometry();
    geometry->set_name(sfn);

    coastline = new CoastLine();
  
    _maxFile = 1;
    _ntim = 1;

    ncfile = new ncReader(fn);

    ufsincr_viewer = NULL;
}

UFSINCRController::~UFSINCRController()
{
    delete coastline;
    delete ncfile;
    
    if(NULL != ufsincr_viewer)
        delete ufsincr_viewer;
    ufsincr_viewer = NULL;
    
    delete geometry;
} 

template<typename T>
void UFSINCRController::_print1d(T* var, int nl)
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

void UFSINCRController::setup()
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

    geometry->set_nlon(ncfile->getNlon());
    geometry->set_nlat(ncfile->getNlat());
  //geometry->set_nlev(ncfile->getNlev());
    geometry->set_nlev(1);
  //geometry->set_ntim(_ntimes[0]);
    geometry->set_ntim(_maxTime);

    geometry->set_lon(ncfile->getLon());
    geometry->set_lat(ncfile->getLat());
    geometry->set_lev(ncfile->getPfull());
    geometry->setup();

    geometry->set_has1dLon(true);
    geometry->set_has1dLat(true);

    geometry->set_has2dLon(false);
    geometry->set_has2dLat(false);

  //_varname = string("sst");
    _varname = string("hgtsfc");

    _sphere = false;
    _ball = false;
    _initialized = false;
    _tvalue = 0;
    _time_interval = 128;
    _curFile = 0;
    _curTime = 0;
    _preFile = _curFile;

    ufsincr_viewer = new UFSINCR2dViewer(colorTable, nvoptions, bmpflnm, ncfile);

  //ufsincr_viewer->set_lister(&lister[0]);
  //cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
  //cout << "\tvarname:" << _varname << endl;

    _value = ncfile->get_fv(_varname.c_str());
    _title = _varname;

  //cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
  //geometry->print();

    ufsincr_viewer->set_coastline(coastline);
    ufsincr_viewer->set_geometry(geometry);
  //ufsincr_3dviewer->set_geoufsincr_(geoufsincr_);

    ufsincr_viewer->setup(_varname, _value);
    _minval = ufsincr_viewer->get_minval();
    _maxval = ufsincr_viewer->get_maxval();
}

void UFSINCRController::draw()
{
    ufsincr_viewer->draw();
}

void UFSINCRController::set1dvarname(string vn)
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

  //ufsincr_viewer->set_lister(&lister[0]);
    ufsincr_viewer->set_geometry(geometry);
    ufsincr_viewer->setup(vn, _value);

    _minval = ufsincr_viewer->get_minval();
    _maxval = ufsincr_viewer->get_maxval();
}

void UFSINCRController::set2dvarname(string vn)
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

  //ufsincr_viewer->set_lister(&lister[0]);
    ufsincr_viewer->set_geometry(geometry);
    ufsincr_viewer->setup(vn, _value);

    _minval = ufsincr_viewer->get_minval();
    _maxval = ufsincr_viewer->get_maxval();
}

void UFSINCRController::set3dvarname(string vn)
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

    geometry->set_nlon(ncfile->getNlon());
    geometry->set_nlat(ncfile->getNlat());
    geometry->set_nlev(ncfile->getNlev());

  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "setup for <" << vn << ">" << endl;
  //cout << "nvfile->get_dim_size('lev') = " << nvfile->get_dim_size("lev") << endl;

  //ufsincr_viewer->set_lister(&lister[0]);
    ufsincr_viewer->set_geometry(geometry);
    ufsincr_viewer->setup(vn, _value);

    _minval = ufsincr_viewer->get_minval();
    _maxval = ufsincr_viewer->get_maxval();
}

void UFSINCRController::set_colorTable(ColorTable *ct)
{
    ufsincr_viewer->reset_texture1d(ct);
}

int UFSINCRController::get_callList()
{
  //cout << "\nFunction: " << __PRETTY_FUNCTION__
  //     << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;

  //return ufsincr_viewer->get_callList();
    return 0;
}

void UFSINCRController::update_file(const char* fn)
{
    cout << "\nFunction: " << __PRETTY_FUNCTION__ 
         << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
    cout << "\tfilename: <" << fn << ">" << endl;
}

int UFSINCRController::get_ndv(int n)
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

vector<string> UFSINCRController::get_ndvNames(int n)
{
    vector<string> varnames;
    if (2 == n)
        varnames = ncfile->getV2dNames();
    else if (3 == n)
        varnames = ncfile->getV3dNames();
    return varnames;
}

string UFSINCRController::get_timestring()
{
    return ncfile->getTimeString();
}

void UFSINCRController::set_fileNtime(int nf, int nt)
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

        _minval = ufsincr_viewer->get_minval();
        _maxval = ufsincr_viewer->get_maxval();

        _initialized = true;
    }

    _preFile = _curFile;

    gridsize = _curTime * geometry->get_nlon() * geometry->get_nlat() * geometry->get_nlev();

    _set_glbTime();
  //ufsincr_viewer->set_lister(&lister[_glbTime]);
    ufsincr_viewer->setup(_varname, &_value[gridsize]);
}

void UFSINCRController::_set_glbTime()
{
    int n;

    _glbTime = _curTime;
    for(n = 0; n < _curFile; ++n)
        _glbTime += _ntimes[n];
}

void UFSINCRController::set_locator(Locator* l)
{
     locator = l;

     ufsincr_viewer->set_locator(l);
}

