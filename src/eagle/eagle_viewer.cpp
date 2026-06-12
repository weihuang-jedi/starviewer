#include <QtOpenGL>

#include <vector>

#include "eagle_viewer.h"

EAGLE2dViewer::EAGLE2dViewer(ColorTable *ct, NVOptions* opt)
{
    colorTable = ct;
    nvoptions = opt;

    texture1d = new Texture1d();
    texture1d->set_colors(ct->get_clen(), ct->get_cmap());
    texture1d->set_name(ct->get_name());

    _var = NULL;
 
    earth = new Earth();

    nvoptions->set_xsec(0);
    nvoptions->set_ysec(0);
    nvoptions->set_zsec(0);

    _nlon = 360;
    _nlat = 180;
    _nlev = 1;

    oneover = 1.0 / 180.0;
    deg2rad = 3.1415926535897932 * oneover;

    lister = new Lister();
    lister->setup(361, 181, 121);

    locator = NULL;

    previoustimelevel = -1;
    current_timelevel = 0;
}

EAGLE2dViewer::EAGLE2dViewer(ColorTable *ct, NVOptions* opt, const char* bmpflnm, EagleReader* nchandler)
{
    colorTable = ct;
    nvoptions = opt;

    texture1d = new Texture1d();
    texture1d->set_colors(ct->get_clen(), ct->get_cmap());
    texture1d->set_name(ct->get_name());

    _var = NULL;

    ncfile = nchandler;
    earth = new Earth(bmpflnm);

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

EAGLE2dViewer::~EAGLE2dViewer()
{
    locator->turnOff();

    delete earth;
    delete lister;
    delete texture1d;
}

void EAGLE2dViewer::set_geometry(EAGLEGeometry *gm)
{
    geometry = gm;

    _initialize();
}

void EAGLE2dViewer::setup(string vn, float *var)
{
    reset();

    _varname  = vn;
    _var = var;
    _nlev = geometry->get_nlev();

    nvoptions->set_xsec(_nlon);
    nvoptions->set_ysec(_nlat);
    nvoptions->set_zsec(_nlev);

    nvoptions->set_xsec(0);
    nvoptions->set_ysec(0);
    nvoptions->set_zsec(0);

    _evaluate(_var);

    previoustimelevel = -1;
}

void EAGLE2dViewer::reset()
{
    lister->reinitialize(_nlon+1, _nlat+1, geometry->get_nlev()+1);
}

void EAGLE2dViewer::_initialize()
{
    int i, j, m, n;

    int positive;
    int negative;

    previoustimelevel = -1;

    _nx = geometry->get_nx();
    _ny = geometry->get_ny();

    _longitude = geometry->get_longitude();
    _latitude = geometry->get_latitude();

  //lister->reinitialize(361, 181, _nlev);

    _xFlat = geometry->get_xFlat();
    _yFlat = geometry->get_yFlat();

    geometry->set_ntime(1);
}

void EAGLE2dViewer::draw()
{
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

void EAGLE2dViewer::_lonlat2xyz(float lon, float lat, double radius, double fact)
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

void EAGLE2dViewer::_lonlat2xyz_texture(float lon, float lat,
		                        double radius, double fact)
{
    double phi = lat * deg2rad;
    double dist = cos(phi);
    double lamda = lon * deg2rad;

    double x = dist * sin(lamda);
    double z = dist * cos(lamda);
    double y = sin(phi);

    glTexCoord1d(fact);

    glNormal3f(x, y, z);
    glVertex3d(x * radius, y * radius, z * radius);
}

void EAGLE2dViewer::_sphereDisplay()
{
    int i, j, k, k1;
    size_t mpos, npos;

    double sv = 1.0;
    double fact;
    double radius = 1.001;

    k1 = nvoptions->get_zsec()+1;
    k = _nlev-k1;
    radius = _k2r(k);
    sv = 1.0 / (_valmax - _valmin);

    zcl = glGenLists(1);
  //glNewList(zcl, GL_COMPILE);
    glNewList(zcl, GL_COMPILE_AND_EXECUTE);
    lister->set_zid(k, zcl);

  //OpenGL should normalize normal vectors
    glEnable(GL_NORMALIZE);
    glPushMatrix();
    glDisable(GL_LIGHTING);
    glColor4f(1.0, 1.0, 1.0, 1.0);
  //glColor4f(0.0, 0.0, 0.0, 0.0);

    glEnable(GL_TEXTURE_1D);
    glBindTexture(GL_TEXTURE_1D, texture1d->get_textureID());
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  //#pragma omp parallel for
    if(k < _nlev || 1 == _nlev) {
    for(j = 1; j < _ny; ++j)
    {
        mpos = (k*_ny + (j-1))*_nx;
        npos = (k*_ny + j)*_nx;
        glBegin(GL_QUAD_STRIP);
        for(i = 0; i < _nx; ++i)
        {
            fact = sv * (pltvar[npos+i] - _valmin);
            _lonlat2xyz_texture(_longitude[npos+i], _latitude[npos+i], radius, fact);

            fact = sv * (pltvar[mpos+i] - _valmin);
            _lonlat2xyz_texture(_longitude[mpos+i], _latitude[mpos+i], radius, fact);
        }
        glEnd();
    }
    coastline->drawOnSphere(radius+0.01);
    }

    glDisable(GL_TEXTURE_1D);
    glPopMatrix();
    glEndList();
}

void EAGLE2dViewer::_flatDisplay()
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
    glEnable(GL_TEXTURE_1D);
    glBindTexture(GL_TEXTURE_1D, texture1d->get_textureID());
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glNormal3f(0.0, 0.0, -1.0);

    if(k < _nlev || 1 == _nlev) {
    for(j = 1; j < _ny; ++j)
    {
      //cout << "\t_yFlat[" << j << "] = " << _yFlat[j] << endl;
        mpos = (k*_ny+(j-1))*_nx;
        npos = (k*_ny+j)*_nx;
        glBegin(GL_QUAD_STRIP);
        for(i = 0; i < _nx; ++i)
        {
            fact = sv * (pltvar[npos+i] - _valmin);
            glTexCoord1d(fact);
            glVertex3d(_xFlat[npos+i], _yFlat[npos+i], height);

            fact = sv * (pltvar[mpos+i] - _valmin);
            glTexCoord1d(fact);
            glVertex3d(_xFlat[mpos+i], _yFlat[mpos+i], height);
        }
        glEnd();
    }
    coastline->drawOnPlane(height+0.01);
    }

    glDisable(GL_TEXTURE_1D);
    glPopMatrix();
    glEndList();
}

void EAGLE2dViewer::_evaluate(float *var)
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

void EAGLE2dViewer::_adjust_minmax(float *var)
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

void EAGLE2dViewer::reset_texture1d(ColorTable *ct)
{
    colorTable = ct;

    glDisable(GL_TEXTURE_1D);

    texture1d->reset();
    texture1d->set_colors(ct->get_clen(), ct->get_cmap());
    texture1d->set_name(ct->get_name());
}

void EAGLE2dViewer::draw_sphere_grids()
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

void EAGLE2dViewer::draw_plane_grids()
{
    int i, j;
    size_t npos;

    double height = 0.01;

    GLfloat line_width = 1.0;

    height = _k2h(nvoptions->get_zsec()) + 0.001;

    glPushMatrix();

  //earth->draw_plane(0.0);

    glColor4f(1.0, 1.0, 0.0, 1.0);
  //glColor4f(0.0, 0.0, 0.0, 0.0);
    glNormal3f(0.0, 0.0, -1.0);

    glLineWidth(line_width);

    for(j = 0; j < _nlat; ++j)
    {
        npos = j*_nlon;
        glBegin(GL_LINE_STRIP);
        for(i = 0; i < _nlon; ++i)
        {
            glVertex3d(_xFlat[i], _yFlat[j], height);
        }
        glEnd();
    }

    glPopMatrix();
}

void EAGLE2dViewer::_sphereBump()
{
    int i, j, k, k1;
    size_t mpos, npos;
    double sv = 1.0;
    double alpha, fact;
    double amp = 1.05;
    double magnifier = 0.125;
    double radius = 1.0;

    k1 = nvoptions->get_zsec()+1;
    k = _nlev-k1;
  //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\t_varname: <" << _varname << ">, lev = " << k << endl;

    // sv = 1.0 / (_valmax - _valmin);
    sv = 1.0 / (_valmax - _valmin);

    zcl = glGenLists(1);
  //glNewList(zcl, GL_COMPILE);
    glNewList(zcl, GL_COMPILE_AND_EXECUTE);
    lister->set_zid(k, zcl);

  //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\t_valmin = " << _valmin << ", _valmax = " << _valmax << ", sv = " << sv << endl;
  //cout << "\tzcl = " << zcl << ", k = " << k << endl;

  //_adjust_minmax(&pltvar[k*_nlat*_nlon]);

    glPushMatrix();

    glClearColor(1.0, 1.0, 1.0, 1.0);

  //OpenGL should normalize normal vectors
    glEnable(GL_NORMALIZE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

  //glShadeModel(GL_SMOOTH);

    earth->draw(0.999);

    glPopMatrix();

    glPushMatrix();

    glNormal3f(0.0, 0.0, -1.0);
    if(k < _nlev || 1 == _nlev) {
    for(j = 1; j < _ny; ++j)
    {
        mpos = (k*_ny + (j-1))*_nx;
        npos = (k*_ny + j)*_nx;
        glBegin(GL_QUAD_STRIP);
        for(i = 0; i < _nx; ++i)
        {
            fact = sv * (pltvar[npos+i] - _valmin);
            _lonlat2xyz(_longitude[npos+i], _latitude[npos+i], radius + magnifier*fact, fact);

            fact = sv * (pltvar[mpos+i] - _valmin);
            _lonlat2xyz(_longitude[mpos+i], _latitude[mpos+i], radius + magnifier*fact, fact);
        }
    }
    coastline->drawOnSphere(0.01);
    }

    glPopMatrix();
    glEndList();
}

void EAGLE2dViewer::_flatBump()
{
    int i, j, k, k1;
    size_t mpos, npos;
    double sv = 1.0;
    double alpha, fact;
    double amp = 1.05;
    double magnifier = 0.125;
    double rlat;

    k1 = nvoptions->get_zsec()+1;
    k = _nlev-k1;

  //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\t_varname: <" << _varname << ">, lev = " << k << endl;

    // sv = 1.0 / (_valmax - _valmin);
    sv = 1.0 / (_valmax - _valmin);

    zcl = glGenLists(1);
  //glNewList(zcl, GL_COMPILE);
    glNewList(zcl, GL_COMPILE_AND_EXECUTE);
    lister->set_zid(k, zcl);

  //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\t_valmin = " << _valmin << ", _valmax = " << _valmax << ", sv = " << sv << endl;
  //cout << "\tzcl = " << zcl << ", k = " << k << endl;

  //_adjust_minmax(&pltvar[k*_nlat*_nlon]);

    glPushMatrix();

    glClearColor(1.0, 1.0, 1.0, 1.0);

  //OpenGL should normalize normal vectors
    glEnable(GL_NORMALIZE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

  //glShadeModel(GL_SMOOTH);

  //earth->bump_plane(height - 0.1);
    earth->draw_plane(-0.001);

    glPopMatrix();

    glPushMatrix();

    glNormal3f(0.0, 0.0, -1.0);
    if(k < _nlev || 1 == _nlev) {
    for(j = 1; j < _ny; ++j)
    {
        mpos = (k*_ny+(j-1))*_nx;
        npos = (k*_ny+j)*_nx;
        glBegin(GL_QUAD_STRIP);
        for(i = 0; i < _nx; ++i)
        {
            fact = sv * (pltvar[npos+i] - _valmin);
            alpha = amp * fact;
            if(alpha < 0.0001)
                alpha = 0.0;
            else if(alpha > 1.0)
                alpha = 1.0;

            glColor4d(fact, fact, fact, alpha);
            glNormal3d(_xFlat[npos+i], _yFlat[npos+i], magnifier*fact);
            glVertex3d(_xFlat[npos+i], _yFlat[npos+i], magnifier*fact);

            fact = sv * (pltvar[mpos+i] - _valmin);
            alpha = amp * fact;
            if(alpha < 0.0001)
                alpha = 0.0;
            else if(alpha > 1.0)
                alpha = 1.0;

            glColor4d(fact, fact, fact, alpha);
            glNormal3d(_xFlat[mpos+i], _yFlat[mpos+i], magnifier*fact);
            glVertex3d(_xFlat[mpos+i], _yFlat[mpos+i],  magnifier*fact);
        }
        glEnd();
    }
    coastline->drawOnPlane(0.01);
    }

    glPopMatrix();
    glEndList();
}

void EAGLE2dViewer::_draw_cross(double radius)
{
    int m, n;
    double x, y, z;
    double lon = (double) locator->x();
    double lat = (double) locator->y();
    double delt = radius * cos(lat * ARC);

    glLineWidth(2.0);
    glColor4d(1.0, 0.0, 1.0, 0.95);

  //m = 2 + (int) fabs(0.1*lat - 1.0);
    m = 2;

    glBegin(GL_LINE_STRIP);
    for(n = -m; n <= m; ++n)
    {
        x =   delt * sin((lon + n) * ARC);
        y = radius * sin(lat * ARC);
        z =   delt * cos((lon + n) * ARC);
        glVertex3f(x, y, z);
    }
    glEnd();

    m = 2;

    glBegin(GL_LINE_STRIP);
    for(n = -m; n <= m; ++n)
    {
        delt = radius * cos((lat + n) * ARC);
        x =   delt * sin(lon * ARC);
        y = radius * sin((lat + n) * ARC);
        z =   delt * cos(lon * ARC);
        glVertex3f(x, y, z);
    }
    glEnd();
}

double EAGLE2dViewer::_k2h(int k)
{
    double height = 0.5 * ((double) (_nlev-k) / _nlev);
    return height;
}

double EAGLE2dViewer::_k2r(int k)
{
    double radius = 1.0 + _k2h(k);
    return radius;
}

