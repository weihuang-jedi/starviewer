#include "ufs_controller.h"

UFSController::UFSController(ColorTable *ct, NVOptions* opt,
                                 const char *fn, bool isList)
{
    string sfn = string(fn);

    colorTable = ct;
    nvoptions = opt;
    strcpy(_flnm, fn);

    _hasMappingFile = false;

    cout << "\tEnter functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    cout << "\tOpen file: <" << fn << ">" << endl;

    geometry = new UFSGeometry();
    geometry->set_name(sfn);

  //cout << "\t\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
  //coastline = new CoastLine();

    cout << "\t\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    cout << "\t\tsfn = " << sfn << endl;
  //nvfile = new NVFile(sfn, isList);
    ncfile = new ncReader(fn);

    ufs_viewer = NULL;
    mappingfile = NULL;

  //cout << "\tLeave functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
}

UFSController::~UFSController()
{
  //delete coastline;
  //delete nvfile;
    delete ncfile;
    
    if(NULL != ufs_viewer)
        delete ufs_viewer;
    ufs_viewer = NULL;
    
    if(NULL != mappingfile)
        delete mappingfile;
    mappingfile = NULL;

    delete geometry;
} 

void UFSController::setup()
{
    int n;

    _ntimes = nvfile->get_ntimes();

    _maxFile = get_nfils();

    _maxTime = 0;
    for(n = 0; n < _maxFile; ++n)
        _maxTime += _ntimes[n];

  //lister = new Lister[_maxTime];

    geometry->set_ufs__ncol(nvfile->get_dim_size("ncol"));
    geometry->set_ufs__lev(nvfile->get_dim_size("lev"));
    geometry->set_nz(nvfile->get_dim_size("lev"));
    geometry->set_nt(nvfile->get_dim_size("time"));

    geometry->set_ufs__lon(nvfile->get_dv("lon"));
    geometry->set_ufs__lat(nvfile->get_dv("lat"));
    geometry->setup_ufs_();

  //_varname = string("sst");
    _varname = string("PSL");

    _sphere = false;
    _ball = false;
    _initialized = false;

    _tvalue = 0;
    _time_interval = 128;

    _curFile = 0;
    _curTime = 0;
    _preFile = _curFile;

    ufs_viewer = new UFS2dViewer(colorTable, nvoptions);

  //ufs_viewer->set_lister(&lister[0]);

    _value = nvfile->get_dv(_varname);
    _title = nvfile->get_title();

    geometry->set_ufs__ncol(nvfile->get_dim_size("ncol"));
    geometry->set_ufs__lev(1);
    geometry->set_nz(1);
    geometry->set_nt(nvfile->get_dim_size("time"));

  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\tUFSController ncenters : " << geometry->get_ufs__ncenters() << endl;
  //cout << "\tUFSController ncorners : " << geometry->get_ufs__ncorners() << endl;
  //cout << "\tUFSController ncol     : " << geometry->get_ufs__ncol() << endl;
  //cout << "\tUFSController lev      : " << geometry->get_ufs__lev() << endl;
  //cout << "\tUFSController nt       : " << geometry->get_nt() << endl;

  //cout << "\n_setup, file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //geometry->print();

  //ufs_viewer->set_coastline(coastline);

    ufs_viewer->set_geometry(geometry);
  //ufs_3dviewer->set_geoufs_(geoufs_);

    ufs_viewer->setup(_varname, _value);
    _minval = ufs_viewer->get_minval();
    _maxval = ufs_viewer->get_maxval();
}

void UFSController::draw()
{
  //if(_sphere)
  //{
      //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
      //cout << "2d draw" << endl;

        ufs_viewer->draw();
      //coastline->draw();
  //}
  //else
  //{
  //  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
  //  //cout << "need to draw a ball" << endl;
      //ufs_3dviewer->draw(_tvalue);
  //}
}

void UFSController::set1dvarname(string vn)
{
    _varname = vn;
    _sphere = true;
    _ball = false;

    _tvalue = 0;

    if(_initialized)
        free(_value);

    _value = nvfile->get_dv(vn);
    _title = nvfile->get_title();

    _initialized = true;

    geometry->set_ufs__ncol(nvfile->get_dim_size("ncol"));
    geometry->set_ufs__lev(1);
    geometry->set_nz(1);
    geometry->set_nt(nvfile->get_dim_size("time"));

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

    _value = nvfile->get_dv(vn);
    _title = nvfile->get_title();

    _initialized = true;

    geometry->set_ufs__ncol(nvfile->get_dim_size("ncol"));
    geometry->set_ufs__lev(1);
    geometry->set_nz(1);
  //geometry->set_ufs__lev(nvfile->get_dim_size("lev"));
  //geometry->set_nz(nvfile->get_dim_size("lev"));
    geometry->set_nt(nvfile->get_dim_size("time"));

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

    _value = nvfile->get_dv(vn);
    _title = nvfile->get_title();

    _initialized = true;

    geometry->set_ufs__ncol(nvfile->get_dim_size("ncol"));
    geometry->set_ufs__lev(nvfile->get_dim_size("lev"));
    geometry->set_nz(nvfile->get_dim_size("lev"));
    geometry->set_nt(nvfile->get_dim_size("time"));

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
    return nvfile->get_ndv(n);
}

string* UFSController::get_ndvNames(int n)
{
    string* varnames = nvfile->get_ndvNames(n);
    return varnames;
}

string* UFSController::get_timestring()
{
    return nvfile->get_timestr();
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

        nvfile->select_file(nf);

        _value = nvfile->get_dv(_varname);
        _title = nvfile->get_title();

        geometry->set_nt(_ntimes[_curFile]);

        _minval = ufs_viewer->get_minval();
        _maxval = ufs_viewer->get_maxval();

        _initialized = true;
    }

    _preFile = _curFile;

    gridsize = _curTime * geometry->get_ufs__ncol() *  geometry->get_ufs__lev();

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

void UFSController::set_mappingFile(string mfnm)
{
    size_t inputfile_ncol = 0;
    size_t mappingfile_ncol = -1;
    ifstream f(mfnm.c_str());

    inputfile_ncol = nvfile->get_dim_size("ncol");

  //cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
  //cout << "\t\tmapping file = " << mfnm << endl;
  //cout << "\t\tinputfile_ncol = " << inputfile_ncol << endl;

    if(f.good())
    {
        f.close();
        _hasMappingFile = true;
        _mappingFilename = mfnm;
      //cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
      //cout << "\tfind matching mapping file:" << _mappingFilename << endl;

        mappingfile = new NVFile(_mappingFilename.c_str(), false);
        mappingfile_ncol = mappingfile->get_dim_size("ncenters");

      //cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
      //cout << "\t\tmappingfile_ncol = " << mappingfile_ncol << endl;
    }
    else
    {
        char mappingfile_str[512];
        f.close();

        _hasMappingFile = false;

        strcpy(mappingfile_str, getenv("NV_DATA"));
        strcat(mappingfile_str, "/ufs__1degree_mapping.nc");
        _mappingFilename = mappingfile_str;

      //cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
      //cout << "\tfind matching mapping file:" << _mappingFilename << endl;

        mappingfile = new NVFile(mappingfile_str, false);
        mappingfile_ncol = mappingfile->get_dim_size("ncenters");

      //cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
      //cout << "\t\tmappingfile_ncol = " << mappingfile_ncol << endl;

        if((inputfile_ncol - 2) != mappingfile_ncol)
        {
            delete mappingfile;
            strcpy(mappingfile_str, getenv("NV_DATA"));
            strcat(mappingfile_str, "/ufs__0.25degree_mapping.nc");
            _mappingFilename = mappingfile_str;
            mappingfile = new NVFile(mappingfile_str, false);
            mappingfile_ncol = mappingfile->get_dim_size("ncenters");
        }   
    }

    if((inputfile_ncol - 2) != mappingfile_ncol)
    {
        cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
        cout << "\tinputfile_ncol = " << inputfile_ncol << ", mappingfile_ncol = " << mappingfile_ncol << endl;
        cout << "\tCan not find matching mapping file." << endl;
        cout << "\tplease run as: '[vglrun] nv -mappingfile se-mapping-file -ufs_ se-filename'" << endl;

      //delete mappingfile;

        exit (-1);
    }

    geometry->set_ufs__ncenters(mappingfile->get_dim_size("ncenters"));
    geometry->set_ufs__ncorners(mappingfile->get_dim_size("ncorners"));
    geometry->set_ufs__element_corners(mappingfile->get_iv("element_corners"));
}

void UFSController::set_locator(Locator* l)
{
     locator = l;

     ufs_viewer->set_locator(l);
}

