#include "camsecontroller.h"

CAMseController::CAMseController(ColorTable *ct, NVOptions* opt,
                                 const char *fn, bool isList)
{
    string sfn = string(fn);

    colorTable = ct;
    nvoptions = opt;
    strcpy(_flnm, fn);

    _hasMappingFile = false;

  //cout << "\tEnter functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
  //cout << "\tOpen file: <" << fn << ">" << endl;

    geometry = new CAMseGeometry();
    geometry->set_name(sfn);

  //cout << "\t\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    coastline = new CoastLine();

  //cout << "\t\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
  //cout << "\t\tsfn = " << sfn << endl;
    nvfile = new NVFile(sfn, isList);

    camse2dviewer = NULL;
    mappingfile = NULL;

  //cout << "\tLeave functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
}

CAMseController::~CAMseController()
{
    delete coastline;
    delete nvfile;
    
    if(NULL != camse2dviewer)
        delete camse2dviewer;
    camse2dviewer = NULL;
    
    if(NULL != mappingfile)
        delete mappingfile;
    mappingfile = NULL;

    delete geometry;
} 

void CAMseController::setup()
{
    int n;

    _ntimes = nvfile->get_ntimes();

    _maxFile = get_nfils();

    _maxTime = 0;
    for(n = 0; n < _maxFile; ++n)
        _maxTime += _ntimes[n];

  //lister = new Lister[_maxTime];

    geometry->set_camse_ncol(nvfile->get_dim_size("ncol"));
    geometry->set_camse_lev(nvfile->get_dim_size("lev"));
    geometry->set_nz(nvfile->get_dim_size("lev"));
    geometry->set_nt(nvfile->get_dim_size("time"));

    geometry->set_camse_lon(nvfile->get_dv("lon"));
    geometry->set_camse_lat(nvfile->get_dv("lat"));
    geometry->setup_camse();

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

    camse2dviewer = new CAMse2dViewer(colorTable, nvoptions);

  //camse2dviewer->set_lister(&lister[0]);

    _value = nvfile->get_dv(_varname);
    _title = nvfile->get_title();

    geometry->set_camse_ncol(nvfile->get_dim_size("ncol"));
    geometry->set_camse_lev(1);
    geometry->set_nz(1);
    geometry->set_nt(nvfile->get_dim_size("time"));

  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\tCAMseController ncenters : " << geometry->get_camse_ncenters() << endl;
  //cout << "\tCAMseController ncorners : " << geometry->get_camse_ncorners() << endl;
  //cout << "\tCAMseController ncol     : " << geometry->get_camse_ncol() << endl;
  //cout << "\tCAMseController lev      : " << geometry->get_camse_lev() << endl;
  //cout << "\tCAMseController nt       : " << geometry->get_nt() << endl;

  //cout << "\n_setup, file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //geometry->print();

    camse2dviewer->set_coastline(coastline);

    camse2dviewer->set_geometry(geometry);
  //camse3dviewer->set_geocamse(geocamse);

    camse2dviewer->setup(_varname, _value);
    _minval = camse2dviewer->get_minval();
    _maxval = camse2dviewer->get_maxval();
}

void CAMseController::draw()
{
  //if(_sphere)
  //{
      //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
      //cout << "2d draw" << endl;

        camse2dviewer->draw();
      //coastline->draw();
  //}
  //else
  //{
  //  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
  //  //cout << "need to draw a ball" << endl;
      //camse3dviewer->draw(_tvalue);
  //}
}

void CAMseController::set1dvarname(string vn)
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

    geometry->set_camse_ncol(nvfile->get_dim_size("ncol"));
    geometry->set_camse_lev(1);
    geometry->set_nz(1);
    geometry->set_nt(nvfile->get_dim_size("time"));

  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "setup for <" << vn << ">" << endl;

  //camse2dviewer->set_lister(&lister[0]);
    camse2dviewer->set_geometry(geometry);
    camse2dviewer->setup(vn, _value);

    _minval = camse2dviewer->get_minval();
    _maxval = camse2dviewer->get_maxval();
}

void CAMseController::set2dvarname(string vn)
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

    geometry->set_camse_ncol(nvfile->get_dim_size("ncol"));
    geometry->set_camse_lev(1);
    geometry->set_nz(1);
  //geometry->set_camse_lev(nvfile->get_dim_size("lev"));
  //geometry->set_nz(nvfile->get_dim_size("lev"));
    geometry->set_nt(nvfile->get_dim_size("time"));

  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "setup for <" << vn << ">" << endl;

  //camse2dviewer->set_lister(&lister[0]);
    camse2dviewer->set_geometry(geometry);
    camse2dviewer->setup(vn, _value);

    _minval = camse2dviewer->get_minval();
    _maxval = camse2dviewer->get_maxval();
}

void CAMseController::set3dvarname(string vn)
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

    geometry->set_camse_ncol(nvfile->get_dim_size("ncol"));
    geometry->set_camse_lev(nvfile->get_dim_size("lev"));
    geometry->set_nz(nvfile->get_dim_size("lev"));
    geometry->set_nt(nvfile->get_dim_size("time"));

  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "setup for <" << vn << ">" << endl;
  //cout << "nvfile->get_dim_size('lev') = " << nvfile->get_dim_size("lev") << endl;

  //camse2dviewer->set_lister(&lister[0]);
    camse2dviewer->set_geometry(geometry);
    camse2dviewer->setup(vn, _value);

    _minval = camse2dviewer->get_minval();
    _maxval = camse2dviewer->get_maxval();
}

void CAMseController::set_colorTable(ColorTable *ct)
{
    camse2dviewer->reset_texture1d(ct);
}

int CAMseController::get_callList()
{
  //cout << "\nFunction: " << __PRETTY_FUNCTION__
  //     << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;

  //return camse2dviewer->get_callList();
    return 0;
}

void CAMseController::update_file(const char* fn)
{
    cout << "\nFunction: " << __PRETTY_FUNCTION__ 
         << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
    cout << "\tfilename: <" << fn << ">" << endl;
}

int CAMseController::get_ndv(int n)
{
    return nvfile->get_ndv(n);
}

string* CAMseController::get_ndvNames(int n)
{
    string* varnames = nvfile->get_ndvNames(n);
    return varnames;
}

string* CAMseController::get_timestring()
{
    return nvfile->get_timestr();
}

void CAMseController::set_fileNtime(int nf, int nt)
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

        _minval = camse2dviewer->get_minval();
        _maxval = camse2dviewer->get_maxval();

        _initialized = true;
    }

    _preFile = _curFile;

    gridsize = _curTime * geometry->get_camse_ncol() *  geometry->get_camse_lev();

    _set_glbTime();
  //camse2dviewer->set_lister(&lister[_glbTime]);
    camse2dviewer->setup(_varname, &_value[gridsize]);
}

void CAMseController::_set_glbTime()
{
    int n;

    _glbTime = _curTime;
    for(n = 0; n < _curFile; ++n)
        _glbTime += _ntimes[n];
}

void CAMseController::set_mappingFile(string mfnm)
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
        strcat(mappingfile_str, "/camse_1degree_mapping.nc");
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
            strcat(mappingfile_str, "/camse_0.25degree_mapping.nc");
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
        cout << "\tplease run as: '[vglrun] nv -mappingfile se-mapping-file -camse se-filename'" << endl;

      //delete mappingfile;

        exit (-1);
    }

    geometry->set_camse_ncenters(mappingfile->get_dim_size("ncenters"));
    geometry->set_camse_ncorners(mappingfile->get_dim_size("ncorners"));
    geometry->set_camse_element_corners(mappingfile->get_iv("element_corners"));
}

void CAMseController::set_locator(Locator* l)
{
     locator = l;

     camse2dviewer->set_locator(l);
}

