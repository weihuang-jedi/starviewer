#include <QtOpenGL>
#include <QOpenGLFunctions>
#include <QOpenGLFunctions_3_3_Core>

#include <vector>
#include <memory>

// #include <GL/glew.h>
// #define GL_GLEXT_PROTOTYPES 1
#include "ufs_viewer.h"

UFS2dViewer::UFS2dViewer(ColorTable *ct, ColorTable *wvct, NVOptions* opt, Earth* e, ncReader* nchandler)
{
    // cout << "\nEnter " << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
    colorTable = ct;
    nvoptions = opt;

    texture1d = new Texture1d();
    texture1d->set_colors(ct->get_clen(), ct->get_cmap());
    texture1d->set_name(ct->get_name());

    _colorLen = colorTable->get_clen();
    _colorMap = colorTable->get_cmap();

    _var = NULL;

    ncfile = nchandler;
    earth = e;
    windvector.reset(new WindVector(wvct, opt));

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

    _current_sphere_varname = "unknown";
    _current_flat_varname = "unknown";

    previoustimelevel = -1;
    current_timelevel = 0;

    gridVAO = 0;
    gridVBO = 0;
    gridEBO = 0;
    dataTexture = 0;
    indexCount = 0;
    colorMapTexture = 0;
    // cout << "Leave " << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
}

UFS2dViewer::~UFS2dViewer()
{
    locator->turnOff();

    delete lister;
    delete texture1d;
    delete myShaderProgram;
}

void UFS2dViewer::set_geometry(UFSGeometry *gm)
{
    geometry = gm;

    _sphereVertex = geometry->get_sphereVertex();
    _flatVertex = geometry->get_flatVertex();

    _initialize();
}

void UFS2dViewer::setup(string vn, float *var)
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

void UFS2dViewer::reset()
{
    lister->reinitialize(_nlon+1, _nlat+1, geometry->get_nlev()+1);
}

void UFS2dViewer::_initialize()
{
    // cout << "\nEnter " << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
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

    // cout << "\t" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
    if (windvector == nullptr) {
        cerr << "Error: WindVector pointer is NULL!" << endl;
        // Handle error or exit
    } else {
        windvector->setup_lonlat(_lon, _lat);
        windvector->setup_xyFlat(_xFlat, _yFlat);
    }
    // cout << "Leave " << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
}

void UFS2dViewer::draw()
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
                {
                    // _flatDisplay();
                    _flatDisplayGPU();
                }
            }

          //draw_plane_grids();

            if(nvoptions->get_xsec() < _nlon && nvoptions->get_xsec() > 0)
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
                {
                    // _sphereDisplayOriginal();
                    _sphereDisplay();
		}
            }
    
            if(nvoptions->get_xsec() < _nlon && nvoptions->get_xsec() > 0)
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
}

void UFS2dViewer::_lonlat2xyz(double lon, double lat, double radius, double fact)
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

void UFS2dViewer::_evaluate(float *var)
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

void UFS2dViewer::_adjust_minmax(float *var)
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

void UFS2dViewer::reset_texture1d(ColorTable *ct)
{
    colorTable = ct;
    _colorLen = colorTable->get_clen();
    _colorMap = colorTable->get_cmap();

    glDisable(GL_TEXTURE_1D);

    texture1d->reset();
    texture1d->set_colors(ct->get_clen(), ct->get_cmap());
    texture1d->set_name(ct->get_name());
}

void UFS2dViewer::draw_sphere_grids()
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

void UFS2dViewer::draw_plane_grids()
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
        height[k] = _k2h(k);

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
        radius[k] = _k2r(k);

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
        radius[k] = _k2r(k);

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
        height[k] = _k2h(k);

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
    for(j = 1; j < _nlat; ++j)
    {
        mpos = (k*_nlat + (j-1))*_nlon;
        npos = (k*_nlat + j)*_nlon;
        glBegin(GL_QUAD_STRIP);
        for(i = 0; i < _nlon; ++i)
        {
            fact = sv * (pltvar[npos+i] - _valmin);
            _lonlat2xyz(_lon[i], _lat[j], radius + magnifier*fact, fact);

            fact = sv * (pltvar[mpos+i] - _valmin);
            _lonlat2xyz(_lon[i], _lat[j-1], radius + magnifier*fact, fact);
        }
        fact = sv * (pltvar[npos] - _valmin);
        _lonlat2xyz(_lon[0], _lat[j], radius + magnifier*fact, fact);

        fact = sv * (pltvar[mpos] - _valmin);
        _lonlat2xyz(_lon[0], _lat[j-1], radius + magnifier*fact, fact);
        glEnd();
    }
    coastline->drawOnSphere(0.01);
    }

    glPopMatrix();
    glEndList();
}

void UFS2dViewer::_flatBump()
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
    for(j = 1; j < _nlat; ++j)
    {
        mpos = (k*_nlat+(j-1))*_nlon;
        npos = (k*_nlat+j)*_nlon;
        glBegin(GL_QUAD_STRIP);
        for(i = _hlon; i < _nlon; ++i)
        {
            fact = sv * (pltvar[npos+i] - _valmin);
            alpha = amp * fact;
            if(alpha < 0.0001)
                alpha = 0.0;
            else if(alpha > 1.0)
                alpha = 1.0;

            glColor4d(fact, fact, fact, alpha);
            glNormal3d(0.0, _yFlat[j], magnifier*fact);
            glVertex3d(_xFlat[i], _yFlat[j], magnifier*fact);

            fact = sv * (pltvar[mpos+i] - _valmin);
            alpha = amp * fact;
            if(alpha < 0.0001)
                alpha = 0.0;
            else if(alpha > 1.0)
                alpha = 1.0;

            glColor4d(fact, fact, fact, alpha);
            glNormal3d(0.0, _yFlat[j-1], magnifier*fact);
            glVertex3d(_xFlat[i], _yFlat[j-1],  magnifier*fact);
        }
        // glEnd();

        // glBegin(GL_QUAD_STRIP);
        for(i = 0; i < _hlon; ++i)
        {
            fact = sv * (pltvar[npos+i] - _valmin);
            alpha = amp * fact;
            if(alpha < 0.0001)
                alpha = 0.0;
            else if(alpha > 1.0)
                alpha = 1.0;

            glColor4d(fact, fact, fact, alpha);
            glNormal3d(0.0, _yFlat[j], magnifier*fact);
            glVertex3d(_xFlat[i], _yFlat[j], magnifier*fact);

            fact = sv * (pltvar[mpos+i] - _valmin);
            alpha = amp * fact;
            if(alpha < 0.0001)
                alpha = 0.0;
            else if(alpha > 1.0)
                alpha = 1.0;

            glColor4d(fact, fact, fact, alpha);
            glNormal3d(0.0, _yFlat[j-1], magnifier*fact);
            glVertex3d(_xFlat[i], _yFlat[j-1],  magnifier*fact);
        }
        glEnd();
    }
    coastline->drawOnPlane(0.01);
    }

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

double UFS2dViewer::_k2h(int k)
{
    double height = 0.5 * ((double) (_nlev-k) / _nlev);
    return height;
}

double UFS2dViewer::_k2r(int k)
{
    double radius = 1.0 + _k2h(k);
    return radius;
}

void UFS2dViewer::setup_wind(float* u, float* v)
{
    // cout << "function: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    // cout << "setup u and v" << endl;
    _u = u;
    _v = v;

    // Safe explicit check
    if (!windvector) {
        cerr << "WindVector is not initialized yet!" << endl;
        return;
    }

    windvector->setup(_nlon, _nlat, _nlev, _u, _v);
}

void UFS2dViewer::_sphereDisplay()
{
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    if (!f) return; // Guard against uninitialized contexts

    // 1. Setup states (Keep your setup clean)
    glEnable(GL_NORMALIZE);
    glPushMatrix();
    glDisable(GL_LIGHTING);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

    // glEnable(GL_TEXTURE_1D);
    // glBindTexture(GL_TEXTURE_1D, texture1d->get_textureID());
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    int k1 = nvoptions->get_zsec() + 1;
    int k = _nlev - k1;
    double radius = _k2r(k);
    double sv = 1.0 / (_valmax - _valmin);

    if(k < _nlev || 1 == _nlev)
    {
        _fillSphereVertexVector(k, _varname);

        // 3. Send data to GPU and Draw
        GLuint vbo;
        f->glGenBuffers(1, &vbo);
        f->glBindBuffer(GL_ARRAY_BUFFER, vbo);
        // Upload entire batch to GPU at once
        f->glBufferData(GL_ARRAY_BUFFER, _sphereVertex.size() * sizeof(VertexPoint), _sphereVertex.data(), GL_STREAM_DRAW);

        // Enable client states to read our struct format
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_NORMAL_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);

        // Tell OpenGL where to find attributes inside the Vertex struct (stride is sizeof(Vertex))
        glVertexPointer(3, GL_FLOAT, sizeof(VertexPoint), (void*)offsetof(VertexPoint, x));
        glNormalPointer(GL_FLOAT, sizeof(VertexPoint), (void*)offsetof(VertexPoint, nx));
        glColorPointer(4, GL_FLOAT, sizeof(VertexPoint), (void*)offsetof(VertexPoint, r));

        // Draw each latitude row as an independent strip ---
        // Total vertices per row: (_nlon * 2) from the two inner loops
        GLsizei verticesPerRow = static_cast<GLsizei>(_nlon * 2);
        int totalRows = _nlat - 1;

        for (int row = 0; row < totalRows; ++row)
        {
            GLint firstVertexOfRow = row * verticesPerRow;
            glDrawArrays(GL_TRIANGLE_STRIP, firstVertexOfRow, verticesPerRow);
        }

        // Cleanup States
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY);
        glDisableClientState(GL_COLOR_ARRAY);
        f->glBindBuffer(GL_ARRAY_BUFFER, 0);
        f->glDeleteBuffers(1, &vbo);

        coastline->drawOnSphere(radius + 0.01);
    }

    // glDisable(GL_TEXTURE_1D);
    glPopMatrix();
}

void UFS2dViewer::_fillVertex(double fact, VertexPoint& vp)
{
    double alpha = 1.05 * fact;
    if(alpha < 0.1)       alpha = 0.0;
    else if(alpha > 1.0)  alpha = 1.0;

    int cidx = (int) (fact*_colorLen);

    // Color parameters (using 'fact' for grayscale representation as in your original code)
    vp.r = static_cast<float>(_colorMap[3*cidx]);
    vp.g = static_cast<float>(_colorMap[3*cidx+1]);
    vp.b = static_cast<float>(_colorMap[3*cidx+2]);
    vp.a = 1.0;
    // vp.a = static_cast<float>(alpha);
}

void UFS2dViewer::_flatDisplay()
{
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    if (!f) return; // Guard against uninitialized contexts

    // 1. Setup states (Keep your setup clean)
    glEnable(GL_NORMALIZE);
    glPushMatrix();
    glDisable(GL_LIGHTING);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

    // glEnable(GL_TEXTURE_1D);
    // glBindTexture(GL_TEXTURE_1D, texture1d->get_textureID());
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    int k1 = nvoptions->get_zsec() + 1;
    int k = _nlev - k1;
    double height = _k2h(k);

    if(k < _nlev || 1 == _nlev)
    {
        _fillFlatVertexVector(k, _varname);

        // 3. Send data to GPU and Draw
        GLuint vbo;
        f->glGenBuffers(1, &vbo);
        f->glBindBuffer(GL_ARRAY_BUFFER, vbo);
        // Upload entire batch to GPU at once
        f->glBufferData(GL_ARRAY_BUFFER, _flatVertex.size() * sizeof(VertexPoint), _flatVertex.data(), GL_STREAM_DRAW);

        // Enable client states to read our struct format
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_NORMAL_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);

        // Tell OpenGL where to find attributes inside the Vertex struct (stride is sizeof(Vertex))
        glVertexPointer(3, GL_FLOAT, sizeof(VertexPoint), (void*)offsetof(VertexPoint, x));
        glNormalPointer(GL_FLOAT, sizeof(VertexPoint), (void*)offsetof(VertexPoint, nx));
        glColorPointer(4, GL_FLOAT, sizeof(VertexPoint), (void*)offsetof(VertexPoint, r));

        // Draw each latitude row as an independent strip ---
        // Total vertices per row: (_nlon * 2) from the two inner loops
        GLsizei verticesPerRow = static_cast<GLsizei>(_nlon * 2);
        int totalRows = _nlat - 1;

        for (int row = 0; row < totalRows; ++row)
        {
            GLint firstVertexOfRow = row * verticesPerRow;
            glDrawArrays(GL_TRIANGLE_STRIP, firstVertexOfRow, verticesPerRow);
        }

        // Cleanup States
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY);
        glDisableClientState(GL_COLOR_ARRAY);
        f->glBindBuffer(GL_ARRAY_BUFFER, 0);
        f->glDeleteBuffers(1, &vbo);

        coastline->drawOnPlane(height+1.01);
    }

    // glDisable(GL_TEXTURE_1D);
    glPopMatrix();
}

void UFS2dViewer::_fillSphereVertexVector(int k, string vn)
{
    geometry->set_sphereVertex(k);
    if(_current_sphere_varname == vn)
	return;
    _current_sphere_varname = vn;

    int i, j;
    size_t rowStartIndex;
    size_t idx;
    size_t mpos, npos;
    double fact_m, fact_n;
    double sv = 1.0 / (_valmax - _valmin);

    // Use OpenMP to spread the row generation across your supercomputer's CPU cores
    #pragma omp parallel for schedule(static)
    for(j = 1; j < _nlat; ++j)
    {
        rowStartIndex = (j - 1) * ((_nlon+1) * 2);
        idx = rowStartIndex;
        mpos = (k * _nlat + (j - 1)) * _nlon;
        npos = (k * _nlat + j) * _nlon;

        for(i = 0; i < _nlon; ++i)
        {
            fact_n = sv * (pltvar[npos + i] - _valmin);
	    _fillVertex(fact_n, _sphereVertex[idx++]);

            fact_m = sv * (pltvar[mpos + i] - _valmin);
	    _fillVertex(fact_m, _sphereVertex[idx++]);
        }
        fact_n = sv * (pltvar[npos] - _valmin);
        _fillVertex(fact_n, _sphereVertex[idx++]);

        fact_m = sv * (pltvar[mpos] - _valmin);
        _fillVertex(fact_m, _sphereVertex[idx++]);
    }
}

void UFS2dViewer::_fillFlatVertexVector(int k, string vn)
{
    geometry->set_flatVertex(k);
    if(_current_flat_varname == vn)
	return;
    _current_flat_varname = vn;

    int i, j;
    size_t rowStartIndex;
    size_t idx;
    size_t mpos, npos;
    double fact_m, fact_n;
    double sv = 1.0 / (_valmax - _valmin);

    // Use OpenMP to spread the row generation across your supercomputer's CPU cores
    #pragma omp parallel for schedule(static)
    for(j = 1; j < _nlat; ++j)
    {
        rowStartIndex = (j - 1) * (_nlon * 2);
        idx = rowStartIndex;
        mpos = (k * _nlat + (j - 1)) * _nlon;
        npos = (k * _nlat + j) * _nlon;

        for(i = _hlon; i < _nlon; ++i)
        {
            fact_n = sv * (pltvar[npos + i] - _valmin);
            _fillVertex(fact_n, _flatVertex[idx++]);

            fact_m = sv * (pltvar[mpos + i] - _valmin);
            _fillVertex(fact_m, _flatVertex[idx++]);
        }

	for(i = 0; i < _hlon; ++i)
        {
            fact_n = sv * (pltvar[npos + i] - _valmin);
            _fillVertex(fact_n, _flatVertex[idx++]);

            fact_m = sv * (pltvar[mpos + i] - _valmin);
            _fillVertex(fact_m, _flatVertex[idx++]);
        }
    }
}

#if 1
void UFS2dViewer::_initStaticGPUGrid() {
    QOpenGLFunctions_3_3_Core *f = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>();
    if (!f) return;

    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    for (int j = 0; j < _nlat; ++j) {
        float v = (float)j / (_nlat - 1);
        for (int i = 0; i < _nlon; ++i) {
            float u = (float)i / (_nlon - 1);
            vertices.push_back(u);
            vertices.push_back(v);
        }
    }

    for (int j = 0; j < _nlat - 1; ++j) {
        for (int i = 0; i < _nlon; ++i) {
            indices.push_back(j * _nlon + i);
            indices.push_back((j + 1) * _nlon + i);
        }
        indices.push_back(0xFFFFFFFF); 
    }
    indexCount = static_cast<GLsizei>(indices.size());

    f->glGenBuffers(1, &gridVBO);
    f->glGenBuffers(1, &gridEBO);

    f->glBindBuffer(GL_ARRAY_BUFFER, gridVBO);
    f->glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    f->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gridEBO);
    f->glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    f->glBindBuffer(GL_ARRAY_BUFFER, 0);
    f->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
#else
void UFS2dViewer::_initStaticGPUGrid() {
    QOpenGLFunctions_3_3_Core *f = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>();
    if (!f) return;

    // Make sure shaders and static grids are allocated on the GPU before drawing
    if (myShaderProgram == nullptr) {
        _initShaders();
        _initStaticGPUGrid(); // The initialization step from the previous answer
    }

    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    // 1. Generate unique UV coordinates for every vertex cell crossing
    for (int j = 0; j < _nlat; ++j) {
        float v = (float)j / (_nlat - 1);
        for (int i = 0; i < _nlon; ++i) {
            float u = (float)i / (_nlon - 1);
            vertices.push_back(u);
            vertices.push_back(v);
        }
    }

    // 2. Generate optimized element array indices (Triangle Strips with Primitive Restart)
    for (int j = 0; j < _nlat - 1; ++j) {
        for (int i = 0; i < _nlon; ++i) {
            indices.push_back(j * _nlon + i);
            indices.push_back((j + 1) * _nlon + i);
        }
        indices.push_back(0xFFFFFFFF); // Restart strip index
    }
    indexCount = static_cast<GLsizei>(indices.size());

    // 3. Bind objects to GPU memory permanently
    f->glGenVertexArrays(1, &gridVAO);
    f->glGenBuffers(1, &gridVBO);
    f->glGenBuffers(1, &gridEBO);

    f->glBindVertexArray(gridVAO);

    f->glBindBuffer(GL_ARRAY_BUFFER, gridVBO);
    f->glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    f->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gridEBO);
    f->glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    f->glEnableVertexAttribArray(0);
    f->glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

    f->glBindVertexArray(0);
}
#endif

void UFS2dViewer::_flatDisplayGPU()
{
    cout << "\nEnter" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
    QOpenGLFunctions_3_3_Core *f = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>();
    // QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    // if (!f || gridVAO == 0) return;

    cout << "\tin" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
    if (!f) return;

    cout << "\tin" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
    // --- FIX: Initialize the static grid assets if they haven't been built yet ---
    if (gridVBO == 0) {
        cout << "\tin" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
        _initStaticGPUGrid();
        cout << "\tin" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
        cout << "\tgridVBO = " << gridVBO << endl;
        if (gridVBO == 0) return;
    }

    cout << "\tin" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
    int k1 = nvoptions->get_zsec() + 1;
    int k = _nlev - k1;
    double height = _k2h(k);

    cout << "\tin" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
    _initShaders();

    cout << "\tin" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
    if (k >= _nlev && _nlev != 1) return;

    cout << "\tin" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
    // 1. Calculate offset pointer where this specific Z vertical level begins in pltvar
    size_t dataOffset = static_cast<size_t>(k) * _nlat * _nlon;
    float* rawDataPtr = &pltvar[dataOffset];

    // 2. Stream raw data bytes to the GPU texture allocation
    if (dataTexture == 0) {
        f->glGenTextures(1, &dataTexture);
        f->glBindTexture(GL_TEXTURE_2D, dataTexture);
        f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	// --- ADD WRAP MODES FOR GL_TEXTURE_2D COMPLETENESS ---
        f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
    f->glBindTexture(GL_TEXTURE_2D, dataTexture);

    // Direct raw single-channel float upload (No CPU processing)
    f->glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, _nlon, _nlat, 0, GL_RED, GL_FLOAT, rawDataPtr);

    cout << "\tin" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
    // 1. Create and upload the colormap if not already done
    if (colorMapTexture == 0) {
        f->glGenTextures(1, &colorMapTexture);
        f->glBindTexture(GL_TEXTURE_1D, colorMapTexture);
        f->glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB32F, _colorLen, 0, GL_RGB, GL_DOUBLE, _colorMap); // matching your double _colorMap array
        f->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        f->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        f->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    }

    cout << "\tin" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
    // 2. Bind both textures to separate texture units
    f->glActiveTexture(GL_TEXTURE0);
    f->glBindTexture(GL_TEXTURE_2D, dataTexture);
    
    f->glActiveTexture(GL_TEXTURE1);
    f->glBindTexture(GL_TEXTURE_1D, colorMapTexture);

    cout << "\tin" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
    // Disable face culling so triangles draw regardless of vertex winding order
    glDisable(GL_CULL_FACE);

     // Temporarily turn off the depth filter test so the grid isn't masked out by structural overlays
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    // Ensure fixed-function fragment color modifications are turned off
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_TEXTURE_1D);

    // 3. Update uniforms (Inside _flatDisplayGPU)
    myShaderProgram->bind();
    myShaderProgram->setUniformValue("u_ValMin", static_cast<float>(_valmin));
    myShaderProgram->setUniformValue("u_ValMax", static_cast<float>(_valmax));
    myShaderProgram->setUniformValue("u_Height", static_cast<float>(height));

    // --- ADD THESE FOUR LINES TO PASS BOUNDARIES ---
    myShaderProgram->setUniformValue("u_XMin", static_cast<float>(_xFlat[0]));
    myShaderProgram->setUniformValue("u_XMax", static_cast<float>(_xFlat[_nlon - 1]));
    myShaderProgram->setUniformValue("u_YMin", static_cast<float>(_yFlat[0]));
    myShaderProgram->setUniformValue("u_YMax", static_cast<float>(_yFlat[_nlat - 1]));
    // ----------------------------------------------

    f->glActiveTexture(GL_TEXTURE0);
    f->glBindTexture(GL_TEXTURE_2D, dataTexture);
    myShaderProgram->setUniformValue("u_PltvarTex", 0); // Unit 0

    f->glActiveTexture(GL_TEXTURE1);
    f->glBindTexture(GL_TEXTURE_1D, colorMapTexture);
    myShaderProgram->setUniformValue("u_ColorMap", 1);  // Unit 1

    cout << "\tin" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
    // 4. Draw the entire 4.7 Million Point Grid instantaneously
    f->glBindBuffer(GL_ARRAY_BUFFER, gridVBO);
    f->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gridEBO);

    // --- FIX: Enable GL_VERTEX_ARRAY so the GPU triggers the draw call ---
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 2 * sizeof(float), (void*)0);

    // Keep your texture coordinate mapping active for the shader
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer(2, GL_FLOAT, 2 * sizeof(float), (void*)0);
    // ---------------------------------------------------------------------

    f->glEnable(GL_PRIMITIVE_RESTART);
    f->glPrimitiveRestartIndex(0xFFFFFFFF);

    f->glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, (void*)0);

    cout << "\tin" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
    // 5. Clean up bound contexts cleanly
    f->glDisable(GL_PRIMITIVE_RESTART);

    // Disable both states cleanly
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    f->glBindBuffer(GL_ARRAY_BUFFER, 0);
    f->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    myShaderProgram->release();

    // --- RESTORE DEPTH STATES BEFORE DRAWING COASTLINE ---
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);

    cout << "\tin" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
    coastline->drawOnPlane(height + 0.01);

    cout << "\tin" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
    coastline->drawOnPlane(height + 0.01);
    cout << "Leave" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
}

void UFS2dViewer::_initShaders()
{
    cout << "\nEnter" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
    if (myShaderProgram != nullptr) return;

    myShaderProgram = new QOpenGLShaderProgram();

#if 1
    const char* vertexShaderSource = R"glsl(
        #version 330 compatibility
        
        uniform sampler2D u_PltvarTex;
        uniform float u_ValMin;
        uniform float u_ValMax;
        uniform float u_Height;

        // We can safely remove or ignore u_XMin, u_XMax, etc.
        out float v_Fact;

        void main() {
            vec2 sampleCoord = gl_MultiTexCoord0.xy;

            // Mirror the CPU's _hlon split shift logic:
            if (sampleCoord.x < 0.5) {
                sampleCoord.x += 0.5;
            } else {
                sampleCoord.x -= 0.5;
            }

            float rawVal = texture(u_PltvarTex, sampleCoord).r;
            float sv = 1.0 / (u_ValMax - u_ValMin);
            float fact = sv * (rawVal - u_ValMin);

            v_Fact = clamp(fact, 0.0, 1.0);

            // --- FIX: Fit the grid perfectly inside the camera's visual view bounds ---
            // Your view runs from -0.75 to +0.75.
            float x = mix(-0.75, 0.75, gl_MultiTexCoord0.x);
            float y = mix(-0.75, 0.75, gl_MultiTexCoord0.y);
            // -------------------------------------------------------------------------

            gl_Position = gl_ModelViewProjectionMatrix * vec4(x, y, u_Height, 1.0);
        }
    )glsl";
#else
    // 1. Clean Vertex Shader - Compatibility Profile
    const char* vertexShaderSource = R"glsl(
        #version 330 compatibility

        layout (location = 0) in vec2 aTexCoord;

        uniform sampler2D u_PltvarTex;
        uniform float u_ValMin;
        uniform float u_ValMax;
        uniform float u_Height;

        uniform float u_XMin;
        uniform float u_XMax;
        uniform float u_YMin;
        uniform float u_YMax;

        out float v_Fact;

        void main() {
            vec2 sampleCoord = aTexCoord;

            // Mirror the CPU's _hlon split shift logic:
            if (sampleCoord.x < 0.5) {
                sampleCoord.x += 0.5;
            } else {
                sampleCoord.x -= 0.5;
            }

            float rawVal = texture(u_PltvarTex, sampleCoord).r;
            float sv = 1.0 / (u_ValMax - u_ValMin);
            float fact = sv * (rawVal - u_ValMin);

            v_Fact = clamp(fact, 0.0, 1.0);

            // CRITICAL: Calculate positions inside main() so uniforms are resolved!
            float x = mix(u_XMin, u_XMax, aTexCoord.x);
            float y = mix(u_YMin, u_YMax, aTexCoord.y);

            gl_Position = gl_ModelViewProjectionMatrix * vec4(x, y, u_Height, 1.0);
        }
    )glsl";
#endif
    if (!myShaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource)) {
        qDebug() << "Vertex shader error:" << myShaderProgram->log();
    }

    // 2. Clean Fragment Shader - Compatibility Profile
#if 1
    const char* fragmentShaderSource = R"glsl(
        #version 330 compatibility
        in float v_Fact;
        uniform sampler1D u_ColorMap;
        out vec4 FragColor;

        void main() {
            // DIAGNOSTIC FALLBACK: Force every pixel to bright opaque red
            FragColor = vec4(1.0, 0.0, 0.0, 1.0);
        }
    )glsl";
#else
    const char* fragmentShaderSource = R"glsl(
        #version 330 compatibility
        in float v_Fact;
        uniform sampler1D u_ColorMap;
        out vec4 FragColor;

        void main() {
            // FragColor = texture(u_ColorMap, v_Fact);
            vec3 color = texture(u_ColorMap, v_Fact).rgb;
            FragColor = vec4(color, 1.0); // Explicitly force 100% opacity
        }
    )glsl";
#endif

    if (!myShaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource)) {
        qDebug() << "Fragment shader error:" << myShaderProgram->log();
    }

    if (!myShaderProgram->link()) {
        qDebug() << "Shader program linking error:" << myShaderProgram->log();
    }
    cout << "Leave" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
}

