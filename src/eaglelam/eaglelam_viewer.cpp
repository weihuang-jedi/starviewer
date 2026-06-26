#include <QtOpenGL>

#include "eaglelam_viewer.h"

#define MINVAL	1.0e-20

EAGLELAM_Viewer::EAGLELAM_Viewer(ColorTable *ct, NVOptions* opt)
{
    cout << "\nEnter function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
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
    locator = NULL;

    _hasOP = false;

    _xfactor = 999.0;
    oneover  = 1.0 / 180.0;

    cout << "Leave function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
}

EAGLELAM_Viewer::~EAGLELAM_Viewer()
{
  //cout << "\tEnter function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;

    if(NULL != locator)
        locator->turnOff();

    delete texture1d;
    delete coastline;
    delete mapprojection;
    delete stateboundary;

  //cout << "\tLeave function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
}

void EAGLELAM_Viewer::scaleNtranslate()
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

void EAGLELAM_Viewer::draw()
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

void EAGLELAM_Viewer::_parameter_setup()
{
    size_t sz;
    Evaluator evaluator;
    string tex1dName = texture1d->get_name();

    if(0 != tex1dName.compare(colorTable->get_name()))
    {
        texture1d->set_colors(colorTable->get_clen(), colorTable->get_cmap());
        texture1d->set_name(colorTable->get_name());
    }

    nx = geometry->get_nx();
    ny = geometry->get_ny();
    nz = geometry->get_nz();

    sz = nx * ny * nz;
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

    lon = geometry->get_longitude();
    lat = geometry->get_latitude();

    _xGrid = geometry->get_xGrid();
    _yGrid = geometry->get_yGrid();

    _grid_mapping_name = geometry->get_grid_mapping_name();
    _latitude_of_projection_origin = geometry->get_latitude_of_projection_origin();
    _longitude_of_central_meridian = geometry->get_longitude_of_central_meridian();
    _standard_parallel = geometry->get_standard_parallel();

    //Projection codes for proj_info structure: (in mapprojection.h)
//#define PROJ_LATLON     0
//#define PROJ_LAMBERT_CONFORMAL 1
//#define PROJ_MERCATER   3
//#define PROJ_POLAR_STEREOGRAPHIC                5
    int map_projection = PROJ_LAMBERT_CONFORMAL;
    // map_projection = 1;
    float dx = static_cast<float>(_xGrid[1] - _xGrid[0]);

    cout << "Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    cout << "\tnx  = " << nx  << ", ny  = " << ny  << ", nz  = " << nz  << endl;
    cout << "\tsetup map projection: " << endl;
    cout << "\t\tcode = " << map_projection;
    cout << "\t\tlon1 = " << lon[0] << endl;
    cout << "\t\tlat1 = " << lat[0] << endl;
    cout << "\t\tdx = " << dx << endl;
    cout << "\t\tstdlon = " << _longitude_of_central_meridian << endl;
    cout << "\t\tcenlon = " << _longitude_of_central_meridian << endl;
    cout << "\t\tcenlat = " << _latitude_of_projection_origin << endl;
    cout << "\t\ttruelat1 = " << _standard_parallel[0] << endl;
    cout << "\t\ttruelat2 = " << _standard_parallel[1] << endl;

    mapprojection->setup(map_projection, lon[0], lat[0], dx,
                         _longitude_of_central_meridian,
                         _standard_parallel[0], _standard_parallel[1]);
    mapprojection->set_dimension(nx, ny);

    // stateboundary->set_mapprojection(mapprojection);
}

void EAGLELAM_Viewer::_display_all()
{
    cout << "\tFunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
  //cout << "\tnvoptions->get_zsec() = " << nvoptions->get_zsec() << endl;

    if(geometry->get_nz() > nvoptions->get_zsec())
    {
        _display_Zplane(nvoptions->get_zsec());
    }
}

void EAGLELAM_Viewer::_display_Zplane(int zs)
{
    int i, j, k, n1, n2;
    float *pltvar = NULL;
    float f;
    float x1, y1, y2, z1;

    // cout << "\nEnter Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;

    pltvar = &_var[0];

    z1 = 0.0;

    k = nvoptions->get_zsec()+1;

    // zcl = glGenLists(1);
    // glNewList(zcl, GL_COMPILE_AND_EXECUTE);
    // lister->set_zid(k, zcl);

    glPushMatrix();
    glClearColor(1.0, 1.0, 1.0, 1.0);
  //OpenGL should normalize normal vectors
    glEnable(GL_NORMALIZE);
    glEnable(GL_TEXTURE_1D);
    glBindTexture(GL_TEXTURE_1D, texture1d->get_textureID());
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glNormal3f(0.0, 0.0, -1.0);

    // cout << "\tFunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    // cout << "\tnx: " << nx << ", ny: " << ny << ", xStart: " << xStart << ", yStart: " << yStart << ", xyDelt = " << xyDelt << endl;
    // cout << "\tvMinimum =" << vMinimum << endl;
    // cout << "\tnx*ny =" << nx*ny << endl;

    for(j = 0; j < ny - 1; ++j)
    {
        y1 = yStart + xyDelt * j;
        y2 = yStart + xyDelt * (j + 1);

        n1 = j * nx;
        n2 = n1 + nx;

        // cout << "\tn1: " << n1 << ", n2: " << n2 << endl;

        glBegin(GL_QUAD_STRIP);
#if 1
        for(i = 0; i < nx; ++i)
        {
            x1 = xStart + xyDelt * i;

            f = scale * (pltvar[n1+i] - vMinimum);
            glTexCoord1f(f);
            glVertex3f(x1, y1, z1);

            f = scale * (pltvar[n2+i] - vMinimum);
            glTexCoord1f(f);
            glVertex3f(x1, y2, z1);
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

    // coastline->drawOnPlane(z1+0.01);
    // stateboundary->drawONplane2(z1, 1);

    glDisable(GL_TEXTURE_1D);
    glPopMatrix();
    // glEndList();

  //if(locator->on())
  //{
  //  //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //  //cout << "\tlocation = (" << locator->x() << ", " << locator->y() << ")" << endl;
  //    _draw_cross(height+ 0.5);
  //}
  //
     // cout << "Leave Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
}

void EAGLELAM_Viewer::_display_with_bump()
{
    if(geometry->get_nz() > nvoptions->get_zsec())
    {
        _display_Zplane_with_bump(nvoptions->get_zsec());
    }
}

void EAGLELAM_Viewer::_display_Zplane_with_bump(int zs)
{
    int i, j, n;
    float *pltvar = NULL;
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

void EAGLELAM_Viewer::_display_on_height_surface()
{
    if(geometry->get_nz() > nvoptions->get_zsec())
    {
        _display_Zplane_on_height_surface(nvoptions->get_zsec());
    }

    if(geometry->get_nz() > nvoptions->get_zsec2())
    {
        _display_Zplane_on_height_surface(nvoptions->get_zsec2());
    }
}

void EAGLELAM_Viewer::_display_Zplane_on_height_surface(int zs)
{
    int i, j, n, np1;
    float height;
    float f;
    float x1, y1, y2, z1;
    
    n = zs * geometry->get_nx() * geometry->get_ny();
    pltvar = &_var[n];

    n = zs * nx * ny;
    height = 0.1;

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
        n = j * nx;
        np1 = (j + 1) * nx;
        y1 = yStart + xyDelt * j;
        y2 = yStart + xyDelt * (j + 1);
                
        glBegin(GL_QUAD_STRIP);
        for(i = 0; i < nx; ++i)
        {   
            x1 = xStart + xyDelt * i;
    
            f = scale * (pltvar[n+i] - vMinimum);
            glTexCoord1f(f);
            glVertex3d(x1, y1, height); 

            f = scale * (pltvar[np1+i] - vMinimum);
            glTexCoord1f(f);
            glVertex3d(x1, y2, height);
        }
        glEnd();
    }

    glDisable(GL_TEXTURE_1D);
}

void EAGLELAM_Viewer::_display_on_sphere()
{
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
void EAGLELAM_Viewer::_sphere1dVertex(float lon, float lat, float r, float f)
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

void EAGLELAM_Viewer::_display_Zplane_on_sphere(int zs)
{
    int i, j, n;
    float f;
    float amp = 0.2 * zScale;
    float x1, y1, z1;
    float height = 1.01;

    if(zs >= nz)
        return;

    n = zs * geometry->get_nx() * geometry->get_ny();
    pltvar = &_var[n];

    n = zs * nx * ny;

    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_2D);
    glEnable( GL_TEXTURE_1D );
    glBindTexture(GL_TEXTURE_1D, texture1d->get_textureID());

    for(j = 0; j < ny - 1; ++j)
    {
        glBegin(GL_QUAD_STRIP);
        for(i = 0; i < nx; ++i)
        {
            x1 = lon[i + j * nx];
            y1 = lat[i + j * nx];
            f = scale * (pltvar[i + j * nx] - vMinimum);
            _sphere1dVertex(x1, y1, height, f);

            x1 = lon[i + (j + 1) * nx];
            y1 = lat[i + (j + 1) * nx];
            f = scale * (pltvar[i + (j + 1)* nx] - vMinimum);
            _sphere1dVertex(x1, y1, height, f);
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

void EAGLELAM_Viewer::set_geometry(EAGLELAM_Geometry *gm)
{
    geometry = gm;
}

void EAGLELAM_Viewer::setup(string vn, float *var)
{
    _varname  = vn;
    _var = var;

    _parameter_setup();

    nvoptions->set_zsec(0);

    draw();
}

void EAGLELAM_Viewer::set_OP(bool s)
{
    _hasOP = s;
}

void EAGLELAM_Viewer::set_opacity(int o)
{
  //cout << "Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__
  //     << ", file: <" << __FILE__ << ">" << endl;
  //cout << "\tOpacity = " << o << endl;

    _opacity = o;

    draw();
}

void EAGLELAM_Viewer::saveDataset()
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

void EAGLELAM_Viewer::_display_on_map()
{
    float height;

    glDisable(GL_LIGHTING);

    height = (float)nvoptions->get_zsec() / nz;
    coastline->drawOnPlane(height+0.01);
    // stateboundary->drawONplane2(height, 1);

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
void EAGLELAM_Viewer::_planeVertex(float lon, float lat, float height, float f)
{
    float x = lon * oneover - _xfactor;
    float y = lat * oneover;

    glNormal3f(0.0, 0.0, 1.0);
    glTexCoord1f(f);
    glVertex3f(x,y,height);
}

void EAGLELAM_Viewer::_display_Zplane_on_map(int zs)
{
    int i, j, n;
    float f;
    float height;
    float* pltvar = NULL;

    if(zs >= nz)
        return;

    n = zs * nxp * nyp;
    pltvar = &_var[n];

    height = 0.01;

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

    coastline->drawOnPlane(height+0.001);
    // stateboundary->drawONplane2(height+0.001, 1);

    glPopMatrix();

  //glEndList();
}

void EAGLELAM_Viewer::_get_factor()
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

void EAGLELAM_Viewer::_display_on_sphere_map()
{
    float radius = 1.01;

    lon = geometry->get_longitude();
    lat = geometry->get_latitude();

    _get_factor();
    _display_Zplane_on_sphere_map(nvoptions->get_zsec());

    coastline->drawOnSphere(radius);
    // stateboundary->draw(radius, 1);
}

void EAGLELAM_Viewer::_display_Zplane_on_sphere_map(int zs)
{
    int i, j, n, n1, n2;
    float f;
    float height;
    float* pltvar = NULL;

    if(zs >= nz)
        return;

    n = zs * nxp * nyp;
    pltvar = &_var[n];

  //cout << "Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
  //cout << "\tnx = " << nx << ", ny = " << ny << ", nz = " << nz << ", n = " << n << endl;

    height = 0.501 + (float) zs / nz;

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
        n1 = j * nx;
        n2 = n1 + nx;
        glBegin(GL_QUAD_STRIP);
            for(i = 0; i < nx; ++i)
            {
                f = scale * (pltvar[n1+i] - vMinimum);
                _sphere1dVertex(lon[n1+i], lat[n1+i], height, f);

                f = scale * (pltvar[n2+i] - vMinimum);
                _sphere1dVertex(lon[n2+i], lat[n2+i], height, f);
            }
        glEnd();
    }

    glDisable(GL_TEXTURE_1D);

    glPopMatrix();

  //glEndList();
}

void EAGLELAM_Viewer::update_colormap()
{
    texture1d->reset();
    texture1d->set_colors(colorTable->get_clen(), colorTable->get_cmap());
    texture1d->set_name(colorTable->get_name());
}

