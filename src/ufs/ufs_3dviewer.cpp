#include <QtOpenGL>

#include <vector>

#include "ufs_3dviewer.h"

UFS3dViewer::UFS3dViewer(ColorTable *ct, NVOptions* opt, const char* bmpflnm, ncReader* nchandler)
{
    colorTable = ct;
    nvoptions = opt;

    texture1d = new Texture1d();
    texture1d->set_colors(ct->get_clen(), ct->get_cmap());
    texture1d->set_name(ct->get_name());

    _var = NULL;

    ncfile = nchandler;
    earth = new Earth(bmpflnm, ncfile);

    nvoptions->set_xsec(0);
    nvoptions->set_ysec(0);
    nvoptions->set_zsec(0);

    _nlon = 360;
    _nlat = 180;

    oneover = 1.0 / 180.0;
    deg2rad = 3.1415926535897932 * oneover;

    lister = new Lister();
    lister->setup(361, 181, 121);

    locator = NULL;

    previoustimelevel = -1;
    current_timelevel = 0;
}

UFS3dViewer::~UFS3dViewer()
{
    locator->turnOff();

    delete earth;
    delete lister;
    delete texture1d;
}

void UFS3dViewer::set_geometry(UFSGeometry *gm)
{
    geometry = gm;

    _initialize();
}

void UFS3dViewer::setup(string vn, float *var)
{
    reset();

    _varname  = vn;
    _var = var;
    _nlev = geometry->get_nlev();

  //nvoptions->set_xsec(_nlon);
  //nvoptions->set_ysec(_nlat);
  //nvoptions->set_zsec(_nlev);

    nvoptions->set_xsec(0);
    nvoptions->set_ysec(0);
    nvoptions->set_zsec(0);

    _evaluate(_var);

    previoustimelevel = -1;
}

void UFS3dViewer::reset()
{
    lister->reinitialize(_nlon+1, _nlat+1, geometry->get_nlev()+1);
}

void UFS3dViewer::_initialize()
{
    int i, j, m, n;

    int positive;
    int negative;

    previoustimelevel = -1;

    _hlon = geometry->get_hlon();
    _nlon = geometry->get_nlon();
    _nlat = geometry->get_nlat();
    _nlev = geometry->get_nlev();

    _lon = geometry->get_lon();
    _lat = geometry->get_lat();
    _lev = geometry->get_lev();

  //lister->reinitialize(361, 181, _nlev);

    _xFlat = geometry->get_xFlat();
    _yFlat = geometry->get_yFlat();

    geometry->set_ntim(1);
}

void UFS3dViewer::draw()
{
    size_t nsquare = _nlon * _nlat;

    if(nvoptions->get_cb(NV_RESET))
    {
        nvoptions->set_cb(NV_RESET, false);
        if(nvoptions->get_cb(NV_BUMPON))
            texture1d->set_gradient_opacity();
        else
            texture1d->set_opacity(1.0);
        reset();
    }

    if(nvoptions->get_cb(NV_STATUS_CHANGED))
        reset();

    current_timelevel = nvoptions->get_tsec();
    if(current_timelevel != previoustimelevel)
        reset();
    previoustimelevel = current_timelevel;
    if(current_timelevel >= geometry->get_nt())
        return;

    if((geometry->get_nlev() <= nvoptions->get_zsec()) && (0 > nvoptions->get_zsec()))
        return;

#if 0
    if(nvoptions->get_cb(NV_HASMINMAX))
    {
        _valmin = nvoptions->get_userminimum();
        _valmax = nvoptions->get_usermaximum();
    }
    else
    {
        _valmin = nvoptions->get_trueminimum();
        _valmax = nvoptions->get_truemaximum();
    }
#endif
  //cout << "\t" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\t" <<"current_timelevel: " << current_timelevel << endl;
  //cout << "\t" <<"_nlev: " << _nlev << endl;

  //pltvar = &_var[current_timelevel * _nlon * _nlat];
    pltvar = &_var[current_timelevel * _nlon * _nlat * _nlev];

  //cout << "\t" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
    zcl = lister->get_zid(nvoptions->get_zsec());
    ycl = lister->get_yid(nvoptions->get_ysec());
    xcl = lister->get_xid(nvoptions->get_xsec());

  //makeCurrent();
  //Clear screen and Z-buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  //glClearColor(0.0, 0.0, 0.0, 0.0);

    if(nvoptions->get_cb(NV_BUMPON))
    {
  //cout << "\t" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
        if(nvoptions->get_cb(NV_FLATON))
        {
            if(zcl)
                glCallList(zcl);
            else
                _flatBump();
        }
        else
        {
            if(zcl)
                glCallList(zcl);
            else
                _sphereBump();
        }
    }
    else
    {
        if(nvoptions->get_cb(NV_FLATON))
        {
            if(nvoptions->get_zsec() < _nlev)
            {
              //cout << "\t" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
              //cout << "\t call  _flatDisplay()" << endl;
                if(zcl)
                    glCallList(zcl);
                else
                    _flatDisplay();
            }

          //draw_plane_grids();
        }
        else
        {
            if(nvoptions->get_zsec() < _nlev)
            {
                if(zcl)
                    glCallList(zcl);
                else
                    _sphereDisplay();
            }
          //draw_sphere_grids();
        }
    }
}

void UFS3dViewer::_lonlat2xyz(double lon, double lat, double radius, double fact)
{
    double phi = lat * deg2rad;
    double dist = cos(phi);
    double lamda = lon * deg2rad;

    double x = dist * sin(lamda);
    double z = dist * cos(lamda);
    double y = sin(phi);

    double alpha = 1.05 * fact;
    if(alpha < 0.1)
        alpha = 0.0;
    else if(alpha > 1.0)
        alpha = 1.0;

    glColor4d(fact, fact, fact, alpha);
    glNormal3d(x, y, z);
    glVertex3d(x * radius, y * radius, z * radius);
}

void UFS3dViewer::_xyVertex(double x, double y, double z, double fact)
{
    double alpha = 1.05 * fact;
    if(alpha < 0.1)
        alpha = 0.0;
    else if(alpha > 1.0)
        alpha = 1.0;

    glColor4d(fact, fact, fact, alpha);
    glNormal3d(x, y, z);
    glVertex3d(x * radius, y * radius, z * radius);
}

void UFS3dViewer::_sphereDisplay()
{
    int i, j, k;
    size_t mpos, npos;

    double sv = 1.0;
    double fact;
    vector<double> radius[_nlev];;

    k1 = nvoptions->get_zsec()+1;
    k = _nlev-k1;
    for(k = 0; k < _nlev; ++k)
        radius[k] = _k2r(k);

    sv = 1.0 / (_valmax - _valmin);

    zcl = glGenLists(1);
    glNewList(zcl, GL_COMPILE_AND_EXECUTE);
    lister->set_zid(k, zcl);

  //OpenGL should normalize normal vectors
    glEnable(GL_NORMALIZE);
    glPushMatrix();
    glDisable(GL_LIGHTING);
    glColor4f(1.0, 1.0, 1.0, 1.0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  //#pragma omp parallel for
    for(k1 = 1; k1 <= _nlev; ++k1)
    {
      k = _nlev - k1;
      for(j = 1; j < _nlat; ++j)
      {
        mpos = (k*_nlat + (j-1))*_nlon;
        npos = (k*_nlat + j)*_nlon;
        glBegin(GL_QUAD_STRIP);
        for(i = 0; i < _nlon; ++i)
        {
            fact = sv * (pltvar[npos+i] - _valmin);
            _lonlat2xyz(_lon[i], _lat[j], radius[k], fact);

            fact = sv * (pltvar[mpos+i] - _valmin);
            _lonlat2xyz(_lon[i], _lat[j-1], radius[k], fact);
        }
        fact = sv * (pltvar[npos] - _valmin);
        _lonlat2xyz(_lon[0], _lat[j], radius[k], fact);

        fact = sv * (pltvar[mpos] - _valmin);
        _lonlat2xyz(_lon[0], _lat[j-1], radius[k], fact);
        glEnd();
      }
    }
    coastline->drawOnSphere(radius[0]+0.01);

    for(k1 = 1; k1 < _nlev; ++k1)
    {
      k = _nlev - k1;
      for(j = 0; j < _nlat; ++j)
      {
        mpos = ((k-1)*_nlat + j))*_nlon;
        npos = (k*_nlat + j)*_nlon;
        glBegin(GL_QUAD_STRIP);
        for(i = 0; i < _nlon; ++i)
        {
            fact = sv * (pltvar[npos+i] - _valmin);
            _lonlat2xyz(_lon[i], _lat[j], radius[k], fact);

            fact = sv * (pltvar[mpos+i] - _valmin);
            _lonlat2xyz(_lon[i], _lat[j], radius[k-1], fact);
        }
        fact = sv * (pltvar[npos+i] - _valmin);
        _lonlat2xyz(_lon[0], _lat[j], radius[k], fact);

        fact = sv * (pltvar[mpos+i] - _valmin);
        _lonlat2xyz(_lon[0], _lat[j], radius[k-1], fact);
        glEnd();
      }

      for(i = 0; i < _nlon; ++i)
      {
        glBegin(GL_QUAD_STRIP);
        for(j = 0; j < _nlat; ++j)
        {
            mpos = ((k-1)*_nlat + j))*_nlon;
            npos = (k*_nlat + j)*_nlon;
            fact = sv * (pltvar[npos+i] - _valmin);
            _lonlat2xyz(_lon[i], _lat[j], radius[k], fact);

            fact = sv * (pltvar[mpos+i] - _valmin);
            _lonlat2xyz(_lon[i], _lat[j], radius[k-1], fact);
        }
        glEnd();
      }
    }

    glPopMatrix();
    glEndList();
}

void UFS3dViewer::_flatDisplay()
{
    int i, j, k, k1;
    size_t mpos, npos;
    double sv = 1.0;
    double fact;
    double height = 0.0;

    k1 = nvoptions->get_zsec()+1;
    k = _nlev-k1;
    height = _k2h(k);
    sv = 1.0 / (_valmax - _valmin);

    zcl = glGenLists(1);
  //glNewList(zcl, GL_COMPILE);
    glNewList(zcl, GL_COMPILE_AND_EXECUTE);
    lister->set_zid(k, zcl);

    glPushMatrix();
    glClearColor(1.0, 1.0, 1.0, 1.0);
  //OpenGL should normalize normal vectors
    glEnable(GL_NORMALIZE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glNormal3f(0.0, 0.0, -1.0);

    for(k1 = 1; k1 < _nlev; ++k1)
    {
      k = _nlev - k1;
      for(j = 1; j < _nlat; ++j)
      {
        mpos = (k*_nlat+(j-1))*_nlon;
        npos = (k*_nlat+j)*_nlon;
        glBegin(GL_QUAD_STRIP);
        for(i = _hlon+1; i < _nlon; ++i)
        {
            fact = sv * (pltvar[npos+i] - _valmin);
            _xyVertex(_xFlat[i], _yFlat[j], height, fact)

            fact = sv * (pltvar[mpos+i] - _valmin);
            _xyVertex(_xFlat[i], _yFlat[j-1], height, fact)
        }

	for(i = 0; i < _hlon; ++i)
        {
            fact = sv * (pltvar[npos+i] - _valmin);
            _xyVertex(_xFlat[i], _yFlat[j], height, fact)

            fact = sv * (pltvar[mpos+i] - _valmin);
            _xyVertex(_xFlat[i], _yFlat[j-1], height, fact)
        }
        glEnd();
    }
    coastline->drawOnPlane(height+0.01);
    }

    glPopMatrix();
    glEndList();
}

void UFS3dViewer::_evaluate(float *var)
{
    size_t varsize;
    size_t n = 0;
    float total;

  //cout << "Enter functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
  //cout << "\t _varname: " << _varname << endl;

    varsize = geometry->get_nlon() * geometry->get_nlat() * geometry->get_nlev();
  //varsize = geometry->get_nlon() * geometry->get_nlat();

  //cout << "\tin <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
  //cout << "\t_nlon =" << _nlon << endl;
  //cout << "\t_nlat =" << _nlat << endl;
  //cout << "\t_nlev =" << _nlev << endl;
  //cout << "\tvarsize =" << varsize << ", _nlon*_nlat=" << _nlon*_nlat << endl;

    _valmax = var[0];
    _valmin = var[0];
    total = var[0];

    for(n = 1; n < varsize; ++n)
    {
        if(_valmax < var[n])
        {
           _valmax = var[n];
        }
        if(_valmin > var[n])
        {
           _valmin = var[n];
        }
        total += var[n];
    }

    if(1.0e-10 > (_valmax - _valmin))
       _valmax += 1.0e-10;

    _valavg = total / varsize;

  //cout << "\t_valmin = " << _valmin << ", _valavg = " << _valavg << ", _valmax = " << _valmax << endl;

#if 0
    nvoptions->set_trueminimum(_valmin);
    nvoptions->set_truemaximum(_valmax);
#endif
  //cout << "Leave functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
}

void UFS3dViewer::_adjust_minmax(float *var)
{
    size_t varsize;
    size_t n = 0;
    float total;

  //cout << "Enter functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;

    varsize = geometry->get_nlon() * geometry->get_nlat();

    _valmax = var[0];
    _valmin = var[0];
    total = var[0];

    for(n = 1; n < varsize; ++n)
    {
        if(_valmax < var[n])
        {
           _valmax = var[n];
        }
        if(_valmin > var[n])
        {
           _valmin = var[n];
        }
        total += var[n];
    }

    if(1.0e-10 > (_valmax - _valmin))
       _valmax += 1.0e-10;

    _valavg = total / varsize;

  //cout << "\t_valmin = " << _valmin << ", _valavg = " << _valavg << ", _valmax = " << _valmax << endl;

#if 0
    // adjust _valmin _valmax;
    for(n = 0; n < 2; ++n)
    {
    double sd = _valavg - _valmin;
    double du = _valmax - _valavg;

    if(sd > du)
      //_valmin = _valavg - du;
        _valmin = 0.5*(_valavg + _valmin);
    else
      //_valmax = _valavg + sd;
        _valmax = 0.5*(_valavg + _valmax);

  //cout << "\t_valmin = " << _valmin << ", _valavg = " << _valavg << ", _valmax = " << _valmax << endl;
    }
#endif
  //cout << "Leave functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
}

void UFS3dViewer::reset_texture1d(ColorTable *ct)
{
    colorTable = ct;

    glDisable(GL_TEXTURE_1D);

    texture1d->reset();
    texture1d->set_colors(ct->get_clen(), ct->get_cmap());
    texture1d->set_name(ct->get_name());
}

void UFS3dViewer::draw_sphere_grids()
{
    int i, j, k;
    size_t npos;

    double radius = 1.001;

    GLfloat line_width = 1.0;

  //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\tncenters = " << ncenters << endl;

    _xSphere = geometry->get_xSphere();
    _ySphere = geometry->get_ySphere();
    _zSphere = geometry->get_zSphere();

    k = nvoptions->get_zsec();
    radius = _k2r(k);

    glPushMatrix();

  //earth->draw();

    glColor4f(0.0, 1.0, 1.0, 1.0);
  //glNormal3f(0.0, 0.0, -1.0);

    glLineWidth(line_width);

  //#pragma omp parallel for
    for(j = 0; j < _nlat; ++j)
    {
        npos = j*_nlon;

        glBegin(GL_LINE_STRIP);
        for(i = 0; i < _nlon; ++i)
        {
            glVertex3d(radius * _xSphere[npos+i], radius * _ySphere[npos+i], radius * _zSphere[npos+i]);
        }
        glEnd();
    }
    glEnd();

    glPopMatrix();
}

double UFS3dViewer::_k2h(int k)
{
    double height = 0.5 * ((double) (_nlev-k) / _nlev);
    return height;
}

double UFS3dViewer::_k2r(int k)
{
    double radius = 1.0 + _k2h(k);
    return radius;
}

