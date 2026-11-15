#include "mpasgridreader.h"
#include "mpasgridcontroller.h"

MPASGridController::MPASGridController(ColorTable *ct, NVOptions* opt,
   		                       string static_flnm, string data_flnm)
{
    colorTable = ct;
    nvoptions = opt;

    // cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__
    //      << ", file: <" << __FILE__ << ">" << endl;

    _preFile = -1;
    _curFile = 0;
    _glbTime = 0;
    _curTime = 0;

    _value = NULL;
    mpasgrid2dviewer = NULL;

    geometry = new MPASGeometry();
    geometry->set_name(static_flnm);

    // cout << "\tat <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    // cout << "\tstatic_flnm: <" << static_flnm << ">" << endl;
    ncstatic = new MPASStaticReader(static_flnm.c_str());
    // cout << "\tat <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    ncdata = new MPASGridReader(data_flnm.c_str());
    // cout << "\tat <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    _ntimes = new int(1);
    _ntimes[0] = 1;

    // cout << "\tat <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    _list2dvars = ncdata->getV2dNames();
    _list3dvars = ncdata->getV3dNames();

    // cout << "\tat <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    geometry->set_nCells(ncstatic->get_nCells());
    geometry->set_nVertices(ncstatic->get_nVertices());
    geometry->set_vertexDegree(ncstatic->get_vertexDegree());
    geometry->set_nVertLevels(1);
    geometry->set_nTime(ncstatic->get_nTime());

    geometry->set_mx(360);
    geometry->set_my(180);
    geometry->set_mz(1);

    geometry->set_nx(360);
    geometry->set_ny(180);
    geometry->set_nz(1);
    geometry->set_nt(1);

    geometry->set_lonCell(ncstatic->get_lonCell());
    geometry->set_latCell(ncstatic->get_latCell());
    geometry->set_cellsOnVertex(ncstatic->get_cellsOnVertex());

    geometry->setup();

    _varname = string("pic");
    // cout << "Leave <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
}

MPASGridController::~MPASGridController()
{
    delete geometry;
    delete ncstatic;
    delete ncdata;

    if(NULL != _value)
        free(_value);

    if(NULL != _ntimes)
        free(_ntimes);

    if(NULL != mpasgrid2dviewer)
        delete mpasgrid2dviewer;
} 

void MPASGridController::setup()
{
    _initialized = false;

    _tvalue = 0;

    // cout << "\nEnter <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    // cout << "\tMPASGridController nCells      : " << geometry->get_nCells() << endl;
    // cout << "\tMPASGridController nVertices   : " << geometry->get_nVertices() << endl;
    // cout << "\tMPASGridController nVertLevels : " << geometry->get_nVertLevels() << endl;
    // cout << "\tMPASGridController vertexDegree: " << geometry->get_vertexDegree() << endl;

    mpasgrid2dviewer = new MPASGrid2dViewer(colorTable, nvoptions);

    _value = ncdata->get_iv(_varname.c_str());
    geometry->set_nVertLevels(1);

    mpasgrid2dviewer->set_geometry(geometry);
    mpasgrid2dviewer->setup(_varname, _value);

    // cout << "Leave <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
}

void MPASGridController::draw()
{
    // cout << "Enter <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    mpasgrid2dviewer->draw();
    // cout << "Leave <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
}

bool MPASGridController::is2d(string vn)
{
    bool is2d = false;
    for(int n =0; n < _list2dvars.size(); ++n)
    {
	if(vn == _list2dvars[n])
	{
            is2d = true;
	    break;
	}
    }
    return is2d;
}

bool MPASGridController::is3d(string vn)
{
    bool is3d = false;
    for(int n =0; n < _list3dvars.size(); ++n)
    {
        if(vn == _list3dvars[n])
        {
            is3d = true;
            break;
        }
    }
    return is3d;
}

void MPASGridController::set1dvarname(string vn)
{
    _varname = vn;

    _tvalue = 0;

    if(_initialized && (NULL != _value))
        free(_value);

    _value = ncdata->get_iv(vn.c_str());

    _initialized = true;

  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "setup for <" << vn << ">" << endl;

    geometry->set_nVertLevels(1);

    mpasgrid2dviewer->set_geometry(geometry);
    mpasgrid2dviewer->setup(vn, _value);

    _curFile = 0;
    _curTime = 0;
    _glbTime = 0;
    geometry->set_nt(1);
    // geometry->set_nt(_ntimes[_curFile]);

    // _minval = mpasgrid2dviewer->get_minval();
    // _maxval = mpasgrid2dviewer->get_maxval();
}

void MPASGridController::set2dvarname(string vn)
{
    // cout << "\nEnter functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    // cout << "setup for <" << vn << ">" << endl;
    _varname = vn;

    _tvalue = 0;

    if(_initialized && (NULL != _value))
        free(_value);

    // cout << "\t" << __PRETTY_FUNCTION__ << ": line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    _value = ncdata->get_iv(vn.c_str());

    _initialized = true;

    // cout << "\t" << __PRETTY_FUNCTION__ << ": line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    // cout << "\tnis2d(" << vn << ") = " << is2d(vn) << endl;

    geometry->set_nVertLevels(1);

    // cout << "\t" << __PRETTY_FUNCTION__ << ": line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;

    mpasgrid2dviewer->set_geometry(geometry);
    // cout << "\t" << __PRETTY_FUNCTION__ << ": line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    mpasgrid2dviewer->setup(vn, _value);

    // cout << "\t" << __PRETTY_FUNCTION__ << ": line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    _curFile = 0;
    _curTime = 0;
    _glbTime = 0;
    geometry->set_nt(1);

    // cout << "\t" << __PRETTY_FUNCTION__ << ": line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    // _minval = mpas2dviewer->get_minval();
    // _maxval = mpas2dviewer->get_maxval();

    // cout << "Leave functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
}

void MPASGridController::set3dvarname(string vn)
{
    // cout << "\nEnter functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    // cout << "setup for <" << vn << ">" << endl;
    _varname = vn;

    _tvalue = 0;

    if(_initialized && (NULL != _value))
        free(_value);

    // cout << "\t" << __PRETTY_FUNCTION__ << ": line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    _value = ncdata->get_iv(vn.c_str());

    _initialized = true;

    // cout << "\t" << __PRETTY_FUNCTION__ << ": line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    // cout << "\tnis2d(" << vn << ") = " << is2d(vn) << endl;

    geometry->set_nVertLevels(1);

    // cout << "\t" << __PRETTY_FUNCTION__ << ": line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;

    mpasgrid2dviewer->set_geometry(geometry);
    // cout << "\t" << __PRETTY_FUNCTION__ << ": line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    mpasgrid2dviewer->setup(vn, _value);

    // cout << "\t" << __PRETTY_FUNCTION__ << ": line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    _curFile = 0;
    _curTime = 0;
    _glbTime = 0;
    geometry->set_nt(1);

    // cout << "\t" << __PRETTY_FUNCTION__ << ": line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    // _minval = mpasgrid2dviewer->get_minval();
    // _maxval = mpasgrid2dviewer->get_maxval();

    // cout << "Leave functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
}

void MPASGridController::set_colorTable(ColorTable *ct)
{
    mpasgrid2dviewer->reset_texture1d(ct);
}

int MPASGridController::get_ndv(int n)
{
    int ndv = 0;
    if(n == 2)
        ndv = ncdata->getNumV2ds();
    else if(n == 3)
        ndv = ncdata->getNumV3ds();
    return ndv;
}

vector<string> MPASGridController::get_ndvNames(int n)
{
    vector<string> varnames;
    if(n == 2)
        varnames = ncdata->getV2dNames();
    else if(n == 3)
        varnames = ncdata->getV3dNames();
    return varnames;
}

string* MPASGridController::get_timestring()
{
    string tsv = string("Unknown");
    string* ts = new string[1];
    ts[0] = tsv;
    return ts;
}

void MPASGridController::set_fileNtime(int nf, int nt)
{
    size_t gridsize = 1;

  //cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
  //cout << "\tcurFile: " << nf << ", curTime: " << nt << ", varname: <" << _varname << ">" << endl;

    _curFile = nf;
    _curTime = nt;

    if(_preFile != _curFile)
    {
        if(_initialized && (NULL != _value))
            free(_value);

        ncdata->select_file(nf);

        _value = ncdata->get_iv(_varname.c_str());
        _title = string("MPASGrid");
        // _title = ncdata->get_title();

        geometry->set_nt(_ntimes[_curFile]);

        _set_glbTime();
        _minval = mpasgrid2dviewer->get_minval();
        _maxval = mpasgrid2dviewer->get_maxval();

        _initialized = true;
    }

    _preFile = _curFile;

    gridsize = _curTime * geometry->get_nVertices() *  geometry->get_nVertLevels();

    mpasgrid2dviewer->setup(_varname, &_value[gridsize]);
}

void MPASGridController::_set_glbTime()
{
    int n;

    _glbTime = _curTime;
    for(n = 0; n < _curFile; ++n)
        _glbTime += _ntimes[n];
}

void MPASGridController::set_locator(Locator* l)
{
     locator = l;

     mpasgrid2dviewer->set_locator(l);
}

void MPASGridController::set_tl(int n)
{
      _tvalue = n;
     mpasgrid2dviewer->set_tvalue(n);
}

