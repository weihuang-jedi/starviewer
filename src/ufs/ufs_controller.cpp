#include "ufs_controller.h"

UFSController::UFSController(ColorTable *ct, NVOptions* opt,
                                 Earth* earth, CoastLine* cl,
                                 const char *fn, bool isList)
{
    // cout << "\nEnter functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    string sfn = string(fn);

    // cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    colorTable = ct;
    // cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    nvoptions = opt;
    // cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    coastline = cl;
    // cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    strcpy(_flnm, fn);

    // cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    geometry = new UFSGeometry();
    // cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    geometry->set_name(sfn);
  
    _maxFile = 1;
    _ntim = 1;

    // cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    ncfile = new ncReader(fn);
    // cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    ufs_viewer = new UFS2dViewer(colorTable, nvoptions, earth, ncfile);
    // cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    ufs_viewer->set_coastline(cl);
    // cout << "Leave functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
}

UFSController::~UFSController()
{
    delete ncfile;
    
    if(NULL != ufs_viewer)
        delete ufs_viewer;
    ufs_viewer = NULL;
    
    delete geometry;
} 

template<typename T>
void UFSController::_print1d(T* var, int nl)
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

void UFSController::setup()
{
    int n;

    // cout << "\nEnter functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;

  //_maxFile = get_nfiles();
    _maxFile = 1;

  //for(n = 0; n < _maxFile; ++n)
  //    _maxTime += _ntimes[n];
    _maxTime = 1;

  //lister = new Lister[_maxTime];

    // cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
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

    // cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
  //_varname = string("sst");
    _varname = string("hgtsfc");

    _sphere = false;
    _initialized = false;
    _tvalue = 0;
    _time_interval = 128;
    _curFile = 0;
    _curTime = 0;
    _preFile = _curFile;

  //ufs_viewer->set_lister(&lister[0]);
    // cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    // cout << "\tvarname:" << _varname << endl;

    _value = ncfile->get_fv(_varname.c_str());
    _title = _varname;

    // cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    ufs_viewer->set_geometry(geometry);
  //ufs_3dviewer->set_geoufs_(geoufs_);

    // cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    ufs_viewer->setup(_varname, _value);
    _minval = ufs_viewer->get_minval();
    _maxval = ufs_viewer->get_maxval();

    // cout << "Leave functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
}

void UFSController::draw()
{
    ufs_viewer->draw();
}

void UFSController::set1dvarname(string vn)
{
    _varname = vn;
    _sphere = true;

    _tvalue = 0;

    if(_initialized)
        free(_value);

    _value = ncfile->get_fv(vn.c_str());
    _title = vn;

    _initialized = true;

    geometry->set_nlev(1);

  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "setup for <" << vn << ">" << endl;

  //ufs_viewer->set_lister(&lister[0]);
    ufs_viewer->set_geometry(geometry);
    ufs_viewer->setup(vn, _value);

    _minval = ufs_viewer->get_minval();
    _maxval = ufs_viewer->get_maxval();
}

void UFSController::set2dvarname(string vn)
{
    _varname = vn;
    _sphere = false;

    _tvalue = 0;

    if(_initialized)
        free(_value);

    _value = ncfile->get_fv(vn.c_str());
    _title = vn;

    _initialized = true;

    geometry->set_nlev(1);

  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "setup for <" << vn << ">" << endl;

  //ufs_viewer->set_lister(&lister[0]);
    ufs_viewer->set_geometry(geometry);
    ufs_viewer->setup(vn, _value);

    _minval = ufs_viewer->get_minval();
    _maxval = ufs_viewer->get_maxval();
}

void UFSController::set3dvarname(string vn)
{
    _varname = vn;
    _sphere = false;

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

  //ufs_viewer->set_lister(&lister[0]);
    ufs_viewer->set_geometry(geometry);
    ufs_viewer->setup(vn, _value);

    _minval = ufs_viewer->get_minval();
    _maxval = ufs_viewer->get_maxval();

    // if(nvoptions->get_cb(NV_VECTORON))
    // {
	float* u = ncfile->get_fv("ugrd");
	float* v = ncfile->get_fv("vgrd");
	ufs_viewer->setup_wind(u, v);
    // }
}

void UFSController::set_colorTable(ColorTable *ct)
{
    ufs_viewer->reset_texture1d(ct);
}

int UFSController::get_callList()
{
  //cout << "\nFunction: " << __PRETTY_FUNCTION__
  //     << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;

  //return ufs_viewer->get_callList();
    return 0;
}

void UFSController::update_file(const char* fn)
{
    cout << "\nFunction: " << __PRETTY_FUNCTION__ 
         << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
    cout << "\tfilename: <" << fn << ">" << endl;
}

int UFSController::get_ndv(int n)
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

vector<string> UFSController::get_ndvNames(int n)
{
    vector<string> varnames;
    if (2 == n)
        varnames = ncfile->getV2dNames();
    else if (3 == n)
        varnames = ncfile->getV3dNames();
    return varnames;
}

string UFSController::get_timestring()
{
    return ncfile->getTimeString();
}

void UFSController::set_fileNtime(int nf, int nt)
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

        _minval = ufs_viewer->get_minval();
        _maxval = ufs_viewer->get_maxval();

        _initialized = true;
    }

    _preFile = _curFile;

    gridsize = _curTime * geometry->get_nlon() * geometry->get_nlat() * geometry->get_nlev();

    _set_glbTime();
  //ufs_viewer->set_lister(&lister[_glbTime]);
    ufs_viewer->setup(_varname, &_value[gridsize]);
}

void UFSController::_set_glbTime()
{
    int n;

    _glbTime = _curTime;
    for(n = 0; n < _curFile; ++n)
        _glbTime += _ntimes[n];
}

void UFSController::set_locator(Locator* l)
{
     locator = l;

     ufs_viewer->set_locator(l);
}

