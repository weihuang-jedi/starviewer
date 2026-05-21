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

  //cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;

  //_maxFile = get_nfiles();
    _maxFile = 1;

  //for(n = 0; n < _maxFile; ++n)
    _maxTime = 1;

  //lister = new Lister[_maxTime];

    for(n = 0; n < _ntiles; ++n)
    {
        ncgridfile[n] = new UFSGridReader(_gridfilenames[n].c_str());
        _nlon = ncgridfile[n]->getNlon();
        _nlat = ncgridfile[n]->getNlat();
        ncincrfile[n] = new UFSIncrementReader(_incrementfilenames[n].c_str());
	_nlev = ncincrfile[n]->getNz();
	_ntim = ncincrfile[n]->getNt();
        geometry[n] = new UFSINCRGeometry(_nlon, _nlat,
			                  ncgridfile[n]->getGeoLon(),
			                  ncgridfile[n]->getGeoLat());
    }

    _varname = string("T_inc");

    _sphere = false;
    _ball = false;
    _initialized = false;
    _tvalue = 0;
    _curFile = 0;
    _curTime = 0;
    _preFile = _curFile;

    ufsincr_viewer = new UFSINCR2dViewer(colorTable, nvoptions, bmpflnm, ncincrfile);

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

void UFSINCRController::set_locator(Locator* l)
{
     locator = l;

     ufsincr_viewer->set_locator(l);
}

