#include "ufs_controller.h"

UFSController::UFSController(ColorTable *ct, NVOptions* opt,
                                 const char *fn, bool isList)
{
    string sfn = string(fn);

    colorTable = ct;
    nvoptions = opt;
    strcpy(_flnm, fn);

  //cout << "Enter functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    cout << "\tOpen file: <" << fn << ">" << endl;

    geometry = new UFSGeometry();
    geometry->set_name(sfn);

  //cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
  //coastline = new CoastLine();
  
    _maxFile = 1;
    _ntim = 1;

  //cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
  //cout << "\tsfn = " << sfn << endl;
  //nvfile = new NVFile(sfn, isList);
    ncfile = new ncReader(fn);

    ufs_viewer = NULL;

  //cout << "Leave functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
}

UFSController::~UFSController()
{
  //delete coastline;
  //delete nvfile;
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

  //cout << "Enter functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;

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
    geometry->set_nlev(ncfile->getNlev());
  //geometry->set_ntim(_ntimes[0]);
    geometry->set_ntim(_maxTime);

    geometry->set_lon(ncfile->getLon());
    geometry->set_lat(ncfile->getLat());
    geometry->set_lev(ncfile->getPfull());

  //geometry->set_lon2d(ncfile->get_lon2d());
  //geometry->set_lat2d(ncfile->get_lat2d());

  //cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;

    geometry->setup();

  //cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;

    geometry->set_has1dLon(true);
    geometry->set_has1dLat(true);

    geometry->set_has2dLon(false);
    geometry->set_has2dLat(false);

  //cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;

  //_varname = string("sst");
    _varname = string("hgtsfc");

    _sphere = false;
    _ball = false;
    _initialized = false;

  //cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;

    _tvalue = 0;
    _time_interval = 128;

  //cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;

    _curFile = 0;
    _curTime = 0;
    _preFile = _curFile;

  //cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;

    ufs_viewer = new UFS2dViewer(colorTable, nvoptions, "/work2/noaa/epic/weihuang/nv/starviewer/data/earth.bmp", ncfile);

  //ufs_viewer->set_lister(&lister[0]);
  //
  //cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
  //cout << "\tvarname:" << _varname << endl;

    _value = ncfile->get_fv(_varname.c_str());
    _title = _varname;

  //cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
  //geometry->print();

  //ufs_viewer->set_coastline(coastline);

    ufs_viewer->set_geometry(geometry);
  //ufs_3dviewer->set_geoufs_(geoufs_);

  //cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;

    ufs_viewer->setup(_varname, _value);
    _minval = ufs_viewer->get_minval();
    _maxval = ufs_viewer->get_maxval();
  //cout << "Leave functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
}

void UFSController::draw()
{
  //cout << "Enter functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
  //cout << "_sphere: " << _sphere << endl;
    if(_sphere)
    {
        cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
        cout << "need to draw a ball" << endl;

      //ufs_3dviewer->draw(_tvalue);
    }
    else
    {
      //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
      //cout << "2d draw" << endl;
        ufs_viewer->draw();
      //coastline->draw();
    }
  //cout << "Leave functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
}

void UFSController::set1dvarname(string vn)
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

  //ufs_viewer->set_lister(&lister[0]);
    ufs_viewer->set_geometry(geometry);
    ufs_viewer->setup(vn, _value);

    _minval = ufs_viewer->get_minval();
    _maxval = ufs_viewer->get_maxval();
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

      //nvfile->select_file(nf);

        _value = ncfile->get_fv(_varname.c_str());
        _title = _varname;

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

