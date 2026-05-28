#include "mom6_controller.h"

MOM6Controller::MOM6Controller(ColorTable *ct, NVOptions* opt,
                                 const char *fn)
{
    string sfn = string(fn);

    colorTable = ct;
    nvoptions = opt;
    strcpy(_flnm, fn);

    geometry = new MOM6Geometry();
    geometry->set_name(sfn);

    coastline = new CoastLine();
  
    _maxFile = 1;
    _ntime = 1;

    ncfile = new UFSMOM6Reader(fn);

    mom6_viewer = NULL;
}

MOM6Controller::~MOM6Controller()
{
    delete coastline;
    delete ncfile;
    
    if(NULL != mom6_viewer)
        delete mom6_viewer;
    mom6_viewer = NULL;
    
    delete geometry;
} 

template<typename T>
void MOM6Controller::_print1d(T* var, int nl)
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

void MOM6Controller::setup()
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

    // cout << "\nEnter functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
  //cout << "\t_ntimes = " << _ntimes << endl;

  //_maxFile = get_nfiles();
    _maxFile = 1;

  //for(n = 0; n < _maxFile; ++n)
  //    _maxTime += _ntimes[n];
    _maxTime = 1;

  //lister = new Lister[_maxTime];

    geometry->set_nxh(ncfile->getNxh());
    geometry->set_nyh(ncfile->getNyh());
    // geometry->set_nzl(ncfile->getNzl());
    geometry->set_nzl(1);
    // geometry->set_ntime(_maxTime);

    // cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    geometry->set_geolon(ncfile->getGeolon());
    geometry->set_geolat(ncfile->getGeolat());
    geometry->set_zl(ncfile->getZl());

    // cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    geometry->setup();

    // cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    geometry->set_has1dLon(false);
    geometry->set_has1dLat(false);

    geometry->set_has2dLon(true);
    geometry->set_has2dLat(true);

    _varname = string("SST");

    _sphere = false;
    _ball = false;
    _initialized = false;
    _tvalue = 0;
    _time_interval = 12;
    _curFile = 0;
    _curTime = 0;
    _preFile = _curFile;

    // cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    mom6_viewer = new MOM62dViewer(colorTable, nvoptions, bmpflnm, ncfile);

  //mom6_viewer->set_lister(&lister[0]);
    // cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    // cout << "\tvarname:" << _varname << endl;

    _value = ncfile->get_fv(_varname.c_str());
    _title = _varname;

  //cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
  //geometry->print();

    mom6_viewer->set_coastline(coastline);
    mom6_viewer->set_geometry(geometry);
  //mom6_3dviewer->set_geomom6_(geomom6_);

    mom6_viewer->setup(_varname, _value);
    _minval = mom6_viewer->get_minval();
    _maxval = mom6_viewer->get_maxval();
    // cout << "Leave functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
}

void MOM6Controller::draw()
{
    mom6_viewer->draw();
}

void MOM6Controller::set1dvarname(string vn)
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

  //mom6_viewer->set_lister(&lister[0]);
    mom6_viewer->setup(vn, _value);

    _minval = mom6_viewer->get_minval();
    _maxval = mom6_viewer->get_maxval();
}

void MOM6Controller::set2dvarname(string vn)
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

  //mom6_viewer->set_lister(&lister[0]);
    mom6_viewer->setup(vn, _value);

    _minval = mom6_viewer->get_minval();
    _maxval = mom6_viewer->get_maxval();
}

void MOM6Controller::set3dvarname(string vn)
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

    geometry->set_nxh(ncfile->getNxh());
    geometry->set_nyh(ncfile->getNyh());
    geometry->set_nzl(ncfile->getNzl());

  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "setup for <" << vn << ">" << endl;
  //cout << "nvfile->get_dim_size('lev') = " << nvfile->get_dim_size("lev") << endl;

  //mom6_viewer->set_lister(&lister[0]);
    mom6_viewer->setup(vn, _value);

    _minval = mom6_viewer->get_minval();
    _maxval = mom6_viewer->get_maxval();
}

void MOM6Controller::set_colorTable(ColorTable *ct)
{
    mom6_viewer->reset_texture1d(ct);
}

int MOM6Controller::get_callList()
{
  //cout << "\nFunction: " << __PRETTY_FUNCTION__
  //     << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;

  //return mom6_viewer->get_callList();
    return 0;
}

void MOM6Controller::update_file(const char* fn)
{
    cout << "\nFunction: " << __PRETTY_FUNCTION__ 
         << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
    cout << "\tfilename: <" << fn << ">" << endl;
}

int MOM6Controller::get_ndv(int n)
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

vector<string> MOM6Controller::get_ndvNames(int n)
{
    vector<string> varnames;
    if (2 == n)
        varnames = ncfile->getV2dNames();
    else if (3 == n)
        varnames = ncfile->getV3dNames();
    return varnames;
}

string MOM6Controller::get_timestring()
{
    string ts = " ";
    return ts;
}

void MOM6Controller::set_fileNtime(int nf, int nt)
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

        _minval = mom6_viewer->get_minval();
        _maxval = mom6_viewer->get_maxval();

        _initialized = true;
    }

    _preFile = _curFile;

    gridsize = _curTime * geometry->get_nlon() * geometry->get_nlat() * geometry->get_nlev();

    _set_glbTime();
  //mom6_viewer->set_lister(&lister[_glbTime]);
    mom6_viewer->setup(_varname, &_value[gridsize]);
}

void MOM6Controller::_set_glbTime()
{
    int n;

    _glbTime = _curTime;
    for(n = 0; n < _curFile; ++n)
        _glbTime += _ntimes[n];
}

void MOM6Controller::set_locator(Locator* l)
{
     locator = l;

     mom6_viewer->set_locator(l);
}

