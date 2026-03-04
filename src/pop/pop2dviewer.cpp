//$Id: pop2dviewer.cpp 5194 2014-08-18 19:51:52Z starviewer $

#include <QtOpenGL>

#include <pop2dviewer.h>

POP2dViewer::POP2dViewer(ColorTable *ct, NVOptions* opt)
{
    colorTable = ct;
    nvoptions = opt;

    texture1d = new Texture1d();
    texture1d->set_colors4pop(ct->get_clen(), ct->get_cmap());
    texture1d->set_name(ct->get_name());

    coastline = new CoastLine();

    _xlist = NULL;
    _ylist = NULL;
    _zlist = NULL;
    _var = NULL;

    _ux2d = NULL;
    _uy2d = NULL;

    _tx2d = NULL;
    _ty2d = NULL;

    mlon = 0;
    mlat = 0;
    mlev = 0;

    deg2rad = 3.1415926535898 / 180.0;
}

POP2dViewer::~POP2dViewer()
{
    if(NULL != _xlist)
        free(_xlist);
    if(NULL != _ylist)
        free(_ylist);
    if(NULL != _zlist)
        free(_zlist);

    delete texture1d;
    delete coastline;
}

void POP2dViewer::reset_texture1d(ColorTable *ct)
{
    colorTable = ct;

    texture1d->reset();
    texture1d->set_colors4pop(ct->get_clen(), ct->get_cmap());
    texture1d->set_name(ct->get_name());
}

void POP2dViewer::set_geometry(POPGeometry *gm)
{
    geometry = gm;

    _initialize();
}

void POP2dViewer::setup(string vn, double *var)
{
    reset();

    _varname  = vn;
    _var = var;

    _evaluate(_var);
}

void POP2dViewer::reset()
{
    int i;

  //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\tgeometry->get_nTime() = " << geometry->get_nTime() << endl;
  //cout << "\tnlon = " << nlon << ", nlat = " << nlat << ", nlev = " << nlev << endl;

    if(NULL == _xlist)
    {
        _xlist = (int*) malloc(nlon * sizeof(int*));
        assert(_xlist);
    }

    if(NULL == _ylist)
    {
        _ylist = (int*) malloc(nlat * sizeof(int*));
        assert(_ylist);
    }

    if(NULL == _zlist)
    {
        _zlist = (int*) malloc(nlev * sizeof(int*));
        assert(_zlist);
    }

    if(0 == mlon)
    {
        mlon = nlon;
    }
    else
    {
        if(nlon > mlon)
        {
            _xlist = (int*) realloc(_xlist, nlon * sizeof(int));
            assert(_xlist);
            mlon = nlon;
        }
    }

    for(i = 0; i < mlon; ++i)
    {
        if(0 <= _xlist[i])
            glDeleteLists(_xlist[i], 1);

        _xlist[i] = -1;
    }

    if(0 == mlat)
    {
        mlat = nlat;
    }
    else
    {
        if(nlat > mlat)
        {
            _ylist = (int*) realloc(_ylist, nlat * sizeof(int));
            assert(_ylist);
            mlat = nlat;
        }
    }

    for(i = 0; i < mlat; ++i)
    {
        if(0 <= _ylist[i])
            glDeleteLists(_ylist[i], 1);

        _ylist[i] = -1;
    }

    if(0 == mlev)
    {
        mlev = nlev;
    }
    else
    {
        if(nlev > mlev)
        {
            _zlist = (int*) realloc(_zlist, nlev * sizeof(int));
            assert(_zlist);
            mlev = nlev;
        }
    }

    for(i = 0; i < mlev; ++i)
    {
        if(0 <= _zlist[i])
            glDeleteLists(_zlist[i], 1);

        _zlist[i] = -1;
    }
}

void POP2dViewer::_initialize()
{
    int i, j, n;

    nlon = geometry->get_pop_nlon();
    nlat = geometry->get_pop_nlat();
    nlev = geometry->get_pop_nlev();

    if(0 == mlon)
    {
        _xlist = (int*) malloc(nlon * sizeof(int));
        assert(_xlist);
        mlon = nlon;
    }
    else
    {
        if(nlon > mlon)
        {
            _xlist = (int*) realloc(_zlist, nlon * sizeof(int));
            assert(_xlist);
            mlon = nlon;
        }
    }

    for(i = 0; i < nlon; ++i)
        _xlist[i] = -1;

    if(0 == mlat)
    {
        _ylist = (int*) malloc(nlat * sizeof(int));
        assert(_ylist);
        mlat = nlat;
    }
    else
    {
        if(nlat > mlat)
        {
            _ylist = (int*) realloc(_ylist, nlat * sizeof(int));
            assert(_ylist);
            mlat = nlat;
        }
    }

    for(i = 0; i < nlat; ++i)
        _ylist[i] = -1;

    if(0 == mlev)
    {
        _zlist = (int*) malloc(nlev * sizeof(int));
        assert(_zlist);
        mlev = nlev;
    }
    else
    {
        if(nlev > mlev)
        {
            _zlist = (int*) realloc(_zlist, nlev * sizeof(int));
            assert(_zlist);
            mlev = nlev;
        }
    }

    for(i = 0; i < nlev; ++i)
        _zlist[i] = -1;

    z_w = geometry->get_pop_z_w();

    if((NULL == _ux2d) || (NULL == _uy2d))
    {
        lon = geometry->get_pop_ulon();
        lat = geometry->get_pop_ulat();

        _ux2d = (double**)calloc(nlat, sizeof(double*));
        _uy2d = (double**)calloc(nlat, sizeof(double*));

        for(j = 0; j < nlat; ++j)
        {
            _ux2d[j] = (double*)calloc(nlon, sizeof(double));
            _uy2d[j] = (double*)calloc(nlon, sizeof(double));

            n = j * nlon;

            for(i = 0; i < nlon; ++i)
            {
                _ux2d[j][i] = lon[n + i] / 180.0 - 1.0;
                _uy2d[j][i] = lat[n + i] / 180.0;
            }
        }
    }

    if((NULL == _tx2d) || (NULL == _ty2d))
    {
        lon = geometry->get_pop_tlon();
        lat = geometry->get_pop_tlat();

        _tx2d = (double**)calloc(nlat, sizeof(double*));
        _ty2d = (double**)calloc(nlat, sizeof(double*));

        for(j = 0; j < nlat; ++j)
        {
            _tx2d[j] = (double*)calloc(nlon, sizeof(double));
            _ty2d[j] = (double*)calloc(nlon, sizeof(double));

            n = j * nlon;

            for(i = 0; i < nlon; ++i)
            {
                _tx2d[j][i] = lon[n + i] / 180.0 - 1.0;
                _ty2d[j][i] = lat[n + i] / 180.0;
            }
        }
    }

    _fillValue = 0.5 * geometry->get_fillValue();
}

void POP2dViewer::draw()
{
    if(nvoptions->get_cb(NV_RESET))
    {
        nvoptions->set_cb(NV_RESET, false);
        reset();
    }

    if((0 <= nvoptions->get_zsec()) && (nlev > nvoptions->get_zsec()))
    {
        if(0 <= _zlist[nvoptions->get_zsec()])
            glCallList(_zlist[nvoptions->get_zsec()]);
        else
        {
            if(nvoptions->get_cb(NV_FLATON))
                _displayZflat(nvoptions->get_zsec());
            else
                _displayZsphere(nvoptions->get_zsec());
        }
    }

    if(1 >= nlev)
        return;

    if((0 <= nvoptions->get_xsec()) && (nlon > nvoptions->get_xsec()))
    {
        if(0 <= _xlist[nvoptions->get_xsec()])
            glCallList(_xlist[nvoptions->get_xsec()]);
        else
        {
            if(nvoptions->get_cb(NV_FLATON))
                _displayXflat(nvoptions->get_xsec());
            else
                _displayXsphere(nvoptions->get_xsec());
        }
    }

    if((0 < nvoptions->get_ysec()) && (nlat > nvoptions->get_ysec()) && (365 > nvoptions->get_ysec()))
    {
        if(0 <= _ylist[nvoptions->get_ysec()])
            glCallList(_ylist[nvoptions->get_ysec()]);
        else
        {
            if(nvoptions->get_cb(NV_FLATON))
                _displayYflat(nvoptions->get_ysec());
            else
                _displayYsphere(nvoptions->get_ysec());
        }
    }
}

void POP2dViewer::_lonlat2xyz(double lon, double lat, double radius, double f)
{
    double phi = lat * deg2rad;
    double dist = radius * cos(phi);
    double lamda = lon * deg2rad;

    double x = dist * sin(lamda);
    double z = dist * cos(lamda);
    double y = radius * sin(phi);

    glNormal3f(x, y, z);
    glTexCoord1d(f);
    glVertex3d(x, y, z);

  //cout << "lon = " << lon << ", lat = " << lat << ", radius = " << radius << endl;
  //cout << "x = " << x << ", y = " << y << ", z = " << z << ", f = " << f << endl;
}

void POP2dViewer::_lonlat2flat(double lon, double lat, double radius, double f)
{
    double x = lon / 180.0 - 1.0;
    double y = lat / 180.0;
    double z = radius;

    glNormal3f(x, y, z);
    glTexCoord1d(f);
    glVertex3d(x, y, z);

  //cout << "lon = " << lon << ", lat = " << lat << ", radius = " << radius << endl;
  //cout << "x = " << x << ", y = " << y << ", z = " << z << ", f = " << f << endl;
}

void POP2dViewer::_displayZsphere(int zl)
{
    int i, j;

    double fact;
    double height = 1.0;

    double plon, plat;
    double v;
    double sz = 1.0;

    if(1 < nlev)
    {
        sz = 0.25 / z_w[nlev - 1];
        height = 1.0 - z_w[zl] * sz;
    }
    else
    {
        height = 1.0;
    }

    _zlist[zl] = glGenLists(1);
    glNewList(_zlist[zl], GL_COMPILE_AND_EXECUTE);

    _vscale = 0.9 / (_valmax - _valmin);

  //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\t_varname: <" << _varname << ">" << endl;
  //cout << "\tnlon = " << nlon << ", nlat = " << nlat << ", nlev = " << nlev << endl;
  //cout << "\t_valmin = " << _valmin << ", _valmax = " << _valmax << ", _vscale = " << _vscale << endl;
  //cout << "\t_zlist[zl] = " << _zlist[zl] << endl;

    if(geometry->isUpoint())
    {
        lon = geometry->get_pop_ulon();
        lat = geometry->get_pop_ulat();
    }
    else
    {
        lon = geometry->get_pop_tlon();
        lat = geometry->get_pop_tlat();
    }

    glPushMatrix();

    if(nvoptions->get_cb(NV_COASTLINEON))
        coastline->draw(height, 2);

    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1, 1);

    glColor4f(1.0, 1.0, 1.0, 1.0);

    glEnable(GL_TEXTURE_1D);
    glBindTexture(GL_TEXTURE_1D, texture1d->get_textureID());

    glBegin(GL_QUAD_STRIP);
    for(j = 0; j < nlat - 1; ++j)
    {
        for(i = 0; i < nlon; ++i)
        {
            v = _var[(zl*nlat + j+1)*nlon + i];

            if(_fillValue > v)
                fact = _vscale*(v - _valmin);
            else
                fact = 0.0;
            _lonlat2xyz(lon[(j+1)*nlon+i], lat[(j+1)*nlon+i], height, fact);

            v = _var[(zl*nlat + j)*nlon + i];
            if(_fillValue > v)
                fact = _vscale*(v - _valmin);
            else
                fact = 0.0;

            _lonlat2xyz(lon[j*nlon+i], lat[j*nlon+i], height, fact);
        }

        v = _var[(zl*nlat + j+1)*nlon];
        if(_fillValue > v)
            fact = _vscale*(v - _valmin);
        else
            fact = 0.0;
        _lonlat2xyz(lon[(j+1)*nlon], lat[(j+1)*nlon], height, fact);

        v = _var[(zl*nlat + j)*nlon];
        if(_fillValue > v)
            fact = _vscale*(v - _valmin);
        else
            fact = 0.0;
        _lonlat2xyz(lon[j*nlon], lat[j*nlon], height, fact);
    }
    glEnd();

  //Do Greenland and south pole.
    glBegin(GL_QUAD_STRIP);
        j = nlat - 1;
        plon = 0.5 * (lon[j*nlon] + lon[j*nlon+nlon/2]);
        plat = 0.5 * (lat[j*nlon] + lat[j*nlon+nlon/2]);
        for(i = 0; i < nlon; ++i)
        {
            _lonlat2xyz(plon, plat, height, 1.0);
            _lonlat2xyz(lon[j*nlon + i], lat[j*nlon+i], height, 1.0);
        }
        _lonlat2xyz(plon, plat, height, 1.0);
        _lonlat2xyz(lon[j*nlon], lat[j*nlon], height, 1.0);
    glEnd();

    glBegin(GL_QUAD_STRIP);
        plon = 0.0;
        plat = -90.0;
        for(i = 0; i < nlon; ++i)
        {
            _lonlat2xyz(plon, plat, height, 1.0);
            _lonlat2xyz(lon[i], lat[i], height, 1.0);
        }

        _lonlat2xyz(plon, plat, height, 1.0);
        _lonlat2xyz(lon[0], lat[0], height, 1.0);
    glEnd();

  //glDisable(GL_CULL_FACE);
    glDisable(GL_POLYGON_OFFSET_FILL);
    glDisable(GL_TEXTURE_1D);

    glColor4f(1.0, 1.0, 1.0, 1.0);

    glPopMatrix();

    glEndList();
}

void POP2dViewer::_displayXsphere(int xl)
{
    int j, k, n;

    double fact;
    double height[nlev];
    double val[nlev][nlat];
    double sz = 0.25 / z_w[nlev - 1];

    for(k = 0; k < nlev; ++k)
    {
        height[k] = 1.0 - z_w[k] * sz;

        n = k * nlat * nlon;
        for(j = 0; j < nlat; ++j)
        {
            val[k][j] = _var[n + j * nlon + xl];
        }
    }

    _xlist[xl] = glGenLists(1);
    glNewList(_xlist[xl], GL_COMPILE_AND_EXECUTE);

    _vscale = 0.9 / (_valmax - _valmin);

  //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\t_varname: <" << _varname << ">" << endl;
  //cout << "\tnlon = " << nlon << ", nlat = " << nlat << ", nlev = " << nlev << endl;
  //cout << "\t_valmin = " << _valmin << ", _valmax = " << _valmax << ", _vscale = " << _vscale << endl;
  //cout << "\t_xlist[xl] = " << _xlist[xl] << endl;

    if(geometry->isUpoint())
    {
        lon = geometry->get_pop_ulon();
        lat = geometry->get_pop_ulat();
    }
    else
    {
        lon = geometry->get_pop_tlon();
        lat = geometry->get_pop_tlat();
    }

    glPushMatrix();

    glColor4f(1.0, 1.0, 1.0, 1.0);

    glEnable(GL_TEXTURE_1D);
    glBindTexture(GL_TEXTURE_1D, texture1d->get_textureID());

    glBegin(GL_QUAD_STRIP);
    for(k = 0; k < nlev - 1; ++k)
    {
        for(j = 0; j < nlat; ++j)
        {
            if(_fillValue > val[k][j])
                fact = _vscale*(val[k][j] - _valmin);
            else
                fact = 0.0;
            _lonlat2xyz(lon[j*nlon+xl], lat[j*nlon+xl], height[k], fact);

            if(_fillValue > val[k+1][j])
                fact = _vscale*(val[k+1][j] - _valmin);
            else
                fact = 0.0;

            _lonlat2xyz(lon[j*nlon+xl], lat[j*nlon+xl], height[k+1], fact);
        }
    }
    glEnd();

  //glDisable(GL_CULL_FACE);

    glDisable(GL_TEXTURE_1D);

    glPopMatrix();

    glEndList();
}

void POP2dViewer::_displayYsphere(int yl)
{
    int i, k, n;

    double fact;
    double height[nlev];
    double v[nlev][nlon];
    double sz = 0.25 / z_w[nlev - 1];

    for(k = 0; k < nlev; ++k)
    {
        height[k] = 1.0 - z_w[k] * sz;

        n = (k * nlat + yl) * nlon;
        for(i = 0; i < nlon; ++i)
        {
            v[k][i] = _var[n + i];
        }
    }

    _ylist[yl] = glGenLists(1);
    glNewList(_ylist[yl], GL_COMPILE_AND_EXECUTE);

    _vscale = 0.9 / (_valmax - _valmin);

  //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\t_varname: <" << _varname << ">" << endl;
  //cout << "\tnlon = " << nlon << ", nlat = " << nlat << ", nlev = " << nlev << endl;
  //cout << "\t_valmin = " << _valmin << ", _valmax = " << _valmax << ", _vscale = " << _vscale << endl;
  //cout << "\t_ylist[yl] = " << _ylist[yl] << endl;

    if(geometry->isUpoint())
    {
        lon = geometry->get_pop_ulon();
        lat = geometry->get_pop_ulat();
    }
    else
    {
        lon = geometry->get_pop_tlon();
        lat = geometry->get_pop_tlat();
    }

    glPushMatrix();

    glColor4f(1.0, 1.0, 1.0, 1.0);

    glEnable(GL_TEXTURE_1D);
    glBindTexture(GL_TEXTURE_1D, texture1d->get_textureID());

    n = yl * nlon;
    glBegin(GL_QUAD_STRIP);
    for(k = 0; k < nlev - 1; ++k)
    {
        for(i = 0; i < nlon; ++i)
        {
            if(_fillValue > v[k][i])
                fact = _vscale*(v[k][i] - _valmin);
            else
                fact = 0.0;
            _lonlat2xyz(lon[n+i], lat[n+i], height[k], fact);

            if(_fillValue > v[k+1][i])
                fact = _vscale*(v[k+1][i] - _valmin);
            else
                fact = 0.0;
            _lonlat2xyz(lon[n+i], lat[n+i], height[k+1], fact);
        }

        if(_fillValue > v[k][0])
            fact = _vscale*(v[k][0] - _valmin);
        else
            fact = 0.0;
        _lonlat2xyz(lon[n], lat[n], height[k], fact);

        if(_fillValue > v[k+1][0])
            fact = _vscale*(v[k+1][0] - _valmin);
        else
            fact = 0.0;
        _lonlat2xyz(lon[n], lat[n], height[k+1], fact);
    }
    glEnd();

  //glDisable(GL_CULL_FACE);

    glDisable(GL_TEXTURE_1D);

    glPopMatrix();

    glEndList();
}

void POP2dViewer::_displayZflat(int zl)
{
    int i, im1, j, k, m, n;

    double** x = NULL;
    double** y = NULL;

    double height = 0.0;

    double f[2][2];
    double f1[2];

    double v[2][2];
    double v1[2];

    double xp, yp;
    double x0, x1, dx, dd;
    double y1[2];

    int boundary[2*nlat][3];

    int numbProcessedBoundaryPoints = 0;
    int numbBoundaryPoints = 0;

    k = zl;

    if(1 < nlev)
    {
        height = 0.5 - z_w[zl] / z_w[nlev - 1];
    }
    else
    {
        height = 0.0;
    }

    _zlist[k] = glGenLists(1);
    glNewList(_zlist[k], GL_COMPILE_AND_EXECUTE);

    _vscale = 0.9 / (_valmax - _valmin);

  //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\t_varname: <" << _varname << ">" << endl;
  //cout << "\tnlon = " << nlon << ", nlat = " << nlat << ", nlev = " << nlev << endl;
  //cout << "\t_valmin = " << _valmin << ", _valmax = " << _valmax << ", _vscale = " << _vscale << endl;
  //cout << "\t_zlist[k] = " << _zlist[k] << endl;

    if(geometry->isUpoint())
    {
        x = _ux2d;
        y = _uy2d;
    }
    else
    {
        x = _tx2d;
        y = _ty2d;
    }

    glPushMatrix();

    glEnable(GL_TEXTURE_1D);
    glBindTexture(GL_TEXTURE_1D, texture1d->get_textureID());

    if(nvoptions->get_cb(NV_COASTLINEON))
    {
      //coastline->drawONplane(1);
        coastline->drawONplane2(height, 2);
    }

    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1, 1);

    glColor3f(1.0, 1.0, 1.0);

    glBegin(GL_QUADS);
    for(j = 0; j < nlat-1; ++j)
    {
        im1 = nlon - 1;

        for(n = 0; n < 2; ++n)
        {
            v[n][0] = _var[(k*nlat + j+n)*nlon + im1];
            if(_fillValue > v[n][0])
                f[n][0] = _vscale*(v[n][0] - _valmin);
            else
                f[n][0] = 0.0;
        }

        for(i = 0; i < nlon; ++i)
        {
            for(n = 0; n < 2; ++n)
            {
                v[n][1] = _var[(k*nlat + j+n)*nlon + i];
                if(_fillValue > v[n][1])
                    f[n][1] = _vscale*(v[n][1] - _valmin);
                else
                    f[n][1] = 0.0;
            }

            if(( 0.5 < x[j][im1]) &&
              ((-0.5 > x[j][i]) || (-0.5 > x[j+1][im1]) || (-0.5 > x[j+1][i])))
            {
                boundary[numbBoundaryPoints][0] = im1;
                boundary[numbBoundaryPoints][1] = i;
                boundary[numbBoundaryPoints][2] = j;

                ++numbBoundaryPoints;
            }
            else if((-0.5 > x[j][im1]) &&
                    ((0.5 < x[j][i]) || (0.5 < x[j+1][im1]) || (0.5 < x[j+1][i])))
            {
                boundary[numbBoundaryPoints][0] = im1;
                boundary[numbBoundaryPoints][1] = i;
                boundary[numbBoundaryPoints][2] = j;

                ++numbBoundaryPoints;
            }
            else
            {
                glTexCoord1d(f[0][0]);
                glVertex3d(x[j][im1], y[j][im1], height);

                glTexCoord1d(f[0][1]);
                glVertex3d(x[j][i], y[j][i], height);

                glTexCoord1d(f[1][1]);
                glVertex3d(x[j+1][i], y[j+1][i], height);

                glTexCoord1d(f[1][0]);
                glVertex3d(x[j+1][im1], y[j+1][im1], height);
            }

            f[0][0] = f[0][1];
            f[1][0] = f[1][1];

            im1 = i;
        }
    }
    glEnd();

  //The boundary
    for(m = 0; m < numbBoundaryPoints; ++m)
    {
        im1 = boundary[m][0];
        i   = boundary[m][1];
        j   = boundary[m][2];

        for(n = 0; n < 2; ++n)
        {
            v[n][0] = _var[(k*nlat + j+n)*nlon + im1];
            if(_fillValue > v[n][0])
                f[n][0] = _vscale*(v[n][0] - _valmin);
            else
                f[n][0] = 0.0;

            v[n][1] = _var[(k*nlat + j+n)*nlon + i];
            if(_fillValue > v[n][1])
                f[n][1] = _vscale*(v[n][1] - _valmin);
            else
                f[n][1] = 0.0;
        }

      //There will be cases:   Wei's note book 3/1/2013
      //1. 1 point  > 0, 3 points < 0
      //2. 2 points > 0, 2 points < 0
      //3. 3 points > 0, 1 point  < 0

      //cout << "\tx[" << j << "][" << im1 << "]=" << x[j][im1]
      //     << ", x[" << j << "][" << i   << "]=" << x[j][i] << endl;

        if(0.0 > x[j][im1])
        {
            if(0.0 > x[j][i])
            {
                if(0.0 > x[j+1][im1])
                {
                    if(0.0 > x[j+1][i])
                    {
                        cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
                        cout << "\tx[" << j+1 << "][" << im1 << "]=" << x[j+1][im1] << endl;
                        cout << "\tx[j+1][im1]=" << x[j+1][im1] << ", x[j+1][i]=" << x[j+1][i] << endl;
                        cout << "\tx[j  ][im1]=" << x[j  ][im1] << ", x[j  ][i]=" << x[j  ][i] << endl;
                    }
                    else
                    {
                      //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
                      //cout << "\tx[j+1][im1]=" << x[j+1][im1] << ", x[j+1][i]=" << x[j+1][i] << endl;
                      //cout << "\tx[j  ][im1]=" << x[j  ][im1] << ", x[j  ][i]=" << x[j  ][i] << endl;

                      //x[j+1][im1]=-0.999508, x[j+1][i]=0.999373
                      //x[j  ][im1]=-0.97756, x[j  ][i]=-0.978374

                        ++numbProcessedBoundaryPoints;

                      //Here j,im1  j,i  j+1,im1 < 0, and j+1,i > 0
                        glBegin(GL_TRIANGLES);
                          //Do triangle: j+1,i, y1[0], y1[1]
                            x0 = x[j+1][i];
                            x1 = 1.0;
                            dd = 2.0+x[j][i]-x0;
                            if(1.0e-10 < fabs(dd))
                                dx = (x1-x0)/dd;
                            else
                                dx = 0.5;
                            y1[0] = (1.0 - dx) * y[j+1][i] + dx * y[j][i];

                            if(_fillValue > v[1][1])
                            {
                                if(_fillValue > v[0][1])
                                {
                                    v1[0] = (1.0 - dx) * v[1][1] + dx * v[0][1];
                                }
                                else
                                {
                                    v1[0] = v[1][1];
                                }

                                f1[0] = _vscale*(v1[0] - _valmin);
                            }
                            else
                            {
                                if(_fillValue > v[0][1])
                                {
                                    v1[0] = v[0][1];
                                    f1[0] = _vscale*(v1[0] - _valmin);
                                }
                                else
                                {
                                    f1[0] = 0.0;
                                }
                            }

                            dd = 2.0+x[j+1][im1]-x0;
                            if(1.0e-10 < fabs(dd))
                                dx = (x1-x0)/dd;
                            else
                                dx = 0.5;
                            y1[1] = (1.0 - dx) * y[j+1][i] + dx * y[j+1][im1];

                            if(_fillValue > v[1][1])
                            {
                                if(_fillValue > v[1][0])
                                {
                                    v1[1] = (1.0 - dx) * v[1][1] + dx * v[1][0];
                                }
                                else
                                {
                                    v1[1] = v[1][0];
                                }
                
                                f1[1] = _vscale*(v1[1] - _valmin);
                            }
                            else
                            {
                                if(_fillValue > v[1][0])
                                {
                                    v1[1] = v[1][0];
                                    f1[1] = _vscale*(v1[1] - _valmin);
                                }
                                else
                                {
                                    f1[1] = 0.0;
                                }
                            }

                            glTexCoord1d(f1[0]);
                            glVertex3d(x1, y1[0], height);

                            glTexCoord1d(f1[1]);
                            glVertex3d(x1, y1[1], height);
                
                            glTexCoord1d(f[1][1]);
                            glVertex3d(x[j+1][i], y[j+1][i], height);

                          //Do triangle: j,i, y1[0], y1[1]
                            x1 = -1.0;
                            glTexCoord1d(f1[0]);
                            glVertex3d(x1, y1[0], height);

                            glTexCoord1d(f1[1]);
                            glVertex3d(x1, y1[1], height);

                            glTexCoord1d(f[1][0]);
                            glVertex3d(x[j][i], y[j][i], height);
                        glEnd();

                        glBegin(GL_QUADS);
                          //Do quad: j+1,im1, y1[1], j,i, j,im1
                            x1 = -1.0;
                            glTexCoord1d(f[1][0]);
                            glVertex3d(x[j+1][im1], y[j+1][im1], height);

                            glTexCoord1d(f1[1]);
                            glVertex3d(x1, y1[1], height);

                            glTexCoord1d(f[0][1]);
                            glVertex3d(x[j][i], y[j][i], height);

                            glTexCoord1d(f[0][0]);
                            glVertex3d(x[j][im1], y[j][im1], height);
                        glEnd();
                    }
                }
                else
                {
                    if(0.0 > x[j+1][i])
                    {
                      //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
                      //cout << "\tx[j+1][im1]=" << x[j+1][im1] << ", x[j+1][i]=" << x[j+1][i] << endl;
                      //cout << "\tx[j  ][im1]=" << x[j  ][im1] << ", x[j  ][i]=" << x[j  ][i] << endl;

                      //x[j+1][im1]=0.999978, x[j+1][i]=-0.999144
                      //x[j  ][im1]=-0.986204, x[j  ][i]=-0.985067

                        ++numbProcessedBoundaryPoints;

                        glBegin(GL_TRIANGLES);
                          //Do triangle: j+1,im1, y1[0], y1[1]
                            x0 = x[j+1][im1];
                            x1 = 1.0;
                            dd = 2.0+x[j][im1]-x0;
                            if(1.0e-10 < fabs(dd))
                                dx = (x1-x0)/dd;
                            else
                                dx = 0.5;
                            y1[0] = (1.0 - dx) * y[j+1][im1] + dx * y[j][im1];

                            if(_fillValue > v[1][0])
                            {
                                if(_fillValue > v[0][0])
                                {
                                    v1[0] = (1.0 - dx) * v[1][0] + dx * v[0][0];
                                }
                                else
                                {
                                    v1[0] = v[1][0];
                                }

                                f1[0] = _vscale*(v1[0] - _valmin);
                            }
                            else
                            {
                                if(_fillValue > v[0][0])
                                {
                                    v1[0] = v[0][0];
                                    f1[0] = _vscale*(v1[0] - _valmin);
                                }
                                else
                                {
                                    f1[0] = 0.0;
                                }
                            }

                            dd = 2.0+x[j+1][i]-x0;
                            if(1.0e-10 < fabs(dd))
                                dx = (x1-x0)/dd;
                            else
                                dx = 0.5;
                            y1[1] = (1.0 - dx) * y[j+1][im1] + dx * y[j+1][i];

                            if(_fillValue > v[1][0])
                            {
                                if(_fillValue > v[1][1])
                                {
                                    v1[1] = (1.0 - dx) * v[1][0] + dx * v[1][1];
                                }
                                else
                                {
                                    v1[1] = v[1][1];
                                }
                
                                f1[1] = _vscale*(v1[1] - _valmin);
                            }
                            else
                            {
                                if(_fillValue > v[1][1])
                                {
                                    v1[1] = v[1][1];
                                    f1[1] = _vscale*(v1[1] - _valmin);
                                }
                                else
                                {
                                    f1[1] = 0.0;
                                }
                            }
                
                            glTexCoord1d(f[1][0]);
                            glVertex3d(x[j+1][im1], y[j+1][im1], height);

                            glTexCoord1d(f1[0]);
                            glVertex3d(x1, y1[0], height);

                            glTexCoord1d(f1[1]);
                            glVertex3d(x1, y1[1], height);

                          //Do triangle: j,im1, y1[0], y1[1]
                            x1 = -1.0;
                            glTexCoord1d(f1[0]);
                            glVertex3d(x1, y1[0], height);

                            glTexCoord1d(f1[1]);
                            glVertex3d(x1, y1[1], height);

                            glTexCoord1d(f[0][0]);
                            glVertex3d(x[j][im1], y[j][im1], height);
                        glEnd();

                        glBegin(GL_QUADS);
                          //Do quad: j,im1, j,i, j+1,i, y1[1]
                            x1 = -1.0;
                            glTexCoord1d(f[0][0]);
                            glVertex3d(x[j][im1], y[j][im1], height);

                            glTexCoord1d(f[0][1]);
                            glVertex3d(x[j][i], y[j][i], height);

                            glTexCoord1d(f[1][1]);
                            glVertex3d(x[j+1][i], y[j+1][i], height);

                            glTexCoord1d(f1[1]);
                            glVertex3d(x1, y1[1], height);
                        glEnd();
                    }
                    else
                    {
                      //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
                      //cout << "\tx[j+1][im1]=" << x[j+1][im1] << ", x[j+1][i]=" << x[j+1][i] << endl;
                      //cout << "\tx[j  ][im1]=" << x[j  ][im1] << ", x[j  ][i]=" << x[j  ][i] << endl;
                      //x[j+1][im1]=0.98053, x[j+1][i]=0.979275
                      //x[j  ][im1]=-0.998486, x[j  ][i]=-0.999508

                        ++numbProcessedBoundaryPoints;

                      //Wei's note: 3/4/2013

                      //Add two quads:
                      //1. j+1,im1, j+1,i, y1[1]. y1[0],
                        x1 = 1.0;
                        x0 = x[j+1][im1];
                        dd = 2.0+x[j][im1]-x0;
                        if(1.0e-10 < fabs(dd))
                            dx = (x1-x0)/dd;
                        else
                            dx = 0.5;
                        y1[0] = (1.0 - dx) * y[j+1][im1] + dx * y[j][im1];

                        if(_fillValue > v[1][0])
                        {
                            if(_fillValue > v[0][0])
                            {
                                v1[0] = (1.0 - dx) * v[1][0] + dx * v[0][0];
                            }
                            else
                            {
                                v1[0] = v[0][0];
                            }

                            f1[0] = _vscale*(v1[0] - _valmin);
                        }
                        else
                        {
                            if(_fillValue > v[0][0])
                            {
                                v1[0] = v[0][0];
                                f1[0] = _vscale*(v1[0] - _valmin);
                            }
                            else
                            {
                                f1[0] = 0.0;
                            }
                        }

                        x0 = x[j+1][i];
                        dd = 2.0+x[j][i]-x0;
                        if(1.0e-10 < fabs(dd))
                            dx = (x1-x0)/dd;
                        else
                            dx = 0.5;
                        y1[1] = (1.0 - dx) * y[j+1][i] + dx * y[j][i];

                        if(_fillValue > v[1][1]) 
                        {
                            if(_fillValue > v[0][1])
                            {
                                v1[1] = (1.0 - dx) * v[1][1] + dx * v[0][1]; 
                            }
                            else
                            {
                                v1[1] = v[0][1];
                            }

                            f1[1] = _vscale*(v1[1] - _valmin);
                        }
                        else 
                        {
                            if(_fillValue > v[0][1])
                            {
                                v1[1] = v[0][1];
                                f1[1] = _vscale*(v1[1] - _valmin);
                            }
                            else
                            {
                                f1[1] = 0.0;
                            }
                        }

                        glBegin(GL_QUADS);
                          //Do quad: j+1,im1, j+1,i, y1[1]. y1[0]
                            glTexCoord1d(f[1][0]);
                            glVertex3d(x[j+1][im1], y[j+1][im1], height);

                            glTexCoord1d(f[1][1]);
                            glVertex3d(x[j+1][i], y[j+1][i], height);

                            glTexCoord1d(f1[1]);
                            glVertex3d(x1, y1[1], height);

                            glTexCoord1d(f1[0]);
                            glVertex3d(x1, y1[0], height);

                            x1 = -1.0;
                          //Do quad: j,im1, j,i, y1[1]. y1[0]
                            glTexCoord1d(f[0][0]);
                            glVertex3d(x[j][im1], y[j][im1], height);

                            glTexCoord1d(f[0][1]);
                            glVertex3d(x[j][i], y[j][i], height);

                            glTexCoord1d(f1[1]);
                            glVertex3d(x1, y1[1], height);

                            glTexCoord1d(f1[0]);
                            glVertex3d(x1, y1[0], height);
                        glEnd();
                    }
                }
            }
            else
            {
                if(0.0 > x[j+1][im1])
                {
                    if(0.0 > x[j+1][i])
                    {
                        cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
                        cout << "\tx[j+1][im1]=" << x[j+1][im1] << ", x[j+1][i]=" << x[j+1][i] << endl;
                        cout << "\tx[j  ][im1]=" << x[j  ][im1] << ", x[j  ][i]=" << x[j  ][i] << endl;
                    }
                    else
                    {
                        cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
                        cout << "\tx[j+1][im1]=" << x[j+1][im1] << ", x[j+1][i]=" << x[j+1][i] << endl;
                        cout << "\tx[j  ][im1]=" << x[j  ][im1] << ", x[j  ][i]=" << x[j  ][i] << endl;
                    }
                }
                else
                {
                    if(0.0 > x[j+1][i])
                    {
                        cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
                        cout << "\tx[j+1][im1]=" << x[j+1][im1] << ", x[j+1][i]=" << x[j+1][i] << endl;
                        cout << "\tx[j  ][im1]=" << x[j  ][im1] << ", x[j  ][i]=" << x[j  ][i] << endl;
                    }
                    else
                    {
                      //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
                      //cout << "\tx[j+1][im1]=" << x[j+1][im1] << ", x[j+1][i]=" << x[j+1][i] << endl;
                      //cout << "\tx[j  ][im1]=" << x[j  ][im1] << ", x[j  ][i]=" << x[j  ][i] << endl;

                      //x[j+1][im1]=0.979275, x[j+1][i]=0.977929
                      //x[j  ][im1]=-0.999508, x[j  ][i]=0.999373

                        ++numbProcessedBoundaryPoints;

                        glBegin(GL_TRIANGLES);
                          //Do triangle: j,im1, y1[0], y1[1]
                            x0 = x[j][i];
                            x1 = 1.0;
                            dd = 2.0+x[j][im1]-x0;
                            if(1.0e-10 < fabs(dd))
                                dx = (x1-x0)/dd;
                            else
                                dx = 0.5;
                            y1[0] = (1.0 - dx) * y[j][i] + dx * y[j][im1];

                            if(_fillValue > v[0][1])
                            {
                                if(_fillValue > v[0][0])
                                {
                                    v1[0] = (1.0 - dx) * v[0][1] + dx * v[0][0];
                                }
                                else
                                {
                                    v1[0] = v[0][1];
                                }

                                f1[0] = _vscale*(v1[0] - _valmin);
                            }
                            else
                            {
                                if(_fillValue > v[0][0])
                                {
                                    v1[0] = v[0][0];
                                    f1[0] = _vscale*(v1[0] - _valmin);
                                }
                                else
                                {
                                    f1[0] = 0.0;
                                }
                            }

                            x0 = x[j+1][im1];
                            dd = 2.0+x[j][im1]-x0;
                            if(1.0e-10 < fabs(dd))
                                dx = (x1-x0)/dd;
                            else
                                dx = 0.5;
                            y1[1] = (1.0 - dx) * y[j+1][im1] + dx * y[j][im1];

                            if(_fillValue > v[1][0])
                            {
                                if(_fillValue > v[0][0])
                                {
                                    v1[1] = (1.0 - dx) * v[1][0] + dx * v[0][0];
                                }
                                else
                                {
                                    v1[1] = v[1][0];
                                }
                
                                f1[1] = _vscale*(v1[1] - _valmin);
                            }
                            else
                            {
                                if(_fillValue > v[0][0])
                                {
                                    v1[1] = v[0][0];
                                    f1[1] = _vscale*(v1[1] - _valmin);
                                }
                                else
                                {
                                    f1[1] = 0.0;
                                }
                            }

                            x1 = -1.0;
                            glTexCoord1d(f1[0]);
                            glVertex3d(x1, y1[0], height);

                            glTexCoord1d(f1[1]);
                            glVertex3d(x1, y1[1], height);
                
                            glTexCoord1d(f[0][0]);
                            glVertex3d(x[j][im1], y[j][im1], height);

                          //Do triangle: y1[0], y1[1], j+1,im1
                            x1 = 1.0;
                            glTexCoord1d(f1[0]);
                            glVertex3d(x1, y1[0], height);

                            glTexCoord1d(f1[1]);
                            glVertex3d(x1, y1[1], height);

                            glTexCoord1d(f[1][0]);
                            glVertex3d(x[j+1][im1], y[j+1][im1], height);
                        glEnd();

                        glBegin(GL_QUADS);
                          //Do quad: j+1,im1, y1[0], j,i, j+1,i
                            x1 = 1.0;
                            glTexCoord1d(f[1][0]);
                            glVertex3d(x[j+1][im1], y[j+1][im1], height);

                            glTexCoord1d(f1[0]);
                            glVertex3d(x1, y1[0], height);

                            glTexCoord1d(f[0][1]);
                            glVertex3d(x[j][i], y[j][i], height);

                            glTexCoord1d(f[1][1]);
                            glVertex3d(x[j+1][i], y[j+1][i], height);
                        glEnd();
                    }
                }
            }
        }
        else
        {
            if(0.0 > x[j][i])
            {
                if(0.0 > x[j+1][im1])
                {
                    if(0.0 > x[j+1][i])
                    {
                        cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
                        cout << "\tx[j+1][im1]=" << x[j+1][im1] << ", x[j+1][i]=" << x[j+1][i] << endl;
                        cout << "\tx[j  ][im1]=" << x[j  ][im1] << ", x[j  ][i]=" << x[j  ][i] << endl;
                    }
                    else
                    {
                        cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
                        cout << "\tx[j+1][im1]=" << x[j+1][im1] << ", x[j+1][i]=" << x[j+1][i] << endl;
                        cout << "\tx[j  ][im1]=" << x[j  ][im1] << ", x[j  ][i]=" << x[j  ][i] << endl;
                    }
                }
                else
                {
                    if(0.0 > x[j+1][i])
                    {
                      //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
                      //cout << "\tx[j+1][im1]=" << x[j+1][im1] << ", x[j+1][i]=" << x[j+1][i] << endl;
                      //cout << "\tx[j  ][im1]=" << x[j  ][im1] << ", x[j  ][i]=" << x[j  ][i] << endl;
                      //x[j+1][im1]=0.99776, x[j+1][i]=-0.997354
                      //x[j  ][im1]=0.999857, x[j  ][i]=-0.995194

                        ++numbProcessedBoundaryPoints;

                      //Wei's note: 3/4/2013

                      //Add two quads:
                      //1. j,im1, y1[0]. y1[1], j+1,im1
                      //x0 = x[j][im1];
                        x1 = 1.0;
                        dd = 2.0+x[j][i]-x[j][im1];
                        if(1.0e-10 < fabs(dd))
                            dx = (x1-x[j][im1])/dd;
                        else
                            dx = 0.5;
                        y1[0] = (1.0 - dx) * y[j][im1] + dx * y[j][i];

                        if(_fillValue > v[0][0])
                        {
                            if(_fillValue > v[0][1])
                            {
                                v1[0] = (1.0 - dx) * v[0][0] + dx * v[0][1];
                            }
                            else
                            {
                                v1[0] = v[0][0];
                            }

                            f1[0] = _vscale*(v1[0] - _valmin);
                        }
                        else
                        {
                            if(_fillValue > v[0][1])
                            {
                                v1[0] = v[0][1];
                                f1[0] = _vscale*(v1[0] - _valmin);
                            }
                            else
                            {
                                f1[0] = 0.0;
                            }
                        }

                      //x0 = x[j+1][im1];
                        dd = 2.0+x[j+1][i]-x[j+1][im1];
                        if(1.0e-10 < fabs(dd))
                            dx = (x1-x[j+1][im1])/dd;
                        else
                            dx = 0.5;
                        y1[1] = (1.0 - dx) * y[j+1][im1] + dx * y[j+1][i];

                        if(_fillValue > v[1][0])
                        {
                            if(_fillValue > v[1][1])
                            {   
                                v1[1] = (1.0 - dx) * v[1][0] + dx * v[1][1];
                            }
                            else
                            {   
                                v1[1] = v[1][0];
                            }

                            f1[1] = _vscale*(v1[1] - _valmin);
                        }
                        else
                        {   
                            if(_fillValue > v[1][1])
                            {
                                v1[1] = v[1][1];
                                f1[1] = _vscale*(v1[1] - _valmin);
                            }
                            else
                            {
                                f1[1] = 0.0;
                            }
                        }

                        glBegin(GL_QUADS);
                          //Do quad: j,im1, y1[0], y1[1], j+1,im1
                            glTexCoord1d(f[0][0]);
                            glVertex3d(x[j][im1], y[j][im1], height);

                            glTexCoord1d(f1[0]);
                            glVertex3d(x1, y1[0], height);

                            glTexCoord1d(f1[1]);
                            glVertex3d(x1, y1[1], height);

                            glTexCoord1d(f[1][0]);
                            glVertex3d(x[j+1][im1], y[j+1][im1], height);

                            x1 = -1.0;
                          //Do quad: y1[0], j,i, j+1,i, y1[1]
                            glTexCoord1d(f1[0]);
                            glVertex3d(x1, y1[0], height);

                            glTexCoord1d(f[0][1]);
                            glVertex3d(x[j][i], y[j][i], height);

                            glTexCoord1d(f[1][1]);
                            glVertex3d(x[j+1][i], y[j+1][i], height);

                            glTexCoord1d(f1[1]);
                            glVertex3d(x1, y1[1], height);
                        glEnd();
                    }
                    else
                    {
                      //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
                      //cout << "\tx[j+1][im1]=" << x[j+1][im1] << ", x[j+1][i]=" << x[j+1][i] << endl;
                      //cout << "\tx[j  ][im1]=" << x[j  ][im1] << ", x[j  ][i]=" << x[j  ][i] << endl;

                      //x[j+1][im1]=0.985859, x[j+1][i]=0.986492
                      //x[j  ][im1]=0.999978, x[j  ][i]=-0.999144

                        ++numbProcessedBoundaryPoints;

                        glBegin(GL_TRIANGLES);
                          //Do triangle: y1[0], y1[1], j,i
                            x0 = x[j][im1];
                            x1 = 1.0;
                            dd = 2.0+x[j+1][i]-x0;
                            if(1.0e-10 < fabs(dd))
                                dx = (x1-x0)/dd;
                            else
                                dx = 0.5;
                            y1[0] = (1.0 - dx) * y[j][im1] + dx * y[j][i];

                            if(_fillValue > v[0][0])
                            {
                                if(_fillValue > v[0][1])
                                {
                                    v1[0] = (1.0 - dx) * v[0][0] + dx * v[0][1];
                                }
                                else
                                {
                                    v1[0] = v[0][0];
                                }

                                f1[0] = _vscale*(v1[0] - _valmin);
                            }
                            else
                            {
                                if(_fillValue > v[0][1])
                                {
                                    v1[0] = v[0][1];
                                    f1[0] = _vscale*(v1[0] - _valmin);
                                }
                                else
                                {
                                    f1[0] = 0.0;
                                }
                            }

                            x0 = x[j+1][i];
                            dd = 2.0+x[j][i]-x0;
                            if(1.0e-10 < fabs(dd))
                                dx = (x1-x0)/dd;
                            else
                                dx = 0.5;
                            y1[1] = (1.0 - dx) * y[j+1][i] + dx * y[j][i];

                            if(_fillValue > v[1][1])
                            {
                                if(_fillValue > v[0][1])
                                {
                                    v1[1] = (1.0 - dx) * v[1][1] + dx * v[0][1];
                                }
                                else
                                {
                                    v1[1] = v[0][1];
                                }
                
                                f1[1] = _vscale*(v1[1] - _valmin);
                            }
                            else
                            {
                                if(_fillValue > v[0][1])
                                {
                                    v1[1] = v[0][1];
                                    f1[1] = _vscale*(v1[1] - _valmin);
                                }
                                else
                                {
                                    f1[1] = 0.0;
                                }
                            }

                            x1 = -1.0;
                            glTexCoord1d(f1[0]);
                            glVertex3d(x1, y1[0], height);

                            glTexCoord1d(f1[1]);
                            glVertex3d(x1, y1[1], height);
                
                            glTexCoord1d(f[0][1]);
                            glVertex3d(x[j][i], y[j][i], height);

                          //Do triangle: y1[0], y1[1], j+1,im1
                            x1 = 1.0;
                            glTexCoord1d(f1[0]);
                            glVertex3d(x1, y1[0], height);

                            glTexCoord1d(f1[1]);
                            glVertex3d(x1, y1[1], height);

                            glTexCoord1d(f[1][1]);
                            glVertex3d(x[j+1][i], y[j+1][i], height);
                        glEnd();

                        glBegin(GL_QUADS);
                          //Do quad: j,im1, y1[0], j+1,i, j_1,im1
                            x1 = 1.0;
                            glTexCoord1d(f[0][0]);
                            glVertex3d(x[j][im1], y[j][im1], height);

                            glTexCoord1d(f1[0]);
                            glVertex3d(x1, y1[0], height);

                            glTexCoord1d(f[1][1]);
                            glVertex3d(x[j+1][i], y[j+1][i], height);

                            glTexCoord1d(f[1][0]);
                            glVertex3d(x[j+1][im1], y[j+1][im1], height);
                        glEnd();
                    }
                }
            }
            else
            {
                if(0.0 > x[j+1][im1])
                {
                    if(0.0 > x[j+1][i])
                    {
                        cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
                        cout << "\tx[j+1][im1]=" << x[j+1][im1] << ", x[j+1][i]=" << x[j+1][i] << endl;
                        cout << "\tx[j  ][im1]=" << x[j  ][im1] << ", x[j  ][i]=" << x[j  ][i] << endl;
                    }
                    else
                    {
                        cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
                        cout << "\tx[j+1][im1]=" << x[j+1][im1] << ", x[j+1][i]=" << x[j+1][i] << endl;
                        cout << "\tx[j  ][im1]=" << x[j  ][im1] << ", x[j  ][i]=" << x[j  ][i] << endl;
                    }
                }
                else
                {
                    if(0.0 > x[j+1][i])
                    {
                        cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
                        cout << "\tx[j+1][im1]=" << x[j+1][im1] << ", x[j+1][i]=" << x[j+1][i] << endl;
                        cout << "\tx[j  ][im1]=" << x[j  ][im1] << ", x[j  ][i]=" << x[j  ][i] << endl;
                    }
                    else
                    {
                        cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
                        cout << "\tx[j+1][im1]=" << x[j+1][im1] << ", x[j+1][i]=" << x[j+1][i] << endl;
                        cout << "\tx[j  ][im1]=" << x[j  ][im1] << ", x[j  ][i]=" << x[j  ][i] << endl;
                    }
                }
            }
        }
    }

    if(numbProcessedBoundaryPoints != numbBoundaryPoints)
    {
        cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
        cout << "\tnumbProcessedBoundaryPoints = " << numbProcessedBoundaryPoints
             << ", numbBoundaryPoints = " << numbBoundaryPoints << endl;
    }

  //Do the greenland, and south pole area.
    glBegin(GL_QUAD_STRIP);
        j = nlat - 1;
        xp = 0.5 * (x[j][0] + x[j][nlon/2]);
        yp = 0.5 * (y[j][0] + y[j][nlon/2]);
        for(i = 0; i < nlon; ++i)
        {
            glTexCoord1d(1.0);
            glVertex3d(xp, yp, height);

            glTexCoord1d(1.0);
            glVertex3d(x[j][i], y[j][i], height);
        }

        glTexCoord1d(1.0);
        glVertex3d(xp, yp, height);

        glTexCoord1d(1.0);
        glVertex3d(x[j][0], y[j][0], height);
    glEnd();

    j = 0;
    yp = -0.5;

    glBegin(GL_QUAD_STRIP);
        for(i = 0; i < nlon; ++i)
        {
            glTexCoord1d(1.0);
            glVertex3d(x[j][i], yp, height);

            glTexCoord1d(1.0);
            glVertex3d(x[j][i], y[j][i], height);
        }
    glEnd();

    for(i = 1; i < nlon; ++i)
    {
        if((0.9 < x[j][i-1]) && (-0.9 > x[j][i]))
        {
            glBegin(GL_QUADS);
                glTexCoord1d(1.0);
                glVertex3d(x[j][i-1], yp, height);

                glTexCoord1d(1.0);
                glVertex3d(1.0, yp, height);

                glTexCoord1d(1.0);
                glVertex3d(1.0, y[j][i-1], height);

                glTexCoord1d(1.0);
                glVertex3d(x[j][i-1], y[j][i-1], height);
            glEnd();

            glBegin(GL_QUADS);
                glTexCoord1d(1.0);
                glVertex3d(-1.0, yp, height);

                glTexCoord1d(1.0);
                glVertex3d(x[j][i], yp, height);

                glTexCoord1d(1.0);
                glVertex3d(x[j][i], y[j][i], height);

                glTexCoord1d(1.0);
                glVertex3d(-1.0, y[j][i], height);
            glEnd();

            break;
        }
    }

    glDisable(GL_POLYGON_OFFSET_FILL);
    glDisable(GL_TEXTURE_1D);

    glLineWidth(2.0);

    glColor3f(1.0, 1.0, 1.0);

    glBegin(GL_LINE_STRIP);
        glVertex3d(-1.0, -0.5, height);
        glVertex3d( 1.0, -0.5, height);
        glVertex3d( 1.0,  0.5, height);
        glVertex3d( 1.0, -0.5, height);
        glVertex3d(-1.0, -0.5, height);
    glEnd();

    glPopMatrix();

    glEndList();
}

void POP2dViewer::_evaluate(double *var)
{
    int size;
    int n = 0;
    char vn[128];

    strcpy(vn, _varname.c_str());

    size = geometry->get_pop_nlon()
         * geometry->get_pop_nlat() 
         * geometry->get_pop_nlev();

  //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\t_varname: <" << _varname << ">" << endl;
  //cout << "\tnlon = " << geometry->get_pop_nlon()
  //     << ", nlat = " << geometry->get_pop_nlat()
  //     << ", nlev = " << geometry->get_pop_nlev() << endl;

    _fillValue = 0.5 * geometry->get_fillValue();

    if(geometry->hasFillValue())
    {
        _valmin = geometry->get_fillValue();
        _valmax = -_valmin;

        for(n = 0; n < size; ++n)
        {
            if(_fillValue < var[n])
                continue;

            if(_valmax < var[n])
            {
               _valmax = var[n];
            }
            if(_valmin > var[n])
            {
               _valmin = var[n];
            }
        }
    }
    else
    {
        _valmax = var[0];
        _valmin = var[0];

        for(n = 0; n < size; ++n)
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
    }

    if(1.0e-10 > (_valmax - _valmin))
       _valmax += 1.0e-10;

  //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\t_valmin = " << _valmin << ", _valmax = " << _valmax << endl;
}

void POP2dViewer::_displayXflat(int xl)
{
    int j, k, n;

    int nbgn, nend;
    double tmplon[nlat];
    double tmplat[nlat];
    bool need_second_part = false;

    double fact;
    double height[nlev];
    double val[nlev][nlat];
    double sz = 1.0 / z_w[nlev - 1];

    for(k = 0; k < nlev; ++k)
    {
        height[k] = 0.5 - z_w[k] * sz;

        n = k * nlat * nlon;
        for(j = 0; j < nlat; ++j)
        {
            val[k][j] = _var[n + j * nlon + xl];
        }
    }

    _xlist[xl] = glGenLists(1);
    glNewList(_xlist[xl], GL_COMPILE_AND_EXECUTE);

    _vscale = 0.9 / (_valmax - _valmin);

  //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\t_varname: <" << _varname << ">" << endl;
  //cout << "\tnlon = " << nlon << ", nlat = " << nlat << ", nlev = " << nlev << endl;
  //cout << "\t_valmin = " << _valmin << ", _valmax = " << _valmax << ", _vscale = " << _vscale << endl;
  //cout << "\t_xlist[xl] = " << _xlist[xl] << endl;

    if(geometry->isUpoint())
    {
        lon = geometry->get_pop_ulon();
        lat = geometry->get_pop_ulat();
    }
    else
    {
        lon = geometry->get_pop_tlon();
        lat = geometry->get_pop_tlat();
    }

    glPushMatrix();

    glColor4f(1.0, 1.0, 1.0, 1.0);

    glEnable(GL_TEXTURE_1D);
    glBindTexture(GL_TEXTURE_1D, texture1d->get_textureID());

    nend = nlat;

    for(j = 0; j < nlat; ++j)
    {
        tmplon[j] = lon[j*nlon+xl];
        tmplat[j] = lat[j*nlon+xl];
    }

    for(j = 1; j < nlat; ++j)
    {
        if(((315.0 < tmplon[j-1]) && (45.00 > tmplon[j])) || 
           ((45.00 > tmplon[j-1]) && (315.0 < tmplon[j])))
        {
            need_second_part = true;

            nend = j;
            nbgn = j - 1;

            break;
        }
    }

    for(j = 1; j < nend; ++j)
    {
        glBegin(GL_QUAD_STRIP);
        for(k = 0; k < nlev - 1; ++k)
        {
            if(_fillValue > val[k][j-1])
                fact = _vscale*(val[k][j-1] - _valmin);
            else
                fact = 0.0;
            _lonlat2flat(tmplon[j-1], tmplat[j-1], height[k], fact);

            if(_fillValue > val[k][j])
                fact = _vscale*(val[k][j] - _valmin);
            else
                fact = 0.0;

            _lonlat2flat(tmplon[j], tmplat[j], height[k], fact);
        }
        glEnd();
    }

    if(need_second_part)
    {
        if(315.0 < tmplon[nbgn])
            tmplon[nbgn] -= 360.0;
        else if(45.0 > tmplon[nbgn])
            tmplon[nbgn] += 360.0;

        for(j = nbgn+1; j < nlat; ++j)
        {
            glBegin(GL_QUAD_STRIP);
            for(k = 0; k < nlev - 1; ++k)
            {
                if(_fillValue > val[k][j-1])
                    fact = _vscale*(val[k][j-1] - _valmin);
                else
                    fact = 0.0;
                _lonlat2flat(tmplon[j-1], tmplat[j-1], height[k], fact);

                if(_fillValue > val[k][j])
                    fact = _vscale*(val[k][j] - _valmin);
                else
                    fact = 0.0;

                _lonlat2flat(tmplon[j], tmplat[j], height[k], fact);
            }
            glEnd();
        }
    }

    glDisable(GL_TEXTURE_1D);

    glPopMatrix();

    glEndList();
}

void POP2dViewer::_displayYflat(int yl)
{
    int i, k, n;

    int nbgn, nend;
    double tmplon[nlon+1];
    double tmplat[nlon+1];
    bool need_second_part = false;

    double fact;
    double height[nlev];
    double v[nlev][nlon+1];
    double sz = 1.0 / z_w[nlev - 1];

    for(k = 0; k < nlev; ++k)
    {
        height[k] = 0.5 - z_w[k] * sz;

        n = (k * nlat + yl) * nlon;
        for(i = 0; i < nlon; ++i)
        {
            v[k][i] = _var[n + i];
        }
        v[k][nlon] = v[k][0];
    }

    _ylist[yl] = glGenLists(1);
    glNewList(_ylist[yl], GL_COMPILE_AND_EXECUTE);

    _vscale = 0.9 / (_valmax - _valmin);

  //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\t_varname: <" << _varname << ">" << endl;
  //cout << "\tnlon = " << nlon << ", nlat = " << nlat << ", nlev = " << nlev << endl;
  //cout << "\t_valmin = " << _valmin << ", _valmax = " << _valmax << ", _vscale = " << _vscale << endl;
  //cout << "\t_ylist[yl] = " << _ylist[yl] << endl;

    if(geometry->isUpoint())
    {
        lon = geometry->get_pop_ulon();
        lat = geometry->get_pop_ulat();
    }
    else
    {
        lon = geometry->get_pop_tlon();
        lat = geometry->get_pop_tlat();
    }

    n = yl * nlon;
    nend = nlat;

    for(i = 0; i < nlon; ++i)
    {
        tmplon[i] = lon[n+i];
        tmplat[i] = lat[n+i];
    }

    tmplon[nlon] = tmplon[0];
    tmplat[nlon] = tmplat[0];

    for(i = 1; i < nlon; ++i)
    {
        if(((315.0 < tmplon[i-1]) && (45.00 > tmplon[i])) ||
           ((45.00 > tmplon[i-1]) && (315.0 < tmplon[i])))
        {
            need_second_part = true;

            nend = i;
            nbgn = i - 1;

          //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
          //cout << "\tnbgn = " << nbgn << ", nend = " << nend << endl;
          //cout << "\tneed_second_part = " << need_second_part << endl;

            break;
        }
    }

    glPushMatrix();

    glColor4f(1.0, 1.0, 1.0, 1.0);

    glEnable(GL_TEXTURE_1D);
    glBindTexture(GL_TEXTURE_1D, texture1d->get_textureID());

    for(i = 1; i < nend; ++i)
    {
        glBegin(GL_QUAD_STRIP);
        for(k = 0; k < nlev - 1; ++k)
        {
            if(_fillValue > v[k][i-1])
                fact = _vscale*(v[k][i-1] - _valmin);
            else
                fact = 0.0;
            _lonlat2flat(tmplon[i-1], tmplat[i-1], height[k], fact);

            if(_fillValue > v[k][i])
                fact = _vscale*(v[k][i] - _valmin);
            else
                fact = 0.0;
            _lonlat2flat(tmplon[i], tmplat[i], height[k], fact);
        }
        glEnd();
    }

    if(need_second_part)
    {
        if(315.0 < tmplon[nbgn])
            tmplon[nbgn] -= 360.0;
        else if(45.0 > tmplon[nbgn])
            tmplon[nbgn] += 360.0;

        for(i = nbgn+1; i <= nlon; ++i)
        {
          //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
          //cout << "\ti = " << i << ", lon = " << tmplon[i] << ", lat = " << tmplat[i] << endl;

            glBegin(GL_QUAD_STRIP);
            for(k = 0; k < nlev - 1; ++k)
            {
                if(_fillValue > v[k][i-1])
                    fact = _vscale*(v[k][i-1] - _valmin);
                else
                    fact = 0.0;
                _lonlat2flat(tmplon[i-1], tmplat[i-1], height[k], fact);

                if(_fillValue > v[k][i])
                    fact = _vscale*(v[k][i] - _valmin);
                else
                    fact = 0.0;
                _lonlat2flat(tmplon[i], tmplat[i], height[k], fact);
            }
            glEnd();
        }
    }

  //glDisable(GL_CULL_FACE);

    glDisable(GL_TEXTURE_1D);

    glPopMatrix();

    glEndList();
}

