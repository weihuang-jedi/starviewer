// #include <GL/glut.h>
#include "ufsincr_viewer.h"

UFSINCR2dViewer::UFSINCR2dViewer(ColorTable *ct, NVOptions* opt)
{
    colorTable = ct;
    nvoptions = opt;

    texture1d = new Texture1d();
    texture1d->set_colors(ct->get_clen(), ct->get_cmap());
    texture1d->set_name(ct->get_name());

    earth = new Earth();
    _needActivateEarth = false;

    nvoptions->set_xsec(0);
    nvoptions->set_ysec(0);
    nvoptions->set_zsec(0);

    _nlon = 360;
    _nlat = 180;
    _nlev = 1;
    _ntiles = 1;

    oneover = 1.0 / 180.0;
    deg2rad = 3.1415926535897932 * oneover;

    lister = new Lister();
    lister->setup(361, 181, 121);

    locator = NULL;

    previoustimelevel = -1;
    current_timelevel = 0;
}

UFSINCR2dViewer::UFSINCR2dViewer(ColorTable *ct, NVOptions* opt,
		                 const char* bmpflnm,
				 vector<UFSIncrementReader*> nchandler)
{
    colorTable = ct;
    nvoptions = opt;

    // cout << "\nEnter functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    texture1d = new Texture1d();
    texture1d->set_colors(ct->get_clen(), ct->get_cmap());
    texture1d->set_name(ct->get_name());

    // cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    ncfile = nchandler;
    // cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    // cout << "\tbmpflnm: <" << bmpflnm << ">" << endl;
    strcpy(_bmpflnm, bmpflnm);

    // cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    nvoptions->set_xsec(0);
    nvoptions->set_ysec(0);
    nvoptions->set_zsec(0);

    _nlon = 360;
    _nlat = 180;
    _ntiles = ncfile.size();

    _var.resize(_ntiles);
    pltvar.resize(_ntiles);

    oneover = 1.0 / 180.0;
    deg2rad = 3.1415926535897932 * oneover;

    // cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    lister = new Lister();
    lister->setup(361, 181, 121);

    locator = NULL;

    previoustimelevel = -1;
    current_timelevel = 0;
  
    _needActivateEarth = true;
    earth = new Earth(_bmpflnm);
    _needActivateEarth = false;
 
    // cout << "Leave functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
}

UFSINCR2dViewer::~UFSINCR2dViewer()
{
    locator->turnOff();

    delete earth;
    delete lister;
    delete texture1d;
}

void UFSINCR2dViewer::set_geometry(vector<UFSINCRGeometry*> gm)
{
    int n;
    // cout << "\nEnter" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
    // cout << "gm.size(): " << gm.size() << endl;
    geometry.resize(gm.size());
    for(n=0; n<_ntiles; ++n)
    {
	geometry[n] = gm[n];
    }
    _initialize();
    // cout << "Leave" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
}

void UFSINCR2dViewer::setup(string vn, vector<float*> var)
{
    reset();

    _varname  = vn;
    _var = var;
    _nlev = ncfile[0]->getNz();

  //nvoptions->set_xsec(_nlon);
  //nvoptions->set_ysec(_nlat);
  //nvoptions->set_zsec(_nlev);

    nvoptions->set_xsec(0);
    nvoptions->set_ysec(0);
    nvoptions->set_zsec(0);

    _evaluate(_var);

    previoustimelevel = -1;
}

void UFSINCR2dViewer::reset()
{
    lister->reinitialize(_nlon+1, _nlat+1, _nlev+1);
}

void UFSINCR2dViewer::_initialize()
{
    int n;
    double* xs;
    double* ys;
    double* zs;

    // cout << "\nEnter" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
    // cout << "\t_ntiles: " << _ntiles << endl;
    previoustimelevel = -1;

    _nlon = geometry[0]->getNlon();
    _nlat = geometry[0]->getNlat();
    _nlev = ncfile[0]->getNz();

    // cout << "\t" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
    // cout << "\t_nlon: " << _nlon << ", _nlat: " << _nlat << ", _nlev: " << _nlev << endl;

  //lister->reinitialize(361, 181, _nlev);

    _lon.resize(_ntiles);
    _lat.resize(_ntiles);
    _xFlat.resize(_ntiles);
    _yFlat.resize(_ntiles);
    _xSphere.resize(_ntiles);
    _ySphere.resize(_ntiles);
    _zSphere.resize(_ntiles);

    // cout << "\t" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
    for(n=0; n<_ntiles; ++n)
    {
        _lon[n] = geometry[n]->get_geolon();
        _lat[n] = geometry[n]->get_geolat();
        // _lev = geometry->get_lev();

        _xFlat[n] = geometry[n]->get_xFlat();
        _yFlat[n] = geometry[n]->get_yFlat();

        // _xSphere[n] = geometry[n]->get_xSphere();
        // _ySphere[n] = geometry[n]->get_ySphere();
        // _zSphere[n] = geometry[n]->get_zSphere();
        xs = geometry[n]->get_xSphere();
        ys = geometry[n]->get_ySphere();
        zs = geometry[n]->get_zSphere();

        _xSphere[n] = xs;
        _ySphere[n] = ys;
        _zSphere[n] = zs;

        // cout << "\tn = " << n << endl;
        // cout << "\tlon = " << _lon[n][0] << ", lat = " << _lat[n][0] << endl;
        // cout << "\txf = " << _xFlat[n][0] << ", yf = " << _yFlat[n][0] << endl;
        // cout << "\txs = " << _xSphere[n][0] << ", ys = " << _ySphere[n][0] << ", zs = " << _zSphere[n][0] << endl;
        // cout << "\txs = " << xs[0]<< ", ys = " << ys[0] << ", zs = " << zs[0] << endl;
    }
    // cout << "Leave " << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
}

void UFSINCR2dViewer::draw()
{
    size_t nsquare = _nlon * _nlat;

    // cout << "\nEnter" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;

    // Do your options state configurations safely on the CPU
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
    if(current_timelevel >= 1)
        return;

    if((_nlev <= nvoptions->get_zsec()) && (0 > nvoptions->get_zsec()))
        return;

    // cout << "\t" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
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
    // cout << "\t" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
    // cout << "\t" << "current_timelevel: " << current_timelevel << endl;
    // cout << "\t" << "_nlon: " << _nlon << endl;
    // cout << "\t" << "_nlat: " << _nlat << endl;
    // cout << "\t" << "_nlev: " << _nlev << endl;
    // cout << "\t" << "_ntiles: " << _ntiles << endl;
    // cout << "\t" << "_var.size(): " << _var.size() << endl;
    // cout << "\t" << "pltvar.size(): " << pltvar.size() << endl;

  //pltvar = &_var[current_timelevel * _nlon * _nlat];
    for(int n=0; n<_ntiles; ++n)
    {
        pltvar[n] = &_var[n][current_timelevel * _nlon * _nlat * _nlev];
    }

    // cout << "\t" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
    zcl = lister->get_zid(nvoptions->get_zsec());
    ycl = lister->get_yid(nvoptions->get_ysec());
    xcl = lister->get_xid(nvoptions->get_xsec());

    // cout << "\t" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
    // Set the color first
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    // cout << "\t" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
    // Then wipe the buffer canvas
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // cout << "\t" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;

    if(nvoptions->get_cb(NV_BUMPON))
    {
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
                if(zcl)
                    glCallList(zcl);
                else
                    _flatDisplay();
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
                    _sphereDisplay();
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

#if 0
    // Initialize QPainter to overlay 2D Text
    QPainter painter(this);
    painter.setPen(Qt::yellow); 
    painter.setFont(QFont("Helvetica", 12, QFont::Bold));
        
    // Optional: Turn on antialiasing for crisp text geometry edges
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

    // Draw text using screen-space coordinates (X, Y from top-left)
    QString frameLabel = _varname.c_str();
    painter.drawText(20, 40, frameLabel); 

    painter.end(); // Always terminate the painter state pass explicitly

    glClear(GL_COLOR_BUFFER_BIT);

    // Set text color using standard glColor hooks
    glColor3f(0.0f, 1.0f, 1.0f); // Cyan Text

    // Common FreeGLUT font selections:
    // - GLUT_BITMAP_HELVETICA_18
    // - GLUT_BITMAP_TIMES_ROMAN_24
    // - GLUT_BITMAP_9_BY_15
    renderBitmapString(-0.9f, 0.8f, GLUT_BITMAP_HELVETICA_18, _varname.c_str());

    glutSwapBuffers();
#endif
    glFlush();             // Force the command queue to execute
}

#if 0
void UFSINCR2dViewer::renderBitmapString(float x, float y, void *font, const string &str)
{
    // 1. Define the exact world-space starting anchor point for the text
    glRasterPos2f(x, y);

    // 2. Loop through every character in the string vector array
    for (char c : str) {
        glutBitmapCharacter(font, c);
    }
}
#endif

void UFSINCR2dViewer::_flatVertex(double x, double y, double z,
                                  double fact)
{
    double alpha = 1.05 * fact;
    if(alpha < 0.1)
        alpha = 0.0;
    else if(alpha > 1.0)
        alpha = 1.0;

    // glColor4d(fact, fact, fact, alpha);
    if(fact > 0.0)
    {
        if(fact > 1.0)
           fact = 1.0;
        glColor4d(fact, 0.0, 0.0, alpha);
    }
    else
    {
        if(fact < -1.0)
           fact = -1.0;
        glColor4d(0.0, 0.0, -fact, alpha);
    }
    glNormal3d(x, y, z);
    glVertex3d(x, y, z);
}

void UFSINCR2dViewer::_flatVertex_texture(double x, double y, double z,
                                          double fact)
{
    glTexCoord1d(fact);
    glNormal3f(x, y, z);
    glVertex3d(x, y, z);
}

void UFSINCR2dViewer::_sphereVertex(double x, double y, double z,
		                    double radius, double fact)
{
    double alpha = 1.05 * abs(fact);
    if(alpha < 0.1)
        alpha = 0.0;
    else if(alpha > 1.0)
        alpha = 1.0;

    // glColor4d(fact, fact, fact, alpha);
    if(fact > 0.0)
    {
	if(fact > 1.0)
	   fact = 1.0;
        glColor4d(fact, 0.0, 0.0, alpha);
    }
    else
    {
	if(fact < -1.0)
	   fact = -1.0;
        glColor4d(0.0, 0.0, -fact, alpha);
    }
    glNormal3d(x, y, z);
    glVertex3d(x * radius, y * radius, z * radius);
}

void UFSINCR2dViewer::_sphereVertex_texture(double x, double y, double z,
		                            double radius, double fact)
{
    if(fact > 1.0)
        fact = 1.0;
    else if(fact < 0.0)
        fact = 0.0;
    glTexCoord1d(fact);
    glNormal3f(x, y, z);
    glVertex3d(x * radius, y * radius, z * radius);
}

void UFSINCR2dViewer::_sphereDisplay()
{
    int i, j, k, k1, n;
    size_t mpos, npos;
    size_t mgeo, ngeo;

    double sv = 1.0;
    double fact;
    double radius = 1.001;

    float adjust_factor = 8.0;
    float adjust_minval = _valmin/adjust_factor;

    // cout << "\nEnter functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    k1 = nvoptions->get_zsec()+1;
    k = _nlev-k1;
    radius = _k2r(k);
    sv = adjust_factor / (_valmax - _valmin);

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

    // cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    // cout << "\tk = " << k << ", radius = " << radius << endl;
    // cout << "\t_valmin = " << _valmin << ", _valmax = " << _valmax << endl;
  //#pragma omp parallel for
    if(k < _nlev || 1 == _nlev)
    {
      for(n = 0; n < _ntiles; ++n)
      {
        for(j = 1; j < _nlat; ++j)
        {
          mpos = (k*_nlat + (j-1))*_nlon;
          npos = (k*_nlat + j)*_nlon;
          mgeo = (j-1)*_nlon;
          ngeo = j*_nlon;
          glBegin(GL_QUAD_STRIP);
          for(i = 0; i < _nlon; ++i)
          {
            fact = sv * (pltvar[n][npos+i] - adjust_minval);
            _sphereVertex_texture(_xSphere[n][ngeo+i], _ySphere[n][ngeo+i],
			          _zSphere[n][ngeo+i], radius, fact);

            fact = sv * (pltvar[n][mpos+i] - adjust_minval);
            _sphereVertex_texture(_xSphere[n][mgeo+i], _ySphere[n][mgeo+i],
                                  _zSphere[n][mgeo+i], radius, fact);
          }
          fact = sv * (pltvar[n][npos] - adjust_minval);
          _sphereVertex_texture(_xSphere[n][ngeo], _ySphere[n][ngeo],
                                _zSphere[n][ngeo], radius, fact);

          fact = sv * (pltvar[n][mpos] - adjust_minval);
          _sphereVertex_texture(_xSphere[n][mgeo], _ySphere[n][mgeo],
                                _zSphere[n][mgeo], radius, fact);
          glEnd();
        }
      }
      coastline->drawOnSphere(radius+0.01);
    }

    glDisable(GL_TEXTURE_1D);
    glPopMatrix();
    glEndList();
    // cout << "Leave functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
}

void UFSINCR2dViewer::_flatDisplay()
{
    int i, j, k, k1, n;
    size_t mpos, npos;
    size_t mgeo, ngeo;
    double sv = 1.0;
    double fact;
    double height = 0.0;

    float adjust_factor = 8.0;
    float adjust_minval = _valmin/adjust_factor;

    k1 = nvoptions->get_zsec()+1;
    k = _nlev-k1;
    height = _k2h(k);
    sv = adjust_factor / (_valmax - _valmin);

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

    if(k < _nlev || 1 == _nlev)
    {
      for(n = 0; n < _ntiles; ++n)
      {
        for(j = 1; j < _nlat; ++j)
        {
          mpos = (k*_nlat+(j-1))*_nlon;
          npos = (k*_nlat+j)*_nlon;
          mgeo = (j-1)*_nlon;
          ngeo = j*_nlon;
          glBegin(GL_QUAD_STRIP);
          for(i = 0; i < _nlon; ++i)
          {
            fact = sv * (pltvar[n][npos+i] - adjust_minval);
	    _flatVertex_texture(_xFlat[n][ngeo+i], _yFlat[n][ngeo+i],
			        height, fact);

            fact = sv * (pltvar[n][mpos+i] - adjust_minval);
	    _flatVertex_texture(_xFlat[n][mgeo+i], _yFlat[n][mgeo+i],
			        height, fact);
          }
          glEnd();
        }
      }
      coastline->drawOnPlane(height+0.01);
    }

    glDisable(GL_TEXTURE_1D);
    glPopMatrix();
    glEndList();
}

void UFSINCR2dViewer::_evaluate(vector<float*> var)
{
    size_t varsize;
    size_t i = 0;
    size_t n = 0;
    float total;

  //cout << "Enter functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
  //cout << "\t _varname: " << _varname << endl;

    varsize = _nlon * _nlat * _nlev;

  //cout << "\tin <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
  //cout << "\t_nlon =" << _nlon << endl;
  //cout << "\t_nlat =" << _nlat << endl;
  //cout << "\t_nlev =" << _nlev << endl;
  //cout << "\tvarsize =" << varsize << ", _nlon*_nlat=" << _nlon*_nlat << endl;

    _valmax = var[0][0];
    _valmin = var[0][0];
    total = var[0][0];

    for(n = 0; n < _ntiles; ++n)
    {
      for(i = 1; i < varsize; ++i)
      {
        if(_valmax < var[n][i])
        {
           _valmax = var[n][i];
        }
        if(_valmin > var[n][i])
        {
           _valmin = var[n][i];
        }
        total += var[n][i];
      }
    }

    if(1.0e-10 > (_valmax - _valmin))
       _valmax += 1.0e-10;

    _valavg = total / varsize;
    _scalelength = _valmax;
    if(_scalelength > abs(_valmin))
	_scalelength = abs(_valmin);

  //cout << "\t_valmin = " << _valmin << ", _valavg = " << _valavg << ", _valmax = " << _valmax << endl;

#if 0
    nvoptions->set_trueminimum(_valmin);
    nvoptions->set_truemaximum(_valmax);
#endif
  //cout << "Leave functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
}

void UFSINCR2dViewer::_adjust_minmax(vector<float*> var)
{
    size_t varsize;
    size_t i = 0;
    size_t n = 0;
    float total;

  //cout << "Enter functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;

    varsize = _nlon * _nlat;

    _valmax = var[0][0];
    _valmin = var[0][0];
    total = var[0][0];

    for(n = 0; n < _ntiles; ++n)
    {
      for(i = 1; i < varsize; ++i)
      {
        if(_valmax < var[n][i])
        {
           _valmax = var[n][i];
        }
        if(_valmin > var[n][i])
        {
           _valmin = var[n][i];
        }
        total += var[n][i];
      }
    }

    if(1.0e-10 > (_valmax - _valmin))
       _valmax += 1.0e-10;

    _valavg = total / varsize;
    _scalelength = _valmax;
    if(_scalelength > abs(_valmin))
	_scalelength = abs(_valmin);

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

void UFSINCR2dViewer::reset_texture1d(ColorTable *ct)
{
    colorTable = ct;

    glDisable(GL_TEXTURE_1D);

    texture1d->reset();
    texture1d->set_colors(ct->get_clen(), ct->get_cmap());
    texture1d->set_name(ct->get_name());
}

void UFSINCR2dViewer::draw_sphere_grids()
{
    int i, j, k, n;
    size_t npos;

    double radius = 1.001;

    GLfloat line_width = 1.0;

  //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\tncenters = " << ncenters << endl;

    k = nvoptions->get_zsec();
    radius = _k2r(k);

    glPushMatrix();

  //earth->draw();

    glColor4f(0.0, 1.0, 1.0, 1.0);
  //glNormal3f(0.0, 0.0, -1.0);

    glLineWidth(line_width);

  //#pragma omp parallel for
    for(n = 0; n < _ntiles; ++n)
    {
      for(j = 0; j < _nlat; ++j)
      {
        npos = j*_nlon;

        glBegin(GL_LINE_STRIP);
        for(i = 0; i < _nlon; ++i)
        {
            glVertex3d(radius * _xSphere[n][npos+i], radius * _ySphere[n][npos+i], radius * _zSphere[n][npos+i]);
        }
        glEnd();
      }
    }

    glPopMatrix();
}

void UFSINCR2dViewer::draw_plane_grids()
{
    int i, j, n;
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

    for(n = 0; n < _ntiles; ++n)
    {
      for(j = 0; j < _nlat; ++j)
      {
        npos = j*_nlon;
        glBegin(GL_LINE_STRIP);
        for(i = 0; i < _nlon; ++i)
        {
            glVertex3d(_xFlat[n][npos+i], _yFlat[n][npos+i], height);
        }
        glEnd();
      }
    }

    glPopMatrix();
}

void UFSINCR2dViewer::_display_Yflat_plane(int ys)
{
    int i, k, n;
    size_t mpos, npos, ngeo;
    double fact;
    double sv = 1.0;
    vector<double> height(_nlev);
    int j = ys-1;

    // if((-85.0 > _lat[j]) || (85.0 < _lat[j]))
    //    return;

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

    for(n = 0; n < _ntiles; ++n)
    {
      for(k = 1; k < _nlev; ++k)
      {
        mpos = ((k-1)*_nlat+j)*_nlon;
        npos = (k*_nlat+j)*_nlon;
        ngeo = j*_nlon;

        glBegin(GL_QUAD_STRIP);
        for(i = 0; i < _nlon; ++i)
        {
            fact = sv * (pltvar[n][mpos+i] - _valmin);
	    _flatVertex_texture(_xFlat[n][ngeo+i], _yFlat[n][ngeo+i],
			        height[k-1], fact);

            fact = sv * (pltvar[n][npos+i] - _valmin);
	    _flatVertex_texture(_xFlat[n][ngeo+i], _yFlat[n][ngeo+i],
			        height[k], fact);
        }
        glEnd();
      }
    }

    glDisable(GL_TEXTURE_1D);
    glPopMatrix();
    glEndList();
}

void UFSINCR2dViewer::_sphereXplane(int xs)
{
    int j, k, n;
    size_t mpos, npos, ngeo;
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
    for(n = 0; n < _ntiles; ++n)
    {
      for(k = 1; k < _nlev; ++k)
      {
        glBegin(GL_QUAD_STRIP);
        for(j = 0; j < _nlat; ++j)
        {
            mpos = ((k-1)*_nlat + j)*_nlon;
            npos = (k*_nlat + j)*_nlon;
            ngeo = j*_nlon;

            fact = sv * (pltvar[n][mpos+i] - _valmin);
            _sphereVertex_texture(_xSphere[n][ngeo+i], _ySphere[n][ngeo+i],
                                  _zSphere[n][ngeo+i], radius[k], fact);

            fact = sv * (pltvar[n][npos+i] - _valmin);
            _sphereVertex_texture(_xSphere[n][ngeo+i], _ySphere[n][ngeo+i],
                                  _zSphere[n][ngeo+i], radius[k-1], fact);
        }
        glEnd();
      }
    }

    glDisable(GL_TEXTURE_1D);
    glPopMatrix();
    glEndList();
}

void UFSINCR2dViewer::_sphereYplane(int ys)
{
    int i, k, n;
    size_t mpos, npos, ngeo;
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
    for(n = 0; n < _ntiles; ++n)
    {
      for(k = 1; k < _nlev; ++k)
      {
        mpos = ((k-1)*_nlat + j)*_nlon;
        npos = (k*_nlat + j)*_nlon;
        ngeo = j*_nlon;
        glBegin(GL_QUAD_STRIP);
        for(i = 0; i < _nlon; ++i)
        {
            fact = sv * (pltvar[n][npos+i] - _valmin);
            _sphereVertex_texture(_xSphere[n][ngeo+i], _ySphere[n][ngeo+i],
                                  _zSphere[n][ngeo+i], radius[k], fact);

            fact = sv * (pltvar[n][mpos+i] - _valmin);
            _sphereVertex_texture(_xSphere[n][ngeo+i], _ySphere[n][ngeo+i],
                                  _zSphere[n][ngeo+i], radius[k], fact);
        }
        fact = sv * (pltvar[n][npos] - _valmin);
        _sphereVertex_texture(_xSphere[n][ngeo], _ySphere[n][ngeo],
                                  _zSphere[n][ngeo], radius[k], fact);

        fact = sv * (pltvar[n][mpos] - _valmin);
        _sphereVertex_texture(_xSphere[n][ngeo], _ySphere[n][ngeo],
                              _zSphere[n][ngeo], radius[k], fact);
        glEnd();
      }
    }

    glDisable(GL_TEXTURE_1D);
    glPopMatrix();
    glEndList();
}


void UFSINCR2dViewer::_display_Xflat_plane(int xs)
{
    int j, k, n;
    size_t mpos, npos, ngeo;
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

    for(n = 0; n < _ntiles; ++n)
    {
      for(k = 1; k < _nlev; ++k)
      {
        glBegin(GL_QUAD_STRIP);
        for(j = 0; j < _nlat; ++j)
        {
            mpos = ((k-1)*_nlat+j)*_nlon;
            npos = (k*_nlat+j)*_nlon;
            ngeo = j*_nlon;

            fact = sv * (pltvar[n][mpos+i] - _valmin);
	    _flatVertex_texture(_xFlat[n][ngeo+i], _yFlat[n][ngeo+i],
			        height[k-1], fact);

            fact = sv * (pltvar[n][npos+i] - _valmin);
	    _flatVertex_texture(_xFlat[n][ngeo+i], _yFlat[n][ngeo+i],
			        height[k], fact);
        }
        glEnd();
      }
    }

    glDisable(GL_TEXTURE_1D);
    glPopMatrix();
    glEndList();
}

void UFSINCR2dViewer::_sphereBump()
{
    int i, j, k, k1, n;
    size_t mpos, npos, mgeo, ngeo;
    double sv = 1.0;
    double alpha, fact;
    double amp = 1.05;
    double magnifier = 0.125;
    double radius = 1.0;

    float adjust_factor = 16.0;

    k1 = nvoptions->get_zsec()+1;
    k = _nlev-k1;
  //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\t_varname: <" << _varname << ">, lev = " << k << endl;

    // sv = 1.0 / (_valmax - _valmin);
    sv = adjust_factor / _scalelength;

    zcl = glGenLists(1);
  //glNewList(zcl, GL_COMPILE);
    glNewList(zcl, GL_COMPILE_AND_EXECUTE);
    lister->set_zid(k, zcl);

  //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\t_valmin = " << _valmin << ", _valmax = " << _valmax << ", sv = " << sv << endl;
  //cout << "\tzcl = " << zcl << ", k = " << k << endl;

  //_adjust_minmax(&pltvar[n][k*_nlat*_nlon]);

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

    if(k < _nlev || 1 == _nlev)
    {
      for(n = 0; n < _ntiles; ++n)
      {
        for(j = 1; j < _nlat; ++j)
        {
          mpos = (k*_nlat + (j-1))*_nlon;
          npos = (k*_nlat + j)*_nlon;
          mgeo = (j-1)*_nlon;
          ngeo = j*_nlon;
          glBegin(GL_QUAD_STRIP);
          for(i = 0; i < _nlon; ++i)
          {
            fact = sv * pltvar[n][npos+i];
            _sphereVertex(_xSphere[n][ngeo+i], _ySphere[n][ngeo+i],
                          _zSphere[n][ngeo+i], radius, fact);

            fact = sv * pltvar[n][mpos+i];
            _sphereVertex(_xSphere[n][mgeo+i], _ySphere[n][mgeo+i],
                          _zSphere[n][mgeo+i], radius, fact);
          }
          fact = sv * pltvar[n][npos];
          _sphereVertex(_xSphere[n][ngeo], _ySphere[n][ngeo],
                        _zSphere[n][ngeo], radius, fact);

          fact = sv * pltvar[n][mpos];
          _sphereVertex(_xSphere[n][mgeo], _ySphere[n][mgeo],
                        _zSphere[n][mgeo], radius, fact);
          glEnd();
        }
      }
      // coastline->drawOnSphere(radius+0.01);
    }

    glPopMatrix();
    glEndList();
}

void UFSINCR2dViewer::_flatBump()
{
    int i, j, k, k1, n;
    size_t mpos, npos, mgeo, ngeo;
    double sv = 1.0;
    double alpha, fact;
    double amp = 1.05;
    double magnifier = 0.125;
    double rlat;

    float adjust_factor = 16.0;
    float adjust_minval = _valmin/adjust_factor;

    k1 = nvoptions->get_zsec()+1;
    k = _nlev-k1;

  //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\t_varname: <" << _varname << ">, lev = " << k << endl;

    // sv = 1.0 / (_valmax - _valmin);
    sv = adjust_factor / (_valmax - _valmin);

    zcl = glGenLists(1);
  //glNewList(zcl, GL_COMPILE);
    glNewList(zcl, GL_COMPILE_AND_EXECUTE);
    lister->set_zid(k, zcl);

  //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\t_valmin = " << _valmin << ", _valmax = " << _valmax << ", sv = " << sv << endl;
  //cout << "\tzcl = " << zcl << ", k = " << k << endl;

  //_adjust_minmax(&pltvar[n][k*_nlat*_nlon]);

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
    if(k < _nlev || 1 == _nlev)
    {
      for(n = 0; n < _ntiles; ++n)
      {
        for(j = 1; j < _nlat; ++j)
        {
          mpos = (k*_nlat+(j-1))*_nlon;
          npos = (k*_nlat+j)*_nlon;
          mpos = (j-1)*_nlon;
          npos = j*_nlon;
          glBegin(GL_QUAD_STRIP);
          for(i = 0; i < _nlon; ++i)
          {
            fact = sv * pltvar[n][npos+i];
	    _flatVertex(_xFlat[n][ngeo+i], _yFlat[n][ngeo+i], magnifier*fact, fact);

            fact = sv * pltvar[n][mpos+i];
	    _flatVertex(_xFlat[n][mgeo+i], _yFlat[n][mgeo+i], magnifier*fact, fact);
          }
          glEnd();
        }
      }
      coastline->drawOnPlane(0.01);
    }

    glPopMatrix();
    glEndList();
}

void UFSINCR2dViewer::_draw_cross(double radius)
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

double UFSINCR2dViewer::_k2h(int k)
{
    double height = 0.5 * ((double) (_nlev-k) / _nlev);
    return height;
}

double UFSINCR2dViewer::_k2r(int k)
{
    double radius = 1.0 + _k2h(k);
    return radius;
}

