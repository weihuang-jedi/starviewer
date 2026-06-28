#include <set>

#include "eagleglobal_controller.h"

EagleGlobalController::EagleGlobalController(ColorTable *ct, NVOptions* opt,
                                             vector<string> vecdfs)
{
    colorTable = ct;
    nvoptions = opt;
    datafiles = vecdfs;

    // cout << "Enter functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    geometry = new EagleGlobalGeometry();

    coastline = new CoastLine();
  
    _maxFile = 1;
    _ntime = 1;

    // for (int n = 0; n < datafiles.size(); ++n)
    //     cout << "datafiles[" << n << "]: <" << datafiles[n] << ">" << endl;

    ncfile = new EagleGlobalReader(datafiles[0]);

    set<string> nc1dvars = {"forecast_reference_time", "time",
                            "latitude", "longitude", "CRS"};

    vector<string> varnames = ncfile->getVarNames();
    _varname = varnames[0];
    for(int n=0; n<varnames.size(); ++n)
    {
	if (nc1dvars.find(varnames[n]) == nc1dvars.end())
	{
            _varname = varnames[n];
	    break;
        }
    }
    // cout << "Select varname: " << _varname << endl;
    eagleglobal_viewer = NULL;
    // cout << "Leave functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
}

EagleGlobalController::~EagleGlobalController()
{
    delete coastline;
    delete ncfile;
    
    if(NULL != eagleglobal_viewer)
        delete eagleglobal_viewer;
    eagleglobal_viewer = NULL;
    
    delete geometry;
} 

template<typename T>
void EagleGlobalController::_print1d(T* var, int nl)
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

void EagleGlobalController::setup()
{
    // cout << "Enter functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    int i, n;
    char bmpflnm[1024];
    const char* path = getenv("STARVIEWERHOME");
    if (path == nullptr) {
        cout << "ERROR: STARVIEWERHOME not set!" << endl;
        throw(errno);
    }
    strcpy(bmpflnm, path);
    strcat(bmpflnm, "/data/earth.bmp");

    set<string> nc1dvars = {"forecast_reference_time", "time",
                            "latitude", "longitude", "CRS"};

    vector<EagleGlobalReader*> nchandler(datafiles.size());
    varlist.resize(datafiles.size());
    
    for (i = 0; i < datafiles.size(); ++i)
    {
        // cout << "datafiles[" << i << "]: <" << datafiles[i] << ">" << endl;

        nchandler[i] = new EagleGlobalReader(datafiles[i]);

        vector<string> varnames = nchandler[i]->getVarNames();

        for(n=0; n<varnames.size(); ++n)
        {
	    if (nc1dvars.find(varnames[n]) == nc1dvars.end())
	    {
                var2file[varnames[n]] = nchandler[i];
                varlist[i] = varnames[n];
                // cout << "varlist[" << i << "]: <" << varlist[i] << ">" << endl;
	        break;
            }
        }
    }

  //_ntimes = ncfile->get_ntimes();

  //cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
  //cout << "\t_ntimes = " << _ntimes << endl;

  //_maxFile = get_nfiles();
    _maxFile = 1;

  //for(n = 0; n < _maxFile; ++n)
  //    _maxTime += _ntimes[n];
    _maxTime = 1;

  //lister = new Lister[_maxTime];

    _nlon = ncfile->getNlon();
    _nlat = ncfile->getNlat();
    _ntime = ncfile->getNtime();
    // cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    // cout << "\t_nlon: " << _nlon << endl;
    // cout << "\t_nlat: " << _nlat << endl;
    // cout << "\t_ntime: " << _ntime << endl;
    geometry->set_nlon(_nlon);
    geometry->set_nlat(_nlat);
    geometry->set_nlev(1);
    geometry->set_ntime(_ntime);

    geometry->set_longitude(ncfile->getLongitude());
    geometry->set_latitude(ncfile->getLatitude());
    geometry->setup();

    // cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    // cout << "\t_nlon: " << geometry->get_nlon() << endl;
    // cout << "\t_nlat: " << geometry->get_nlat() << endl;
    // cout << "\t_ntime: " << geometry->get_ntime() << endl;

    _sphere = false;
    _ball = false;
    _initialized = false;
    _tvalue = 0;
    _time_interval = 128;
    _curFile = 0;
    _curTime = 0;
    _preFile = _curFile;

    eagleglobal_viewer = new EagleGlobal2dViewer(colorTable, nvoptions, bmpflnm);

  //eagleglobal_viewer->set_lister(&lister[0]);
    // cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    // cout << "\tvarname: " << _varname << endl;

    _value = ncfile->get_fv(_varname.c_str());
    _title = _varname;

    // cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    // cout << "\t_value[0] = " << _value[0] << endl;
  //geometry->print();

    eagleglobal_viewer->set_coastline(coastline);
    eagleglobal_viewer->set_geometry(geometry);
  //eagleglobal_3dviewer->set_geoeagleglobal_(geoeagleglobal_);

    eagleglobal_viewer->setup(_varname, _value);
    _minval = eagleglobal_viewer->get_minval();
    _maxval = eagleglobal_viewer->get_maxval();
    // cout << "Leave functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
}

void EagleGlobalController::draw()
{
    eagleglobal_viewer->draw();
}

void EagleGlobalController::setvarname(string vn)
{
    _varname = vn;
    _sphere = false;
    _ball = true;

    _tvalue = 0;

    if(_initialized)
        free(_value);

    // cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    // cout << "\tvarname: " << vn << endl;

    _value = var2file[vn]->get_fv(vn.c_str());
    _title = vn;

    // cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    // cout << "\t_value[0] = " << _value[0] << endl;

    _initialized = true;

    geometry->set_nlev(1);

  //eagleglobal_viewer->set_lister(&lister[0]);
    eagleglobal_viewer->set_geometry(geometry);
    eagleglobal_viewer->setup(vn, _value);

    _minval = eagleglobal_viewer->get_minval();
    _maxval = eagleglobal_viewer->get_maxval();
}

void EagleGlobalController::set_colorTable(ColorTable *ct)
{
    eagleglobal_viewer->reset_texture1d(ct);
}

int EagleGlobalController::get_callList()
{
  //cout << "\nFunction: " << __PRETTY_FUNCTION__
  //     << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;

  //return eagleglobal_viewer->get_callList();
    return 0;
}

void EagleGlobalController::update_file(const char* fn)
{
    cout << "\nFunction: " << __PRETTY_FUNCTION__ 
         << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
    cout << "\tfilename: <" << fn << ">" << endl;
}

int EagleGlobalController::get_ndv(int n)
{
    int ndv = 0;
    ndv = ncfile->getNumVars();
    return ndv;
}

vector<string> EagleGlobalController::get_ndvNames(int n)
{
    vector<string> varnames;
    varnames = ncfile->getVarNames();
    return varnames;
}

string EagleGlobalController::get_timestring()
{
    return string("");
}

void EagleGlobalController::set_fileNtime(int nf, int nt)
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

        // cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
        // cout << "\tvarname: " << _varname << endl;

        _value = ncfile->get_fv(_varname.c_str());
        _title = _varname;

        // cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
        // cout << "\t_value[0] = " << _value[0] << endl;

        geometry->set_nlev(1);
        geometry->set_ntim(_ntimes[_curFile]);

        _minval = eagleglobal_viewer->get_minval();
        _maxval = eagleglobal_viewer->get_maxval();

        _initialized = true;
    }

    _preFile = _curFile;

    gridsize = _curTime * geometry->get_nlon() * geometry->get_nlat();

    _set_glbTime();
  //eagleglobal_viewer->set_lister(&lister[_glbTime]);
    eagleglobal_viewer->setup(_varname, &_value[gridsize]);
}

void EagleGlobalController::_set_glbTime()
{
    int n;

    _glbTime = _curTime;
    for(n = 0; n < _curFile; ++n)
        _glbTime += _ntimes[n];
}

void EagleGlobalController::set_locator(Locator* l)
{
     locator = l;

     eagleglobal_viewer->set_locator(l);
}

