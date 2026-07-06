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
    delete myFlatShaderProgram;
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
	            if(_current_flat_varname != _varname)
                    {
			dataTexture = 0;
		    }
		    else
		    {
			if(_current_draw_level != nvoptions->get_zsec())
			   dataTexture = 0;
		    }
                    _current_draw_level = nvoptions->get_zsec();

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
		{
                    // _display_Xflat_plane(nvoptions->get_xsec());
                    _display_Xflat_plane_GPU(nvoptions->get_xsec());
		}
            }

            if((nvoptions->get_ysec() > 5) && (nvoptions->get_ysec() < (_nlat-5)))
            {
                if(ycl)
                    glCallList(ycl);
                else
		{
                    // _display_Yflat_plane(nvoptions->get_ysec());
                    _display_Yflat_plane_GPU(nvoptions->get_ysec());
		}
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
	            if(_current_sphere_varname != _varname)
                    {
			dataTexture = 0;
		    }
		    else
		    {
			if(_current_draw_level != nvoptions->get_zsec())
			   dataTexture = 0;
		    }

                    _current_draw_level = nvoptions->get_zsec();
                    // _sphereDisplay();
                    _sphereDisplayGPU();
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

void UFS2dViewer::_flatDisplayGPU()
{
    // cout << "\nEnter" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
    QOpenGLFunctions_3_3_Core *f = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>();
    // QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    // if (!f || gridVAO == 0) return;

    if (!f) return;

    // --- FIX: Initialize the static grid assets if they haven't been built yet ---
    if (gridVBO == 0) {
        _initStaticGPUGrid();
        if (gridVBO == 0) return;
    }

    int k1 = nvoptions->get_zsec() + 1;
    int k = _nlev - k1;
    double height = _k2h(k);

    _initFlatShaders();

    if (k >= _nlev && _nlev != 1) return;

    // 1. Calculate offset pointer where this specific Z vertical level begins in pltvar
    size_t dataOffset = static_cast<size_t>(k) * _nlat * _nlon;
    float* rawDataPtr = &pltvar[dataOffset];

    // 2. Stream raw data bytes to the GPU texture allocation
    if (dataTexture == 0) {
        f->glGenTextures(1, &dataTexture);
        f->glBindTexture(GL_TEXTURE_2D, dataTexture);

        // --- ADD THIS LINE right before glTexImage2D ---
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        f->glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, _nlon, _nlat, 0, GL_RED, GL_FLOAT, rawDataPtr);

        f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	// --- ADD WRAP MODES FOR GL_TEXTURE_2D COMPLETENESS ---
        f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    // 1. Create and upload the colormap if not already done
    if (colorMapTexture == 0) {
        f->glGenTextures(1, &colorMapTexture);
        f->glBindTexture(GL_TEXTURE_1D, colorMapTexture);

        // --- FIX: Pack doubles into standard floats for safe GPU consumption ---
        std::vector<float> floatColorMap(_colorLen * 3);
        for (int c = 0; c < _colorLen * 3; ++c) {
            floatColorMap[c] = static_cast<float>(_colorMap[c]);
        }

        // Upload as standard GL_FLOAT instead of GL_DOUBLE
        f->glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB32F, _colorLen, 0, GL_RGB, GL_FLOAT, floatColorMap.data());
        // ------------------------------------------------------------------------
	
        f->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        f->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        f->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    }

    // Disable face culling so triangles draw regardless of vertex winding order
    glDisable(GL_CULL_FACE);

     // Temporarily turn off the depth filter test so the grid isn't masked out by structural overlays
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    // Ensure fixed-function fragment color modifications are turned off
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_TEXTURE_1D);

    // Clear texturing overrides
     glDisable(GL_LIGHTING);

    // Bind Data to Texture Unit 0
    f->glActiveTexture(GL_TEXTURE0);
    f->glBindTexture(GL_TEXTURE_2D, dataTexture);

    // Bind ColorMap to Texture Unit 1
    f->glActiveTexture(GL_TEXTURE1);
    f->glBindTexture(GL_TEXTURE_1D, colorMapTexture);

    // 3. Update uniforms (Inside _flatDisplayGPU)
    myFlatShaderProgram->bind();
    myFlatShaderProgram->setUniformValue("u_ValMin", static_cast<float>(_valmin));
    myFlatShaderProgram->setUniformValue("u_ValMax", static_cast<float>(_valmax));
    myFlatShaderProgram->setUniformValue("u_Height", static_cast<float>(height));

    // --- ADD THESE FOUR LINES TO PASS BOUNDARIES ---
    myFlatShaderProgram->setUniformValue("u_XMin", static_cast<float>(_xFlat[0]));
    myFlatShaderProgram->setUniformValue("u_XMax", static_cast<float>(_xFlat[_nlon - 1]));
    myFlatShaderProgram->setUniformValue("u_YMin", static_cast<float>(_yFlat[0]));
    myFlatShaderProgram->setUniformValue("u_YMax", static_cast<float>(_yFlat[_nlat - 1]));
    // ----------------------------------------------


    myFlatShaderProgram->setUniformValue("u_PltvarTex", 0); // Unit 0

    f->glActiveTexture(GL_TEXTURE1);
    f->glBindTexture(GL_TEXTURE_1D, colorMapTexture);
    myFlatShaderProgram->setUniformValue("u_ColorMap", 1);  // Unit 1

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

    // 5. Clean up bound contexts cleanly
    f->glDisable(GL_PRIMITIVE_RESTART);

    // Disable both states cleanly
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    f->glBindBuffer(GL_ARRAY_BUFFER, 0);
    f->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    myFlatShaderProgram->release();

    // --- RESTORE DEPTH STATES BEFORE DRAWING COASTLINE ---
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);

    coastline->drawOnPlane(height + 0.01);

    coastline->drawOnPlane(height + 0.01);
}

void UFS2dViewer::_initFlatShaders()
{
    if (myFlatShaderProgram != nullptr) return;

    myFlatShaderProgram = new QOpenGLShaderProgram();

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
            sampleCoord.y = 1.0 - sampleCoord.y;

            float rawVal = texture(u_PltvarTex, sampleCoord).r;
            float sv = 1.0 / (u_ValMax - u_ValMin);
            float fact = sv * (rawVal - u_ValMin);

            v_Fact = clamp(fact, 0.0, 1.0);

            // Set X bounds to twice the scale of Y bounds (2:1 aspect ratio) ---
            // Y spans 1.0 units total (-0.5 to 0.5). 
            // X spans 2.0 units total (-1.0 to 1.0).
            float x = mix(-1.0, 1.0, gl_MultiTexCoord0.x);
            float y = mix(-0.5, 0.5, gl_MultiTexCoord0.y);
            // -------------------------------------------------------------------------

            gl_Position = gl_ModelViewProjectionMatrix * vec4(x, y, u_Height, 1.0);
        }
    )glsl";
    if (!myFlatShaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource)) {
        qDebug() << "Vertex shader error:" << myFlatShaderProgram->log();
    }

    // 2. Clean Fragment Shader - Compatibility Profile
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

    if (!myFlatShaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource)) {
        qDebug() << "Fragment shader error:" << myFlatShaderProgram->log();
    }

    if (!myFlatShaderProgram->link()) {
        qDebug() << "Shader program linking error:" << myFlatShaderProgram->log();
    }
}

void UFS2dViewer::_initSphereShaders()
{
    if (mySphereShaderProgram != nullptr) return;

    mySphereShaderProgram = new QOpenGLShaderProgram();

    const char* vertexShaderSource4Sphere = R"glsl(
        #version 330 compatibility

        uniform sampler2D u_PltvarTex;
        uniform float u_ValMin;
        uniform float u_ValMax;
        uniform float u_Radius;

        out float v_Fact;

        const float PI = 3.14159265358979323846;

        void main() {
            // 1. Start with raw grid texture coordinates [0.0 to 1.0]
            vec2 sampleCoord = gl_MultiTexCoord0.xy;

            // --- ADJUSTABLE LONGITUDE SHIFT CONTROL ---
            // If +0.5 didn't line it up, it means we need to shift the opposite direction.
            // Let's apply a clean 180-degree wrap-around shift:
            sampleCoord.x = fract(sampleCoord.x + 0.5); 
            // ------------------------------------------

            // 2. Sample raw data value from the newly adjusted coordinate column
            float rawVal = texture(u_PltvarTex, sampleCoord).r;
            
            float range = u_ValMax - u_ValMin;
            if (range <= 0.00001) {
                range = 1.0; 
            }
            float fact = (rawVal - u_ValMin) / range;
            v_Fact = clamp(fact, 0.0, 1.0);

            // 3. Keep geometry generation cleanly aligned with your native coastlines
            float lon_rad = (gl_MultiTexCoord0.x * 2.0 * PI) - PI;
            float lat_rad = (gl_MultiTexCoord0.y * PI) - (PI / 2.0);

            // Spherical Trigonometry 
            float dist = cos(lat_rad);
            
            float x = dist * sin(lon_rad);
            float z = dist * cos(lon_rad);
            float y = -sin(lat_rad); // Opaque right-side-up mapping

            vec3 spherePos = vec3(x, y, z) * u_Radius;

            // Project via the active camera matrix stack
            gl_Position = gl_ModelViewProjectionMatrix * vec4(spherePos, 1.0);
        }
    )glsl";

    if (!mySphereShaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource4Sphere)) {
        qDebug() << "Sphere Vertex shader error:" << mySphereShaderProgram->log();
    }

    const char* fragmentShaderSource4Sphere = R"glsl(
        #version 330 compatibility
        
        in float v_Fact; 
        uniform sampler1D u_ColorMap;
        out vec4 FragColor;

        void main() {
            vec3 color = texture(u_ColorMap, v_Fact).rgb;
            FragColor = vec4(color, 1.0); 
        }
    )glsl";

    if (!mySphereShaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource4Sphere)) {
        qDebug() << "Sphere Fragment shader error:" << mySphereShaderProgram->log();
    }

    if (!mySphereShaderProgram->link()) {
        qDebug() << "Sphere Shader program linking error:" << mySphereShaderProgram->log();
    }
}

void UFS2dViewer::_sphereDisplayGPU()
{
     QOpenGLFunctions_3_3_Core *f = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>();
     if (!f) return;

     if (gridVBO == 0) {
          _initStaticGPUGrid();
          if (gridVBO == 0) return;
     }

     int k1 = nvoptions->get_zsec() + 1;
     int k = _nlev - k1;
     double radius = _k2r(k);

     if (k >= _nlev && _nlev != 1) return;

     size_t dataOffset = static_cast<size_t>(k) * _nlat * _nlon;
     float* rawDataPtr = &pltvar[dataOffset];

     // A. Handle Data Texture Allocation
     if (dataTexture == 0) {
          f->glGenTextures(1, &dataTexture);
          f->glBindTexture(GL_TEXTURE_2D, dataTexture);
          f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
          f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
          f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
          f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
     }
     f->glBindTexture(GL_TEXTURE_2D, dataTexture);
     glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
     f->glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, _nlon, _nlat, 0, GL_RED, GL_FLOAT, rawDataPtr);

     // --- B. FIX: Handle ColorMap Texture Allocation if starting in Sphere view first ---
     if (colorMapTexture == 0) {
          f->glGenTextures(1, &colorMapTexture);
          f->glBindTexture(GL_TEXTURE_1D, colorMapTexture);

          std::vector<float> floatColorMap(_colorLen * 3);
          for (int c = 0; c < _colorLen * 3; ++c) {
               floatColorMap[c] = static_cast<float>(_colorMap[c]);
          }

          f->glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB32F, _colorLen, 0, GL_RGB, GL_FLOAT, floatColorMap.data());
          f->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
          f->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
          f->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
     }
     // ----------------------------------------------------------------------------------

     if (mySphereShaderProgram == nullptr) {
           _initSphereShaders();
     }

     glEnable(GL_DEPTH_TEST);
     glDepthMask(GL_TRUE);
     glDepthFunc(GL_LEQUAL);
     glDisable(GL_CULL_FACE);
     glDisable(GL_LIGHTING);

     glDisable(GL_TEXTURE_2D);
     glDisable(GL_TEXTURE_1D);

     // Bind textures to clear pipelines
     f->glActiveTexture(GL_TEXTURE0);
     f->glBindTexture(GL_TEXTURE_2D, dataTexture);

     f->glActiveTexture(GL_TEXTURE1);
     f->glBindTexture(GL_TEXTURE_1D, colorMapTexture);

     mySphereShaderProgram->bind();
     mySphereShaderProgram->setUniformValue("u_ValMin", static_cast<float>(_valmin));
     mySphereShaderProgram->setUniformValue("u_ValMax", static_cast<float>(_valmax));
     mySphereShaderProgram->setUniformValue("u_Radius", static_cast<float>(radius));
     mySphereShaderProgram->setUniformValue("u_PltvarTex", 0);
     mySphereShaderProgram->setUniformValue("u_ColorMap", 1);

     f->glBindBuffer(GL_ARRAY_BUFFER, gridVBO);
     f->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gridEBO);

     glEnableClientState(GL_VERTEX_ARRAY);
     glVertexPointer(2, GL_FLOAT, 2 * sizeof(float), (void*)0);

     glEnableClientState(GL_TEXTURE_COORD_ARRAY);
     glTexCoordPointer(2, GL_FLOAT, 2 * sizeof(float), (void*)0);

     f->glEnable(GL_PRIMITIVE_RESTART);
     f->glPrimitiveRestartIndex(0xFFFFFFFF);

     f->glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, (void*)0);

     f->glDisable(GL_PRIMITIVE_RESTART);
     glDisableClientState(GL_VERTEX_ARRAY);
     glDisableClientState(GL_TEXTURE_COORD_ARRAY);
     f->glBindBuffer(GL_ARRAY_BUFFER, 0);
     f->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
     mySphereShaderProgram->release();

     coastline->drawOnSphere(radius + 0.01);
}

void UFS2dViewer::_initCrossShaders()
{
    if (crossShaderProgram != nullptr) return;

    crossShaderProgram = new QOpenGLShaderProgram();

    // 1. Precise Cross-Section Vertex Shader
    const char* vsSource = R"glsl(
        #version 330 compatibility

        uniform float u_SliceCoord; // Static X or Y plane position
        uniform int u_IsXSlice;     // 1 if X-plane (Longitude), 0 if Y-plane (Latitude)

        uniform float u_MinCoord;   // Minimum spatial boundary limit
        uniform float u_MaxCoord;   // Maximum spatial boundary limit

        uniform float u_Heights[128]; // Atmospheric altitude array tracker

        void main() {
            // Unpack layout coordinates
            vec2 incomingUV = gl_MultiTexCoord0.xy;

            // Pass the clean texture coordinates directly to the fragment pipeline
            gl_TexCoord[0] = gl_MultiTexCoord0;

            // Extract vertical layer depth index
            int kIndex = int(incomingUV.y);
            float currentHeight = u_Heights[kIndex];

            // Linearly scale spatial positions dynamically between true data bounds
            float dynamicCoord = mix(u_MinCoord, u_MaxCoord, incomingUV.x);

            vec3 pos;
            if (u_IsXSlice == 1) {
                // X-plane slice: X is fixed, Y varies across the bounds, Z is height
                pos = vec3(u_SliceCoord, dynamicCoord, currentHeight);
            } else {
                // Y-plane slice: X varies across the bounds, Y is fixed, Z is height
                pos = vec3(dynamicCoord, u_SliceCoord, currentHeight);
            }

            gl_Position = gl_ModelViewProjectionMatrix * vec4(pos, 1.0);
        }
    )glsl";

    // 2. Fragment Shader
    const char* fsSource = R"glsl(
        #version 330 compatibility

        uniform sampler2D u_SliceDataTex;
        uniform sampler1D u_ColorMap;
        uniform float u_ValMin;
        uniform float u_ValMax;

        void main() {
            // Scale and map texture sampling components linearly
            vec2 uv = gl_TexCoord[0].xy;

            // Extract the true float height layer factor from raw incoming integer metrics
            // (Converts structural vertex layer integer steps back to clean 0.0-1.0 UV space)
            float v_norm = uv.y / (textureSize(u_SliceDataTex, 0).y - 1.0);
            vec2 samplePos = vec2(uv.x, v_norm);

            // Sample raw data value from the cross-section coordinate map
            float rawVal = texture(u_SliceDataTex, samplePos).r;

            float range = u_ValMax - u_ValMin;
            if (range <= 0.00001) range = 1.0;

            float fact = clamp((rawVal - u_ValMin) / range, 0.0, 1.0);

            vec3 color = texture(u_ColorMap, fact).rgb;
            gl_FragColor = vec4(color, 1.0);
        }
    )glsl";

    crossShaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, vsSource);
    crossShaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, fsSource);
    crossShaderProgram->link();
}

void UFS2dViewer::_initCrossStaticGrid(int horizontalSize)
{
    QOpenGLFunctions_3_3_Core *f = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>();
    if (!f) return;

    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    // Generate UV positions
    for (int k = 0; k < _nlev; ++k) {
        float v_tex = (float)k / (_nlev - 1);
        for (int i = 0; i < horizontalSize; ++i) {
            float u_tex = (float)i / (horizontalSize - 1);

            vertices.push_back(u_tex);
            vertices.push_back((float)k); // Pass index directly as float for shader matching
        }
    }

    // Interleave index quad strip rows
    for (int k = 0; k < _nlev - 1; ++k) {
        for (int i = 0; i < horizontalSize; ++i) {
            indices.push_back(k * horizontalSize + i);
            indices.push_back((k + 1) * horizontalSize + i);
        }
        indices.push_back(0xFFFFFFFF); // Restart strip
    }

    if (crossVBO == 0) f->glGenBuffers(1, &crossVBO);
    if (crossEBO == 0) f->glGenBuffers(1, &crossEBO);

    f->glBindBuffer(GL_ARRAY_BUFFER, crossVBO);
    f->glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    f->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, crossEBO);
    f->glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    f->glBindBuffer(GL_ARRAY_BUFFER, 0);
    f->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void UFS2dViewer::_display_Xflat_plane_GPU(int xs)
{
    QOpenGLFunctions_3_3_Core *f = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>();
    if (!f) return;

    int i = xs - 1;

    // 1. Re-initialize coordinate layouts on demand
    _initCrossStaticGrid(_nlat);
    _initCrossShaders();

    // 2. Extract and pack the slice data from the 3D volume into a clean 2D slice buffer
    std::vector<float> sliceData(_nlat * _nlev);
    for (int k = 0; k < _nlev; ++k) {
        for (int j = 0; j < _nlat; ++j) {
            size_t mpos = (static_cast<size_t>(k) * _nlat + j) * _nlon;
            sliceData[k * _nlat + j] = pltvar[mpos + i];
        }
    }

    // 3. Stream data texture to GPU allocation
    if (crossDataTex == 0) {
        f->glGenTextures(1, &crossDataTex);
    }
    f->glBindTexture(GL_TEXTURE_2D, crossDataTex);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    f->glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, _nlat, _nlev, 0, GL_RED, GL_FLOAT, sliceData.data());
    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // 4. Capture current height layers into float precision vectors
    std::vector<float> fHeights(_nlev);
    for (int k = 0; k < _nlev; ++k) fHeights[k] = static_cast<float>(_k2h(k));

    // 5. Update pipeline state overrides
    glDisable(GL_LIGHTING);
    glDisable(GL_CULL_FACE);
    glDisable(GL_TEXTURE_2D);

    f->glActiveTexture(GL_TEXTURE0);
    f->glBindTexture(GL_TEXTURE_2D, crossDataTex);

    f->glActiveTexture(GL_TEXTURE1);
    f->glBindTexture(GL_TEXTURE_1D, colorMapTexture);

    crossShaderProgram->bind();
    crossShaderProgram->setUniformValue("u_SliceCoord", static_cast<float>(_xFlat[i]));
    crossShaderProgram->setUniformValue("u_IsXSlice", 1);

    // --- ADD BOUNDARY UNIFORMS TO STOP THE HORIZONTAL STRETCH OVERFLOW ---
    crossShaderProgram->setUniformValue("u_MinCoord", static_cast<float>(_yFlat[0]));
    crossShaderProgram->setUniformValue("u_MaxCoord", static_cast<float>(_yFlat[_nlat - 1]));
    // ---------------------------------------------------------------------

    crossShaderProgram->setUniformValue("u_ValMin", static_cast<float>(_valmin));
    crossShaderProgram->setUniformValue("u_ValMax", static_cast<float>(_valmax));
    crossShaderProgram->setUniformValue("u_SliceDataTex", 0);
    crossShaderProgram->setUniformValue("u_ColorMap", 1);
    crossShaderProgram->setUniformValueArray("u_Heights", fHeights.data(), _nlev, 1);

    // 6. Draw the complete cross-section plane instantaneously
    f->glBindBuffer(GL_ARRAY_BUFFER, crossVBO);
    f->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, crossEBO);

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 2 * sizeof(float), (void*)0);

    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer(2, GL_FLOAT, 2 * sizeof(float), (void*)0);

    f->glEnable(GL_PRIMITIVE_RESTART);
    f->glPrimitiveRestartIndex(0xFFFFFFFF);

    // Dynamic element draw based on index count size calculations
    GLsizei localIndexCount = (_nlev - 1) * (_nlat * 2 + 1);
    f->glDrawElements(GL_TRIANGLE_STRIP, localIndexCount, GL_UNSIGNED_INT, (void*)0);

    f->glDisable(GL_PRIMITIVE_RESTART);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    f->glBindBuffer(GL_ARRAY_BUFFER, 0);
    f->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    crossShaderProgram->release();
}

void UFS2dViewer::_display_Yflat_plane_GPU(int ys)
{
    QOpenGLFunctions_3_3_Core *f = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>();
    if (!f) return;

    int j = ys - 1;
    if ((-85.0 > _lat[j]) || (85.0 < _lat[j])) return;

    // 1. Re-initialize assets
    _initCrossStaticGrid(_nlon);
    _initCrossShaders();

    // 2. Extract data slice and account for the horizontal _hlon shift check
    std::vector<float> sliceData(_nlon * _nlev);
    for (int k = 0; k < _nlev; ++k) {
        size_t mpos = (static_cast<size_t>(k) * _nlat + j) * _nlon;
        int idx = 0;

        // First part: from _hlon to end
        for (int i = _hlon; i < _nlon; ++i) {
            sliceData[k * _nlon + idx] = pltvar[mpos + i];
            idx++;
        }
        // Second part: from 0 wrap-around to _hlon
        for (int i = 0; i < _hlon; ++i) {
            sliceData[k * _nlon + idx] = pltvar[mpos + i];
            idx++;
        }
    }

    // 3. Upload data texture to target GPU slot
    if (crossDataTex == 0) {
        f->glGenTextures(1, &crossDataTex);
    }
    f->glBindTexture(GL_TEXTURE_2D, crossDataTex);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    f->glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, _nlon, _nlev, 0, GL_RED, GL_FLOAT, sliceData.data());
    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // 4. Capture heights array
    std::vector<float> fHeights(_nlev);
    for (int k = 0; k < _nlev; ++k) fHeights[k] = static_cast<float>(_k2h(k));

    // 5. Setup shader uniforms
    glDisable(GL_LIGHTING);
    glDisable(GL_CULL_FACE);
    glDisable(GL_TEXTURE_2D);

    f->glActiveTexture(GL_TEXTURE0);
    f->glBindTexture(GL_TEXTURE_2D, crossDataTex);

    f->glActiveTexture(GL_TEXTURE1);
    f->glBindTexture(GL_TEXTURE_1D, colorMapTexture);

    crossShaderProgram->bind();
    crossShaderProgram->setUniformValue("u_SliceCoord", static_cast<float>(_yFlat[j]));
    crossShaderProgram->setUniformValue("u_IsXSlice", 0); // Explicitly zero out for Y-Plane tracking layout

    // --- ADD BOUNDARY UNIFORMS MATCHING YOUR FLAT HORIZONTAL ASPECT SIZE ---
    crossShaderProgram->setUniformValue("u_MinCoord", -1.0f);
    crossShaderProgram->setUniformValue("u_MaxCoord", 1.0f);
    // ----------------------------------------------------------------------

    crossShaderProgram->setUniformValue("u_ValMin", static_cast<float>(_valmin));
    crossShaderProgram->setUniformValue("u_ValMax", static_cast<float>(_valmax));
    crossShaderProgram->setUniformValue("u_SliceDataTex", 0);
    crossShaderProgram->setUniformValue("u_ColorMap", 1);
    crossShaderProgram->setUniformValueArray("u_Heights", fHeights.data(), _nlev, 1);

    // 6. Push data elements array down the pipeline loop instantly
    f->glBindBuffer(GL_ARRAY_BUFFER, crossVBO);
    f->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, crossEBO);

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 2 * sizeof(float), (void*)0);

    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer(2, GL_FLOAT, 2 * sizeof(float), (void*)0);

    f->glEnable(GL_PRIMITIVE_RESTART);
    f->glPrimitiveRestartIndex(0xFFFFFFFF);

    GLsizei localIndexCount = (_nlev - 1) * (_nlon * 2 + 1);
    f->glDrawElements(GL_TRIANGLE_STRIP, localIndexCount, GL_UNSIGNED_INT, (void*)0);

    f->glDisable(GL_PRIMITIVE_RESTART);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    f->glBindBuffer(GL_ARRAY_BUFFER, 0);
    f->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    crossShaderProgram->release();
}

