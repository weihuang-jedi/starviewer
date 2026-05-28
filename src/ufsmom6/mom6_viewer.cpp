#include <QtOpenGL>

#include <vector>

#include "mom6_viewer.h"

MOM62dViewer::MOM62dViewer(ColorTable *ct, NVOptions* opt)
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

    _nxh = 360;
    _nyh = 180;
    _nzl = 1;
    _missing_value = -1e+34;
    _half_missing_value = 0.5*_missing_value;

    oneover = 1.0 / 180.0;
    deg2rad = 3.1415926535897932 * oneover;

    lister = new Lister();
    lister->setup(361, 181, 121);

    locator = NULL;

    previoustimelevel = -1;
    current_timelevel = 0;
}

MOM62dViewer::MOM62dViewer(ColorTable *ct, NVOptions* opt, const char* bmpflnm, UFSMOM6Reader* nchandler)
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

    _nxh = 360;
    _nyh = 180;
    _missing_value = -1e+34;
    _half_missing_value = 0.5*_missing_value;

    oneover = 1.0 / 180.0;
    deg2rad = 3.1415926535897932 * oneover;

    lister = new Lister();
    lister->setup(361, 181, 32);

    locator = NULL;

    previoustimelevel = -1;
    current_timelevel = 0;
}

MOM62dViewer::~MOM62dViewer()
{
    locator->turnOff();

    delete earth;
    delete lister;
    delete texture1d;
}

void MOM62dViewer::set_geometry(MOM6Geometry *gm)
{
    geometry = gm;

    _initialize();
}

void MOM62dViewer::setup(string vn, float *var)
{
    reset();

    _varname  = vn;
    _var = var;
    _nzl = geometry->get_nzl();
  //nvoptions->set_xsec(_nxh);
  //nvoptions->set_ysec(_nyh);
  //nvoptions->set_zsec(_nzl);

    nvoptions->set_xsec(0);
    nvoptions->set_ysec(0);
    nvoptions->set_zsec(0);

    _evaluate(_var);

    previoustimelevel = -1;
}

void MOM62dViewer::reset()
{
    lister->reinitialize(_nxh+1, _nyh+1, geometry->get_nzl()+1);
}

void MOM62dViewer::_initialize()
{
    int i, j, k;
    size_t ngeo;

    previoustimelevel = -1;

    _nxsp = geometry->get_nxsp();
    _nxh = geometry->get_nxh();
    _nyh = geometry->get_nyh();
    _nzl = geometry->get_nzl();

    _zl = ncfile->getZl();

    for(k=0; k<_nzl; ++k)
	cout << "_zl[" << k << "]=" << _zl[k] << endl;

  //lister->reinitialize(361, 181, _nzl);

    _xSphere = geometry->get_xSphere();
    _ySphere = geometry->get_ySphere();
    _zSphere = geometry->get_zSphere();

    _xFlat = geometry->get_xFlat();
    _yFlat = geometry->get_yFlat();

    geometry->set_ntim(1);
}

void MOM62dViewer::draw()
{
    size_t nsquare = _nxh * _nyh;

    // cout << "\nEnter " << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
    if(nvoptions->get_cb(NV_RESET))
    {
        nvoptions->set_cb(NV_RESET, false);
        if(nvoptions->get_cb(NV_BUMPON))
            texture1d->set_gradient_opacity();
        else
            texture1d->set_opacity(1.0);
        reset();
    }

    // cout << "\t" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
    if(nvoptions->get_cb(NV_STATUS_CHANGED))
        reset();

    current_timelevel = nvoptions->get_tsec();
    if(current_timelevel != previoustimelevel)
        reset();
    previoustimelevel = current_timelevel;
    if(current_timelevel >= geometry->get_nt())
        return;

    // cout << "\t" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
    if((geometry->get_nzl() <= nvoptions->get_zsec()) && (0 > nvoptions->get_zsec()))
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
    // cout << "\t" <<"_nzl: " << _nzl << endl;

  //pltvar = &_var[current_timelevel * _nxh * _nyh];
    pltvar = &_var[current_timelevel * _nxh * _nyh * _nzl];

    // cout << "\t" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
    zcl = lister->get_zid(nvoptions->get_zsec());
    ycl = lister->get_yid(nvoptions->get_ysec());
    xcl = lister->get_xid(nvoptions->get_xsec());

  //makeCurrent();
  //Clear screen and Z-buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // cout << "\t" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //glClearColor(0.0, 0.0, 0.0, 0.0);

    if(nvoptions->get_cb(NV_BUMPON))
    {
        // cout << "\t" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
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
        // cout << "\t" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
        if(nvoptions->get_cb(NV_FLATON))
        {
            // cout << "\t" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
            if(nvoptions->get_zsec() < _nzl)
            {
                if(zcl)
                    glCallList(zcl);
                else
                    _flatDisplay();
            }

          //draw_plane_grids();

            if(nvoptions->get_xsec() < _nxh && nvoptions->get_xsec() > 0)
            {
                if(xcl)
                    glCallList(xcl);
                else
                    _display_Xflat_plane(nvoptions->get_xsec());
            }

            if((nvoptions->get_ysec() > 5) && (nvoptions->get_ysec() < (_nyh-5)))
            {
                if(ycl)
                    glCallList(ycl);
                else
                    _display_Yflat_plane(nvoptions->get_ysec());
            }
        }
        else
        {
            // cout << "\t" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
            if(nvoptions->get_zsec() < _nzl)
            {
                if(zcl)
                    glCallList(zcl);
                else
                    _sphereDisplay();
            }
    
            if(nvoptions->get_xsec() < _nxh && nvoptions->get_xsec() > 0)
            {
                if(xcl)
                    glCallList(xcl);
                else
                    _sphereXplane(nvoptions->get_xsec());
            }
    
            if((nvoptions->get_ysec() > 5) && (nvoptions->get_ysec() < (_nyh-5)))
            {
                if(ycl)
                    glCallList(ycl);
                else
                    _sphereYplane(nvoptions->get_ysec());
            }
    
          //draw_sphere_grids();
        }
    }
    // cout << "Leave " << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
}

void MOM62dViewer::_sphere2xyz(double x, double y, double z, double radius, double fact)
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

void MOM62dViewer::_sphere2xyz_texture(double x, double y, double z,
		                       double radius, double fact)
{
    glTexCoord1d(fact);
    glNormal3f(x, y, z);
    glVertex3d(x * radius, y * radius, z * radius);
}

void MOM62dViewer::_flat2xyz(double x, double y, double z, double fact)
{
    double alpha = 1.05 * fact;
    if(alpha < 0.1)
        alpha = 0.0;
    else if(alpha > 1.0)
        alpha = 1.0;

    glColor4d(fact, fact, fact, alpha);
    glNormal3d(x, y, z);
    glVertex3d(x, y, z);
}

void MOM62dViewer::_flat2xyz_texture(double x, double y, double z, double fact)
{
    glTexCoord1d(fact);
    glNormal3f(x, y, z);
    glVertex3d(x, y, z);
}

void MOM62dViewer::_sphereDisplay()
{
    int i, j, k, k1;
    size_t mpos, npos;
    size_t mgeo, ngeo;

    double sv = 1.0;
    double fact;
    double radius = 1.001;

    // cout << "\nEnter " << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
    k1 = nvoptions->get_zsec()+1;
    k = _nzl-k1;
    radius = _k2r(k);
    sv = 1.0 / (_valmax - _valmin);

    // cout << "\t" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
    // cout << "k = " << k << ", sv: " << sv << ", radius: " << radius << endl;
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

    // cout << "\t" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //#pragma omp parallel for
    if(k < _nzl || 1 == _nzl) {
    for(j = 1; j < _nyh; ++j)
    {
        mpos = (k*_nyh + (j-1))*_nxh;
        npos = (k*_nyh + j)*_nxh;
        mgeo = (j-1)*_nxh;
        ngeo = j*_nxh;
        for(i = 1; i < _nxh; ++i)
        {
            if((pltvar[mpos+i-1] > _missing_value) && (pltvar[mpos+i] > _missing_value) &&
               (pltvar[npos+i-1] > _missing_value) && (pltvar[npos+i] > _missing_value))
            {
                glBegin(GL_QUADS);
                fact = sv * (pltvar[mpos+i-1] - _valmin);
                _sphere2xyz_texture(_xSphere[mgeo+i-1], _ySphere[mgeo+i-1],
                                    _zSphere[mgeo+i-1], radius, fact);

                fact = sv * (pltvar[mpos+i] - _valmin);
                _sphere2xyz_texture(_xSphere[mgeo+i], _ySphere[mgeo+i],
                                    _zSphere[mgeo+i], radius, fact);

                fact = sv * (pltvar[npos+i] - _valmin);
                _sphere2xyz_texture(_xSphere[ngeo+i], _ySphere[ngeo+i],
                                    _zSphere[ngeo+i], radius, fact);

                fact = sv * (pltvar[npos+i-1] - _valmin);
                _sphere2xyz_texture(_xSphere[ngeo+i-1], _ySphere[ngeo+i-1],
                                    _zSphere[ngeo+i-1], radius, fact);
                glEnd();
	    }
	    else
	    {
                glBegin(GL_QUADS);
                fact = 0.0;
                _sphere2xyz_texture(_xSphere[mgeo+i-1], _ySphere[mgeo+i-1],
                                    _zSphere[mgeo+i-1], radius, fact);

                _sphere2xyz_texture(_xSphere[mgeo+i], _ySphere[mgeo+i],
                                    _zSphere[mgeo+i], radius, fact);

                _sphere2xyz_texture(_xSphere[ngeo+i], _ySphere[ngeo+i],
                                    _zSphere[ngeo+i], radius, fact);

                _sphere2xyz_texture(_xSphere[ngeo+i-1], _ySphere[ngeo+i-1],
                                    _zSphere[ngeo+i-1], radius, fact);
                glEnd();
	    }
        }
        
	if((pltvar[mpos+_nxh-1] > _missing_value) && (pltvar[mpos] > _missing_value) &&
           (pltvar[npos+_nxh-1] > _missing_value) && (pltvar[npos] > _missing_value))
        {
            glBegin(GL_QUADS);
            fact = sv * (pltvar[mpos+_nxh-1] - _valmin);
            _sphere2xyz_texture(_xSphere[mgeo+_nxh-1], _ySphere[mgeo+_nxh-1],
                                _zSphere[mgeo+_nxh-1], radius, fact);

            fact = sv * (pltvar[mpos] - _valmin);
            _sphere2xyz_texture(_xSphere[mgeo], _ySphere[mgeo],
                                _zSphere[mgeo], radius, fact);

            fact = sv * (pltvar[npos] - _valmin);
            _sphere2xyz_texture(_xSphere[ngeo], _ySphere[ngeo],
                                _zSphere[ngeo], radius, fact);

            fact = sv * (pltvar[npos+_nxh-1] - _valmin);
            _sphere2xyz_texture(_xSphere[ngeo+_nxh-1], _ySphere[ngeo+_nxh-1],
                                _zSphere[ngeo+_nxh-1], radius, fact);
            glEnd();
        }
        else
        {
            glBegin(GL_QUADS);
            fact = 0.0;
            _sphere2xyz_texture(_xSphere[mgeo+_nxh-1], _ySphere[mgeo+_nxh-1],
                                _zSphere[mgeo+_nxh-1], radius, fact);

            _sphere2xyz_texture(_xSphere[mgeo], _ySphere[mgeo],
                                _zSphere[mgeo], radius, fact);

            _sphere2xyz_texture(_xSphere[ngeo], _ySphere[ngeo],
                                _zSphere[ngeo], radius, fact);

            _sphere2xyz_texture(_xSphere[ngeo+_nxh-1], _ySphere[ngeo+_nxh-1],
                                _zSphere[ngeo+_nxh-1], radius, fact);
            glEnd();
        }
    }
    fact = 0.0;
    glBegin(GL_TRIANGLE_STRIP);
    for(i = 0; i < _nxh; ++i)
    {
        _sphere2xyz_texture(_xSphere[i], _ySphere[i],
                            _zSphere[i], radius, fact);

        _sphere2xyz_texture(0.0, 0.0, -1.0, radius, fact);
    }
    _sphere2xyz_texture(_xSphere[0], _ySphere[0],
                        _zSphere[0], radius, fact);

    _sphere2xyz_texture(0.0, 0.0, -1.0, radius, fact);
    glEnd();

    mgeo = (_nyh-1)*_nxh;
    glBegin(GL_POLYGON);
    for(i = 0; i < _nxh; ++i)
    {
        _sphere2xyz_texture(_xSphere[mgeo+i], _ySphere[mgeo+i],
                            _zSphere[mgeo+i], radius, fact);
    }
    glEnd();
    coastline->drawOnSphere(radius+0.01);
    }

    glDisable(GL_TEXTURE_1D);
    glPopMatrix();
    glEndList();
    // cout << "Leave " << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
}

void MOM62dViewer::_flatDisplay()
{
    int i, j, k, k1;
    size_t mpos, npos;
    size_t mgeo, ngeo;
    double sv = 1.0;
    double fact;
    double height = 0.0;

    k1 = nvoptions->get_zsec()+1;
    k = _nzl-k1;
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

    if(k < _nzl || 1 == _nzl) {
    for(j = 1; j < _nyh; ++j)
    {
      //cout << "\t_yFlat[" << j << "] = " << _yFlat[j] << endl;
        mpos = (k*_nyh+(j-1))*_nxh;
        npos = (k*_nyh+j)*_nxh;
        mgeo = (j-1)*_nxh;
        ngeo = j*_nxh;
        for(i = _nxsp[j]+1; i < _nxh; ++i)
        {
            if((pltvar[mpos+i-1] > _missing_value) && (pltvar[mpos+i] > _missing_value) &&
               (pltvar[npos+i-1] > _missing_value) && (pltvar[npos+i] > _missing_value))
	    {
                glBegin(GL_QUADS);
                fact = sv * (pltvar[mpos+i-1] - _valmin);
	        _flat2xyz_texture(_xFlat[mgeo+i-1], _yFlat[mgeo+i-1], height, fact);
    
                fact = sv * (pltvar[mpos+i] - _valmin);
	        _flat2xyz_texture(_xFlat[mgeo+i], _yFlat[mgeo+i], height, fact);

                fact = sv * (pltvar[npos+i] - _valmin);
	        _flat2xyz_texture(_xFlat[ngeo+i], _yFlat[ngeo+i], height, fact);
    
                fact = sv * (pltvar[npos+i-1] - _valmin);
	        _flat2xyz_texture(_xFlat[ngeo+i-1], _yFlat[ngeo+i-1], height, fact);
                glEnd();
	    }
	    else
	    {
                glBegin(GL_QUADS);
                fact = 0.0;
                _flat2xyz_texture(_xFlat[mgeo+i-1], _yFlat[mgeo+i-1], height, fact);
                _flat2xyz_texture(_xFlat[mgeo+i], _yFlat[mgeo+i], height, fact);
                _flat2xyz_texture(_xFlat[ngeo+i], _yFlat[ngeo+i], height, fact);
                _flat2xyz_texture(_xFlat[ngeo+i-1], _yFlat[ngeo+i-1], height, fact);
                glEnd();
	    }
        }

	for(i = 1; i < _nxsp[j]; ++i)
        {
            if((pltvar[mpos+i-1] > _missing_value) && (pltvar[mpos+i] > _missing_value) &&
               (pltvar[npos+i-1] > _missing_value) && (pltvar[npos+i] > _missing_value))
            {
                glBegin(GL_QUADS);
                fact = sv * (pltvar[npos+i-1] - _valmin);
                _flat2xyz_texture(_xFlat[ngeo+i-1], _yFlat[ngeo+i-1], height, fact);

                fact = sv * (pltvar[npos+i] - _valmin);
                _flat2xyz_texture(_xFlat[ngeo+i], _yFlat[ngeo+i], height, fact);

                fact = sv * (pltvar[mpos+i] - _valmin);
                _flat2xyz_texture(_xFlat[mgeo+i], _yFlat[mgeo+i], height, fact);

                fact = sv * (pltvar[mpos+i-1] - _valmin);
                _flat2xyz_texture(_xFlat[mgeo+i-1], _yFlat[mgeo+i-1], height, fact);
                glEnd();
            }
            else
            {
                glBegin(GL_QUADS);
                fact = 0.0;
                _flat2xyz_texture(_xFlat[ngeo+i-1], _yFlat[ngeo+i-1], height, fact);
                _flat2xyz_texture(_xFlat[ngeo+i], _yFlat[ngeo+i], height, fact);
                _flat2xyz_texture(_xFlat[mgeo+i], _yFlat[mgeo+i], height, fact);
                _flat2xyz_texture(_xFlat[mgeo+i-1], _yFlat[mgeo+i-1], height, fact);
                glEnd();
            }
        }

	if((pltvar[mpos+_nxh-1] > _missing_value) && (pltvar[mpos+i] > _missing_value) &&
           (pltvar[npos+_nxh-1] > _missing_value) && (pltvar[npos+i] > _missing_value))
        {
            glBegin(GL_QUADS);
            fact = sv * (pltvar[npos+_nxh-1] - _valmin);
            _flat2xyz_texture(_xFlat[ngeo+_nxh-1], _yFlat[ngeo+_nxh-1], height, fact);

            fact = sv * (pltvar[npos] - _valmin);
            _flat2xyz_texture(_xFlat[ngeo], _yFlat[ngeo], height, fact);

            fact = sv * (pltvar[mpos] - _valmin);
            _flat2xyz_texture(_xFlat[mgeo], _yFlat[mgeo], height, fact);

            fact = sv * (pltvar[mpos+_nxh-1] - _valmin);
            _flat2xyz_texture(_xFlat[mgeo+_nxh-1], _yFlat[mgeo+_nxh-1], height, fact);
            glEnd();
        }
        else
        {
            glBegin(GL_QUADS);
            fact = 0.0;
            _flat2xyz_texture(_xFlat[ngeo+_nxh-1], _yFlat[ngeo+_nxh-1], height, fact);
            _flat2xyz_texture(_xFlat[ngeo], _yFlat[ngeo], height, fact);
            _flat2xyz_texture(_xFlat[mgeo], _yFlat[mgeo], height, fact);
            _flat2xyz_texture(_xFlat[mgeo+_nxh-1], _yFlat[mgeo+_nxh-1], height, fact);
            glEnd();
        }
    }
    coastline->drawOnPlane(height+0.01);
    }

    glDisable(GL_TEXTURE_1D);
    glPopMatrix();
    glEndList();
}

void MOM62dViewer::_evaluate(float *var)
{
    size_t varsize;
    size_t n = 0;
    size_t ns = 0;
    float total;

    // cout << "Enter functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    // cout << "\t _varname: " << _varname << endl;

    varsize = geometry->get_nxh() * geometry->get_nyh() * geometry->get_nzl();
  //varsize = geometry->get_nxh() * geometry->get_nyh();

    // cout << "\tin <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    // cout << "\t_nxh =" << _nxh << endl;
    // cout << "\t_nyh =" << _nyh << endl;
    // cout << "\t_nzl =" << _nzl << endl;
    // cout << "\tvarsize =" << varsize << ", _nxh*_nyh=" << _nxh*_nyh << endl;
    // cout << "\t_half_missing_value=" << _half_missing_value << endl;

    for(n = 0; n < varsize; ++n)
    {
        // cout << "\t _var[" << n << "] =" << _var[n] << endl;
        _valmax = var[n];
        _valmin = var[n];
        if(var[n] > _half_missing_value)
        {
	    ns = n;
	    break;
	}
    }

    total = var[ns];
    for(n = ns+1; n < varsize; ++n)
    {
        // cout << "\t _var[" << n << "] =" << _var[n] << endl;
        if(var[n] < _half_missing_value)
            continue;

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

    // cout << "\t_valmin = " << _valmin << ", _valavg = " << _valavg << ", _valmax = " << _valmax << endl;

#if 0
    nvoptions->set_trueminimum(_valmin);
    nvoptions->set_truemaximum(_valmax);
#endif
    // cout << "Leave functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
}

void MOM62dViewer::_adjust_minmax(float *var)
{
    size_t varsize;
    size_t n = 0;
    float total;

  //cout << "Enter functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;

    varsize = geometry->get_nxh() * geometry->get_nyh();

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

void MOM62dViewer::reset_texture1d(ColorTable *ct)
{
    colorTable = ct;

    glDisable(GL_TEXTURE_1D);

    texture1d->reset();
    texture1d->set_colors(ct->get_clen(), ct->get_cmap());
    texture1d->set_name(ct->get_name());
}

void MOM62dViewer::draw_sphere_grids()
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
    for(j = 0; j < _nyh; ++j)
    {
        npos = j*_nxh;

        glBegin(GL_LINE_STRIP);
        for(i = 0; i < _nxh; ++i)
        {
            glVertex3d(radius * _xSphere[npos+i], radius * _ySphere[npos+i], radius * _zSphere[npos+i]);
        }
        glEnd();
    }
    glEnd();

    glPopMatrix();
}

void MOM62dViewer::draw_plane_grids()
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

    for(j = 0; j < _nyh; ++j)
    {
        npos = j*_nxh;
        glBegin(GL_LINE_STRIP);
        for(i = 0; i < _nxh; ++i)
        {
            glVertex3d(_xFlat[i], _yFlat[j], height);
        }
        glEnd();
    }

    glPopMatrix();
}

void MOM62dViewer::_display_Yflat_plane(int ys)
{
    int i, k;
    size_t mpos, npos, ngeo;
    double fact;
    double sv = 1.0;
    vector<double> height(_nzl);
    int j = ys-1;

    if((10 > j) || (80 < j))
       return;

    for(k = 0; k < _nzl; ++k)
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

    for(k = 1; k < _nzl; ++k)
    {
        mpos = ((k-1)*_nyh+j)*_nxh;
        npos = (k*_nyh+j)*_nxh;
        ngeo = j*_nxh;

        glBegin(GL_QUAD_STRIP);
        for(i = 0; i < _nxh; ++i)
        {
            fact = sv * (pltvar[mpos+i] - _valmin);
	    _flat2xyz_texture(_xFlat[ngeo+i], _yFlat[ngeo+i], height[k-1], fact);

            fact = sv * (pltvar[npos+i] - _valmin);
	    _flat2xyz_texture(_xFlat[ngeo+i], _yFlat[ngeo+i], height[k], fact);
        }
        glEnd();
    }

    glDisable(GL_TEXTURE_1D);
    glPopMatrix();
    glEndList();
}

void MOM62dViewer::_sphereXplane(int xs)
{
    int j, k;
    size_t mpos, npos;
    size_t ngeo;
    double fact;
    double sv = 1.0;
    double radius[_nzl];

    int i = xs - 1;

    for(k = 0; k < _nzl; ++k)
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
    for(k = 1; k < _nzl; ++k)
    {
        glBegin(GL_QUAD_STRIP);
        for(j = 0; j < _nyh; ++j)
        {
            mpos = ((k-1)*_nyh + j)*_nxh;
            npos = (k*_nyh + j)*_nxh;
            ngeo = j*_nxh;

            fact = sv * (pltvar[mpos+i] - _valmin);
	    _sphere2xyz_texture(_xSphere[ngeo+i], _ySphere[ngeo+i],
                                _zSphere[ngeo+i], radius[k-1], fact);

            fact = sv * (pltvar[npos+i] - _valmin);
	    _sphere2xyz_texture(_xSphere[ngeo+i], _ySphere[ngeo+i],
                                _zSphere[ngeo+i], radius[k], fact);
        }
        glEnd();
    }

    glDisable(GL_TEXTURE_1D);
    glPopMatrix();
    glEndList();
}

void MOM62dViewer::_sphereYplane(int ys)
{
    int i, k;
    size_t mpos, npos, ngeo;
    double fact;
    double sv = 1.0;
    double radius[_nzl];
    int j = ys - 1;

    for(k = 0; k < _nzl; ++k)
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
    for(k = 1; k < _nzl; ++k)
    {
        mpos = ((k-1)*_nyh + j)*_nxh;
        npos = (k*_nyh + j)*_nxh;
        ngeo = j*_nxh;
        glBegin(GL_QUAD_STRIP);
        for(i = 0; i < _nxh; ++i)
        {
            fact = sv * (pltvar[mpos+i] - _valmin);
	    _sphere2xyz_texture(_xSphere[ngeo+i], _ySphere[ngeo+i],
                                _zSphere[ngeo+i], radius[k-1], fact);

            fact = sv * (pltvar[npos+i] - _valmin);
	    _sphere2xyz_texture(_xSphere[ngeo+i], _ySphere[ngeo+i],
                                _zSphere[ngeo+i], radius[k], fact);
        }
        fact = sv * (pltvar[mpos] - _valmin);
	_sphere2xyz_texture(_xSphere[ngeo], _ySphere[ngeo],
                            _zSphere[ngeo], radius[k-1], fact);

        fact = sv * (pltvar[npos] - _valmin);
	_sphere2xyz_texture(_xSphere[ngeo], _ySphere[ngeo],
                            _zSphere[ngeo], radius[k], fact);
        glEnd();
    }

    glDisable(GL_TEXTURE_1D);
    glPopMatrix();
    glEndList();
}


void MOM62dViewer::_display_Xflat_plane(int xs)
{
    int j, k;
    size_t mpos, npos, ngeo;
    double fact;
    double sv = 1.0;
    vector<double> height(_nzl);
    int i = xs-1;

    for(k = 0; k < _nzl; ++k)
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

    for(k = 1; k < _nzl; ++k)
    {
        glBegin(GL_QUAD_STRIP);
        for(j = 0; j < _nyh; ++j)
        {
            mpos = ((k-1)*_nyh+j)*_nxh;
            npos = (k*_nyh+j)*_nxh;
            ngeo = j*_nxh+i;

            fact = sv * (pltvar[mpos+i] - _valmin);
	    _flat2xyz_texture(_xFlat[ngeo], _yFlat[ngeo], height[k-1], fact);

            fact = sv * (pltvar[npos+i] - _valmin);
	    _flat2xyz_texture(_xFlat[ngeo], _yFlat[ngeo], height[k], fact);
        }
        glEnd();
    }

    glDisable(GL_TEXTURE_1D);
    glPopMatrix();
    glEndList();
}

void MOM62dViewer::_sphereBump()
{
    int i, j, k, k1;
    size_t mpos, npos;
    size_t mgeo, ngeo;
    double sv = 1.0;
    double alpha, fact;
    double amp = 1.05;
    double magnifier = 0.125;
    double radius = 1.0;

    k1 = nvoptions->get_zsec()+1;
    k = _nzl-k1;
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

  //_adjust_minmax(&pltvar[k*_nyh*_nxh]);

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
    if(k < _nzl || 1 == _nzl) {
    for(j = 1; j < _nyh; ++j)
    {
        mpos = (k*_nyh + (j-1))*_nxh;
        npos = (k*_nyh + j)*_nxh;
        mgeo = (j-1)*_nxh;
        ngeo = j*_nxh;
        glBegin(GL_QUAD_STRIP);
        for(i = 0; i < _nxh; ++i)
        {
            fact = sv * (pltvar[npos+i] - _valmin);
            _sphere2xyz(_xSphere[ngeo+i], _ySphere[ngeo+i],
                        _zSphere[ngeo+i], radius + magnifier*fact, fact);

            fact = sv * (pltvar[mpos+i] - _valmin);
            _sphere2xyz(_xSphere[mgeo+i], _ySphere[mgeo+i],
                        _zSphere[ngeo+i], radius + magnifier*fact, fact);
        }
        fact = sv * (pltvar[npos] - _valmin);
        _sphere2xyz(_xSphere[ngeo], _ySphere[ngeo],
                    _zSphere[ngeo], radius + magnifier*fact, fact);

        fact = sv * (pltvar[mpos] - _valmin);
        _sphere2xyz(_xSphere[mgeo], _ySphere[mgeo],
                    _zSphere[mgeo], radius + magnifier*fact, fact);
        glEnd();
    }
    coastline->drawOnSphere(0.01);
    }

    glPopMatrix();
    glEndList();
}

void MOM62dViewer::_flatBump()
{
    int i, j, k, k1;
    size_t mpos, npos;
    double sv = 1.0;
    double alpha, fact;
    double amp = 1.05;
    double magnifier = 0.125;
    double rlat;

    k1 = nvoptions->get_zsec()+1;
    k = _nzl-k1;

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

  //_adjust_minmax(&pltvar[k*_nyh*_nxh]);

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
    if(k < _nzl || 1 == _nzl) {
    for(j = 1; j < _nyh; ++j)
    {
        mpos = (k*_nyh+(j-1))*_nxh;
        npos = (k*_nyh+j)*_nxh;
        glBegin(GL_QUAD_STRIP);
        for(i = 0; i < _nxh; ++i)
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
        for(i = 0; i < _nxh; ++i)
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

void MOM62dViewer::_draw_cross(double radius)
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

double MOM62dViewer::_k2h(int k)
{
    double height = 0.5 * ((double) (_nzl-k) / _nzl);
    return height;
}

double MOM62dViewer::_k2r(int k)
{
    double radius = 1.0 + _k2h(k);
    return radius;
}

