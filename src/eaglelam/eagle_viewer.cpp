#include <QtOpenGL>

#include "eagle_viewer.h"

#define MINVAL	1.0e-20

EAGLE_Viewer::EAGLE_Viewer(ColorTable *ct, NVOptions* opt)
{
    colorTable = ct;
    nvoptions = opt;
    string name = colorTable->get_name();

    texture1d = new Texture1d();
    texture1d->set_colors(ct->get_clen(), ct->get_cmap());

    texture1d->set_name(name);

    colorTable->set_opacity(0);

    lon = NULL;
    lat = NULL;
    pltvar = NULL;

    deg2rad = 3.1416/180.0;

    _var = NULL;

    coastline = new CoastLine();
    mapprojection = new MapProjection();
    stateboundary = new StateBoundary();
    stateboundary->set_mapprojection(mapprojection);
    locator = NULL;

    _hasOP = false;

    _xfactor = 999.0;
    oneover  = 1.0 / 180.0;

    first_time = true;
}

EAGLE_Viewer::~EAGLE_Viewer()
{
  //cout << "\tEnter function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;

    if(NULL != locator)
        locator->turnOff();

    delete texture1d;
    delete coastline;
  //delete mapprojection;
    delete stateboundary;

  //cout << "\tLeave function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
}

void EAGLE_Viewer::scaleNtranslate()
{
    glScalef(1.0, 1.0, 0.4);
    if(geometry->get_nz() > 1)
    {
        glTranslatef(-0.5, -0.5, -0.5);
    }
    else
    {
        glTranslatef(-0.5, -0.5, 0.0);
    }
}

void EAGLE_Viewer::draw()
{
    glPushMatrix();

  //cout << "\tFunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
  //cout << "\tnvoptions->get_xsec() = " << nvoptions->get_xsec() << endl;
  //cout << "\tnvoptions->get_ysec() = " << nvoptions->get_ysec() << endl;
  //cout << "\tnvoptions->get_zsec() = " << nvoptions->get_zsec() << endl;

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
	scaleNtranslate();

        _display_with_bump();
    }
    else if(nvoptions->get_cb(NV_ONHEIGHTSURFACE))
    {
	scaleNtranslate();

        _display_on_height_surface();
    }
    else if(nvoptions->get_cb(NV_ONMAPON))
    {
        _display_on_map();
    }
    else
    {
	scaleNtranslate();

        _display_all();
    }

    glPopMatrix();
}

void EAGLE_Viewer::_parameter_setup()
{
    size_t sz;
    Evaluator evaluator;
    string tex1dName = texture1d->get_name();
    double* hgt = geometry->get_hgt();

    if(0 != tex1dName.compare(colorTable->get_name()))
    {
        texture1d->set_colors(colorTable->get_clen(), colorTable->get_cmap());
        texture1d->set_name(colorTable->get_name());
    }

    nxp = geometry->get_nx();
    nyp = geometry->get_ny();
    nzp = geometry->get_nz();

    nx = nxp;
    if(geometry->isXstaggered())
        --nx;

    ny = nyp;
    if(geometry->isYstaggered())
        --ny;

    nz = nzp;
    if(geometry->isZstaggered())
        --nz;

    sz = nxp * nyp * nzp;
    evaluator.set_value(sz, _var);

    vMinimum = evaluator.get_min();
    vMaximum = evaluator.get_max();
    scale  = 1.0/(vMaximum - vMinimum + MINVAL);
    if(MINVAL > (vMaximum - vMinimum))
	vMaximum += MINVAL;

    nicemms.process(vMinimum, vMaximum, 10, true);
    niceMin = nicemms.get_min_flt();
    niceMax = nicemms.get_max_flt();
    scale  = 1.0/(niceMax - niceMin + MINVAL);

    sz = nx * ny * nz;
    evaluator.set_value(sz, hgt);

    zScale = 1.0/(evaluator.get_max() + 0.01);

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

    if(1 < nz)
        zDelt = 1.25 / (nz - 1.0);
    else
        zDelt = 1.0;

    if(geometry->isXstaggered())
    {
        lon = geometry->get_ulon();
        lat = geometry->get_ulat();
    }
    else if(geometry->isYstaggered())
    {
        lon = geometry->get_vlon();
        lat = geometry->get_vlat();
    }
    else
    {
        lon = geometry->get_lon();
        lat = geometry->get_lat();
    }

  //cout << "Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
  //cout << "\tnx  = " << nx  << ", ny  = " << ny  << ", nz  = " << nz  << endl;
  //cout << "\tnxp = " << nxp << ", nyp = " << nyp << ", nzp = " << nzp << endl;
  //cout << "\tsetup map projection: " << endl;
  //cout << "\t\tcode = " << geometry->get_map_proj() << endl;
  //cout << "\t\tlon1 = " << lon[0] << endl;
  //cout << "\t\tlat1 = " << lat[0] << endl;
  //cout << "\t\tdx = " << geometry->get_dx() << endl;
  //cout << "\t\tstdlon = " << geometry->get_stand_lon() << endl;
  //cout << "\t\tcenlon = " << geometry->get_cen_lon() << endl;
  //cout << "\t\tcenlat = " << geometry->get_cen_lat() << endl;
  //cout << "\t\ttruelat1 = " << geometry->get_truelat1() << endl;
  //cout << "\t\ttruelat2 = " << geometry->get_truelat2() << endl;

    mapprojection->setup(geometry->get_map_proj(),
                         lon[0], lat[0],
                         geometry->get_dx(),
                         geometry->get_stand_lon(),
                         geometry->get_truelat1(),
                         geometry->get_truelat2());
    mapprojection->set_dimension(nx, ny);

  //int i;
  //double x, y;

  //mapprojection->lc_llxy(lon[0], lat[0], x, y);
  //cout << "\tlon = " << lon[0] << ", lat = " << lat[0] << ", x = " << x << ", y = " << y << endl;
  //mapprojection->lc_llxy(lon[1], lat[1], x, y);
  //cout << "\tlon = " << lon[1] << ", lat = " << lat[1] << ", x = " << x << ", y = " << y << endl;
  //mapprojection->lc_llxy(lon[nx], lat[nx], x, y);
  //cout << "\tlon = " << lon[nx] << ", lat = " << lat[nx] << ", x = " << x << ", y = " << y << endl;

  //for(i = 0; i < nx; ++i)
  //{
  //    mapprojection->lc_llxy(lon[i], lat[i], x, y);
  //    cout << "\ti = " << i << ", lon = " << lon[i] << ", lat = " << lat[i] << ", x = " << x << ", y = " << y << endl;
  //}
}

void EAGLE_Viewer::_display_all()
{
  //cout << "\tFunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
  //cout << "\tnvoptions->get_xsec() = " << nvoptions->get_xsec() << endl;
  //cout << "\tnvoptions->get_ysec() = " << nvoptions->get_ysec() << endl;
  //cout << "\tnvoptions->get_zsec() = " << nvoptions->get_zsec() << endl;

    if((geometry->get_nx() > nvoptions->get_xsec()) && (geometry->get_nz() > 1))
    {
        _display_Xplane(nvoptions->get_xsec());
    }

    if(nvoptions->get_cb(NV_HASX2) && (geometry->get_nx() > nvoptions->get_xsec2()) && (geometry->get_nz() > 1))
    {
        _display_Xplane(nvoptions->get_xsec2());
    }

    if((geometry->get_ny() > nvoptions->get_ysec()) && (geometry->get_nz() > 1))
    {
        _display_Yplane(nvoptions->get_ysec());
    }

    if(nvoptions->get_cb(NV_HASY2) && (geometry->get_ny() > nvoptions->get_ysec2()) && (geometry->get_nz() > 1))
    {
        _display_Yplane(nvoptions->get_ysec2());
    }

    if(geometry->get_nz() > nvoptions->get_zsec())
    {
        _display_Zplane(nvoptions->get_zsec());
    }

    if(nvoptions->get_cb(NV_HASZ2) && (geometry->get_nz() > nvoptions->get_zsec2()) && (geometry->get_nz() > 1))
    {
        _display_Zplane(nvoptions->get_zsec2());
    }
}

void EAGLE_Viewer::_display_Xplane(int xs)
{
    int j, k, n;
    float f;
    float x1, y1, z1, z2;
    float value[nz][ny];

    for(k = 0; k < nz; ++k)
    {
        for(j = 0; j < ny; ++j)
        {
            n = xs + (j + k * nyp) * nxp;
            value[k][j] = _var[n];
        }
    }

    x1 = xStart + xs * xyDelt;

    glDisable(GL_TEXTURE_GEN_S); 
    glDisable(GL_TEXTURE_2D);
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
                glTexCoord1f(f);
                glVertex3d(x1, y1, z1);

                f = scale * (value[k+1][j] - vMinimum);
                glTexCoord1f(f);
                glVertex3d(x1, y1, z2);
            }
        glEnd();
    }

    glDisable(GL_TEXTURE_1D);
}

void EAGLE_Viewer::_display_Yplane(int ys)
{
    int i, k, n;
    float f;
    float x1, y1, z1, z2;
    float value[nz][nx];

    for(k = 0; k < nz; ++k)
    {
        for(i = 0; i < nx; ++i)
        {
            n = i + (ys + k * nyp) * nxp;
            value[k][i] = _var[n];
        }
    }

    y1 = yStart + ys * xyDelt;

    glDisable(GL_TEXTURE_GEN_S); 
    glDisable(GL_TEXTURE_2D);
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
                glTexCoord1f(f);
                glVertex3d(x1, y1, z1);

                f = scale * (value[k+1][i] - vMinimum);
                glTexCoord1f(f);
                glVertex3d(x1, y1, z2);
            }
        glEnd();
    }

    glDisable(GL_TEXTURE_1D);
}

void EAGLE_Viewer::_display_Zplane(int zs)
{
    int i, j, n;
    double *pltvar = NULL;
    float f;
    float x1, y1, y2, z1;

#if 0
    double color4v[4];
    double factor;

    glColor3f(1,1,1);

  //OpenGL should normalize normal vectors
  //glEnable(GL_NORMALIZE);

    glEnable(GL_BLEND);
  //glBlendFunc(GL_SRC_ALPHA,GL_ONE);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

  //glShadeModel(GL_SMOOTH);

  //glDisable(GL_CULL_FACE);
  //glEnable(GL_CULL_FACE);
#endif

    n = zs * geometry->get_nx() * geometry->get_ny();

    pltvar = &_var[n];

    if(1 < nz)
    {
       z1 = (float) zs / (nz - 1.0);
    }
    else
    {
       z1 = 0.0;
    }

    if(NULL != locator)
        locator->set_height(z1);

  //The texture coordinate comes from the data, it it not
  //generated from the vertex position!!!
    glDisable(GL_TEXTURE_GEN_S); 
    glDisable(GL_TEXTURE_2D);
    glEnable( GL_TEXTURE_1D );
    glBindTexture(GL_TEXTURE_1D, texture1d->get_textureID());

  //glEnable(GL_CULL_FACE);
  //glEnable(GL_NORMALIZE);
  //glNormal3f(0.0, 0.0, 1.0);

    for(j = 0; j < ny - 1; ++j)
    {
        y1 = yStart + xyDelt * j;
        y2 = yStart + xyDelt * (j + 1);

        glBegin(GL_QUAD_STRIP);
#if 1
        for(i = 0; i < nx; ++i)
        {
            x1 = xStart + xyDelt * i;

            n = j * nxp + i;
            f = scale * (pltvar[n] - vMinimum);
            glTexCoord1f(f);
            glVertex3d(x1, y1, z1);

            n += nxp;
            f = scale * (pltvar[n] - vMinimum);
            glTexCoord1f(f);
            glVertex3d(x1, y2, z1);
        }
#else
      //cout << "j = " << j << endl;
        scale = 1.0 / (exp(1.0) - 1.0);
        for(i = 0; i < nx; ++i)
        {
            x1 = xStart + xyDelt * i;

            n = j * nxp + i;
            if(pltvar[n] > vMinimum)
                factor = (exp(pltvar[n]/vMaximum) - 1.0) * scale;
            else
                factor = 0.0;
            colorTable->set_color4v(factor, color4v);
            glColor4dv(color4v);
            glVertex3d(x1, y1, z1);

          //cout << "\ti = " << i << ", factor = " << factor << endl;
          //cout << "\t\tcolor: (" << color4v[0];
          //cout << ", " << color4v[1];
          //cout << ", " << color4v[2];
          //cout << ", " << color4v[3] << ")" << endl;

            n += nxp;
            factor = (exp(pltvar[n]/vMaximum) - 1.0) * scale;
            colorTable->set_color4v(factor, color4v);
            glColor4dv(color4v);
            glVertex3d(x1, y2, z1);
        }
#endif
        glEnd();
    }

    stateboundary->drawONplane2(z1, 1);

    glDisable(GL_TEXTURE_1D);

  //if(locator->on())
  //{
  //  //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //  //cout << "\tlocation = (" << locator->x() << ", " << locator->y() << ")" << endl;

  //    _draw_cross(height+ 0.5);
  //}
}

void EAGLE_Viewer::_display_with_bump()
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
        _display_Zplane_with_bump(nvoptions->get_zsec());
    }
}

void EAGLE_Viewer::_display_Xplane_with_bump(int xs)
{
    int j, k, n;
    float f;
    float amp = 0.2;
    float offset = 0.5;
    float x1, y1, z1, z2;
    float value[nz][ny];

    for(k = 0; k < nz; ++k)
    {
        for(j = 0; j < ny; ++j)
        {
            n = xs + (j + k * nyp) * nxp;
            value[k][j] = _var[n];
        }
    }

    x1 = xStart + xs * xyDelt;

    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_2D);
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
                glTexCoord1f(f);
                glVertex3d(x1 + amp * (f - offset), y1, z1);

                f = scale * (value[k+1][j] - vMinimum);
                glTexCoord1f(f);
                glVertex3d(x1 + amp * (f - offset), y1, z2);
            }
        glEnd();
    }

    glDisable(GL_TEXTURE_1D);
}

void EAGLE_Viewer::_display_Yplane_with_bump(int ys)
{
    int i, k, n;
    float f;
    float amp = 0.2;
    float offset = 0.5;
    float x1, y1, z1, z2;
    float value[nz][nx];
    
    for(k = 0; k < nz; ++k)
    {
        for(i = 0; i < nx; ++i)
        {
            n = i + (ys + k * nyp) * nxp;
            value[k][i] = _var[n];
        }   
    }       
        
    y1 = yStart + ys * xyDelt;

    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_2D);
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
                glTexCoord1f(f);
                glVertex3d(x1, y1 + amp * (f - offset), z1);
                
                f = scale * (value[k+1][i] - vMinimum);
                glTexCoord1f(f);
                glVertex3d(x1, y1 + amp * (f - offset), z2);
            }   
        glEnd();
    }       
        
    glDisable(GL_TEXTURE_1D);
}

void EAGLE_Viewer::_display_Zplane_with_bump(int zs)
{
    int i, j, n;
    double *pltvar = NULL;
    float f;
    float amp = 0.2;
    float offset = 0.5;

    float x1, y1, y2, z1;

    n = zs * geometry->get_nx() * geometry->get_ny();

    pltvar = &_var[n];

    if(1 < nz)
    {
       z1 = (float) zs / (nz - 1.0);
    }
    else
    {
       z1 = 0.0;
    }

  //The texture coordinate comes from the data, it it not
  //generated from the vertex position!!!
    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_2D);
    glEnable( GL_TEXTURE_1D );
    glBindTexture(GL_TEXTURE_1D, texture1d->get_textureID());

  //glEnable(GL_CULL_FACE);
  //glEnable(GL_NORMALIZE);
  //glNormal3f(0.0, 0.0, 1.0);

    for(j = 0; j < ny - 1; ++j)
    {
        y1 = yStart + xyDelt * j;
        y2 = yStart + xyDelt * (j + 1);

        glBegin(GL_QUAD_STRIP);
        for(i = 0; i < nx; ++i)
        {
            x1 = xStart + xyDelt * i;

            n = j * nxp + i;
            f = scale * (pltvar[n] - vMinimum);
            glTexCoord1f(f);
            glVertex3d(x1, y1, z1 + amp * (f - offset));

            n += nxp;
            f = scale * (pltvar[n] - vMinimum);
            glTexCoord1f(f);
            glVertex3d(x1, y2, z1 + amp * (f - offset));
        }
        glEnd();
    }

    glDisable(GL_TEXTURE_1D);
}

void EAGLE_Viewer::_display_on_height_surface()
{
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

void EAGLE_Viewer::_display_Xplane_on_height_surface(int xs)
{
    int j, k, n;
    float f;
    float x1, y1, z1, z2;
    float height[nz][ny];
    float values[nz][ny];
    double* terrain = geometry->get_hgt();
    
    for(k = 0; k < nz; ++k)
    {
        for(j = 0; j < ny; ++j)
        {
            n = xs + (j + k * nyp) * nxp;
            values[k][j] = _var[n];

            n = xs + (j + k * ny) * nx;
            height[k][j] = terrain[n];
        }
    }

    x1 = xStart + xs * xyDelt;

    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_2D);
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
                glTexCoord1f(f);
                glVertex3d(x1, y1, z1);

                z2 = zScale * height[k+1][j];
                f = scale * (values[k+1][j] - vMinimum);
                glTexCoord1f(f);
                glVertex3d(x1, y1, z2);
            }
        glEnd();
    }

    glDisable(GL_TEXTURE_1D);
}

void EAGLE_Viewer::_display_Yplane_on_height_surface(int ys)
{
    int i, k, n;
    float f;
    float x1, y1, z1, z2;
    float height[nz][nx];
    float values[nz][nx];
    double* terrain = geometry->get_hgt();
    
    for(k = 0; k < nz; ++k)
    {
        for(i = 0; i < nx; ++i)
        {
            n = i + (ys + k * nyp) * nxp;
            values[k][i] = _var[n];

            n = i + (ys + k * ny) * nx;
            height[k][i] = terrain[n];
        }
    }       
            
    y1 = yStart + ys * xyDelt;
    
    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_2D);
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
                glTexCoord1f(f);
                glVertex3d(x1, y1, z1);
                
                z2 = zScale * height[k+1][i];
                f = scale * (values[k+1][i] - vMinimum);
                glTexCoord1f(f);
                glVertex3d(x1, y1, z2);
            }   
        glEnd();
    }       
        
    glDisable(GL_TEXTURE_1D);
}

void EAGLE_Viewer::_display_Zplane_on_height_surface(int zs)
{
    int i, j, n;
    double* height = NULL;
    float f;
    float x1, y1, y2, z1;
    double* terrain = geometry->get_hgt();
    
    n = zs * geometry->get_nx() * geometry->get_ny();
    pltvar = &_var[n];

    n = zs * nx * ny;
    height = &terrain[n];

  //cout << "Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
  //cout << "\t_varname = " << _varname << endl;
  //cout << "\tzs = " << zs << endl;
  //cout << "\tnx = " << nx << endl;
  //cout << "\tny = " << ny << endl;
  //cout << "\tgeometry->get_nx() = " << geometry->get_nx() << endl;
  //cout << "\tgeometry->get_ny() = " << geometry->get_ny() << endl;
  //cout << "\tgeometry->get_nz() = " << geometry->get_nz() << endl;
            
  //The texture coordinate comes from the data, it it not
  //generated from the vertex position!!!
    glDisable(GL_TEXTURE_GEN_S); 
    glDisable(GL_TEXTURE_2D);
    glEnable( GL_TEXTURE_1D );
    glBindTexture(GL_TEXTURE_1D, texture1d->get_textureID());
        
  //glEnable(GL_CULL_FACE);
  //glEnable(GL_NORMALIZE); 
  //glNormal3f(0.0, 0.0, 1.0);
                
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
    
            n = j * nxp + i;
            f = scale * (pltvar[n] - vMinimum);
            glTexCoord1f(f);
            glVertex3d(x1, y1, z1); 

            n += nxp;
            f = scale * (pltvar[n] - vMinimum);
            n = (j + 1) * nx + i;
            z1 = zScale * height[n];
            glTexCoord1f(f);
            glVertex3d(x1, y2, z1);
        }
        glEnd();
    }

    glDisable(GL_TEXTURE_1D);
}

void EAGLE_Viewer::_display_on_sphere()
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

        if(geometry->get_ny() > nvoptions->get_ysec())
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

  //if(locator->on())
  //{
  //  //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //  //cout << "\tlocation = (" << locator->x() << ", " << locator->y() << ")" << endl;

  //    _draw_cross(height+ 0.5);
  //}
}

//Draw vertex in polar coordinates
void EAGLE_Viewer::_sphere1dVertex(float lon, float lat, float r, float f)
{
    float lamda = lon * deg2rad;
    float phi   = lat * deg2rad;
    float dist  = r * cos(phi);

    float x = dist * sin(lamda);
    float z = dist * cos(lamda);
    float y =    r * sin(phi);

    glTexCoord1f(f);
    glNormal3f(x, y, z);
    glVertex3f(x, y, z);
}

void EAGLE_Viewer::_display_Xplane_on_sphere(int xs)
{
    int j, k, n;
    float f;
    float amp = 0.2 * zScale;
    float x1, y1, z1;
    float height[nz][ny];
    float values[nz][ny];
    double* terrain = geometry->get_hgt();

    if(xs >= nx)
        return;

    for(k = 0; k < nz; ++k)
    {
        for(j = 0; j < ny; ++j)
        {
            n = xs + (j + k * nyp) * nxp;
            values[k][j] = _var[n];

            n = xs + (j + k * ny) * nx;
            height[k][j] = terrain[n];
        }
    }

    if(geometry->isXstaggered())
    {
        lon = geometry->get_ulon();
        lat = geometry->get_ulat();
    }
    else if(geometry->isYstaggered())
    {
        lon = geometry->get_vlon();
        lat = geometry->get_vlat();
    }
    else
    {
        lon = geometry->get_lon();
        lat = geometry->get_lat();
    }

    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_2D);
    glEnable( GL_TEXTURE_1D );
    glBindTexture(GL_TEXTURE_1D, texture1d->get_textureID());

    n = nx - 1 - xs;
    for(k = 0; k < nz - 1; ++k)
    {
        glBegin(GL_QUAD_STRIP);
            for(j = 0; j < ny; ++j)
            {
                x1 = lon[n + j * nxp];
                y1 = lat[n + j * nxp];

                z1 = 1.001 + amp * height[k][j];
                f = scale * (values[k][j] - vMinimum);
                _sphere1dVertex(x1, y1, z1, f);

                z1 = 1.001 + amp * height[k+1][j];
                f = scale * (values[k+1][j] - vMinimum);
                _sphere1dVertex(x1, y1, z1, f);
            }
        glEnd();
    }

    glDisable(GL_TEXTURE_1D);
}

void EAGLE_Viewer::_display_Yplane_on_sphere(int ys)
{
    int i, k, n;
    float f;
    float amp = 0.2 * zScale;
    float x1, y1, z1;
    float height[nz][nx];
    float values[nz][nx];
    double* terrain = geometry->get_hgt();

    if(ys >= ny)
        return;

    for(k = 0; k < nz; ++k)
    {
        for(i = 0; i < nx; ++i)
        {
            n = i + (ys + k * nyp) * nxp;
            values[k][i] = _var[n];

            n = i + (ys + k * ny) * nx;
            height[k][i] = terrain[n];
        }
    }

    if(geometry->isXstaggered())
    {
        lon = geometry->get_ulon();
        lat = geometry->get_ulat();
    }
    else if(geometry->isYstaggered())
    {
        lon = geometry->get_vlon();
        lat = geometry->get_vlat();
    }
    else
    {
        lon = geometry->get_lon();
        lat = geometry->get_lat();
    }

    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_2D);
    glEnable( GL_TEXTURE_1D );
    glBindTexture(GL_TEXTURE_1D, texture1d->get_textureID());

    for(k = 0; k < nz - 1; ++k)
    {
        glBegin(GL_QUAD_STRIP);
            n = nxp;
            for(i = 0; i < nx; ++i)
            {
                --n;
                x1 = lon[n + ys * nxp];
                y1 = lat[n + ys * nxp];

                z1 = 1.001 + amp * height[k][i];
                f = scale * (values[k][i] - vMinimum);
                _sphere1dVertex(x1, y1, z1, f);

                z1 = 1.001 + amp * height[k+1][i];
                f = scale * (values[k+1][i] - vMinimum);
                _sphere1dVertex(x1, y1, z1, f);
            }
        glEnd();
    }

    glDisable(GL_TEXTURE_1D);
}

void EAGLE_Viewer::_display_Zplane_on_sphere(int zs)
{
    int i, j, n;
    float f;
    float amp = 0.2 * zScale;
    float x1, y1, z1;
    double* height = NULL;
    double* terrain = geometry->get_hgt();

    if(zs >= nz)
        return;

    if(geometry->isXstaggered())
    {
        lon = geometry->get_ulon();
        lat = geometry->get_ulat();
    }
    else if(geometry->isYstaggered())
    {
        lon = geometry->get_vlon();
        lat = geometry->get_vlat();
    }
    else
    {
        lon = geometry->get_lon();
        lat = geometry->get_lat();
    }

    n = zs * geometry->get_nx() * geometry->get_ny();
    pltvar = &_var[n];

    n = zs * nx * ny;
    height = &terrain[n];

    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_2D);
    glEnable( GL_TEXTURE_1D );
    glBindTexture(GL_TEXTURE_1D, texture1d->get_textureID());

    for(j = 0; j < ny - 1; ++j)
    {
        glBegin(GL_QUAD_STRIP);
        for(i = 0; i < nx; ++i)
        {
            x1 = lon[i + j * nxp];
            y1 = lat[i + j * nxp];
            z1 = 1.001 + amp * height[i + j * nx];
            f = scale * (pltvar[i + j * nxp] - vMinimum);
            _sphere1dVertex(x1, y1, z1, f);

            x1 = lon[i + (j + 1) * nxp];
            y1 = lat[i + (j + 1) * nxp];
            z1 = 1.001 + amp * height[i + (j + 1) * nx];
            f = scale * (pltvar[i + (j + 1)* nxp] - vMinimum);
            _sphere1dVertex(x1, y1, z1, f);
        }
        glEnd();
    }

    glDisable(GL_TEXTURE_1D);

  //if(locator->on())
  //{
  //  //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //  //cout << "\tlocation = (" << locator->x() << ", " << locator->y() << ")" << endl;

  //    _draw_cross(height+ 0.5);
  //}
}

void EAGLE_Viewer::set_geometry(EAGLE_Geometry *gm)
{
    geometry = gm;
}

void EAGLE_Viewer::setup(string vn, double *var)
{
    _varname  = vn;
    _var = var;

    _parameter_setup();

    if(first_time)
    {
        nvoptions->set_xsec(geometry->get_nx() + 1);
        nvoptions->set_xsec2(geometry->get_nx() + 1);
        nvoptions->set_ysec(geometry->get_ny() + 1);
        nvoptions->set_ysec2(geometry->get_ny() + 1);
        nvoptions->set_zsec(0);
        nvoptions->set_zsec2(geometry->get_nz() + 1);

        first_time = false;
    }

  //_marchingCubeSetup();

    draw();
}

void EAGLE_Viewer::set_OP(bool s)
{
    _hasOP = s;
}

void EAGLE_Viewer::set_opacity(int o)
{
  //cout << "Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__
  //     << ", file: <" << __FILE__ << ">" << endl;
  //cout << "\tOpacity = " << o << endl;

    _opacity = o;

    draw();
}

void EAGLE_Viewer::saveDataset()
{
    FILE *outfl;

    int nxyz = nxp * nyp * nzp;

  //cout << "\nFunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
  //cout << "\tSave <" << _varname << "> to savedDataset.bin" << endl;

    outfl = fopen("savedDataset.bin", "wb");
    if(NULL == outfl)
    {
        cout << "\nFunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__
             << ", file: <" << __FILE__ << ">" << endl;
        cout << "\nCan not open 'savedDataset.bin' for writing\n" << endl;
        return;
    }

  //cout << "\tnx = " << nxp << ", ny = " << nyp << ", nz = " << nzp << ", nxyz = " << nxyz << endl;

    fwrite(&nxp, 1, sizeof(int), outfl);
    fwrite(&nyp, 1, sizeof(int), outfl);
    fwrite(&nzp, 1, sizeof(int), outfl);
    fwrite(_var, nxyz, sizeof(float), outfl);

    fclose(outfl);
}

void EAGLE_Viewer::_display_on_map()
{
    double height;

    glDisable(GL_LIGHTING);

    height = (double)nvoptions->get_zsec() / nz;
    coastline->drawONplane2(height, 3);
    stateboundary->drawONplane2(height, 1);

    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1, 1);

    _get_factor();
    _display_Zplane_on_map(nvoptions->get_zsec());

    glDisable(GL_POLYGON_OFFSET_FILL);

  //if(NULL != locator)
  //    locator->set_height(height);

  //if(locator->on())
  //{
  //  //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //  //cout << "\tlocation = (" << locator->x() << ", " << locator->y() << ")" << endl;

  //    _draw_cross(height+ 0.5);
  //}
}

//Draw vertex in xyz coordinates
void EAGLE_Viewer::_planeVertex(double lon, double lat, double height, double f)
{
    double x = lon * oneover - _xfactor;
    double y = lat * oneover;

    glNormal3d(0.0, 0.0, 1.0);
    glTexCoord1d(f);
    glVertex3d(x,y,height);
}

void EAGLE_Viewer::_display_Zplane_on_map(int zs)
{
    int i, j, n;
    double f;
    double height;
    double* pltvar = NULL;

    if(zs >= nz)
        return;

    n = zs * nxp * nyp;
    pltvar = &_var[n];

  //height = (double) zs / (double) geometry->get_nz() - 0.5;
    height = (double) zs / (double) geometry->get_nz();

  //_zlist[zs] = glGenLists(1);
  //glNewList(_zlist[zs], GL_COMPILE);
  //glNewList(_zlist[zs], GL_COMPILE_AND_EXECUTE);

    glPushMatrix();

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

    coastline->drawONplane2(height+0.001, 3);
    stateboundary->drawONplane2(height+0.001, 1);

    glPopMatrix();

  //glEndList();
}

void EAGLE_Viewer::_get_factor()
{
    int i, j, n;
    bool need_adjust = false;

    if(9.0 > _xfactor)
        return;

    _xfactor = 1.0;
    for(n = 0; n < nx*ny; ++n)
    {
        if(0.0 > lon[n])
            lon[n] += 360.0;
    }

    need_adjust = false;
    for(j = 0; j < ny; ++j)
    {
        n = j * nxp;
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

void EAGLE_Viewer::_display_on_sphere_map()
{
    double height;

    if(geometry->isXstaggered())
    {
        lon = geometry->get_ulon();
        lat = geometry->get_ulat();
    }
    else if(geometry->isYstaggered())
    {
        lon = geometry->get_vlon();
        lat = geometry->get_vlat();
    }
    else
    {
        lon = geometry->get_lon();
        lat = geometry->get_lat();
    }

    _get_factor();
    _display_Zplane_on_sphere_map(nvoptions->get_zsec());

  //height = 0.501 + (nz - 1 - nvoptions->get_zsec()) * zDelt;
    height = 0.501 + (double) nvoptions->get_zsec() / nz;

    coastline->draw(height, 3);
    stateboundary->draw(height, 1);
}

void EAGLE_Viewer::_display_Zplane_on_sphere_map(int zs)
{
    int i, j, n, n1, n2;
    double f;
    double height;
    double* pltvar = NULL;

    if(zs >= nz)
        return;

    n = zs * nxp * nyp;
    pltvar = &_var[n];

  //cout << "Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
  //cout << "\tnx = " << nx << ", ny = " << ny << ", nz = " << nz << ", n = " << n << endl;

    height = 0.501 + (double) zs / nz;

  //_zlist[zs] = glGenLists(1);
  //glNewList(_zlist[zs], GL_COMPILE);
  //glNewList(_zlist[zs], GL_COMPILE_AND_EXECUTE);

    glPushMatrix();

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
                n1 = i + j * nxp;
                n2 = n1 + nxp;

                f = scale * (pltvar[n1] - vMinimum);
                _sphere1dVertex(lon[n1], lat[n1], height, f);

                f = scale * (pltvar[n2] - vMinimum);
                _sphere1dVertex(lon[n2], lat[n2], height, f);
            }
        glEnd();
    }

    glDisable(GL_TEXTURE_1D);

    glPopMatrix();

  //glEndList();
}

void EAGLE_Viewer::update_colormap()
{
    texture1d->reset();
    texture1d->set_colors(colorTable->get_clen(), colorTable->get_cmap());
    texture1d->set_name(colorTable->get_name());
}

