//$Id: wrf_controller.cpp 5339 2015-02-22 04:21:52Z starviewer $

#include "wrf_controller.h"

template<typename T>
T string2number(string sn);

template<typename T>
string number2string(T n);

WRF_Controller::WRF_Controller(ColorTable *ct, NVOptions* opt,
                               const char *fn, bool isList)
              :     Controller(ct, opt, fn, isList)
{
  //cout << "\tEnter function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;

  //cout << "\tfilename: <" << fn << ">, isList = " << isList << endl;

    _varname = "HGT";

    _initialized = false;
    drawWindVector = false;

    _preFile = -1;

    _value = NULL;

    ua = NULL;
    va = NULL;
    wa = NULL;
    ha = NULL;

    u1 = NULL;
    v1 = NULL;
    w1 = NULL;
    h1 = NULL;

    u2 = NULL;
    v2 = NULL;
    w2 = NULL;
    h2 = NULL;

    lon = NULL;
    lat = NULL;

    lic = NULL;
    windvector = NULL;
    trajectory = NULL;

  //cout << "\tfunction: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;

    marchingCube.set_colorTable(ct);
    marchingCube.set_nvoptions(opt);

  //cout << "\tfunction: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;

    wrf_nclviewer = NULL;
    wrf_glviewer = NULL;

    wrf_geometry = new WRF_Geometry();

  //cout << "\tfunction: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;

    nvfile = new NVFile(fn, isList, true);

  //cout << "\tLeave function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
}

WRF_Controller::~WRF_Controller()
{
  //cout << "\tEnter function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;

    if(NULL != ua)
        free(ua);
    if(NULL != va)
        free(va);
    if(NULL != wa)
        free(wa);
    if(NULL != ha)
        free(ha);

    if(NULL != u1)
        free(u1);
    if(NULL != v1)
        free(v1);
    if(NULL != w1)
        free(w1);
    if(NULL != h1)
        free(h1);

    if(NULL != lon)
        free(lon);
    if(NULL != lat)
        free(lat);

    if(NULL != lic)
        delete lic;
    if(NULL != windvector)
        delete windvector;
    if(NULL != trajectory)
        delete trajectory;
    if(NULL != wrf_geometry)
        delete wrf_geometry;
    if(NULL != nvfile)
        delete nvfile;
    if(NULL != wrf_nclviewer)
        delete wrf_nclviewer;
    if(NULL != wrf_glviewer)
        delete wrf_glviewer;

  //cout << "\tLeave function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
} 

void WRF_Controller::setup()
{
    size_t i;
    size_t xy_size;
    size_t gridsize;
    float* ph = NULL;
    float* phb = NULL;

    _grdsize = nvfile->get_grdsize();
    _ntimes = nvfile->get_ntimes();

    _setup4wrf();

    wrf_geometry->set_hasFillValue(false);

    _mx = nvfile->get_dim_size("west_east_stag");
    _my = nvfile->get_dim_size("south_north_stag");
    _mz = nvfile->get_dim_size("bottom_top_stag");

    wrf_geometry->set_mx(_mx);
    wrf_geometry->set_my(_my);
    wrf_geometry->set_mz(_mz);

    _nx = nvfile->get_dim_size("west_east");
    _ny = nvfile->get_dim_size("south_north");
    _nz = nvfile->get_dim_size("bottom_top");
    _nt = nvfile->get_dim_size("Time");

    wrf_geometry->set_nx(_nx);
    wrf_geometry->set_ny(_ny);
    wrf_geometry->set_nz(_nz);
    wrf_geometry->set_nt(_nt);

    wrf_geometry->set_lon(nvfile->get_dv("XLONG"));
    wrf_geometry->set_lat(nvfile->get_dv("XLAT"));
    wrf_geometry->set_ulon(nvfile->get_dv("XLONG_U"));
    wrf_geometry->set_ulat(nvfile->get_dv("XLAT_U"));
    wrf_geometry->set_vlon(nvfile->get_dv("XLONG_V"));
    wrf_geometry->set_vlat(nvfile->get_dv("XLAT_V"));

    xy_size = _nx * _ny;
    gridsize = xy_size * _nz;
    ph  = nvfile->get_fv("PH");
    phb = nvfile->get_fv("PHB");

    for(i = 0; i < gridsize; ++i)
        ph[i] = 0.5*(phb[i] + ph[i] + phb[i + xy_size] + ph[i + xy_size]);

    wrf_geometry->set_hgt(ph);

    free(phb);

    wrf_geometry->unset_Xstaggered();
    wrf_geometry->unset_Ystaggered();
    wrf_geometry->unset_Zstaggered();

    _varname = "HGT";
    _value = nvfile->get_fv(_varname);
    _title = _varname;
    _setup_wrf_timestring();

    _varsize = nvfile->get_varsize();
    wrf_geometry->set_nx(_varsize[2]);
    wrf_geometry->set_ny(_varsize[1]);
    wrf_geometry->set_nz(_varsize[0]);

    _nx = wrf_geometry->get_nx();
    _ny = wrf_geometry->get_ny();
    _nz = wrf_geometry->get_nz();

  //We need to setup 2 viewers, no matter which one is currently used.
    wrf_nclviewer = new WRF_NCL_Viewer(colorTable, nvoptions);
    wrf_nclviewer->set_geometry(wrf_geometry);
    wrf_nclviewer->setup(_varname, _value);

    wrf_glviewer = new WRF_GL_Viewer(colorTable, nvoptions);
    wrf_glviewer->set_geometry(wrf_geometry);
    wrf_glviewer->setup(_varname, _value);

    _minval = wrf_glviewer->get_minval();
    _maxval = wrf_glviewer->get_maxval();

    _initialized = true;

    geometry = wrf_geometry;
}

void WRF_Controller::set2dvarname(string vn)
{
    _varname = vn;

    if(_initialized)
        free(_value);

    wrf_geometry->unset_Xstaggered();
    wrf_geometry->unset_Ystaggered();
    wrf_geometry->unset_Zstaggered();

    _value = nvfile->get_fv(_varname);
    _title = nvfile->get_title();

    _varsize = nvfile->get_varsize();
    wrf_geometry->set_nx(_varsize[2]);
    wrf_geometry->set_ny(_varsize[1]);
    wrf_geometry->set_nz(1);

    _nx = wrf_geometry->get_nx();
    _ny = wrf_geometry->get_ny();
    _nz = wrf_geometry->get_nz();

  //cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">, line: "
  //     << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
  //cout << "\tvariable name: <" << _varname << ">" << endl;
  //cout << "\ttitle: <" << _title << ">" << endl;
  //cout << "\twrf_geometry->get_mx() = " << wrf_geometry->get_mx() << endl;
  //cout << "\twrf_geometry->get_my() = " << wrf_geometry->get_my() << endl;
  //cout << "\twrf_geometry->get_mz() = " << wrf_geometry->get_mz() << endl;
  //cout << "\twrf_geometry->get_nx() = " << wrf_geometry->get_nx() << endl;
  //cout << "\twrf_geometry->get_ny() = " << wrf_geometry->get_ny() << endl;
  //cout << "\twrf_geometry->get_nz() = " << wrf_geometry->get_nz() << endl;

    _initialized = true;

    if(wrf_geometry->get_nx() == _mx)
        wrf_geometry->set_Xstaggered();
    if(wrf_geometry->get_ny() == _my)
        wrf_geometry->set_Ystaggered();

    wrf_nclviewer->set_geometry(wrf_geometry);
    wrf_nclviewer->setup(vn, _value);

    wrf_glviewer->setup(vn, _value);
    wrf_glviewer->setup(_varname, _value);

    _minval = wrf_glviewer->get_minval();
    _maxval = wrf_glviewer->get_maxval();
}

void WRF_Controller::set3dvarname(string vn)
{
    _varname = vn;

    if(_initialized)
        free(_value);

    wrf_geometry->unset_Xstaggered();
    wrf_geometry->unset_Ystaggered();
    wrf_geometry->unset_Zstaggered();

    _value = nvfile->get_fv(_varname);
    _title = nvfile->get_title();

    _varsize = nvfile->get_varsize();
    wrf_geometry->set_nx(_varsize[2]);
    wrf_geometry->set_ny(_varsize[1]);
    wrf_geometry->set_nz(_varsize[0]);

    _nx = _varsize[2];
    _ny = _varsize[1];
    _nz = _varsize[0];

  //cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">, line: "
  //     << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
  //cout << "\tvariable name: <" << _varname << ">" << endl;
  //cout << "\ttitle: <" << _title << ">" << endl;
  //cout << "\twrf_geometry->get_nx() = " << wrf_geometry->get_nx() << endl;
  //cout << "\twrf_geometry->get_ny() = " << wrf_geometry->get_ny() << endl;
  //cout << "\twrf_geometry->get_nz() = " << wrf_geometry->get_nz() << endl;

    _initialized = true;

    if(wrf_geometry->get_nx() == _mx)
        wrf_geometry->set_Xstaggered();
    if(wrf_geometry->get_ny() == _my)
        wrf_geometry->set_Ystaggered();
    if(wrf_geometry->get_nz() == _mz)
        wrf_geometry->set_Zstaggered();

    wrf_nclviewer->set_geometry(wrf_geometry);
    wrf_nclviewer->setup(vn, _value);

    wrf_glviewer->setup(vn, _value);
    wrf_glviewer->setup(_varname, _value);

    _minval = wrf_glviewer->get_minval();
    _maxval = wrf_glviewer->get_maxval();

    lon = nvfile->get_dv(string("XLONG"));
    lat = nvfile->get_dv(string("XLAT"));

    if(nvoptions->get_cb(NV_ISOSURFACEON))
    {
        marchingCube.reset();
      //marchingCube.setup(wrf_geometry->get_nx(), wrf_geometry->get_ny(), wrf_geometry->get_nz(),
      //                  &_value[0], _minval, _maxval);
    }
}

void WRF_Controller::_setup4wrf()
{
    int n;

    string attname;
    string attvalue;

    int iv;
    float fv;

    int natts = nvfile->get_natts();
 
  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\tnatts = " << natts << endl;

    for(n = 0; n < natts; ++n)
    {
        attvalue = nvfile->get_att_value("TITLE");
      //if(attvalue.compare("unknown"))
      //    wrf_geometry->set_model(WRF);

        attvalue = nvfile->get_att_value("MMINLU");
        if(0 == attvalue.compare("unknown"))
             wrf_geometry->set_mminlu(attvalue.c_str());

        attvalue = nvfile->get_att_value("WEST-EAST_GRID_DIMENSION");
        if(attvalue.compare("unknown"))
        {
            iv = string2number<int>(attvalue);
            _nx = iv - 1;
        }

        attvalue = nvfile->get_att_value("SOUTH-NORTH_GRID_DIMENSION");
        if(attvalue.compare("unknown"))
        {
            iv = string2number<int>(attvalue);
            _ny = iv - 1;
        }

        attvalue = nvfile->get_att_value("BOTTOM-TOP_GRID_DIMENSION");
        if(attvalue.compare("unknown"))
        {
            iv = string2number<int>(attvalue);
            _nz = iv - 1;
        }

        attvalue = nvfile->get_att_value("GRID_ID");
        if(attvalue.compare("unknown"))
        {
            iv = string2number<int>(attvalue);
            wrf_geometry->set_grid_id(iv);
        }

        attvalue = nvfile->get_att_value("PARENT_ID");
        if(attvalue.compare("unknown"))
        {
            iv = string2number<int>(attvalue);
            wrf_geometry->set_parent_id(iv);
        }

        attvalue = nvfile->get_att_value("PARENT_GRID_RATIO");
        if(attvalue.compare("unknown"))
        {
            iv = string2number<int>(attvalue);
            wrf_geometry->set_parent_grid_ratio(iv);
        }

        attvalue = nvfile->get_att_value("I_PARENT_START");
        if(attvalue.compare("unknown"))
        {
            iv = string2number<int>(attvalue);
            wrf_geometry->set_i_parent_start(iv);
        }

        attvalue = nvfile->get_att_value("J_PARENT_START");
        if(attvalue.compare("unknown"))
        {
            iv = string2number<int>(attvalue);
            wrf_geometry->set_j_parent_start(iv);
        }

        attvalue = nvfile->get_att_value("MAP_PROJ");
        if(attvalue.compare("unknown"))
        {
            iv = string2number<int>(attvalue);
            wrf_geometry->set_map_proj(iv);
        }

        attvalue = nvfile->get_att_value("JULYR");
        if(attvalue.compare("unknown"))
        {
            iv = string2number<int>(attvalue);
            wrf_geometry->set_julyr(iv);
        }

        attvalue = nvfile->get_att_value("JULDAY");
        if(attvalue.compare("unknown"))
        {
            iv = string2number<int>(attvalue);
            wrf_geometry->set_julday(iv);
        }

        attvalue = nvfile->get_att_value("NUM_LAND_CAT");
        if(attvalue.compare("unknown"))
        {
            iv = string2number<int>(attvalue);
            wrf_geometry->set_num_land_cat(iv);
        }

        attvalue = nvfile->get_att_value("ISWATER");
        if(attvalue.compare("unknown"))
        {
            iv = string2number<int>(attvalue);
            wrf_geometry->set_iswater(iv);
        }

        attvalue = nvfile->get_att_value("ISLAKE");
        if(attvalue.compare("unknown"))
        {
            iv = string2number<int>(attvalue);
            wrf_geometry->set_islake(iv);
        }

        attvalue = nvfile->get_att_value("ISICE");
        if(attvalue.compare("unknown"))
        {
            iv = string2number<int>(attvalue);
            wrf_geometry->set_isice(iv);
        }

        attvalue = nvfile->get_att_value("ISURBAN");
        if(attvalue.compare("unknown"))
        {
            iv = string2number<int>(attvalue);
            wrf_geometry->set_isurban(iv);
        }

        attvalue = nvfile->get_att_value("ISOILWATER");
        if(attvalue.compare("unknown"))
        {
            iv = string2number<int>(attvalue);
            wrf_geometry->set_issoilwater(iv);
        }

        attvalue = nvfile->get_att_value("DX");
        if(attvalue.compare("unknown"))
        {
            fv = string2number<float>(attvalue);
            wrf_geometry->set_dx(fv);
        }

        attvalue = nvfile->get_att_value("DY");
        if(attvalue.compare("unknown"))
        {
            fv = string2number<float>(attvalue);
            wrf_geometry->set_dy(fv);
        }

        attvalue = nvfile->get_att_value("DT");
        if(attvalue.compare("unknown"))
        {
            fv = string2number<float>(attvalue);
            wrf_geometry->set_dt(fv);
        }

        attvalue = nvfile->get_att_value("CEN_LAT");
        if(attvalue.compare("unknown"))
        {
            fv = string2number<float>(attvalue);
            wrf_geometry->set_cen_lat(fv);
        }

        attvalue = nvfile->get_att_value("CEN_LON");
        if(attvalue.compare("unknown"))
        {
            fv = string2number<float>(attvalue);
            wrf_geometry->set_cen_lon(fv);
        }

        attvalue = nvfile->get_att_value("TRUELAT1");
        if(attvalue.compare("unknown"))
        {
            fv = string2number<float>(attvalue);
            wrf_geometry->set_truelat1(fv);
        }

        attvalue = nvfile->get_att_value("TRUELAT2");
        if(attvalue.compare("unknown"))
        {
            fv = string2number<float>(attvalue);
            wrf_geometry->set_truelat2(fv);
        }

        attvalue = nvfile->get_att_value("MOAD_CEN_LAT");
        if(attvalue.compare("unknown"))
        {
            fv = string2number<float>(attvalue);
            wrf_geometry->set_moad_cen_lat(fv);
        }

        attvalue = nvfile->get_att_value("STAND_LON");
        if(attvalue.compare("unknown"))
        {
            fv = string2number<float>(attvalue);
            wrf_geometry->set_stand_lon(fv);
        }

        attvalue = nvfile->get_att_value("POLE_LAT");
        if(attvalue.compare("unknown"))
        {
            fv = string2number<float>(attvalue);
            wrf_geometry->set_pole_lat(fv);
        }

        attvalue = nvfile->get_att_value("POLE_LON");
        if(attvalue.compare("unknown"))
        {
            fv = string2number<float>(attvalue);
            wrf_geometry->set_pole_lon(fv);
        }

        attvalue = nvfile->get_att_value("GMT");
        if(attvalue.compare("unknown"))
        {
            fv = string2number<float>(attvalue);
            wrf_geometry->set_gmt(fv);
        }
    }
}

void WRF_Controller::set_fileNtime(int nf, int nt)
{
    size_t gridsize = 1;

  //cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
  //cout << "\tcurFile: " << nf << ", curTime: " << nt << ", varname: <" << _varname << ">" << endl;

  //cout << "\tnvoptions->get_xsec() = " << nvoptions->get_xsec() << endl;
  //cout << "\tnvoptions->get_ysec() = " << nvoptions->get_ysec() << endl;
  //cout << "\tnvoptions->get_zsec() = " << nvoptions->get_zsec() << endl;

    _curFile = nf;
    _curTime = nt;

    if(_preFile != _curFile)
    {
        _preFile = _curFile;
        _preTime = -1;

        if(_initialized)
            free(_value);

        nvfile->select_file(nf);

        _value = nvfile->get_fv(_varname);
        _title = nvfile->get_title();
        _nt = _ntimes[_curFile];
        _setup_wrf_timestring();

        _varsize = nvfile->get_varsize();
        wrf_geometry->set_nx(_varsize[2]);
        wrf_geometry->set_ny(_varsize[1]);
        wrf_geometry->set_nz(_varsize[0]);

        wrf_geometry->set_nt(_ntimes[_curFile]);

        _minval = wrf_glviewer->get_minval();
        _maxval = wrf_glviewer->get_maxval();

        if(nvoptions->get_cb(NV_VECTORON) || nvoptions->get_cb(NV_TRAJECTORY_ON))
        {
          //cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
          //cout << "\tcurFile: " << nf << ", curTime: " << _curTime << ", varname: <" << _varname << ">" << endl;

            windvector->setup(nxs, nys, nzs, ua, va, wa);
            _get_vector(_curTime);
          //unset_vector();
            setup_vector();
        }

        gridsize = _curTime * geometry->get_nx() * geometry->get_ny() * geometry->get_nz();

        wrf_nclviewer->setup(_varname, &_value[gridsize]);
        wrf_glviewer->setup(_varname, &_value[gridsize]);

        _initialized = true;
        _preTime = _curTime;
    }

  //cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
  //cout << "\tcurFile: " << nf << ", curTime: " << nt << ", varname: <" << _varname << ">" << endl;

  //cout << "\tnvoptions->get_xsec() = " << nvoptions->get_xsec() << endl;
  //cout << "\tnvoptions->get_ysec() = " << nvoptions->get_ysec() << endl;
  //cout << "\tnvoptions->get_zsec() = " << nvoptions->get_zsec() << endl;
}

void WRF_Controller::draw()
{
  //cout << "\n\tEnter functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << endl;
  //cout << "\tnvoptions->get_xsec() = " << nvoptions->get_xsec() << endl;
  //cout << "\tnvoptions->get_ysec() = " << nvoptions->get_ysec() << endl;
  //cout << "\tnvoptions->get_zsec() = " << nvoptions->get_zsec() << endl;

    if(nvoptions->get_cb(NV_ISOSURFACEON))
    {
        if(nvoptions->get_cb(NV_CLIP_ON))
        {
            GLdouble eqn0[4] = {1.0, 0.0, 0.0, 0.0};
            GLdouble eqn1[4] = {0.0, 1.0, 0.0, 0.0};
            GLdouble eqn2[4] = {0.0, 0.0, 1.0, 0.0};

            glPushMatrix();
              //clip lower half -- x < 0
                glClipPlane(GL_CLIP_PLANE0, eqn0);
                glEnable(GL_CLIP_PLANE0);
            glPopMatrix();

            glPushMatrix();
              //clip left half -- y < 0
                glClipPlane(GL_CLIP_PLANE1, eqn1);
                glEnable(GL_CLIP_PLANE1);
            glPopMatrix();

            glPushMatrix();
              //clip left half -- z < 0
                glClipPlane(GL_CLIP_PLANE2, eqn2);
                glEnable(GL_CLIP_PLANE2);
            glPopMatrix();
        }
        else
        {
            glPushMatrix();
                glDisable(GL_CLIP_PLANE0);
                glDisable(GL_CLIP_PLANE1);
                glDisable(GL_CLIP_PLANE2);
            glPopMatrix();
        }

        draw_isosurface();

        if(! nvoptions->get_cb(NV_ISOSURFACEONLY))
        {
            if(nvoptions->get_cb(NV_USENCL))
                wrf_nclviewer->draw();
            else
                wrf_glviewer->draw();
        }
    }
    else if(nvoptions->get_cb(NV_LICON))
    {
      //cout << "before lic at line: " << __LINE__ << endl;

        draw_lic();

        if(nvoptions->get_cb(NV_LICONLY))
            _title = "LIC";
        else if(nvoptions->get_cb(NV_USENCL))
            wrf_nclviewer->draw();
        else
            wrf_glviewer->draw();

      //cout << "after lic at line: " << __LINE__ << endl;
    }
    else if(nvoptions->get_cb(NV_VECTORON))
    {
        draw_vector();

        if(nvoptions->get_cb(NV_VECTORONLY))
            _title = "VECTOR";
        else if(nvoptions->get_cb(NV_USENCL))
            wrf_nclviewer->draw();
        else
            wrf_glviewer->draw();
    }
    else if(nvoptions->get_cb(NV_TRAJECTORY_ON))
    {
        draw_trajectory();

        if(nvoptions->get_cb(NV_TRAJECTORY_ONLY))
            _title = "TRAJECTORY";
        else if(nvoptions->get_cb(NV_USENCL))
            wrf_nclviewer->draw();
        else
            wrf_glviewer->draw();
    }
    else if(nvoptions->get_cb(NV_USENCL))
        wrf_nclviewer->draw();
    else
        wrf_glviewer->draw();

  //cout << "\tnvoptions->get_xsec() = " << nvoptions->get_xsec() << endl;
  //cout << "\tnvoptions->get_ysec() = " << nvoptions->get_ysec() << endl;
  //cout << "\tnvoptions->get_zsec() = " << nvoptions->get_zsec() << endl;
  //cout << "\tLeave functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << endl << endl;
}

void WRF_Controller::update_colormap()
{
    wrf_nclviewer->update_colormap();
    wrf_glviewer->update_colormap();

    if(nvoptions->get_cb(NV_ISOSURFACEON))
        marchingCube.reset();
}

void WRF_Controller::draw_isosurface()
{
    size_t gridsize = _curTime * geometry->get_nx() * geometry->get_ny() * geometry->get_nz();

  //cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__;
  //cout << ", file: <" << __FILE__ << ">" << endl;
  //cout << "\t_curTime = " << _curTime << ", gridsize = " << gridsize << endl;

    if(nvoptions->get_cb(NV_ISOSURFACEON))
    {
      //marchingCube.set_colorTable(colorTable);
    
        if((marchingCube.get_curtime() != _curTime) || _varname.compare(marchingCube.get_varname()))
        {
            marchingCube.set_curtime(_curTime);
            marchingCube.set_varname(_varname);

          //cout << "\nIn functions: <" << __PRETTY_FUNCTION__;
          //cout << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
          //cout << "\t_minval = " << _minval << ", _maxval = " << _maxval << endl;

	    double vg = _value[gridsize];
            marchingCube.setup(wrf_geometry->get_nx(), wrf_geometry->get_ny(), wrf_geometry->get_nz(),
                               &vg, _minval, _maxval);
        }

        marchingCube.display();
    }
}

void WRF_Controller::draw_vector()
{
  //cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">, line: "
  //     << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
  //cout << "\tdrawWindVector = " << drawWindVector << endl;

    if(nvoptions->get_cb(NV_VECTORON))
    {
        if(NULL == windvector)
        {
            windvector = new WindVector(colorTable, nvoptions);
            windvector->set_stepsize(5);
            windvector->setup_position(lon, lat);
        }

        if(! drawWindVector)
        {
            setup_vector();

            windvector->setup(nxs, nys, nzs, ua, va, wa);
            windvector->set_maxspeed(maxspd);

            windvector->set_colorTable(colorTable);
        }

        windvector->draw();

        drawWindVector = true;
    }
    else
    {
        if(drawWindVector)
            unset_vector();
    }
}

void WRF_Controller::draw_trajectory()
{
  //cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">, line: "
  //     << __LINE__ << ", file: <" << __FILE__ << ">" << endl;

    if(nvoptions->get_cb(NV_TRAJECTORY_ON))
    {  
        if(NULL == trajectory)
        {
            trajectory = new Trajectory(colorTable, nvoptions);
            trajectory->setup_position(lon, lat);
            trajectory->set_dtNdx(wrf_geometry->get_dt(), wrf_geometry->get_dx());
            trajectory->set_tspan(0.0, 360.0);
        }

        if(! drawWindVector)
        {
            setup_vector();

            trajectory->setup(nxs, nys, nzs, ua, va, wa, ha);
        }

        trajectory->draw();
    }
}

void WRF_Controller::draw_lic()
{
    if(nvoptions->get_cb(NV_LICON))
    {
        if(NULL == lic)
            lic = new LineIntegralConvolution(colorTable, nvoptions);

        if(! drawWindVector)
        {
            setup_vector();
            lic->setup(nxs, nys, nzs, ua, va, wa);
            lic->set_maxvalue(maxspd);
        }

        lic->set_colorTable(colorTable);
        lic->draw();
    }
    else
    {
        if(drawWindVector)
            unset_vector();
    }
}

void WRF_Controller::_setup_wrf_timestring()
{
    int n = 0;
    int ns = 0;
    int dsl = nvfile->get_dim_size("DateStrLen");
    int wtsl = wrf_timestring.size();
    string tstr = nvfile->get_sv("Times");
    string nstr;
    char* cstr = new char[_nt * dsl + 1];
    char* item = new char[dsl + 1];

  //cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">, line: "
  //     << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
  //cout << "\ttstr = " << tstr << endl;

    memset(item, 0, dsl+1);
    strcpy(cstr, tstr.c_str());

    ns = 0;
    if(wtsl < _nt)
    {
        for(n = 0; n < wtsl; ++n)
        {
            memcpy(item, cstr+ns, dsl);
            ns += dsl;

          //nstr = "Time: " + number2string<int>(n) + ", ";
            nstr = ", ";

            wrf_timestring[n] = nstr + item;
        }

        for(n = wtsl; n < _nt; ++n)
        {
            memcpy(item, cstr+ns, dsl);
            ns += dsl;

          //nstr = "Time: " + number2string<int>(n) + ", " + item;
            nstr = string(", ") + item;
            wrf_timestring.push_back(nstr);

          //cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">, line: "
          //     << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
          //cout << "\twrf_timestring[" << n << "] = " << wrf_timestring[n] << endl;
        }
    }
    else
    {
        for(n = 0; n < wtsl; ++n)
        {
            memcpy(item, cstr+ns, dsl);
            ns += dsl;

          //wrf_timestring[n] = "Time: " + number2string<int>(n) + ", " + item;
            wrf_timestring[n] = string(", ") + item;
        }
    }

    delete [] cstr;
}

string* WRF_Controller::get_timestring()
{
    int n = 0;
    string* timestring = nvfile->get_timestr();

    for(n = 0; n < _nt; ++n)
    {
        timestring[n] += wrf_timestring[n];

      //cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">, line: "
      //     << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
      //cout << "\ttimestring[" << n << "] = " << timestring[n] << endl;
    }

    return timestring;
}

template<typename T>
T string2number(string sn)
{
    T value;

    stringstream stream(sn);
    stream >> value;
    if(stream.fail())
    {
        cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
        cout << "\tFailed convert " << sn << " to T." << endl;
        exit (-1);
    }

    return value;
}

template<typename T>
string number2string(T n)
{
    string value;

    stringstream stream;
    stream << n;
    if(stream.fail())
    {
        cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
        cout << "\tFailed convert " << n << " to string." << endl;
        exit (-1);
    }

    value = stream.str();
    return value;
}

void WRF_Controller::unset_vector()
{
    drawWindVector = false;
    if(NULL != ua)
        free(ua);
    if(NULL != va)
        free(va);
    if(NULL != wa)
        free(wa);
    if(NULL != ha)
        free(ha);
}

void WRF_Controller::setup_vector()
{
    size_t i;
    size_t xy_size;
    size_t gridsize;
    float* ph = NULL;
    float* phb = NULL;

    drawWindVector = true;

    u1 = nvfile->get_fv(string("U"));
    _varsize = nvfile->get_varsize();
    nxp = _varsize[2];
    nys = _varsize[1];
    nzs = _varsize[0];

    v1 = nvfile->get_fv(string("V"));
    _varsize = nvfile->get_varsize();
    nxs = _varsize[2];
    nyp = _varsize[1];
  //nzs = _varsize[0];

    w1 = nvfile->get_fv(string("W"));
    _varsize = nvfile->get_varsize();
  //nxs = _varsize[2];
  //nys = _varsize[1];
    nzp = _varsize[0];

    xy_size = nxs * nys;
    gridsize = xy_size * nzs;
    ph  = nvfile->get_fv(string("PH"));
    phb = nvfile->get_fv(string("PHB"));

    ua = (float *)malloc(gridsize * sizeof(float));
    va = (float *)malloc(gridsize * sizeof(float));
    wa = (float *)malloc(gridsize * sizeof(float));
    ha = (float *)malloc(gridsize * sizeof(float));

    h1 = (float *)malloc((xy_size + gridsize) * _nt * sizeof(float));

    for(i = 0; i < _nt*(xy_size + gridsize); ++i)
    {
        h1[i] = 0.5*(phb[i] + ph[i] + phb[i + xy_size] + ph[i + xy_size])/9.80;
    }

    free(ph);
    free(phb);

    _get_vector(0);
}

void WRF_Controller::_get_vector(int nt)
{
    int i, j, k;
    size_t n, nu, nv, nw;
    size_t nu_start, nv_start, nw_start;

    float spd = 0.0;

    nu_start = nt * nxp * nys * nzs;
    nv_start = nt * nxs * nyp * nzs;
    nw_start = nt * nxs * nys * nzp;

    n = 0;
    for(k = 0; k < nzs; ++k)
    {
        for(j = 0; j < nys; ++j)
        {
            nu = nu_start + (k * nys + j) * nxp;
            nv = nv_start + (k * nyp + j) * nxs;
            nw = nw_start + (k * nys + j) * nxs;

            for(i = 0; i < nxs; ++i)
            {
                ua[n] = 0.5 * (u1[nu + i] + u1[nu + i + 1]);
                va[n] = 0.5 * (v1[nv + i] + v1[nv + i + nxs]);
                wa[n] = 0.5 * (w1[nw + i] + w1[nw + i + nxs*nys]);
                ha[n] = 0.5 * (h1[nw + i] + h1[nw + i + nxs*nys]);

                spd = sqrt(ua[n]*ua[n] + va[n]*va[n] + wa[n]*wa[n]);
                if(spd > maxspd)
                    maxspd = spd;
                ++n;
            }
        }
    }
}

void WRF_Controller::set_locator(Locator* l)
{
     locator = l;

     wrf_glviewer->set_locator(l);
}

int WRF_Controller::get_ndv(int n)
{
    return nvfile->get_ndv(n);
}

string* WRF_Controller::get_ndvNames(int n)
{
    return nvfile->get_ndvNames(n);
}

