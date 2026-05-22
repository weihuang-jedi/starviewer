#include "ufsincr_controller.h"

UFSINCRController::UFSINCRController(ColorTable *ct, NVOptions* opt,
                                     vector<string> gridflnm, vector<string> incrflnm)
{
    _ntiles = gridflnm.size();

    colorTable = ct;
    nvoptions = opt;

    _gridfilenames = gridflnm;
    _incrementfilenames = incrflnm;

    ncgridfile.resize(_ntiles);
    ncincrfile.resize(_ntiles);
    geometry.resize(_ntiles);

    coastline = new CoastLine();
  
    _maxFile = 1;
    _ntim = 1;

    ufsincr_viewer = NULL;
}

UFSINCRController::~UFSINCRController()
{
    delete coastline;
    
    if(NULL != ufsincr_viewer)
        delete ufsincr_viewer;
    
    for(UFSGridReader* ncfl : ncgridfile) {
        delete ncfl; // Safely deletes the concrete object on the heap
    }

    // Optional: clear the vector structure.
    // Not strictly necessary here since the vector is about to die anyway.
    ncgridfile.clear();

    for(UFSIncrementReader* ncfl : ncincrfile) {
        delete ncfl; // Safely deletes the concrete object on the heap
    }

    ncincrfile.clear();

    for(UFSINCRGeometry* gm : geometry) {
        delete gm; // Safely deletes the concrete object on the heap
    }
    geometry.clear();
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

    cout << "\nEnter functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;

  //_maxFile = get_nfiles();
    _maxFile = 1;

  //for(n = 0; n < _maxFile; ++n)
    _maxTime = 1;

  //lister = new Lister[_maxTime];

    _varname = string("T_inc");
    _value.resize(_ntiles);

    // cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    float* geolon;
    float* geolat;
    for(n = 0; n < _ntiles; ++n)
    {
        // cout << "\t_gridfilenames[" << n << "]: <" << _gridfilenames[n] << ">" << endl;
        ncgridfile[n] = new UFSGridReader(_gridfilenames[n].c_str());
        _nlon = ncgridfile[n]->getNlon();
        _nlat = ncgridfile[n]->getNlat();
        // cout << "\t_nlon: " << _nlon << ", _nlat: " << _nlat << endl;
        // cout << "\t_incrementfilenames[" << n << "]: <" << _incrementfilenames[n] << ">" << endl;
        ncincrfile[n] = new UFSIncrementReader(_incrementfilenames[n].c_str());
	_nlev = ncincrfile[n]->getNz();
	_ntim = ncincrfile[n]->getNt();
        // cout << "\t_nlev: " << _nlev << ", _ntim: " << _ntim << endl;
	geolon = ncgridfile[n]->getGeoLon();
	geolat = ncgridfile[n]->getGeoLat();
        geometry[n] = new UFSINCRGeometry(_nlon, _nlat, geolon, geolat);
    }

    _update_value(_varname.c_str());

    _sphere = false;
    _ball = false;
    _initialized = false;
    _tvalue = 0;
    _curFile = 0;
    _curTime = 0;
    _preFile = _curFile;
    // cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;

    ufsincr_viewer = new UFSINCR2dViewer(colorTable, nvoptions, bmpflnm, ncincrfile);

  //ufsincr_viewer->set_lister(&lister[0]);
    // cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    // cout << "\tvarname:" << _varname << endl;

    _title = _varname;

    // cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;

    ufsincr_viewer->set_coastline(coastline);
    ufsincr_viewer->set_geometry(geometry);
  //ufsincr_3dviewer->set_geoufsincr_(geoufsincr_);

    ufsincr_viewer->setup(_varname, _value);
    _minval = ufsincr_viewer->get_minval();
    _maxval = ufsincr_viewer->get_maxval();
    cout << "Leave functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
}

void UFSINCRController::draw()
{
    ufsincr_viewer->draw();
}

void UFSINCRController::_update_value(const char* vn)
{
    for(int n = 0; n < _ntiles; ++n)
    {
        _value[n] = ncincrfile[n]->getFloat(vn);
    }
}

void UFSINCRController::set1dvarname(string vn)
{
    _varname = vn;
    _sphere = true;
    _ball = false;

    _tvalue = 0;

    _title = vn;
    _update_value(vn.c_str());

    _initialized = true;

  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "setup for <" << vn << ">" << endl;

  //ufsincr_viewer->set_lister(&lister[0]);
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

    _title = vn;
    _update_value(vn.c_str());

    _initialized = true;

  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "setup for <" << vn << ">" << endl;

  //ufsincr_viewer->set_lister(&lister[0]);
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

    _title = vn;
    _update_value(vn.c_str());

    _initialized = true;

  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "setup for <" << vn << ">" << endl;
  //cout << "nvfile->get_dim_size('lev') = " << nvfile->get_dim_size("lev") << endl;

  //ufsincr_viewer->set_lister(&lister[0]);
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
        ndv = ncgridfile[0]->getNumV2ds();
    else if (3 == n)
        ndv = ncincrfile[0]->getNumV3ds();
    return ndv;
}

vector<string> UFSINCRController::get_ndvNames(int n)
{
    vector<string> varnames;
    if (2 == n)
        varnames = ncgridfile[0]->getV2dNames();
    else if (3 == n)
        varnames = ncincrfile[0]->getV3dNames();
    return varnames;
}

void UFSINCRController::set_locator(Locator* l)
{
     locator = l;

     ufsincr_viewer->set_locator(l);
}

