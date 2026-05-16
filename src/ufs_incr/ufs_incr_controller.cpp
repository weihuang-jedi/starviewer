#include "ufs_incr_controller.h"

UFSincrController::UFSincrController(ColorTable *ct, NVOptions* opt,
                                     string atmfile, string _sfcfile,
                                     vector<string> datafiles)
{
    cout << "Enter functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    string sfn = string(fn);

    colorTable = ct;
    nvoptions = opt;

    _atmfile = atmfile;
    _sfcfile = sfcfile;
    _datafiles = datafiles;

    cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    cout << "\t_sfcfile: <" << sfcfile << endl;
    ncfile = new ncReader(sfcfile);
    cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;

    incr_geometry = new UFSincrGeometry();
    cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    incr_geometry->set_name(sfn);
    cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    cout << "\tsfn: <" << sfn << endl;

    coastline = new CoastLine();
    cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    cout << "\tsfn: <" << sfn << endl;
  
    _maxFile = 1;
    _ntim = 1;

    ufs_incr_viewer = NULL;
    cout << "Leave functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
}

UFSincrController::~UFSincrController()
{
    delete coastline;
    delete ncfile;
    
    if(NULL != ufs_incr_viewer)
        delete ufs_incr_viewer;
    ufs_incr_viewer = NULL;
    
    delete incr_geometry;
} 

template<typename T>
void UFSincrController::_print1d(T* var, int nl)
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

void UFSincrController::setup()
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

    cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
  //cout << "\t_ntimes = " << _ntimes << endl;

  //_maxFile = get_nfiles();
    _maxFile = 1;

  //for(n = 0; n < _maxFile; ++n)
  //    _maxTime += _ntimes[n];
    _maxTime = 1;

  //lister = new Lister[_maxTime];

    incr_geometry->set_nlon(ncfile->getNlon());
    incr_geometry->set_nlat(ncfile->getNlat());
  //incr_geometry->set_nlev(ncfile->getNlev());
    incr_geometry->set_nlev(1);
  //incr_geometry->set_ntim(_ntimes[0]);
    incr_geometry->set_ntim(_maxTime);

    incr_geometry->set_lon(ncfile->getLon());
    incr_geometry->set_lat(ncfile->getLat());
    incr_geometry->set_lev(ncfile->getPfull());
    incr_geometry->setup();

    incr_geometry->set_has1dLon(true);
    incr_geometry->set_has1dLat(true);

    incr_geometry->set_has2dLon(false);
    incr_geometry->set_has2dLat(false);

    cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
  //_varname = string("sst");
  //_varname = string("hgtsfc");
    _varname = string("pressfc");

    _sphere = false;
    _ball = false;
    _initialized = false;
    _tvalue = 0;
    _time_interval = 128;
    _curFile = 0;
    _curTime = 0;
    _preFile = _curFile;

    ufs_incr_viewer = new UFSincr2dViewer(colorTable, nvoptions, bmpflnm, ncfile);

  //ufs_incr_viewer->set_lister(&lister[0]);
    cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    cout << "\tvarname:" << _varname << endl;

    _value = ncfile->get_fv(_varname.c_str());
    _title = _varname;

    cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
  //incr_geometry->print();

    ufs_incr_viewer->set_coastline(coastline);
    ufs_incr_viewer->set_incr_geometry(incr_geometry);
  //ufs_3dviewer->set_geoufs_(geoufs_);

    ufs_incr_viewer->setup(_varname, _value);
    _minval = ufs_incr_viewer->get_minval();
    _maxval = ufs_incr_viewer->get_maxval();
}

void UFSincrController::draw()
{
    ufs_incr_viewer->draw();
}

void UFSincrController::set1dvarname(string vn)
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

    incr_geometry->set_nlev(1);

  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "setup for <" << vn << ">" << endl;

  //ufs_incr_viewer->set_lister(&lister[0]);
    ufs_incr_viewer->set_incr_geometry(incr_geometry);
    ufs_incr_viewer->setup(vn, _value);

    _minval = ufs_incr_viewer->get_minval();
    _maxval = ufs_incr_viewer->get_maxval();
}

void UFSincrController::set2dvarname(string vn)
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

    incr_geometry->set_nlev(1);

  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "setup for <" << vn << ">" << endl;

  //ufs_incr_viewer->set_lister(&lister[0]);
    ufs_incr_viewer->set_incr_geometry(incr_geometry);
    ufs_incr_viewer->setup(vn, _value);

    _minval = ufs_incr_viewer->get_minval();
    _maxval = ufs_incr_viewer->get_maxval();
}

void UFSincrController::set3dvarname(string vn)
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

    incr_geometry->set_nlon(ncfile->getNlon());
    incr_geometry->set_nlat(ncfile->getNlat());
    incr_geometry->set_nlev(ncfile->getNlev());

  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "setup for <" << vn << ">" << endl;
  //cout << "nvfile->get_dim_size('lev') = " << nvfile->get_dim_size("lev") << endl;

  //ufs_incr_viewer->set_lister(&lister[0]);
    ufs_incr_viewer->set_incr_geometry(incr_geometry);
    ufs_incr_viewer->setup(vn, _value);

    _minval = ufs_incr_viewer->get_minval();
    _maxval = ufs_incr_viewer->get_maxval();
}

void UFSincrController::set_colorTable(ColorTable *ct)
{
    ufs_incr_viewer->reset_texture1d(ct);
}

int UFSincrController::get_callList()
{
  //cout << "\nFunction: " << __PRETTY_FUNCTION__
  //     << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;

  //return ufs_incr_viewer->get_callList();
    return 0;
}

void UFSincrController::update_file(const char* fn)
{
    cout << "\nFunction: " << __PRETTY_FUNCTION__ 
         << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
    cout << "\tfilename: <" << fn << ">" << endl;
}

int UFSincrController::get_ndv(int n)
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

vector<string> UFSincrController::get_ndvNames(int n)
{
    vector<string> varnames;
    if (2 == n)
        varnames = ncfile->getV2dNames();
    else if (3 == n)
        varnames = ncfile->getV3dNames();
    return varnames;
}

string UFSincrController::get_timestring()
{
    return ncfile->getTimeString();
}

void UFSincrController::set_fileNtime(int nf, int nt)
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

        incr_geometry->set_nlev(1);
        incr_geometry->set_ntim(_ntimes[_curFile]);

        _minval = ufs_incr_viewer->get_minval();
        _maxval = ufs_incr_viewer->get_maxval();

        _initialized = true;
    }

    _preFile = _curFile;

    gridsize = _curTime * incr_geometry->get_nlon() * incr_geometry->get_nlat() * incr_geometry->get_nlev();

    _set_glbTime();
  //ufs_incr_viewer->set_lister(&lister[_glbTime]);
    ufs_incr_viewer->setup(_varname, &_value[gridsize]);
}

void UFSincrController::_set_glbTime()
{
    int n;

    _glbTime = _curTime;
    for(n = 0; n < _curFile; ++n)
        _glbTime += _ntimes[n];
}

void UFSincrController::set_locator(Locator* l)
{
     locator = l;

     ufs_incr_viewer->set_locator(l);
}

