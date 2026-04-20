#include <QtOpenGL>

#include <vector>

#include "ufs_viewer.h"

UFS2dViewer::UFS2dViewer(ColorTable *ct, NVOptions* opt)
{
    colorTable = ct;
    nvoptions = opt;

    texture1d = new Texture1d();
    texture1d->set_colors(ct->get_clen(), ct->get_cmap());
    texture1d->set_name(ct->get_name());

    _var = NULL;
 
    earth = new Earth();

    nvoptions->set_xsec(360);
    nvoptions->set_ysec(90);
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

UFS2dViewer::UFS2dViewer(ColorTable *ct, NVOptions* opt, const char* bmpflnm, ncReader* nchandler)
{
    colorTable = ct;
    nvoptions = opt;

  //cout << "Enter functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    texture1d = new Texture1d();
  //cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    texture1d->set_colors(ct->get_clen(), ct->get_cmap());
  //cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    texture1d->set_name(ct->get_name());

  //cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    _var = NULL;

    ncfile = nchandler;
  //cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
  //cout << "\tbmpflnm: " << bmpflnm << endl;
    earth = new Earth(bmpflnm, ncfile);

  //cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    nvoptions->set_xsec(360);
    nvoptions->set_ysec(90);
    nvoptions->set_zsec(0);

    _nlon = 360;
    _nlat = 180;

  //cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    oneover = 1.0 / 180.0;
    deg2rad = 3.1415926535897932 * oneover;

    lister = new Lister();
    lister->setup(361, 181, 121);

  //cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    locator = NULL;

    previoustimelevel = -1;
    current_timelevel = 0;
  //cout << "Leave functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
}

UFS2dViewer::~UFS2dViewer()
{
    locator->turnOff();

    delete earth;
    delete lister;
    delete texture1d;
}

void UFS2dViewer::set_geometry(UFSGeometry *gm)
{
    geometry = gm;

    _initialize();
}

void UFS2dViewer::setup(string vn, float *var)
{
    reset();

    _varname  = vn;
    _var = var;
    _nlev = geometry->get_nlev();

    nvoptions->set_xsec(_nlon);
    nvoptions->set_ysec(_nlat);
    nvoptions->set_zsec(_nlev);

    _evaluate(_var);

    previoustimelevel = -1;
}

void UFS2dViewer::reset()
{
    lister->reinitialize(_nlon+1, _nlat+1, geometry->get_nlev()+1);
}

void UFS2dViewer::_initialize()
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

    _deltlon = _lon[1] - _lon[0];
    _deltlat = abs(_lat[1] - _lat[0]);

    geometry->set_ntim(1);

  //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\n" << _deltlon << " _lon: " <<  _lon << endl;
  //cout << "\n" << _deltlat << " _lat: " <<  _lat << endl;
  //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
}

void UFS2dViewer::draw()
{
    size_t nsquare = _nlon * _nlat;

  //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\tnvoptions->get_tsec() = " << nvoptions->get_tsec() << ", previoustimelevel = " << previoustimelevel << endl;

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

  //cout << "\t" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
    current_timelevel = nvoptions->get_tsec();
    if(current_timelevel != previoustimelevel)
        reset();
    previoustimelevel = current_timelevel;
    if(current_timelevel >= geometry->get_nt())
        return;

  //cout << "\t" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
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
  //cout << "\t" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\tnvoptions->get_cb(NV_FLATON): " << nvoptions->get_cb(NV_FLATON) << endl;
      //cout << "\t" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
      //cout << "\tnvoptions->get_xsec() = " << nvoptions->get_xsec() << endl;
      //cout << "\tnvoptions->get_ysec() = " << nvoptions->get_ysec() << endl;
      //cout << "\tnvoptions->get_zsec() = " << nvoptions->get_zsec() << endl;
      //cout << "\txcl = " << xcl << endl;
      //cout << "\tycl = " << ycl << endl;
      //cout << "\tzcl = " << zcl << endl;

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

            if(nvoptions->get_xsec() > 0)
            {
                if(xcl)
                    glCallList(xcl);
                else
                    _display_Xflat_plane(nvoptions->get_xsec());
            }

            if((nvoptions->get_ysec() > 5) && (nvoptions->get_ysec() < (_nlat-5)))
            {
                if(ycl)
                    glCallList(ycl);
                else
                    _display_Yflat_plane(nvoptions->get_ysec());
            }
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
    
            if(nvoptions->get_xsec() < _nlon)
            {
                if(xcl)
                    glCallList(xcl);
                else
                    _sphereXplane(nvoptions->get_xsec());
            }
    
            if((nvoptions->get_ysec() > 5) && (nvoptions->get_ysec() < (_nlat-5)))
            {
                if(ycl)
                    glCallList(ycl);
                else
                    _sphereYplane(nvoptions->get_ysec());
            }
    
          //draw_sphere_grids();
        }
    }
  //cout << "\t" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
}

void UFS2dViewer::_lonlat2xyz(double lon, double lat, double radius, double fact)
{
    double phi = lat * deg2rad;
    double dist = cos(phi);
    double lamda = lon * deg2rad;

    double x = dist * sin(lamda);
    double z = dist * cos(lamda);
    double y = sin(phi);

    if(nvoptions->get_cb(NV_BUMPON))
    {
        double alpha = 1.25 * fact;
        if(alpha < 0.1)
            alpha = 0.0;
        if(alpha > 1.0)
            alpha = 1.0;
        glColor4d(fact, fact, fact, alpha);
    }
    else
        glTexCoord1d(fact);

    glNormal3f(x, y, z);
    glVertex3d(x * radius, y * radius, z * radius);
}

void UFS2dViewer::_lonlat2xyz_texture(double lon, double lat,
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

void UFS2dViewer::_sphereDisplay()
{
    int i, j, k, k1;
    size_t mpos, npos;

    double sv = 1.0;
    double fact;
    double radius = 1.001;

    k1 = nvoptions->get_zsec();
    k = _nlev-k1;
    radius = 1.0 + ((double) (k1-1) / _nlev);
    sv = 1.0 / (_valmax - _valmin);

    zcl = glGenLists(1);
  //glNewList(zcl, GL_COMPILE);
    glNewList(zcl, GL_COMPILE_AND_EXECUTE);
    lister->set_zid(k1, zcl);

  //OpenGL should normalize normal vectors
    glEnable(GL_NORMALIZE);
    glPushMatrix();
    glDisable(GL_LIGHTING);
    glColor4f(1.0, 1.0, 1.0, 1.0);
  //glColor4f(0.0, 0.0, 0.0, 0.0);

    glEnable(GL_TEXTURE_1D);
    glBindTexture(GL_TEXTURE_1D, texture1d->get_textureID());

  //#pragma omp parallel for
    if(k < _nlev || 1 == _nlev) {
    for(j = 1; j < _nlat; ++j)
    {
        mpos = (k*_nlat + (j-1))*_nlon;
        npos = (k*_nlat + j)*_nlon;
        glBegin(GL_QUAD_STRIP);
        for(i = 0; i < _nlon; ++i)
        {
            fact = sv * (pltvar[npos+i] - _valmin);
            _lonlat2xyz_texture(_lon[i], _lat[j], radius, fact);

            fact = sv * (pltvar[mpos+i] - _valmin);
            _lonlat2xyz_texture(_lon[i], _lat[j-1], radius, fact);
        }
        fact = sv * (pltvar[npos] - _valmin);
        _lonlat2xyz_texture(_lon[0], _lat[j], radius, fact);

        fact = sv * (pltvar[mpos] - _valmin);
        _lonlat2xyz_texture(_lon[0], _lat[j-1], radius, fact);
        glEnd();
    }
    coastline->drawOnSphere(radius+0.01);
    }

    glDisable(GL_TEXTURE_1D);
    glPopMatrix();
    glEndList();
}

void UFS2dViewer::_flatDisplay()
{
    int i, j, k, k1;
    size_t mpos, npos;
    double sv = 1.0;
    double fact;
    double height = 0.00;

    k1 = nvoptions->get_zsec();
    k = _nlev-k1;
    height = (double) (k1-1) / _nlev;
    sv = 1.0 / (_valmax - _valmin);

    zcl = glGenLists(1);
  //glNewList(zcl, GL_COMPILE);
    glNewList(zcl, GL_COMPILE_AND_EXECUTE);
    lister->set_zid(k1, zcl);

    glPushMatrix();
    glClearColor(1.0, 1.0, 1.0, 1.0);
  //OpenGL should normalize normal vectors
    glEnable(GL_NORMALIZE);
    glEnable(GL_TEXTURE_1D);
    glBindTexture(GL_TEXTURE_1D, texture1d->get_textureID());
    glNormal3f(0.0, 0.0, -1.0);

    if(k < _nlev || 1 == _nlev) {
    for(j = 1; j < _nlat; ++j)
    {
      //cout << "\t_yFlat[" << j << "] = " << _yFlat[j] << endl;
        mpos = (k*_nlat+(j-1))*_nlon;
        npos = (k*_nlat+j)*_nlon;
        glBegin(GL_QUAD_STRIP);
        for(i = _hlon; i < _nlon; ++i)
        {
            fact = sv * (pltvar[npos+i] - _valmin);
            glTexCoord1d(fact);
            glVertex3d(_xFlat[i], _yFlat[j], height);

            fact = sv * (pltvar[mpos+i] - _valmin);
            glTexCoord1d(fact);
            glVertex3d(_xFlat[i], _yFlat[j-1], height);
        }

	for(i = 0; i < _hlon; ++i)
        {
            fact = sv * (pltvar[npos+i] - _valmin);
            glTexCoord1d(fact);
            glVertex3d(_xFlat[i], _yFlat[j], height);

            fact = sv * (pltvar[mpos+i] - _valmin);
            glTexCoord1d(fact);
            glVertex3d(_xFlat[i], _yFlat[j-1], height);
        }
        glEnd();
    }
    coastline->drawOnPlane(height+0.01);
    }

    glDisable(GL_TEXTURE_1D);
    glPopMatrix();
    glEndList();
}

void UFS2dViewer::_evaluate(float *var)
{
    size_t varsize;
    size_t n = 0;
    char vn[128];

  //cout << "Enter functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
  //cout << "\t _varname: " << _varname << endl;
    strcpy(vn, _varname.c_str());

    varsize = geometry->get_nlon() * geometry->get_nlat() * geometry->get_nlev();
  //varsize = geometry->get_nlon() * geometry->get_nlat();

    cout << "\tin <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    cout << "\t_nlon =" << _nlon << endl;
    cout << "\t_nlat =" << _nlat << endl;
    cout << "\t_nlev =" << _nlev << endl;
    cout << "\tvarsize =" << varsize << ", _nlon*_nlat=" << _nlon*_nlat << endl;

    _valmax = var[0];
    _valmin = var[0];

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
    }

    if(1.0e-10 > (_valmax - _valmin))
       _valmax += 1.0e-10;

  //cout << "\t_valmin = " << _valmin << ", _valmax = " << _valmax << endl;

#if 0
    nvoptions->set_trueminimum(_valmin);
    nvoptions->set_truemaximum(_valmax);
#endif
  //cout << "Leave functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
}

void UFS2dViewer::reset_texture1d(ColorTable *ct)
{
    colorTable = ct;

    glDisable(GL_TEXTURE_1D);

    texture1d->reset();
    texture1d->set_colors(ct->get_clen(), ct->get_cmap());
    texture1d->set_name(ct->get_name());
}

void UFS2dViewer::draw_sphere_grids()
{
    int i, j;
    size_t npos;

    double radius = 1.001;

    GLfloat line_width = 1.0;

  //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\tncenters = " << ncenters << endl;

    _xSphere = geometry->get_xSphere();
    _ySphere = geometry->get_ySphere();
    _zSphere = geometry->get_zSphere();

    radius = 0.75 + 0.5 * ( 1.0 - (nvoptions->get_zsec() + 1.0) / _nlev);

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

void UFS2dViewer::draw_plane_grids()
{
    int i, j;
    size_t npos;

    double height = 0.01;

    GLfloat line_width = 1.0;

  //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\tncenters = " << ncenters << endl;

    height = 0.8 * (0.5 - (nvoptions->get_zsec() + 1.0) / _nlev);

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

void UFS2dViewer::_display_Yflat_plane(int ys)
{
    int i, k;
    size_t mpos, npos;
    double fact;
    double sv = 1.0;
    vector<double> height(_nlev);
    int j = ys-1;

    if((-85.0 > _lat[j]) || (85.0 < _lat[j]))
       return;

    for(k = 0; k < _nlev; ++k)
        height[k] = (double) (_nlev - 1 - k) / _nlev;

    sv = 1.0 / (_valmax - _valmin);

    ycl = glGenLists(1);
  //glNewList(ycl, GL_COMPILE);
    glNewList(ycl, GL_COMPILE_AND_EXECUTE);
    lister->set_yid(j, ycl);

    glPushMatrix();
    glClearColor(1.0, 1.0, 1.0, 1.0);
  //OpenGL should normalize normal vectors
    glEnable(GL_NORMALIZE);
    glEnable(GL_TEXTURE_1D);
    glBindTexture(GL_TEXTURE_1D, texture1d->get_textureID());
    glNormal3f(0.0, 0.0, -1.0);

    for(k = 1; k < _nlev; ++k)
    {
        mpos = ((k-1)*_nlat+j)*_nlon;
        npos = (k*_nlat+j)*_nlon;

        glBegin(GL_QUAD_STRIP);
        for(i = _hlon; i < _nlon; ++i)
        {
            fact = sv * (pltvar[mpos+i] - _valmin);
            glTexCoord1d(fact);
            glVertex3d(_xFlat[i], _yFlat[j], height[k-1]);

            fact = sv * (pltvar[npos+i] - _valmin);
            glTexCoord1d(fact);
            glVertex3d(_xFlat[i], _yFlat[j], height[k]);
        }

	for(i = 0; i < _hlon; ++i)
        {
            fact = sv * (pltvar[mpos+i] - _valmin);
            glTexCoord1d(fact);
            glVertex3d(_xFlat[i], _yFlat[j], height[k-1]);

            fact = sv * (pltvar[mpos+i] - _valmin);
            glTexCoord1d(fact);
            glVertex3d(_xFlat[i], _yFlat[j], height[k]);
        }
        glEnd();
    }

    glDisable(GL_TEXTURE_1D);
    glPopMatrix();
    glEndList();
}

void UFS2dViewer::_sphereXplane(int xs)
{
    int j, k;
    size_t mpos, npos;
    double fact;
    double sv = 1.0;
    double radius[_nlev];

    int i = xs - 1;

    for(k = 0; k < _nlev; ++k)
        radius[k] = 1.0 + (double) (_nlev-1-k) / _nlev;

    sv = 1.0 / (_valmax - _valmin);

    xcl = glGenLists(1);
  //glNewList(xcl, GL_COMPILE);
    glNewList(xcl, GL_COMPILE_AND_EXECUTE);
    lister->set_xid(i, xcl);

  //OpenGL should normalize normal vectors
    glEnable(GL_NORMALIZE);
    glPushMatrix();
    glDisable(GL_LIGHTING);
    glColor4f(1.0, 1.0, 1.0, 1.0);
  //glColor4f(0.0, 0.0, 0.0, 0.0);

    glEnable(GL_TEXTURE_1D);
    glBindTexture(GL_TEXTURE_1D, texture1d->get_textureID());

  //#pragma omp parallel for
    for(k = 1; k < _nlev; ++k)
    {
        glBegin(GL_QUAD_STRIP);
        for(j = 0; j < _nlat; ++j)
        {
            mpos = ((k-1)*_nlat + j)*_nlon;
            npos = (k*_nlat + j)*_nlon;

            fact = sv * (pltvar[mpos+i] - _valmin);
            _lonlat2xyz_texture(_lon[i], _lat[j], radius[k-1], fact);

            fact = sv * (pltvar[npos+i] - _valmin);
            _lonlat2xyz_texture(_lon[i], _lat[j], radius[k], fact);
        }
        glEnd();
    }

    glDisable(GL_TEXTURE_1D);
    glPopMatrix();
    glEndList();
}

void UFS2dViewer::_sphereYplane(int ys)
{
    int i, k;
    size_t mpos, npos;
    double fact;
    double sv = 1.0;
    double radius[_nlev];
    int j = ys - 1;

    for(k = 0; k < _nlev; ++k)
        radius[k] = 1.0 + (double) (_nlev-1-k) / _nlev;

    sv = 1.0 / (_valmax - _valmin);

    ycl = glGenLists(1);
  //glNewList(ycl, GL_COMPILE);
    glNewList(ycl, GL_COMPILE_AND_EXECUTE);
    lister->set_yid(j, ycl);

  //OpenGL should normalize normal vectors
    glEnable(GL_NORMALIZE);
    glPushMatrix();
    glDisable(GL_LIGHTING);
    glColor4f(1.0, 1.0, 1.0, 1.0);
  //glColor4f(0.0, 0.0, 0.0, 0.0);

    glEnable(GL_TEXTURE_1D);
    glBindTexture(GL_TEXTURE_1D, texture1d->get_textureID());

  //#pragma omp parallel for
    for(k = 1; k < _nlev; ++k)
    {
        mpos = ((k-1)*_nlat + j)*_nlon;
        npos = (k*_nlat + j)*_nlon;
        glBegin(GL_QUAD_STRIP);
        for(i = 0; i < _nlon; ++i)
        {
            fact = sv * (pltvar[mpos+i] - _valmin);
            _lonlat2xyz_texture(_lon[i], _lat[j], radius[k-1], fact);

            fact = sv * (pltvar[npos+i] - _valmin);
            _lonlat2xyz_texture(_lon[i], _lat[j], radius[k], fact);
        }
        fact = sv * (pltvar[mpos] - _valmin);
        _lonlat2xyz_texture(_lon[0], _lat[j], radius[k-1], fact);

        fact = sv * (pltvar[npos] - _valmin);
        _lonlat2xyz_texture(_lon[0], _lat[j], radius[k], fact);
        glEnd();
    }

    glDisable(GL_TEXTURE_1D);
    glPopMatrix();
    glEndList();
}


void UFS2dViewer::_display_Xflat_plane(int xs)
{
    int j, k;
    size_t mpos, npos;
    double fact;
    double sv = 1.0;
    vector<double> height(_nlev);
    int i = xs-1;

    for(k = 0; k < _nlev; ++k)
        height[k] = (double) (_nlev - 1 - k) / _nlev;

    sv = 1.0 / (_valmax - _valmin);

    xcl = glGenLists(1);
  //glNewList(xcl, GL_COMPILE);
    glNewList(xcl, GL_COMPILE_AND_EXECUTE);
    lister->set_xid(i, xcl);

    glPushMatrix();
    glClearColor(1.0, 1.0, 1.0, 1.0);
  //OpenGL should normalize normal vectors
    glEnable(GL_NORMALIZE);
    glEnable(GL_TEXTURE_1D);
    glBindTexture(GL_TEXTURE_1D, texture1d->get_textureID());
    glNormal3f(0.0, 0.0, -1.0);

    for(k = 1; k < _nlev; ++k)
    {
        glBegin(GL_QUAD_STRIP);
        for(j = 0; j < _nlat; ++j)
        {
            mpos = ((k-1)*_nlat+j)*_nlon;
            npos = (k*_nlat+j)*_nlon;

            fact = sv * (pltvar[mpos+i] - _valmin);
            glTexCoord1d(fact);
            glVertex3d(_xFlat[i], _yFlat[j], height[k-1]);

            fact = sv * (pltvar[npos+i] - _valmin);
            glTexCoord1d(fact);
            glVertex3d(_xFlat[i], _yFlat[j], height[k]);
        }
        glEnd();
    }

    glDisable(GL_TEXTURE_1D);
    glPopMatrix();
    glEndList();
}

void UFS2dViewer::_sphereBump()
{
    int i, j, k;
    size_t mpos, npos;

    double sv = 1.0;
    double fact;
    double radius = 1.001;

    double amp = 0.1;
    double offset = 0.5;

    k = nvoptions->get_zsec();

  //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\t_varname: <" << _varname << ">, lev = " << lev << endl;
  //cout << "\tncenters = " << ncenters << ", nvoptions->get_zsec() = " << nvoptions->get_zsec() << endl;


    if(k < _nlev)
    {
        radius = 0.725 + 0.5 * ( 1.0 - (k + 1.0) / _nlev);
    }
    else
    {
        radius = 1.0;
    }

    sv = 1.0 / (_valmax - _valmin);

    zcl = glGenLists(1);
  //glNewList(zcl, GL_COMPILE);
    glNewList(zcl, GL_COMPILE_AND_EXECUTE);
    lister->set_zid(k, zcl);

  //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\t_valmin = " << _valmin << ", _valmax = " << _valmax << ", sv = " << sv << endl;
  //cout << "\tzcl = " << zcl << endl;

  //OpenGL should normalize normal vectors
    glEnable(GL_NORMALIZE);

    glEnable(GL_BLEND);
  //glBlendFunc(GL_SRC_ALPHA,GL_ONE);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

    glShadeModel(GL_SMOOTH);

    glPopMatrix();

    earth->bump(radius-0.15);

    glPopMatrix();

    glPushMatrix();

    glDisable(GL_LIGHTING);

    glColor4f(1.0, 1.0, 1.0, 1.0);
    glColor4f(0.0, 0.0, 0.0, 0.0);

    glEnable(GL_TEXTURE_1D);
    glBindTexture(GL_TEXTURE_1D, texture1d->get_textureID());

  //#pragma omp parallel for
    for(j = 1; j < _nlat; ++j)
    {
        mpos = (k*_nlat + (j-1))*_nlon;
        npos = (k*_nlat + j)*_nlon;
        glBegin(GL_QUAD_STRIP);
        for(i = 0; i < _nlon; ++i)
        {
            fact = sv * (pltvar[npos+i] - _valmin);
            _lonlat2xyz(_lon[i], _lat[j], radius + amp * (fact - offset), fact);

            fact = sv * (pltvar[mpos+i] - _valmin);
            _lonlat2xyz(_lon[i], _lat[j-1], radius + amp * (fact - offset), fact);
        }
        fact = sv * (pltvar[npos] - _valmin);
        _lonlat2xyz(_lon[0], _lat[j], radius + amp * (fact - offset), fact);

        fact = sv * (pltvar[mpos] - _valmin);
        _lonlat2xyz(_lon[0], _lat[j-1], radius + amp * (fact - offset), fact);
        glEnd();
    }

    glDisable(GL_TEXTURE_1D);

    glPopMatrix();

    glEndList();
}

void UFS2dViewer::_flatBump()
{
    int i, j, k;
    size_t mpos, npos;
    double sv = 1.0;
    double alpha, fact;
    double height = 0.00;
    double amp = 0.2;
    double offset = 0.5;

    k = nvoptions->get_zsec();

    if(1 < _nlev)
        height = ((double) (_nlev-k) / _nlev);
    else
        height = 0.001;

  //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\t_varname: <" << _varname << ">, lev = " << k << endl;
  //cout << "\tnvoptions->get_zsec() = " << nvoptions->get_zsec() << endl;
  //cout << "\tcurrent_timelevel = " << current_timelevel << endl;

    sv = 1.0 / (_valmax - _valmin);

    zcl = glGenLists(1);
  //glNewList(zcl, GL_COMPILE);
    glNewList(zcl, GL_COMPILE_AND_EXECUTE);
    lister->set_zid(k, zcl);

  //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\t_valmin = " << _valmin << ", _valmax = " << _valmax << ", sv = " << sv << endl;
  //cout << "\tzcl = " << zcl << endl;

    glPushMatrix();

    glClearColor(1.0, 1.0, 1.0, 1.0);

  //OpenGL should normalize normal vectors
    glEnable(GL_NORMALIZE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

  //glShadeModel(GL_SMOOTH);

    earth->bump_plane(height - 0.1);

    glPopMatrix();

    glPushMatrix();

    glEnable(GL_TEXTURE_1D);
    glBindTexture(GL_TEXTURE_1D, texture1d->get_textureID());

    glNormal3f(0.0, 0.0, -1.0);
    if(nvoptions->get_cb(NV_BUMPON))
    {
        for(j = 1; j < _nlat; ++j)
        {
	    mpos = (k*_nlat+(j-1))*_nlon;
            npos = (k*_nlat+j)*_nlon;
            glBegin(GL_QUAD_STRIP);
	    for(i = _hlon; i < _nlon; ++i)
            {
                fact = sv * (pltvar[npos+i] - _valmin);
                alpha = 1.125 * fact;
                if(alpha < 0.01)
                    alpha = 0.0;
                else if(alpha > 1.0)
                {
                    alpha = 1.0;
                    glColor4d(1.5*fact, 1.5*fact, 1.5*fact, alpha);
                }
                else
                    glTexCoord1d(fact);
                glVertex3d(_xFlat[i], _yFlat[j], height + amp * (fact - offset));

                fact = sv * (pltvar[mpos+i] - _valmin);
                alpha = 1.125 * fact;
                if(alpha < 0.01)
                    alpha = 0.0;
                else if(alpha > 1.0)
                {
                    alpha = 1.0;
                    glColor4d(1.5*fact, 1.5*fact, 1.5*fact, alpha);
                }
                else
                    glTexCoord1d(fact);
                glVertex3d(_xFlat[i], _yFlat[j-1], height + amp * (fact - offset));
            }

            for(i = 0; i < _hlon; ++i)
            {
                fact = sv * (pltvar[npos+i] - _valmin);
                alpha = 1.125 * fact;
                if(alpha < 0.01)
                    alpha = 0.0;
		else if(alpha > 1.0)
		{
                    alpha = 1.0;
                    glColor4d(1.5*fact, 1.5*fact, 1.5*fact, alpha);
                }
                else
                    glTexCoord1d(fact);
                glVertex3d(_xFlat[i], _yFlat[j], height + amp * (fact - offset));

		fact = sv * (pltvar[mpos+i] - _valmin);
                alpha = 1.125 * fact;
                if(alpha < 0.01)
                    alpha = 0.0;
                else if(alpha > 1.0)
                {
                    alpha = 1.0;
                    glColor4d(1.5*fact, 1.5*fact, 1.5*fact, alpha);
                }
                else
                    glTexCoord1d(fact);
                glVertex3d(_xFlat[i], _yFlat[j-1], height + amp * (fact - offset));
            }
            glEnd();
        }
    }
    else
    {
        for(j = 1; j < _nlat; ++j)
        {
	    mpos = (k*_nlat+(j-1))*_nlon;
            npos = (k*_nlat+j)*_nlon;
            glBegin(GL_QUAD_STRIP);
            for(i = 0; i < _nlon; ++i)
            {
                fact = sv * (pltvar[npos+i] - _valmin);
                glTexCoord1d(fact);
                glVertex3d(_xFlat[i], _yFlat[j], height);

		fact = sv * (pltvar[mpos+i] - _valmin);
                glTexCoord1d(fact);
                glVertex3d(_xFlat[i], _yFlat[j-1], height);
            }
            glEnd();
        }
    }
    glDisable(GL_TEXTURE_1D);
    glPopMatrix();

    glEndList();
}

void UFS2dViewer::_draw_cross(double radius)
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

