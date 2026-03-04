//$Id: gl_viewer.cpp 5321 2015-02-20 03:20:52Z starviewer $

#include <QtOpenGL>

#include "gl_viewer.h"

#define ARC     (3.14159256359/180.0)
#define Cos(th) cos(ARC*(th))
#define Sin(th) sin(ARC*(th))

GL_Viewer::GL_Viewer(ColorTable* ct, NVOptions* opt)
{
    colorTable = ct;
    nvoptions = opt;

    leveler = new Leveler();

    texture1d = new Texture1d();
    texture1d->set_colors(ct->get_clen(), ct->get_cmap());
    texture1d->set_name(ct->get_name());

    coastline = new CoastLine();

    deg2rad = 3.14159265359/180.0;
    oneover = 1.0/180.0;

    lon = NULL;
    lat = NULL;
    lev = NULL;
    pltvar = NULL;

    xa = NULL;
    xb = NULL;
    ya = NULL;

    _var = NULL;

    evaluator = new Evaluator();
  //earth = new Earth();

    _selectingOn = false;

    _xlist = NULL;
    _ylist = NULL;
    _zlist = NULL;

    _xfactor = 999.0;

    _first_time = true;
}

GL_Viewer::~GL_Viewer()
{
    free(_xlist);
    free(_ylist);
    free(_zlist);

    delete [] xa;
    delete [] xb;
    delete [] ya;

    delete coastline;
  //delete earth;
    delete evaluator;
    delete leveler;
    delete texture1d;
}

void GL_Viewer::draw()
{
    if(nvoptions->get_cb(NV_RESET))
    {
        nvoptions->set_cb(NV_RESET, false);
        reset();
    }

  //glPushMatrix();

    if(nvoptions->get_cb(NV_SPHEREON))
    {
        if(nvoptions->get_cb(NV_ONMAPON))
        {
            _display_on_sphere_map();
        }
        else
             _display_on_sphere();
    }
    else if(nvoptions->get_cb(NV_BUMPON))
    {
        _display_with_bump();
    }
    else if(nvoptions->get_cb(NV_ONHEIGHTSURFACE))
    {
        _display_on_height_surface();
    }
    else if(nvoptions->get_cb(NV_ONMAPON))
    {
        _display_on_map();
    }
    else
    {
        _display_all();
    }

  //glPopMatrix();
}

void GL_Viewer::_parameter_setup()
{
    int i, j;

    string tex1dName = texture1d->get_name();

    if(0 != tex1dName.compare(colorTable->get_name()))
    {
        texture1d->set_colors(colorTable->get_clen(), colorTable->get_cmap());
        texture1d->set_name(colorTable->get_name());
    }

    hasFillValue = geometry->get_hasFillValue();
    if(hasFillValue)
    {
        fillValue = geometry->get_fillValue();

      //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
      //cout << "\tfillValue = " << fillValue << endl;
    }

    vMinimum = evaluator->get_min();
    vMaximum = evaluator->get_max();
    scale  = 1.0/(vMaximum - vMinimum + 1.0e-20);
    zScale = 1.0/(nz + 0.01);

    if(nx > ny)
    {
        xyDelt = 1.0 / (nx - 1.0);
        yStart = 0.5 * (1.0 - (ny - 1.0) * xyDelt);
        xStart = 0.0;
    }
    else
    {
        xyDelt = 1.0 / (ny - 1.0);
        xStart = 0.5 * (1.0 - (nx - 1.0) * xyDelt);
        yStart = 0.0;
    }

  //xStart -= 0.5;
  //yStart -= 0.5;

    if(1 < nz)
        zDelt = 0.5 / (nz - 1.0);
    else
        zDelt = 1.0;

    if(NULL == xa)
        xa = new double[nx];
    if(NULL == xb)
        xb = new double[nx];
    if(NULL == ya)
        ya = new double[ny];

    need_adjust = false;
    _hasGeoInfo = false;

    if(geometry->get_has1dLon() && geometry->get_has1dLat())
        _hasGeoInfo = true;

    if(_hasGeoInfo)
    {
        lon = geometry->get_lon();
        lat = geometry->get_lat();
        lev = geometry->get_lev();

        if((lon[0] < -100.0) && (lon[nx-1] > 100.0))
            need_adjust = true;

        for(i = 0; i < nx; ++i)
        {
            xa[i] = lon[i] * oneover - 1.0;
            xb[i] = lon[i] * oneover + 1.0;
        }

        for(j = 0; j < ny; ++j)
            ya[j] = lat[j] * oneover;
    }
    else
    {
        for(i = 0; i < nx; ++i)
        {
            xa[i] = xStart + i * xyDelt - 0.5;
            xb[i] = xStart + i * xyDelt - 0.5;
        }
        for(j = 0; j < ny; ++j)
            ya[j] = yStart + j * xyDelt - 0.5;
    }

    if((NULL == _xlist) && (NULL == _ylist) && (NULL == _zlist))
    {
        _xlist = (int*) malloc(nx * sizeof(int));
        assert(_xlist);
        _ylist = (int*) malloc(ny * sizeof(int));
        assert(_ylist);
        _zlist = (int*) malloc(nz * sizeof(int));
        assert(_zlist);

        for(i = 0; i < nx; ++i)
            _xlist[i] = -1;
        for(i = 0; i < ny; ++i)
            _ylist[i] = -1;
        for(i = 0; i < nz; ++i)
            _zlist[i] = -1;
    }

    reset();
}

void GL_Viewer::update_colormap()
{
    reset();
    texture1d->reset();
    texture1d->set_colors(colorTable->get_clen(), colorTable->get_cmap());
    texture1d->set_name(colorTable->get_name());
}

void GL_Viewer::reset()
{
    int i;

    for(i = 0; i < nz; ++i)
    {
        if(0 <= _zlist[i])
            glDeleteLists(_zlist[i], 1);

        _zlist[i] = -1;
    }

    for(i = 0; i < ny; ++i)
    {
        if(0 <= _ylist[i])
            glDeleteLists(_ylist[i], 1);

        _ylist[i] = -1;
    }

    for(i = 0; i < nx; ++i)
    {
        if(0 <= _xlist[i])
            glDeleteLists(_xlist[i], 1);

        _xlist[i] = -1;
    }
}

void GL_Viewer::_display_all()
{
    if((nx > nvoptions->get_xsec()) && (nz > 1))
    {
        if(0 <= _xlist[nvoptions->get_xsec()])
            glCallList(_xlist[nvoptions->get_xsec()]);
        else
            _display_Xplane(nvoptions->get_xsec());

        nvoptions->set_xsec(nvoptions->get_xsec());
    }

    if(nvoptions->get_cb(NV_HASX2) && (nx > nvoptions->get_xsec2()) && (nz > 1))
    {
        if(0 <= _xlist[nvoptions->get_xsec2()])
            glCallList(_xlist[nvoptions->get_xsec2()]);
        else
            _display_Xplane(nvoptions->get_xsec2());

        nvoptions->set_xsec2(nvoptions->get_xsec2());
    }

    if((ny > nvoptions->get_ysec()) && (nz > 1))
    {
        if(0 <= _ylist[nvoptions->get_ysec()])
            glCallList(_ylist[nvoptions->get_ysec()]);
        else
            _display_Yplane(nvoptions->get_ysec());

        nvoptions->set_ysec(nvoptions->get_ysec());
    }

    if(nvoptions->get_cb(NV_HASY2) && (ny > nvoptions->get_ysec2()) && (nz > 1))
    {
        if(0 <= _ylist[nvoptions->get_ysec2()])
            glCallList(_ylist[nvoptions->get_ysec2()]);
        else
            _display_Yplane(nvoptions->get_ysec2());

        nvoptions->set_ysec2(nvoptions->get_ysec2());
    }

    if(nz > nvoptions->get_zsec())
    {
        if(0 <= _zlist[nvoptions->get_zsec()])
            glCallList(_zlist[nvoptions->get_zsec()]);
        else
            _display_Zplane(nvoptions->get_zsec());

        nvoptions->set_zsec(nvoptions->get_zsec());
    }
  //else
  //    earth->draw_plane(0.0);

    if(nvoptions->get_cb(NV_HASZ2) && (nz > nvoptions->get_zsec2()) && (nz > 1))
    {
        if(0 <= _zlist[nvoptions->get_zsec2()])
            glCallList(_zlist[nvoptions->get_zsec2()]);
        else
            _display_Zplane(nvoptions->get_zsec2());

        nvoptions->set_zsec2(nvoptions->get_zsec2());
    }
}

void GL_Viewer::_display_Xplane(int xs)
{
    int j, k, n;
    double f;
    double x, z1, z2;
    double value[nz][ny];

    for(k = 0; k < nz; ++k)
    {
        for(j = 0; j < ny; ++j)
        {
            n = xs + (j + k * ny) * nx;
            value[k][j] = _var[n];
        }
    }

    if(need_adjust)
    {
        if(xs >= nx/2)
            x = xa[xs];
        else
            x = xb[xs];
    }
    else
        x = xa[xs];

    _xlist[k] = glGenLists(1);
  //glNewList(_xlist[k], GL_COMPILE);
    glNewList(_xlist[k], GL_COMPILE_AND_EXECUTE);

  //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\t_xlist[k] = " << _xlist[k] << endl;

    glColor4d(1.0, 1.0, 1.0, 1.0);
  //glColor4d(0.0, 0.0, 0.0, 0.0);
  //glNormal3d(0.0, 0.0, -1.0);

    glEnable( GL_TEXTURE_1D );
    glBindTexture(GL_TEXTURE_1D, texture1d->get_textureID());

    for(k = 0; k < nz - 1; ++k)
    {
      //z1 = (nz - 1 - k) * zDelt;
      //z2 = z1 - zDelt;
        z1 = k * zDelt;
        z2 = z1 + zDelt;

        glBegin(GL_QUAD_STRIP);
            for(j = 0; j < ny; ++j)
            {
                f = scale * (value[k][j] - vMinimum);
                glTexCoord1d(f);
                glVertex3d(x, ya[j], z1);

                f = scale * (value[k+1][j] - vMinimum);
                glTexCoord1d(f);
                glVertex3d(x, ya[j], z2);
            }
        glEnd();
    }

    glDisable(GL_TEXTURE_1D);

    glEndList();
}

void GL_Viewer::_display_Yplane(int ys)
{
    int i, k, n;
    double f;
    double y, z1, z2;
    double value[nz][nx];

    for(k = 0; k < nz; ++k)
    {
        for(i = 0; i < nx; ++i)
        {
            n = i + (ys + k * ny) * nx;
            value[k][i] = _var[n];
        }
    }

    y = ya[ys];

    _ylist[k] = glGenLists(1);
  //glNewList(_ylist[k], GL_COMPILE);
    glNewList(_ylist[k], GL_COMPILE_AND_EXECUTE);

  //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\t_ylist[k] = " << _ylist[k] << endl;

    glColor4d(1.0, 1.0, 1.0, 1.0);
  //glColor4d(0.0, 0.0, 0.0, 0.0);
  //glNormal3d(0.0, 0.0, -1.0);

    glEnable( GL_TEXTURE_1D );
    glBindTexture(GL_TEXTURE_1D, texture1d->get_textureID());

    for(k = 0; k < nz - 1; ++k)
    {
      //z1 = (nz - 1 - k) * zDelt;
      //z2 = z1 - zDelt;
        z1 = k * zDelt;
        z2 = z1 + zDelt;

        if(need_adjust)
        {
            glBegin(GL_QUAD_STRIP);
            for(i = nx/2; i < nx; ++i)
            {
                f = scale * (value[k][i] - vMinimum);
                glTexCoord1d(f);
                glVertex3d(xa[i], y, z1);

                f = scale * (value[k+1][i] - vMinimum);
                glTexCoord1d(f);
                glVertex3d(xa[i], y, z2);
            }

            for(i = 0; i <= nx/2; ++i)
            {
                f = scale * (value[k][i] - vMinimum);
                glTexCoord1d(f);
                glVertex3d(xb[i], y, z1);

                f = scale * (value[k+1][i] - vMinimum);
                glTexCoord1d(f);
                glVertex3d(xb[i], y, z2);
            }
            glEnd();
        }
        else
        {
            glBegin(GL_QUAD_STRIP);
            for(i = 0; i < nx; ++i)
            {
                f = scale * (value[k][i] - vMinimum);
                glTexCoord1d(f);
                glVertex3d(xa[i], y, z1);

                f = scale * (value[k+1][i] - vMinimum);
                glTexCoord1d(f);
                glVertex3d(xa[i], y, z2);
            }
            glEnd();
        }
    }

    glDisable(GL_TEXTURE_1D);

    glEndList();
}

void GL_Viewer::_display_Zplane(int zs)
{
    int i, j, n;
    double *pltvar = NULL;
    double f;
    double y1, y2, z;

    n = zs * geometry->get_nx() * geometry->get_ny();

    pltvar = &_var[n];

    if(1 < nz)
    {
      //z = zs * zDelt - 0.5;
        z = zs * zDelt;
    }
    else
    {
        z = 0.0;
      //z = 0.5;
    }

    _zlist[zs] = glGenLists(1);
  //glNewList(_zlist[zs], GL_COMPILE);
    glNewList(_zlist[zs], GL_COMPILE_AND_EXECUTE);

  //cout << "\nfunction: " << __PRETTY_FUNCTION__
  //     << ", file: " << __FILE__
  //     << ", line: " << __LINE__ << endl;
  //cout << "\tvMinimum = " << vMinimum << ", vMaximum = " << vMaximum << endl;
  //     << ", GL_SELECT = " << GL_SELECT << endl;
  //cout << "\t_zlist[zs] = " << _zlist[zs] << endl;

    glColor4d(1.0, 1.0, 1.0, 1.0);
  //glColor4d(0.0, 0.0, 0.0, 0.0);
  //glNormal3d(0.0, 0.0, -1.0);

    glEnable( GL_TEXTURE_1D );
    glBindTexture(GL_TEXTURE_1D, texture1d->get_textureID());

  //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //for(i = 0; i < nx; ++i)
  //    cout << "\tlon[" << i << "] = " << lon[i] << endl;

    for(j = 0; j < ny - 1; ++j)
    {
        y1 = ya[j];
        y2 = ya[j + 1];

        if(hasFillValue && (0.0 < fillValue))
        {
            if(need_adjust)
            {
                glBegin(GL_QUAD_STRIP);
                for(i = 0; i <= nx/2; ++i)
                {
                    n = j * nx + i;
                    if(pltvar[n] > 0.95*fillValue)
                        f = 1.0;
                    else
                        f = scale * (pltvar[n] - vMinimum);
                    glTexCoord1d(f);
                    glVertex3d(xb[i], y1, z);

                    n += nx;
                    if(pltvar[n] > 0.95*fillValue)
                        f = 1.0;
                    else
                        f = scale * (pltvar[n] - vMinimum);
                    glTexCoord1d(f);
                    glVertex3d(xb[i], y2, z);
                }

                for(i = nx/2; i < nx; ++i)
                {
                    n = j * nx + i;
                    if(pltvar[n] > 0.95*fillValue)
                        f = 1.0;
                    else
                        f = scale * (pltvar[n] - vMinimum);
                    glTexCoord1d(f);
                    glVertex3d(xa[i], y1, z);

                    n += nx;
                    if(pltvar[n] > 0.95*fillValue)
                        f = 1.0;
                    else
                        f = scale * (pltvar[n] - vMinimum);
                    glTexCoord1d(f);
                    glVertex3d(xa[i], y2, z);
                }
                glEnd();
            }
            else
            {
                glBegin(GL_QUAD_STRIP);
                for(i = 0; i < nx; ++i)
                {
                    n = j * nx + i;
                    if(pltvar[n] > 0.95*fillValue)
                        f = 1.0;
                    else
                        f = scale * (pltvar[n] - vMinimum);
                    glTexCoord1d(f);
                    glVertex3d(xa[i], y1, z);

                    n += nx;
                    if(pltvar[n] > 0.95*fillValue)
                        f = 1.0;
                    else
                        f = scale * (pltvar[n] - vMinimum);
                    glTexCoord1d(f);
                    glVertex3d(xa[i], y2, z);
                }
                glEnd();
            }
        }
        else if(hasFillValue && (0.0 >= fillValue))
        {
            if(need_adjust)
            {
                glBegin(GL_QUAD_STRIP);
                for(i = 0; i <= nx/2; ++i)
                {
                    n = j * nx + i;
                    if(pltvar[n] < 0.95*fillValue)
                        f = 0.0;
                    else
                        f = scale * (pltvar[n] - vMinimum);
                    glTexCoord1d(f);
                    glVertex3d(xb[i], y1, z);

                    n += nx;
                    if(pltvar[n] < 0.95*fillValue)
                        f = 0.0;
                    else
                        f = scale * (pltvar[n] - vMinimum);
                    glTexCoord1d(f);
                    glVertex3d(xb[i], y2, z);
                }

                for(i = nx/2; i < nx; ++i)
                {
                    n = j * nx + i;
                    if(pltvar[n] < 0.95*fillValue)
                        f = 0.0;
                    else
                        f = scale * (pltvar[n] - vMinimum);
                    glTexCoord1d(f);
                    glVertex3d(xa[i], y1, z);

                    n += nx;
                    if(pltvar[n] < 0.95*fillValue)
                        f = 0.0;
                    else
                        f = scale * (pltvar[n] - vMinimum);
                    glTexCoord1d(f);
                    glVertex3d(xa[i], y2, z);
                }
                glEnd();
            }
            else
            {
                glBegin(GL_QUAD_STRIP);
                for(i = 0; i < nx; ++i)
                {
                    n = j * nx + i;
                    if(pltvar[n] < 0.95*fillValue)
                        f = 0.0;
                    else
                        f = scale * (pltvar[n] - vMinimum);
                    glTexCoord1d(f);
                    glVertex3d(xa[i], y1, z);

                    n += nx;
                    if(pltvar[n] < 0.95*fillValue)
                        f = 0.0;
                    else
                        f = scale * (pltvar[n] - vMinimum);
                    glTexCoord1d(f);
                    glVertex3d(xa[i], y2, z);
                }
                glEnd();
            }
        }
        else
        {
            if(need_adjust)
            {
                glBegin(GL_QUAD_STRIP);
                for(i = 0; i <= nx/2; ++i)
                {
                    n = j * nx + i;
                    f = scale * (pltvar[n] - vMinimum);
                    glTexCoord1d(f);
                    glVertex3d(xb[i], y1, z);

                    n += nx;
                    f = scale * (pltvar[n] - vMinimum);
                    glTexCoord1d(f);
                    glVertex3d(xb[i], y2, z);
                }

                for(i = nx/2; i < nx; ++i)
                {
                    n = j * nx + i;
                    f = scale * (pltvar[n] - vMinimum);
                    glTexCoord1d(f);
                    glVertex3d(xa[i], y1, z);

                    n += nx;
                    f = scale * (pltvar[n] - vMinimum);
                    glTexCoord1d(f);
                    glVertex3d(xa[i], y2, z);
                }
                glEnd();
            }
            else
            {
                glBegin(GL_QUAD_STRIP);
                for(i = 0; i < nx; ++i)
                {
                    n = j * nx + i;
                    f = scale * (pltvar[n] - vMinimum);
                    glTexCoord1d(f);
                    glVertex3d(xa[i], y1, z);

                    n += nx;
                    f = scale * (pltvar[n] - vMinimum);
                    glTexCoord1d(f);
                    glVertex3d(xa[i], y2, z);
                }
                glEnd();
            }
        }
    }

    glDisable(GL_TEXTURE_1D);

  //if(_hasGeoInfo)
  //    coastline->drawONplane2(z+0.01, 2);

    glEndList();
}

void GL_Viewer::_display_with_bump()
{
    if((geometry->get_nx() > nvoptions->get_xsec()) && (geometry->get_nz() > 1))
    {
        _display_Xplane_with_bump(nvoptions->get_xsec());
    }

    if((geometry->get_ny() > nvoptions->get_ysec()) && (geometry->get_nz() > 1))
    {
        _display_Yplane_with_bump(nvoptions->get_ysec());
    }

    if(geometry->get_nz() > nvoptions->get_zsec())
    {
        if(0 <= _zlist[nvoptions->get_zsec()])
            glCallList(_zlist[nvoptions->get_zsec()]);
        else
            _display_Zplane_with_bump(nvoptions->get_zsec());
    }
}

void GL_Viewer::_display_Xplane_with_bump(int xs)
{
    cout << "Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__
         << ", file: <" << __FILE__ << ">" << endl;
    cout << "\tWant to view: <" << _varname << ">, xs = " << xs << endl;

#if 0
    int j, k, n;
    double f;
    double amp = 0.2;
    double offset = 0.5;
    double x1, y1, z1, z2;
    double value[nz][ny];

    for(k = 0; k < nz; ++k)
    {
        for(j = 0; j < ny; ++j)
        {
            n = xs + (j + k * ny) * nx;
            value[k][j] = _var[n];
        }
    }

    x1 = xStart + xs * xyDelt;

    glDisable(GL_TEXTURE_GEN_S);
    glEnable( GL_TEXTURE_1D );
    glBindTexture(GL_TEXTURE_1D, texture1d->get_textureID());

    for(k = 0; k < nz - 1; ++k)
    {
        z1 = k * zDelt;
        z2 = z1 + zDelt;

        glBegin(GL_QUAD_STRIP);
            for(j = 0; j < ny; ++j)
            {
                y1 = yStart + j * xyDelt;

                f = scale * (value[k][j] - vMinimum);
                glTexCoord1d(f);
                glVertex3d(x1 + amp * (f - offset), y1, z1);

                f = scale * (value[k+1][j] - vMinimum);
                glTexCoord1d(f);
                glVertex3d(x1 + amp * (f - offset), y1, z2);
            }
        glEnd();
    }

    glDisable(GL_TEXTURE_1D);
#endif
}

void GL_Viewer::_display_Yplane_with_bump(int ys)
{
    cout << "Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__
         << ", file: <" << __FILE__ << ">" << endl;
    cout << "\tWant to view: <" << _varname << ">, ys = " << ys << endl;

#if 0
    int i, k, n;
    double f;
    double amp = 0.2;
    double offset = 0.5;
    double x1, y1, z1, z2;
    double value[nz][nx];
    
    for(k = 0; k < nz; ++k)
    {
        for(i = 0; i < nx; ++i)
        {
            n = i + (ys + k * ny) * nx;
            value[k][i] = _var[n];
        }   
    }       
        
    y1 = yStart + ys * xyDelt;

    glDisable(GL_TEXTURE_GEN_S);
    glEnable( GL_TEXTURE_1D );
    glBindTexture(GL_TEXTURE_1D, texture1d->get_textureID());
    
    for(k = 0; k < nz - 1; ++k)
    {
        z1 = k * zDelt; 
        z2 = z1 + zDelt;
        
        glBegin(GL_QUAD_STRIP);
            for(i = 0; i < nx; ++i)
            {
                x1 = xStart + i * xyDelt;

                f = scale * (value[k][i] - vMinimum);
                glTexCoord1d(f);
                glVertex3d(x1, y1 + amp * (f - offset), z1);
                
                f = scale * (value[k+1][i] - vMinimum);
                glTexCoord1d(f);
                glVertex3d(x1, y1 + amp * (f - offset), z2);
            }   
        glEnd();
    }       
        
    glDisable(GL_TEXTURE_1D);
#endif
}

void GL_Viewer::_display_Zplane_with_bump(int zs)
{
    int i, j, n;
    double *pltvar = NULL;
    double f;
    double amp = 0.2;
    double offset = 0.5;

    double x1, y1, y2, z1;

    n = zs * geometry->get_nx() * geometry->get_ny();

    pltvar = &_var[n];

    if(1 < nz)
    {
        z1 = zs * zDelt - 0.5;
    }
    else
    {
        z1 = 0.0;
    }

    _zlist[zs] = glGenLists(1);
  //glNewList(_zlist[zs], GL_COMPILE);
    glNewList(_zlist[zs], GL_COMPILE_AND_EXECUTE);

  //OpenGL should normalize normal vectors
    glEnable(GL_NORMALIZE);

    glEnable(GL_BLEND);
  //glBlendFunc(GL_SRC_ALPHA,GL_ONE);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

    glShadeModel(GL_SMOOTH);

    glEnable( GL_TEXTURE_1D );
    glBindTexture(GL_TEXTURE_1D, texture1d->get_textureID());

    for(j = 0; j < ny - 1; ++j)
    {
        y1 = yStart + j * xyDelt - 0.5;
        y2 = y1 + xyDelt;

        glBegin(GL_QUAD_STRIP);
        for(i = 0; i < nx; ++i)
        {
            x1 = xStart + i * xyDelt - 0.5;

            n = j * nx + i;
            f = scale * (pltvar[n] - vMinimum);
            glTexCoord1d(f);
            glVertex3d(x1, y1, z1 + amp * (f - offset));

            n += nx;
            f = scale * (pltvar[n] - vMinimum);
            glTexCoord1d(f);
            glVertex3d(x1, y2, z1 + amp * (f - offset));
        }
        glEnd();
    }

    glDisable(GL_TEXTURE_1D);

    glEndList();
}

void GL_Viewer::_display_on_height_surface()
{
    if(geometry->get_nz() > 1)
    {
        glTranslatef(-0.5, -0.5, -0.5);
    }
    else
    {
        glTranslatef(-0.5, -0.5, 0.0);
    }

    if(geometry->get_nz() > 1)
    {
        if(geometry->get_nx() > nvoptions->get_xsec())
        {
            _display_Xplane_on_height_surface(nvoptions->get_xsec());
        }

        if(geometry->get_nx() > nvoptions->get_xsec2())
        {
            _display_Xplane_on_height_surface(nvoptions->get_xsec2());
        }

        if(geometry->get_ny() > nvoptions->get_ysec())
        {
            _display_Yplane_on_height_surface(nvoptions->get_ysec());
        }

        if(geometry->get_ny() > nvoptions->get_ysec2())
        {
            _display_Yplane_on_height_surface(nvoptions->get_ysec2());
        }
    }

    if(geometry->get_nz() > nvoptions->get_zsec())
    {
        _display_Zplane_on_height_surface(nvoptions->get_zsec());
    }

    if(geometry->get_nz() > nvoptions->get_zsec2())
    {
        _display_Zplane_on_height_surface(nvoptions->get_zsec2());
    }
}

void GL_Viewer::_display_Xplane_on_height_surface(int xs)
{
    cout << "Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__
         << ", file: <" << __FILE__ << ">" << endl;
    cout << "\tWant to view: <" << _varname << ">, xs = " << xs << endl;

#if 0
    int j, k, n;
    double f;
    double x1, y1, z1, z2;
    double height[nz][ny];
    double values[nz][ny];
    double *terrain = geometry->get_lev();
    
    for(k = 0; k < nz; ++k)
    {
        for(j = 0; j < ny; ++j)
        {
            n = xs + (j + k * ny) * nx;
            values[k][j] = _var[n];

            n = xs + (j + k * ny) * nx;
            height[k][j] = terrain[n];
        }
    }

    x1 = xStart + xs * xyDelt;

    glDisable(GL_TEXTURE_GEN_S);
    glEnable( GL_TEXTURE_1D );
    glBindTexture(GL_TEXTURE_1D, texture1d->get_textureID());

    for(k = 0; k < nz - 1; ++k)
    {
        glBegin(GL_QUAD_STRIP);
            for(j = 0; j < ny; ++j)
            {
                y1 = yStart + j * xyDelt;
                z1 = zScale * height[k][j];
                f = scale * (values[k][j] - vMinimum);
                glTexCoord1d(f);
                glVertex3d(x1, y1, z1);

                z2 = zScale * height[k+1][j];
                f = scale * (values[k+1][j] - vMinimum);
                glTexCoord1d(f);
                glVertex3d(x1, y1, z2);
            }
        glEnd();
    }

    glDisable(GL_TEXTURE_1D);
#endif
}

void GL_Viewer::_display_Yplane_on_height_surface(int ys)
{
    cout << "Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__
         << ", file: <" << __FILE__ << ">" << endl;
    cout << "\tWant to view: <" << _varname << ">, ys = " << ys << endl;

#if 0
    int i, k, n;
    double f;
    double x1, y1, z1, z2;
    double height[nz][nx];
    double values[nz][nx];
    double *terrain = geometry->get_lev();
    
    for(k = 0; k < nz; ++k)
    {
        for(i = 0; i < nx; ++i)
        {
            n = i + (ys + k * ny) * nx;
            values[k][i] = _var[n];

            n = i + (ys + k * ny) * nx;
            height[k][i] = terrain[n];
        }
    }       
            
    y1 = yStart + ys * xyDelt;
    
    glDisable(GL_TEXTURE_GEN_S);
    glEnable( GL_TEXTURE_1D );
    glBindTexture(GL_TEXTURE_1D, texture1d->get_textureID());
    
    for(k = 0; k < nz - 1; ++k)
    {
        glBegin(GL_QUAD_STRIP);
            for(i = 0; i < nx; ++i)
            {
                x1 = xStart + i * xyDelt;
                z1 = zScale * height[k][i];
                f = scale * (values[k][i] - vMinimum);
                glTexCoord1d(f);
                glVertex3d(x1, y1, z1);
                
                z2 = zScale * height[k+1][i];
                f = scale * (values[k+1][i] - vMinimum);
                glTexCoord1d(f);
                glVertex3d(x1, y1, z2);
            }   
        glEnd();
    }       
        
    glDisable(GL_TEXTURE_1D);
#endif
}

void GL_Viewer::_display_Zplane_on_height_surface(int zs)
{
    cout << "Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__
         << ", file: <" << __FILE__ << ">" << endl;
    cout << "\tWant to view: <" << _varname << ">, zs = " << zs << endl;

#if 0
    int i, j, n;
    double *height = NULL;
    double f;
    double x1, y1, y2, z1;
    double *terrain = geometry->get_lev();
    
    n = zs * geometry->get_nx() * geometry->get_ny();
    pltvar = &_var[n];

    n = zs * nx * ny;
    height = &terrain[n];
            
  //The texture coordinate comes from the data, it it not
  //generated from the vertex position!!!
    glDisable(GL_TEXTURE_GEN_S); 
    glEnable( GL_TEXTURE_1D );
    glBindTexture(GL_TEXTURE_1D, texture1d->get_textureID());
        
  //glEnable(GL_CULL_FACE);
  //glEnable(GL_DEPTH_TEST);
  //glEnable(GL_NORMALIZE); 
  //glNormal3d(0.0, 0.0, 1.0);
                
    for(j = 0; j < ny - 1; ++j)
    {           
        y1 = yStart + xyDelt * j;
        y2 = yStart + xyDelt * (j + 1);
                
        glBegin(GL_QUAD_STRIP);
        for(i = 0; i < nx; ++i)
        {   
            x1 = xStart + xyDelt * i;
            n = j * nx + i;
            z1 = zScale * height[n];
    
            n = j * nx + i;
            f = scale * (pltvar[n] - vMinimum);
            glTexCoord1d(f);
            glVertex3d(x1, y1, z1); 

            n += nx;
            f = scale * (pltvar[n] - vMinimum);
            n = (j + 1) * nx + i;
            z1 = zScale * height[n];
            glTexCoord1d(f);
            glVertex3d(x1, y2, z1);
        }
        glEnd();
    }

    glDisable(GL_TEXTURE_1D);
#endif
}

void GL_Viewer::_display_on_sphere()
{
    if(geometry->get_nz() > 1)
    {
        if(geometry->get_nx() > nvoptions->get_xsec())
        {
            _display_Xplane_on_sphere(nvoptions->get_xsec());
        }

      //if(nvoptions->get_cb(NV_HASX2) && (geometry->get_nx() > nvoptions->get_xsec2()))
      //{
      //    _display_Xplane_on_sphere(nvoptions->get_xsec2());
      //}

        if((nvoptions->get_ysec() >= 10) && (nvoptions->get_ysec() <= geometry->get_ny() - 10))
        {
            _display_Yplane_on_sphere(nvoptions->get_ysec());
        }

      //if(nvoptions->get_cb(NV_HASY2) && (geometry->get_ny() > nvoptions->get_ysec2()))
      //{
      //    _display_Yplane_on_sphere(nvoptions->get_ysec2());
      //}
    }

    if(geometry->get_nz() > nvoptions->get_zsec())
    {
      //cout << "Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__
      //     << ", file: <" << __FILE__ << ">" << endl;
      //cout << "\tVar name: <" << _varname << ">, nvoptions->get_zsec() = " << nvoptions->get_zsec() << endl;

        _display_Zplane_on_sphere(nvoptions->get_zsec());
    }

  //if(nvoptions->get_cb(NV_HASZ2) && (geometry->get_nz() > nvoptions->get_zsec2()))
  //{
  //    _display_Zplane_on_sphere(nvoptions->get_zsec2());
  //}

  //earth->draw(0.5);
}

//Draw vertex in polar coordinates
void GL_Viewer::_sphere1dVertex(double lon, double lat, double radius, double f)
{
    double phi = lat * deg2rad;
    double dist = radius * cos(phi);
    double lamda = lon * deg2rad;

    double x = dist * sin(lamda);
    double z = dist * cos(lamda);
    double y = radius * sin(phi);

  //glNormal3d(x,y,z);
    glTexCoord1d(f);
    glVertex3d(x,y,z);

  //cout << "\tlon = " << lon << ", lat = " << lat << ", r = " << r << endl;
  //cout << "\tx = " << x << ", y = " << y << ", z = " << z << ", f = " << f << endl;
}

void GL_Viewer::_display_Xplane_on_sphere(int xs)
{
    int j, k, n;
    double f;
    double height[nz][ny];
    double values[nz][ny];

    if(xs >= nx)
        return;

    for(k = 0; k < nz; ++k)
    {
        for(j = 0; j < ny; ++j)
        {
            n = xs + (j + k * ny) * nx;
            values[k][j] = _var[n];

          //n = xs + (j + k * ny) * nx;
          //height[k][j] = terrain[n];
          //height[k][j] = 0.501 + (nz - 1 - k) * zDelt;
            height[k][j] = 0.501 + k * zDelt;
        }
    }

    _xlist[k] = glGenLists(1);
  //glNewList(_xlist[k], GL_COMPILE);
    glNewList(_xlist[k], GL_COMPILE_AND_EXECUTE);

  //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\t_xlist[k] = " << _xlist[k] << endl;

    glColor4d(1.0, 1.0, 1.0, 1.0);
  //glColor4d(0.0, 0.0, 0.0, 0.0);
  //glNormal3d(0.0, 0.0, -1.0);

    glEnable( GL_TEXTURE_1D );
    glBindTexture(GL_TEXTURE_1D, texture1d->get_textureID());

    for(k = 0; k < nz - 1; ++k)
    {
        glBegin(GL_QUAD_STRIP);
            for(j = 0; j < ny; ++j)
            {
                f = scale * (values[k][j] - vMinimum);
                _sphere1dVertex(lon[xs], lat[j], height[k][j], f);

                f = scale * (values[k+1][j] - vMinimum);
                _sphere1dVertex(lon[xs], lat[j], height[k+1][j], f);
            }
        glEnd();
    }

    glDisable(GL_TEXTURE_1D);

    glEndList();
}

void GL_Viewer::_display_Yplane_on_sphere(int ys)
{
    int i, k, n;
    double f;
    double height[nz][nx];
    double values[nz][nx];
  //double *terrain = geometry->get_lev();

    if((ys < 10) || (ys > ny - 10))
        return;

    for(k = 0; k < nz; ++k)
    {
        for(i = 0; i < nx; ++i)
        {
            n = i + (ys + k * ny) * nx;
            values[k][i] = _var[n];

          //n = i + (ys + k * ny) * nx;
          //height[k][i] = terrain[n];
          //height[k][i] = 0.501 + (nz - 1 - k) * zDelt;
            height[k][i] = 0.501 + k * zDelt;
        }
    }

    lon = geometry->get_lon();
    lat = geometry->get_lat();

    _ylist[k] = glGenLists(1);
  //glNewList(_ylist[k], GL_COMPILE);
    glNewList(_ylist[k], GL_COMPILE_AND_EXECUTE);

  //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\t_ylist[k] = " << _ylist[k] << endl;

    glColor4d(1.0, 1.0, 1.0, 1.0);
  //glColor4d(0.0, 0.0, 0.0, 0.0);
  //glNormal3d(0.0, 0.0, -1.0);

    glEnable( GL_TEXTURE_1D );
    glBindTexture(GL_TEXTURE_1D, texture1d->get_textureID());

    for(k = 0; k < nz - 1; ++k)
    {
        glBegin(GL_QUAD_STRIP);
            for(i = 0; i < nx; ++i)
            {
                f = scale * (values[k][i] - vMinimum);
                _sphere1dVertex(lon[i], lat[ys], height[k][i], f);

                f = scale * (values[k+1][i] - vMinimum);
                _sphere1dVertex(lon[i], lat[ys], height[k+1][i], f);
            }

            f = scale * (values[k][0] - vMinimum);
            _sphere1dVertex(lon[0], lat[ys], height[k][0], f);

            f = scale * (values[k+1][0] - vMinimum);
            _sphere1dVertex(lon[0], lat[ys], height[k+1][0], f);
        glEnd();
    }

    glDisable(GL_TEXTURE_1D);

    glEndList();
}

void GL_Viewer::_display_Zplane_on_sphere(int zs)
{
    int i, j, n;
    double f;
    double height;
    double* pltvar = NULL;
  //double* height = NULL;
  //double* terrain = geometry->get_lev();

    if(zs >= nz)
        return;

    n = zs * nx * ny;
    pltvar = &_var[n];

  //cout << "Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
  //cout << "\tnx = " << nx << ", ny = " << ny << ", nz = " << nz << ", n = " << n << endl;

  //n = zs * nx * ny;
  //height = &terrain[n];

  //height = 0.501 + (nz - 1 - zs) * zDelt;
    height = 0.501 + zs * zDelt;

    _zlist[zs] = glGenLists(1);
  //glNewList(_zlist[zs], GL_COMPILE);
    glNewList(_zlist[zs], GL_COMPILE_AND_EXECUTE);

  //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\t_zlist[zs] = " << _zlist[zs] << endl;

    glColor4d(1.0, 1.0, 1.0, 1.0);
  //glColor4d(0.0, 0.0, 0.0, 0.0);
  //glNormal3d(0.0, 0.0, -1.0);

    glEnable( GL_TEXTURE_1D );
    glBindTexture(GL_TEXTURE_1D, texture1d->get_textureID());

    if(geometry->get_has1dLon() && geometry->get_has1dLat())
    {
        for(j = 0; j < ny - 1; ++j)
        {
            glBegin(GL_QUAD_STRIP);
                for(i = 0; i < nx; ++i)
                {
                    f = scale * (pltvar[i + j * nx] - vMinimum);
                    _sphere1dVertex(lon[i], lat[j], height, f);

                    f = scale * (pltvar[i + (j + 1)* nx] - vMinimum);
                    _sphere1dVertex(lon[i], lat[j+1], height, f);
                }

                f = scale * (pltvar[j * nx] - vMinimum);
                _sphere1dVertex(lon[0], lat[j], height, f);

                f = scale * (pltvar[(j + 1)* nx] - vMinimum);
                _sphere1dVertex(lon[0], lat[j+1], height, f);
            glEnd();
        }
    }

    glDisable(GL_TEXTURE_1D);

    coastline->draw(height+0.01, 2);

    glEndList();
}

void GL_Viewer::set_geometry(Geometry *gm)
{
    geometry = gm;
}

void GL_Viewer::setup(string vn, double *var)
{
    size_t gridsize = 0;

    _varname  = vn;
    _var = var;

    nx = geometry->get_nx();
    ny = geometry->get_ny();
    nz = geometry->get_nz();

    gridsize = nx * ny * nz;

    if(_first_time)
    {
        nvoptions->set_xsec(geometry->get_nx() + 1);
        nvoptions->set_xsec2(geometry->get_nx() + 1);
        nvoptions->set_ysec(geometry->get_ny() + 1);
        nvoptions->set_ysec2(geometry->get_ny() + 1);
        nvoptions->set_zsec(0);
        nvoptions->set_zsec2(geometry->get_nz() + 1);

        _first_time = false;
    }

  //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\tnx = " << nx << ", ny = " << ny << ", nz = " << nz << endl;
  //cout << "\thasFillValue = " << hasFillValue << ", fillValue = " << fillValue << endl;

    evaluator->set_FillValue(fillValue, hasFillValue);
    evaluator->set_value(gridsize, _var);

    _parameter_setup();

    draw();
}

void GL_Viewer::set_OP(bool s)
{
    _hasOP = s;
}

void GL_Viewer::set_opacity(int o)
{
  //cout << "Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__
  //     << ", file: <" << __FILE__ << ">" << endl;
  //cout << "\tOpacity = " << o << endl;

    _opacity = o;
    draw();
}

void GL_Viewer::set_selecting(bool s)
{
    _selectingOn = s;
}

void GL_Viewer::_display_on_map()
{
    double height;

    if(geometry->get_has1dLon() && geometry->get_has1dLat())
    {
        lon = geometry->get_lon();
        lat = geometry->get_lat();
        _get_factor();
        _display_Zplane_on_map_1d(nvoptions->get_zsec());
    }
    else if(geometry->get_has2dLon() && geometry->get_has2dLat())
    {
        lon = geometry->get_lon();
        lat = geometry->get_lat();
        _get_factor();
        _display_Zplane_on_map_2d(nvoptions->get_zsec());
    }

    height = nvoptions->get_zsec() * zDelt;
    coastline->drawONplane2(height+0.01, 2);
}

//Draw vertex in xyz coordinates
void GL_Viewer::_planeVertex(double lon, double lat, double height, double f)
{
    double x = lon * oneover - _xfactor;
    double y = lat * oneover;

    glNormal3d(0.0, 0.0, 1.0);
    glTexCoord1d(f);
    glVertex3d(x,y,height);
}

void GL_Viewer::_display_Zplane_on_map_1d(int zs)
{
    int i, j, n;
    double f;
    double height;
    double* pltvar = NULL;

    if(zs >= nz)
        return;

    n = zs * nx * ny;
    pltvar = &_var[n];

  //cout << "Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
  //cout << "\tnx = " << nx << ", ny = " << ny << ", nz = " << nz << ", n = " << n << endl;

    if(1 < geometry->get_nz())
        height = zs * zDelt - 0.5;
    else
        height = 0.0;

    _zlist[zs] = glGenLists(1);
  //glNewList(_zlist[zs], GL_COMPILE);
    glNewList(_zlist[zs], GL_COMPILE_AND_EXECUTE);

  //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\t_zlist[zs] = " << _zlist[zs] << endl;

    glColor4d(1.0, 1.0, 1.0, 1.0);
  //glColor4d(0.0, 0.0, 0.0, 0.0);
  //glNormal3d(0.0, 0.0, -1.0);

    glEnable( GL_TEXTURE_1D );
    glBindTexture(GL_TEXTURE_1D, texture1d->get_textureID());

    for(j = 0; j < ny - 1; ++j)
    {
        glBegin(GL_QUAD_STRIP);
            for(i = 0; i < nx; ++i)
            {
                f = scale * (pltvar[i + j * nx] - vMinimum);
                _planeVertex(lon[i], lat[j], height, f);

                f = scale * (pltvar[i + (j + 1)* nx] - vMinimum);
                _planeVertex(lon[i], lat[j+1], height, f);
            }
        glEnd();
    }

    glDisable(GL_TEXTURE_1D);

    coastline->drawONplane2(height+0.01, 2);

    glEndList();
}

void GL_Viewer::_display_Zplane_on_map_2d(int zs)
{
    int i, j, n;
    double f;
    double height;
    double* pltvar = NULL;

    if(zs >= nz)
        return;

    n = zs * nx * ny;
    pltvar = &_var[n];

    if(1 < geometry->get_nz())
        height = zs * zDelt - 0.5;
    else
        height = 0.0;

    _zlist[zs] = glGenLists(1);
  //glNewList(_zlist[zs], GL_COMPILE);
    glNewList(_zlist[zs], GL_COMPILE_AND_EXECUTE);

    glColor4d(1.0, 1.0, 1.0, 1.0);
  //glColor4d(0.0, 0.0, 0.0, 0.0);
  //glNormal3d(0.0, 0.0, -1.0);

    glEnable( GL_TEXTURE_1D );
    glBindTexture(GL_TEXTURE_1D, texture1d->get_textureID());

    for(j = 0; j < ny - 1; ++j)
    {
        glBegin(GL_QUAD_STRIP);
            for(i = 0; i < nx; ++i)
            {
                f = scale * (pltvar[i + j * nx] - vMinimum);
                _planeVertex(lon[i + j * nx], lat[i + j * nx], height, f);

                f = scale * (pltvar[i + (j + 1)* nx] - vMinimum);
                _planeVertex(lon[i + (j + 1)* nx], lat[i + (j + 1)* nx], height, f);
            }
        glEnd();
    }

    glDisable(GL_TEXTURE_1D);

    coastline->drawONplane2(height+0.01, 2);

    glEndList();
}

void GL_Viewer::_get_factor()
{
    int i, j, n;
    bool need_adjust = false;

    if(9.0 > _xfactor)
        return;

    if(geometry->get_has1dLon() && geometry->get_has1dLat())
    {
        _xfactor = 1.0;
        for(i = 0; i < nx; ++i)
        {
            if(0.0 > lon[i])
                lon[i] += 360.0;
        }

        need_adjust = false;
        for(j = 0; j < ny; ++j)
        {
            if(90.0 < lat[j])
            {
                need_adjust = true;
                break;
            }
        }

        if(need_adjust)
        {
            for(j = 0; j < ny; ++j)
            {
                lat[j] -= 90.0;
            }
        }
    }
    else if(geometry->get_has2dLon() && geometry->get_has2dLat())
    {
        _xfactor = 1.0;
        for(n = 0; n < nx*ny; ++n)
        {
            if(0.0 > lon[n])
                lon[n] += 360.0;
        }

        need_adjust = false;
        for(j = 0; j < ny; ++j)
        {
            n = j * nx;
            for(i = 0; i < nx; ++i)
            {
                if(90.0 < lat[n+i])
                {
                    need_adjust = true;
                    break;
                }
            }
        }

        if(need_adjust)
        {
            for(n = 0; n < nx*ny; ++n)
            {
                lat[n] -= 90.0;
            }
        }
    }
}


void GL_Viewer::_display_on_sphere_map()
{
    double height;

    if(geometry->get_has1dLon() && geometry->get_has1dLat())
    {
        lon = geometry->get_lon();
        lat = geometry->get_lat();
        _get_factor();
        _display_Zplane_on_sphere_map_1d(nvoptions->get_zsec());
    }
    else if(geometry->get_has2dLon() && geometry->get_has2dLat())
    {
        lon = geometry->get_lon();
        lat = geometry->get_lat();
        _get_factor();
        _display_Zplane_on_sphere_map_2d(nvoptions->get_zsec());
    }

  //height = 0.501 + (nz - 1 - nvoptions->get_zsec()) * zDelt;
    height = 0.501 + nvoptions->get_zsec() * zDelt;

    coastline->draw(height+0.01, 2);
}

void GL_Viewer::_display_Zplane_on_sphere_map_1d(int zs)
{
    int i, j, n;
    double f;
    double height;
    double* pltvar = NULL;

    if(zs >= nz)
        return;

    n = zs * nx * ny;
    pltvar = &_var[n];

    cout << "Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    cout << "\tnx = " << nx << ", ny = " << ny << ", nz = " << nz << ", n = " << n << endl;

  //height = 0.501 + (nz - 1 - zs) * zDelt;
    height = 0.501 + zs * zDelt;

  //_zlist[zs] = glGenLists(1);
  //glNewList(_zlist[zs], GL_COMPILE);
  //glNewList(_zlist[zs], GL_COMPILE_AND_EXECUTE);

  //glColor4d(1.0, 1.0, 1.0, 1.0);
  //glColor4d(0.0, 0.0, 0.0, 0.0);
  //glNormal3d(0.0, 0.0, -1.0);

    glEnable( GL_TEXTURE_1D );
    glBindTexture(GL_TEXTURE_1D, texture1d->get_textureID());

    for(j = 0; j < ny - 1; ++j)
    {
        glBegin(GL_QUAD_STRIP);
            for(i = 0; i < nx; ++i)
            {
                f = scale * (pltvar[i + j * nx] - vMinimum);
                _sphere1dVertex(lon[i], lat[j], height, f);

                f = scale * (pltvar[i + (j + 1)* nx] - vMinimum);
                _sphere1dVertex(lon[i], lat[j+1], height, f);
            }

            f = scale * (pltvar[j * nx] - vMinimum);
            _sphere1dVertex(lon[0], lat[j], height, f);

            f = scale * (pltvar[(j + 1)* nx] - vMinimum);
            _sphere1dVertex(lon[0], lat[j+1], height, f);
        glEnd();
    }

    glDisable(GL_TEXTURE_1D);

  //glEndList();
}

void GL_Viewer::_display_Zplane_on_sphere_map_2d(int zs)
{
    int i, j, n, n1, n2;
    double f;
    double height;
    double* pltvar = NULL;

    if(zs >= nz)
        return;

    n = zs * nx * ny;
    pltvar = &_var[n];

  //cout << "Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
  //cout << "\tnx = " << nx << ", ny = " << ny << ", nz = " << nz << ", n = " << n << endl;

  //height = 0.501 + (nz - 1 - zs) * zDelt;
    height = 0.501 + zs * zDelt;

  //_zlist[zs] = glGenLists(1);
  //glNewList(_zlist[zs], GL_COMPILE);
  //glNewList(_zlist[zs], GL_COMPILE_AND_EXECUTE);

  //glColor4d(1.0, 1.0, 1.0, 1.0);
  //glColor4d(0.0, 0.0, 0.0, 0.0);
  //glNormal3d(0.0, 0.0, -1.0);

    glEnable( GL_TEXTURE_1D );
    glBindTexture(GL_TEXTURE_1D, texture1d->get_textureID());

    for(j = 0; j < ny - 1; ++j)
    {
        glBegin(GL_QUAD_STRIP);
            for(i = 0; i < nx; ++i)
            {
                n1 = i + j * nx;
                n2 = n1 + nx;

                f = scale * (pltvar[n1] - vMinimum);
                _sphere1dVertex(lon[n1], lat[n1], height, f);

                f = scale * (pltvar[n2] - vMinimum);
                _sphere1dVertex(lon[n2], lat[n2], height, f);
            }
        glEnd();
    }

    glDisable(GL_TEXTURE_1D);

  //glEndList();
}

