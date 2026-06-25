#include "eaglelam_controller.h"

template<typename T>
T string2number(string sn);

template<typename T>
string number2string(T n);

EAGLELAM_Controller::EAGLELAM_Controller(ColorTable *ct, NVOptions* opt,
                                         vector<string> vecdfs)
{
  //cout << "\tEnter function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;

  //cout << "\tfilename: <" << fn << ">, isList = " << isList << endl;

    colorTable = ct;
    nvoptions = opt;
    datafiles = vecdfs;
    _numvars = datafiles.size();

    drawWindVector = false;

    _value = NULL;
    u = NULL;
    v = NULL;

    lon = NULL;
    lat = NULL;

    lic = NULL;
    windvector = NULL;
    trajectory = NULL;

  //cout << "\tfunction: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;

    eaglelam_viewer = NULL;

    geometry = new EAGLELAM_Geometry();
    // coastline = new CoastLine();

    ncfile = new EagleReader(datafiles[0].c_str());

    set<string> nc1dvars = {"forecast_reference_time", "time",
                            "latitude", "longitude", "x", "y", "CRS"};

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

    _nx = ncfile->getNx();
    _ny = ncfile->getNy();
    _nz = 1;
    _nt = 1;

  //cout << "\tLeave function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
}

EAGLELAM_Controller::~EAGLELAM_Controller()
{
  //cout << "\tEnter function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;

    if(NULL != u)
        free(u);
    if(NULL != v)
        free(v);
    if(NULL != _value)
        free(_value);

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
    if(NULL != geometry)
        delete geometry;
    if(NULL != ncfile)
        delete ncfile;
    if(NULL != eaglelam_viewer)
        delete eaglelam_viewer;

  //cout << "\tLeave function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
} 

void EAGLELAM_Controller::setup()
{
    size_t i, n;

    set<string> nc1dvars = {"forecast_reference_time", "time",
                            "latitude", "longitude", "x", "y", "CRS"};

    vector<EagleReader*> nchandler(datafiles.size());
    varlist.resize(datafiles.size());

    for (i = 0; i < datafiles.size(); ++i)
    {
        nchandler[i] = new EagleReader(datafiles[i].c_str());

        vector<string> varnames = nchandler[i]->getVarNames();

        for(n=0; n<varnames.size(); ++n)
        {
            if (nc1dvars.find(varnames[n]) == nc1dvars.end())
            {
                var2file[varnames[n]] = nchandler[i];
                varlist[i] = varnames[n];
                break;
            }
        }
    }

    _setup4eaglelam();

    _nx = ncfile->getNx();
    _ny = ncfile->getNy();
    _nz = 1;
    _nt = 1;

    geometry->set_nx(_nx);
    geometry->set_ny(_ny);
    geometry->set_nz(_nz);
    geometry->set_nt(_nt);

    lon = ncfile->get_fv("longitude");
    lat = ncfile->get_fv("latitude");
    geometry->set_longitude(lon);
    geometry->set_latitude(lat);

    _value = ncfile->get_fv(_varname.c_str());
    _title = "EAGLE LAM";
    // _setup_eaglelam_timestring();

    eaglelam_viewer = new EAGLELAM_Viewer(colorTable, nvoptions);
    eaglelam_viewer->set_geometry(geometry);
    eaglelam_viewer->setup(_varname, _value);

    _valmin = eaglelam_viewer->get_minval();
    _valmax = eaglelam_viewer->get_maxval();

    geometry = geometry;
}

void EAGLELAM_Controller::setvarname(string vn)
{
    _varname = vn;

    _value = ncfile->get_fv(_varname.c_str());

    eaglelam_viewer->setup(vn, _value);
    eaglelam_viewer->setup(_varname, _value);

    _valmin = eaglelam_viewer->get_minval();
    _valmax = eaglelam_viewer->get_maxval();
}

void EAGLELAM_Controller::_setup4eaglelam()
{
    int n;

    string attname;
    string attvalue;

    int iv;
    float fv;

/*
        attvalue = ncfile->get_att_value("CEN_LAT");
        if(attvalue.compare("unknown"))
        {
            fv = string2number<float>(attvalue);
            geometry->set_cen_lat(fv);
        }

        attvalue = ncfile->get_att_value("CEN_LON");
        if(attvalue.compare("unknown"))
        {
            fv = string2number<float>(attvalue);
            geometry->set_cen_lon(fv);
        }

        attvalue = ncfile->get_att_value("TRUELAT1");
        if(attvalue.compare("unknown"))
        {
            fv = string2number<float>(attvalue);
            geometry->set_truelat1(fv);
        }

        attvalue = ncfile->get_att_value("TRUELAT2");
        if(attvalue.compare("unknown"))
        {
            fv = string2number<float>(attvalue);
            geometry->set_truelat2(fv);
        }
*/
}

void EAGLELAM_Controller::set_fileNtime(int nf, int nt)
{
    cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    cout << "\tcurFile: " << nf << ", curTime: " << nt << ", varname: <" << _varname << ">" << endl;
}

void EAGLELAM_Controller::draw()
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
            eaglelam_viewer->draw();
        }
    }
    else if(nvoptions->get_cb(NV_LICON))
    {
      //cout << "before lic at line: " << __LINE__ << endl;

        draw_lic();

        if(nvoptions->get_cb(NV_LICONLY))
            _title = "LIC";
        else
            eaglelam_viewer->draw();

      //cout << "after lic at line: " << __LINE__ << endl;
    }
    else if(nvoptions->get_cb(NV_VECTORON))
    {
        draw_vector();

        if(nvoptions->get_cb(NV_VECTORONLY))
            _title = "VECTOR";
        else
            eaglelam_viewer->draw();
    }
    else if(nvoptions->get_cb(NV_TRAJECTORY_ON))
    {
        draw_trajectory();

        if(nvoptions->get_cb(NV_TRAJECTORY_ONLY))
            _title = "TRAJECTORY";
        else
            eaglelam_viewer->draw();
    }
    else
        eaglelam_viewer->draw();

  //cout << "\tnvoptions->get_xsec() = " << nvoptions->get_xsec() << endl;
  //cout << "\tnvoptions->get_ysec() = " << nvoptions->get_ysec() << endl;
  //cout << "\tnvoptions->get_zsec() = " << nvoptions->get_zsec() << endl;
  //cout << "\tLeave functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << endl << endl;
}

void EAGLELAM_Controller::update_colormap()
{
    eaglelam_viewer->update_colormap();

    if(nvoptions->get_cb(NV_ISOSURFACEON))
        marchingCube.reset();
}

void EAGLELAM_Controller::draw_isosurface()
{
/*
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
          //cout << "\t_valmin = " << _valmin << ", _valmax = " << _valmax << endl;

            marchingCube.setup(geometry->get_nx(), geometry->get_ny(), geometry->get_nz(),
                               &_value[gridsize], _valmin, _valmax);
        }

        marchingCube.display();
    }
*/
}

void EAGLELAM_Controller::draw_vector()
{
    cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">, line: "
         << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    cout << "\tdrawWindVector = " << drawWindVector << endl;

#if 0
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
#endif
}

void EAGLELAM_Controller::draw_trajectory()
{
    cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">, line: "
         << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
#if 0
    if(nvoptions->get_cb(NV_TRAJECTORY_ON))
    {  
        if(NULL == trajectory)
        {
            trajectory = new Trajectory(colorTable, nvoptions);
            trajectory->setup_position(lon, lat);
            trajectory->set_dtNdx(geometry->get_dt(), geometry->get_dx());
            trajectory->set_tspan(0.0, 360.0);
        }

        if(! drawWindVector)
        {
            setup_vector();

            trajectory->setup(nxs, nys, nzs, ua, va, wa, ha);
        }

        trajectory->draw();
    }
#endif
}

void EAGLELAM_Controller::draw_lic()
{
    cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">, line: "
         << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
#if 0
    if(nvoptions->get_cb(NV_LICON))
    {
        if(NULL == lic)
            lic = new LineIntegralConvolution(colorTable, nvoptions);

        if(! drawWindVector)
        {
            setup_vector();
            lic->setup(nxs, nys, nzs, ua, va, wa);
            lic->set_valmaxue(maxspd);
        }

        lic->set_colorTable(colorTable);
        lic->draw();
    }
    else
    {
        if(drawWindVector)
            unset_vector();
    }
#endif
}

void EAGLELAM_Controller::_setup_eaglelam_timestring()
{
    string tstr = " ";
    eaglelam_timestring.push_back(tstr);
}

string* EAGLELAM_Controller::get_timestring()
{
    int n = 0;
    string* timestring = new string[eaglelam_timestring.size()];

    for(n = 0; n < eaglelam_timestring.size(); ++n)
    {
        timestring[n] = eaglelam_timestring[n];
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

void EAGLELAM_Controller::unset_vector()
{
    drawWindVector = false;
    if(NULL != u)
        free(u);
    if(NULL != v)
        free(v);
    if(NULL != _value)
        free(_value);
}

void EAGLELAM_Controller::setup_vector()
{
    size_t i;

    drawWindVector = true;

    u = ncfile->get_fv("u");
    v = ncfile->get_fv("v");

    _get_vector(0);
}

void EAGLELAM_Controller::_get_vector(int nt)
{
    int n;

    float spd = 0.0;

    for(n = 0; n < _nx*_ny; ++n)
    {
        spd = sqrt(u[n]*u[n] + v[n]*v[n]);
        if(spd > maxspd)
            maxspd = spd;
    }
}

void EAGLELAM_Controller::set_locator(Locator* l)
{
     locator = l;

     eaglelam_viewer->set_locator(l);
}

