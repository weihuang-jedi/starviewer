#include <QtOpenGL>

#include <vector>

#include "camse2dviewer.h"

CAMse2dViewer::CAMse2dViewer(ColorTable *ct, NVOptions* opt)
{
    colorTable = ct;
    nvoptions = opt;

    texture1d = new Texture1d();
    texture1d->set_colors(ct->get_clen(), ct->get_cmap());
    texture1d->set_name(ct->get_name());

    _var = NULL;
 
    _innerPoints = NULL;
    _nBoundaryPoints = 0;
    num_south_pole_cols = 0;
    num_north_pole_cols = 0;
    num_boundary_cols = 0;

    earth = new Earth();

    nvoptions->set_xsec(360);
    nvoptions->set_ysec(90);
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

CAMse2dViewer::~CAMse2dViewer()
{
    locator->turnOff();

    if(NULL !=  _innerPoints)
        delete []_innerPoints;
    _innerPoints = NULL;

    delete earth;
    delete lister;
    delete texture1d;
}

void CAMse2dViewer::set_geometry(CAMseGeometry *gm)
{
    geometry = gm;

    _initialize();
}

void CAMse2dViewer::setup(string vn, double *var)
{
    reset();

    _varname  = vn;
    _var = var;

    _evaluate(_var);

    lev = geometry->get_camse_lev();

    previoustimelevel = -1;
}

void CAMse2dViewer::reset()
{
    lister->reinitialize(_nlon+1, _nlat+1, geometry->get_camse_lev()+1);
}

void CAMse2dViewer::_initialize()
{
    int i, j, m, n;

    int positive;
    int negative;

    previoustimelevel = -1;

    ncenters = geometry->get_camse_ncenters();
    ncorners = geometry->get_camse_ncorners();
    ncol     = geometry->get_camse_ncol();
    lev      = geometry->get_camse_lev();

    element_corners = geometry->get_camse_element_corners();

    lon = geometry->get_camse_lon();
    lat = geometry->get_camse_lat();

  //lister->reinitialize(361, 181, lev);

    _xFlat = geometry->get_xFlat();
    _yFlat = geometry->get_yFlat();

    if(0 != _nBoundaryPoints)
        return;

    _innerPoints = new bool[ncenters];

  //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\tncenters = " << ncenters << ", ncorners = " << ncorners << endl;

  //#pragma omp parallel for
    for(n = 0; n < ncenters; ++n)
    {
        j = n * ncorners;

        positive = 0;
        negative = 0;
        _innerPoints[n] = true;
        for(i = 0; i < ncorners; ++i)
        {
            m = element_corners[j+i];

            if(0.75 < _xFlat[m])
                ++positive;
            else if(-0.75 > _xFlat[m])
                ++negative;

            if(NEAR_NORTH_POLE < _yFlat[m])
            {
              //North Pole
                _innerPoints[n] = false;
                ++_nBoundaryPoints;
                north_pole_cols.push_back(n);
                ++num_north_pole_cols;

                positive = 0;
                negative = 0;

                break;
            }
            else if(NEAR_SOUTH_POLE > _yFlat[m])
            {
              //South Pole
                _innerPoints[n] = false;
                ++_nBoundaryPoints;
                south_pole_cols.push_back(n);
                ++num_south_pole_cols;

                positive = 0;
                negative = 0;
                
                break;
            }
        }

        if(positive && negative)
        {
            _innerPoints[n] = false;
            ++_nBoundaryPoints;
            boundary_cols.push_back(n);
            ++num_boundary_cols;
        }
    }

  //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\tncenters = " << ncenters << ", _nBoundaryPoints = " << _nBoundaryPoints << endl;
  //cout << "\tnum_south_pole_cols = " << num_south_pole_cols << ", num_north_pole_cols = " << num_north_pole_cols
  //     << ", num_boundary_cols = " << num_boundary_cols << endl;
}

void CAMse2dViewer::draw()
{
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

    current_timelevel = nvoptions->get_tsec();
    if(current_timelevel != previoustimelevel)
        reset();
    previoustimelevel = current_timelevel;
    if(current_timelevel >= geometry->get_nt())
        return;

    if((geometry->get_camse_lev() <= nvoptions->get_zsec()) && (0 > nvoptions->get_zsec()))
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

    pltvar = &_var[current_timelevel * ncol];

    zcl = lister->get_zid(nvoptions->get_zsec());
    ycl = lister->get_yid(nvoptions->get_ysec());
    xcl = lister->get_xid(nvoptions->get_xsec());

  //Clear screen and Z-buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  //glClearColor(0.0, 0.0, 0.0, 0.0);

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
            if(nvoptions->get_zsec() < lev)
            {
                if(zcl)
                    glCallList(zcl);
                else
                    _flatDisplay();
            }

          //draw_plane_grids();

            if(360 > nvoptions->get_xsec())
            {
                if(xcl)
                    glCallList(xcl);
                else
                    _display_Xflat_plane(nvoptions->get_xsec());
            }

            if((-80 < nvoptions->get_ysec()) && (80 > nvoptions->get_ysec()))
            {
                if(ycl)
                    glCallList(ycl);
                else
                    _display_Yflat_plane(nvoptions->get_ysec());
            }
        }
        else
        {
            if(nvoptions->get_zsec() < lev)
            {
                if(zcl)
                    glCallList(zcl);
                else
                    _sphereDisplay();
            }
    
            if(360 > nvoptions->get_xsec())
            {
                if(xcl)
                    glCallList(xcl);
                else
                    _sphereXplane(nvoptions->get_xsec());
            }
    
            if((-80 < nvoptions->get_ysec()) && (80 > nvoptions->get_ysec()))
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

void CAMse2dViewer::_lonlat2xyz(double lon, double lat, double radius, double fact)
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

void CAMse2dViewer::_sphereDisplay()
{
    int i, j, k, m, n;

    double sv = 1.0;
    double fact;
    double radius = 1.001;

    current_timelevel = nvoptions->get_tsec();
    if(current_timelevel >= geometry->get_nt())
        return;

    k = nvoptions->get_zsec();

  //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\t_varname: <" << _varname << ">, lev = " << lev << endl;
  //cout << "\tncenters = " << ncenters << ", nvoptions->get_zsec() = " << nvoptions->get_zsec() << endl;

    radius = 0.725 + 0.5 * ( 1.0 - (k + 1.0) / lev);

    locator->set_height(radius);

    sv = 1.0 / (_valmax - _valmin);

    zcl = glGenLists(1);
  //glNewList(zcl, GL_COMPILE);
    glNewList(zcl, GL_COMPILE_AND_EXECUTE);
    lister->set_zid(k, zcl);

    glPushMatrix();

    glDisable(GL_LIGHTING);

    if(nvoptions->get_cb(NV_COASTLINEON))
    {
        coastline->draw(radius + 0.001, 2);
    }

    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1, 1);

    glColor4f(1.0, 1.0, 1.0, 1.0);
  //glColor4f(0.0, 0.0, 0.0, 0.0);

    glEnable(GL_TEXTURE_1D);
    glBindTexture(GL_TEXTURE_1D, texture1d->get_textureID());

    glBegin(GL_QUADS);
  //#pragma omp parallel for
    for(n = 0; n < ncenters; ++n)
    {
        j = n * ncorners;
        for(i = 0; i < ncorners; ++i)
        {
            m = element_corners[j+i];
            fact = sv * (pltvar[m+k*ncol] - _valmin);
            _lonlat2xyz(lon[m], lat[m], radius, fact);
        }
    }
    glEnd();

    glDisable(GL_POLYGON_OFFSET_FILL);
    glDisable(GL_TEXTURE_1D);

    if(locator->on())
    {
      //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
      //cout << "\tlocation = (" << locator->x() << ", " << locator->y() << ")" << endl;

        _draw_cross(radius+ 0.1);
        if((NULL != nvoptions) && nvoptions->get_cb(NV_GRIDON))
            draw_sphere_grids();
    }

    glPopMatrix();

    glEndList();
}

void CAMse2dViewer::_flatDisplay()
{
    int i, j, k, m, n, num;
    int numProcessedBoundaryPoints = 0;

    double sv = 1.0;
    double fact;
    double height = 0.00;

    double xb[ncorners];
    double yb[ncorners];
    double vb[ncorners];
    double fb[ncorners];

    GLfloat line_width = 1.0;

    current_timelevel = nvoptions->get_tsec();
    if(current_timelevel >= geometry->get_nt())
        return;

  //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\t_varname: <" << _varname << ">, lev = " << lev << endl;
  //cout << "\tcurrent_timelevel = " << current_timelevel << endl;

    k = nvoptions->get_zsec();

    height = 0.8 * (0.5 - (k + 1.0) / lev);

    if(1 == lev)
        height = 0.001;

    locator->set_height(height);

    sv = 1.0 / (_valmax - _valmin);

    zcl = glGenLists(1);
  //glNewList(zcl, GL_COMPILE);
    glNewList(zcl, GL_COMPILE_AND_EXECUTE);
    lister->set_zid(k, zcl);

    glPushMatrix();

    if(nvoptions->get_cb(NV_COASTLINEON))
    {
        coastline->drawONplane(height+0.001, 1);
    }

    glColor3f(1.0, 1.0, 0.0);

    line_width = 2.0;
    glLineWidth(line_width);

    glBegin(GL_LINE_STRIP);
        glVertex3d(-1.0, -0.5, height);
        glVertex3d( 1.0, -0.5, height);
        glVertex3d( 1.0,  0.5, height);
        glVertex3d(-1.0,  0.5, height);
        glVertex3d(-1.0, -0.5, height);
    glEnd();

    if(1 == lev)
    {
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(1, 1);
    }

    glColor4f(1.0, 1.0, 1.0, 1.0);
  //glColor4f(0.0, 0.0, 0.0, 0.0);
    glNormal3f(0.0, 0.0, -1.0);

    glEnable(GL_TEXTURE_1D);
    glBindTexture(GL_TEXTURE_1D, texture1d->get_textureID());

    glBegin(GL_QUADS);
    for(n = 0; n < ncenters; ++n)
    {
        if(_innerPoints[n])
        {
            j = n * ncorners;

            for(i = 0; i < ncorners; ++i)
            {
                m = element_corners[j+i];
                fact = sv * (pltvar[m+k*ncol] - _valmin);
                glTexCoord1d(fact);

                glVertex3d(_xFlat[m], _yFlat[m], height);
            }
        }
    }
    glEnd();

  //handle South Pole
    for(num = 0; num < num_south_pole_cols; ++num)
    {
        n = south_pole_cols[num];

        j = n * ncorners;

        for(i = 0; i < ncorners; ++i)
        {
            m = element_corners[j+i];
            vb[i] = pltvar[m+k*ncol];
            fb[i] = sv * (vb[i] - _valmin);

            xb[i] = _xFlat[m];
            yb[i] = _yFlat[m];
        }

      //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__
      //     << ", line: " << __LINE__ << endl;
      //cout << "\txb[0] = " << xb[0] << ", xb[1] = " << xb[1] << ", xb[2] = " << xb[2] << ", xb[3] = " << xb[3] << endl;
      //cout << "\tyb[0] = " << yb[0] << ", yb[1] = " << yb[1] << ", yb[2] = " << yb[2] << ", yb[3] = " << yb[3] << endl;

        if(NEAR_SOUTH_POLE > yb[0])
        {
          //Point 0 at South pole
          //Make 2 quads: 0, 1, 0, 2 and 0, 2, 0, 3.

            ++numProcessedBoundaryPoints;

            glBegin(GL_QUAD_STRIP);

            glTexCoord1d(fb[0]);
            glVertex3d(xb[1], yb[0], height);

            glTexCoord1d(fb[1]);
            glVertex3d(xb[1], yb[1], height);

            glTexCoord1d(fb[0]);
            glVertex3d(xb[2], yb[0], height);

            glTexCoord1d(fb[2]);
            glVertex3d(xb[2], yb[2], height);

            glTexCoord1d(fb[0]);
            glVertex3d(xb[3], yb[0], height);

            glTexCoord1d(fb[3]);
            glVertex3d(xb[3], yb[3], height);

            glEnd();

            continue;
        }
        else if(NEAR_SOUTH_POLE > yb[1])
        {
          //Point 1 at South pole
          //Make 2 quads: 1, 0, 1, 2 and 1, 2, 1, 3.

            if((NEAR_NEGATIVE_ZERO < xb[2]) && (NEAR_NEGATIVE_ZERO < xb[3]) && (NEAR_WEST_BOUNDARY > xb[0]))
                xb[0] = 1.0;

            ++numProcessedBoundaryPoints;

            glBegin(GL_QUAD_STRIP);

            glTexCoord1d(fb[1]);
            glVertex3d(xb[0], yb[1], height);

            glTexCoord1d(fb[0]);
            glVertex3d(xb[0], yb[0], height);

            glTexCoord1d(fb[1]);
            glVertex3d(xb[3], yb[1], height);

            glTexCoord1d(fb[3]);
            glVertex3d(xb[3], yb[3], height);

            glTexCoord1d(fb[1]);
            glVertex3d(xb[2], yb[1], height);

            glTexCoord1d(fb[2]);
            glVertex3d(xb[2], yb[2], height);

            glEnd();

            continue;
        }
        else if(NEAR_SOUTH_POLE > yb[2])
        {
          //Point 2 at South pole
          //Make 2 quads: 2, 0, 2, 1 and 2, 1, 2, 3.

            ++numProcessedBoundaryPoints;

            glBegin(GL_QUAD_STRIP);

            glTexCoord1d(fb[2]);
            glVertex3d(xb[1], yb[2], height);

            glTexCoord1d(fb[1]);
            glVertex3d(xb[1], yb[1], height);

            glTexCoord1d(fb[2]);
            glVertex3d(xb[0], yb[2], height);

            glTexCoord1d(fb[0]);
            glVertex3d(xb[0], yb[0], height);

            glTexCoord1d(fb[2]);
            glVertex3d(xb[3], yb[2], height);

            glTexCoord1d(fb[3]);
            glVertex3d(xb[3], yb[3], height);

            glEnd();

            continue;
        }
        else if(NEAR_SOUTH_POLE > yb[3])
        {
          //Point 3 at South pole
          //Make 2 quads: 3, 0, 3, 1 and 3, 1, 3, 2.

            ++numProcessedBoundaryPoints;

            glBegin(GL_QUAD_STRIP);

            glTexCoord1d(fb[3]);
            glVertex3d(xb[2], yb[3], height);

            glTexCoord1d(fb[2]);
            glVertex3d(xb[2], yb[2], height);

            glTexCoord1d(fb[3]);
            glVertex3d(xb[1], yb[3], height);

            glTexCoord1d(fb[1]);
            glVertex3d(xb[1], yb[1], height);

            glTexCoord1d(fb[3]);
            glVertex3d(xb[0], yb[3], height);

            glTexCoord1d(fb[0]);
            glVertex3d(xb[0], yb[0], height);

            glEnd();

            continue;
        }
        else
        {
            cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__
                 << ", line: " << __LINE__ << endl;
            cout << "\txb[0] = " << xb[0] << ", xb[1] = " << xb[1] << ", xb[2] = " << xb[2] << ", xb[3] = " << xb[3] << endl;
            cout << "\tyb[0] = " << yb[0] << ", yb[1] = " << yb[1] << ", yb[2] = " << yb[2] << ", yb[3] = " << yb[3] << endl;
        }
    }

  //Handle North Pole
    for(num = 0; num < num_north_pole_cols; ++num)
    {
        n = north_pole_cols[num];

        j = n * ncorners;

        for(i = 0; i < ncorners; ++i)
        {
            m = element_corners[j+i];
            vb[i] = pltvar[m+k*ncol];
            fb[i] = sv * (vb[i] - _valmin);

            xb[i] = _xFlat[m];
            yb[i] = _yFlat[m];
        }

      //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__
      //     << ", line: " << __LINE__ << endl;
      //cout << "\txb[0] = " << xb[0] << ", xb[1] = " << xb[1] << ", xb[2] = " << xb[2] << ", xb[3] = " << xb[3] << endl;
      //cout << "\tyb[0] = " << yb[0] << ", yb[1] = " << yb[1] << ", yb[2] = " << yb[2] << ", yb[3] = " << yb[3] << endl;

        if(NEAR_NORTH_POLE < yb[0])
        {
          //Point 0 is at North Pole.
          //Make 2 quads: 0, 1, 0, 2 and 0, 2, 0, 3.

            if((NEAR_NEGATIVE_ZERO < xb[2]) && (NEAR_NEGATIVE_ZERO < xb[3]) && (NEAR_WEST_BOUNDARY > xb[1]))
                xb[1] = 1.0;

            ++numProcessedBoundaryPoints;

            glBegin(GL_QUAD_STRIP);

            glTexCoord1d(fb[0]);
            glVertex3d(xb[1], yb[0], height);

            glTexCoord1d(fb[1]);
            glVertex3d(xb[1], yb[1], height);

            glTexCoord1d(fb[0]);
            glVertex3d(xb[2], yb[0], height);

            glTexCoord1d(fb[2]);
            glVertex3d(xb[2], yb[2], height);

            glTexCoord1d(fb[0]);
            glVertex3d(xb[3], yb[0], height);

            glTexCoord1d(fb[3]);
            glVertex3d(xb[3], yb[3], height);

            glEnd();

            continue;
        }
        else if(NEAR_NORTH_POLE < yb[1])
        {
          //Point 1 is at North Pole.
          //Make 2 quads: 1, 0, 1, 2 and 1, 2, 1, 3.

            ++numProcessedBoundaryPoints;

            glBegin(GL_QUAD_STRIP);

            glTexCoord1d(fb[1]);
            glVertex3d(xb[0], yb[1], height);

            glTexCoord1d(fb[0]);
            glVertex3d(xb[0], yb[0], height);

            glTexCoord1d(fb[1]);
            glVertex3d(xb[3], yb[1], height);

            glTexCoord1d(fb[3]);
            glVertex3d(xb[3], yb[3], height);

            glTexCoord1d(fb[1]);
            glVertex3d(xb[2], yb[1], height);

            glTexCoord1d(fb[2]);
            glVertex3d(xb[2], yb[2], height);

            glEnd();

            continue;
        }
        else if(NEAR_NORTH_POLE < yb[2])
        {
          //Point 2 is at North Pole.
          //Make 2 quads: 2, 0, 2, 1 and 2, 1, 2, 3. 

            ++numProcessedBoundaryPoints;

            glBegin(GL_QUAD_STRIP);

            glTexCoord1d(fb[2]);
            glVertex3d(xb[1], yb[2], height); 

            glTexCoord1d(fb[1]);
            glVertex3d(xb[1], yb[1], height); 

            glTexCoord1d(fb[2]);
            glVertex3d(xb[0], yb[2], height);

            glTexCoord1d(fb[0]);
            glVertex3d(xb[0], yb[0], height);

            glTexCoord1d(fb[2]);
            glVertex3d(xb[3], yb[2], height); 

            glTexCoord1d(fb[3]);
            glVertex3d(xb[3], yb[3], height);

            glEnd();

            continue;
        }
        else if(NEAR_NORTH_POLE < yb[3])
        {
          //Point 3 is at North Pole.
          //Make 2 quads: 3, 0, 3, 1 and 3, 1, 3, 2.

            ++numProcessedBoundaryPoints;

            glBegin(GL_QUAD_STRIP);

            glTexCoord1d(fb[3]);
            glVertex3d(xb[2], yb[3], height);

            glTexCoord1d(fb[2]);
            glVertex3d(xb[2], yb[2], height);

            glTexCoord1d(fb[3]);
            glVertex3d(xb[1], yb[3], height);

            glTexCoord1d(fb[1]);
            glVertex3d(xb[1], yb[1], height);

            glTexCoord1d(fb[3]);
            glVertex3d(xb[0], yb[3], height);

            glTexCoord1d(fb[0]);
            glVertex3d(xb[0], yb[0], height);

            glEnd();

            continue;
        }
        else
        {
            cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__
                 << ", line: " << __LINE__ << endl;
            cout << "\txb[0] = " << xb[0] << ", xb[1] = " << xb[1] << ", xb[2] = " << xb[2] << ", xb[3] = " << xb[3] << endl;
            cout << "\tyb[0] = " << yb[0] << ", yb[1] = " << yb[1] << ", yb[2] = " << yb[2] << ", yb[3] = " << yb[3] << endl;
        }
    }

  //The middle boundary
    for(num = 0; num < num_boundary_cols; ++num)
    {
        n = boundary_cols[num];

        j = n * ncorners;

        for(i = 0; i < ncorners; ++i)
        {
            m = element_corners[j+i];
            vb[i] = pltvar[m+k*ncol];
            fb[i] = sv * (vb[i] - _valmin);

            xb[i] = _xFlat[m];
            yb[i] = _yFlat[m];
        }

        if((NEAR_POSITIVE_ZERO > xb[0]) && (NEAR_POSITIVE_ZERO > xb[1]))
        {
            if((NEAR_EAST_BOUNDARY < xb[2]) && (NEAR_EAST_BOUNDARY < xb[3]))
            {
                xb[2] = -1.0;
                xb[3] = -1.0;

                ++numProcessedBoundaryPoints;

                glBegin(GL_QUADS);

                glTexCoord1d(fb[0]);
                glVertex3d(xb[0], yb[0], height);

                glTexCoord1d(fb[1]);
                glVertex3d(xb[1], yb[1], height);

                glTexCoord1d(fb[2]);
                glVertex3d(xb[2], yb[2], height);

                glTexCoord1d(fb[3]);
                glVertex3d(xb[3], yb[3], height);

                glEnd();

                continue;
            }

            if((NEAR_EAST_BOUNDARY < xb[2]) && (NEAR_WEST_BOUNDARY > xb[3])) 
            {
                xb[2] = -1.0;

                ++numProcessedBoundaryPoints;

                glBegin(GL_QUADS);

                glTexCoord1d(fb[0]);
                glVertex3d(xb[0], yb[0], height);

                glTexCoord1d(fb[1]);
                glVertex3d(xb[1], yb[1], height);

                glTexCoord1d(fb[2]);
                glVertex3d(xb[2], yb[2], height);

                glTexCoord1d(fb[3]);
                glVertex3d(xb[3], yb[3], height);

                glEnd();

                continue;
            }

            if((NEAR_WEST_BOUNDARY > xb[2]) && (NEAR_EAST_BOUNDARY < xb[3]))
            {
                xb[3] = -1.0;

                ++numProcessedBoundaryPoints;

                glBegin(GL_QUADS);

                glTexCoord1d(fb[0]);
                glVertex3d(xb[0], yb[0], height);

                glTexCoord1d(fb[1]);
                glVertex3d(xb[1], yb[1], height);

                glTexCoord1d(fb[2]);
                glVertex3d(xb[2], yb[2], height);

                glTexCoord1d(fb[3]);
                glVertex3d(xb[3], yb[3], height);

                glEnd();

                continue;
            }
        }

        if((NEAR_POSITIVE_ZERO > xb[2]) && (NEAR_POSITIVE_ZERO > xb[3]))
        {
            if((NEAR_EAST_BOUNDARY < xb[0]) && (NEAR_EAST_BOUNDARY < xb[1])) 
            {
                xb[0] = -1.0;
                xb[1] = -1.0;

                ++numProcessedBoundaryPoints;

                glBegin(GL_QUADS);

                glTexCoord1d(fb[0]);
                glVertex3d(xb[0], yb[0], height);

                glTexCoord1d(fb[1]);
                glVertex3d(xb[1], yb[1], height);

                glTexCoord1d(fb[2]);
                glVertex3d(xb[2], yb[2], height);

                glTexCoord1d(fb[3]);
                glVertex3d(xb[3], yb[3], height);

                glEnd();

                continue;
            }

            if((NEAR_WEST_BOUNDARY > xb[0]) && (NEAR_EAST_BOUNDARY < xb[1])) 
            {
                xb[1] = -1.0;

                ++numProcessedBoundaryPoints;

                glBegin(GL_QUADS);

                glTexCoord1d(fb[0]);
                glVertex3d(xb[0], yb[0], height);

                glTexCoord1d(fb[1]);
                glVertex3d(xb[1], yb[1], height);

                glTexCoord1d(fb[2]);
                glVertex3d(xb[2], yb[2], height);

                glTexCoord1d(fb[3]);
                glVertex3d(xb[3], yb[3], height);

                glEnd();

                continue;
            }

            if((NEAR_EAST_BOUNDARY < xb[0]) && (NEAR_WEST_BOUNDARY > xb[1]))
            {
                xb[0] = -1.0;

                ++numProcessedBoundaryPoints;

                glBegin(GL_QUADS);

                glTexCoord1d(fb[0]);
                glVertex3d(xb[0], yb[0], height);

                glTexCoord1d(fb[1]);
                glVertex3d(xb[1], yb[1], height);

                glTexCoord1d(fb[2]);
                glVertex3d(xb[2], yb[2], height);

                glTexCoord1d(fb[3]);
                glVertex3d(xb[3], yb[3], height);

                glEnd();

                continue;
            }
        }

        if((NEAR_NEGATIVE_ZERO < xb[2]) && (NEAR_NEGATIVE_ZERO < xb[3]))
        {
            if((NEAR_WEST_BOUNDARY > xb[0]) && (NEAR_WEST_BOUNDARY > xb[1]))
            {
                xb[0] = 1.0;
                xb[1] = 1.0;

                ++numProcessedBoundaryPoints;

                glBegin(GL_QUADS);

                glTexCoord1d(fb[0]);
                glVertex3d(xb[0], yb[0], height);

                glTexCoord1d(fb[1]);
                glVertex3d(xb[1], yb[1], height);

                glTexCoord1d(fb[2]);
                glVertex3d(xb[2], yb[2], height);

                glTexCoord1d(fb[3]);
                glVertex3d(xb[3], yb[3], height);

                glEnd();

                continue;
            }

            if((NEAR_WEST_BOUNDARY > xb[0]) && (NEAR_EAST_BOUNDARY < xb[1]))
            {
                xb[0] = 1.0;

                ++numProcessedBoundaryPoints;

                glBegin(GL_QUADS);

                glTexCoord1d(fb[0]);
                glVertex3d(xb[0], yb[0], height);

                glTexCoord1d(fb[1]);
                glVertex3d(xb[1], yb[1], height);

                glTexCoord1d(fb[2]);
                glVertex3d(xb[2], yb[2], height);

                glTexCoord1d(fb[3]);
                glVertex3d(xb[3], yb[3], height);

                glEnd();

                continue;
            }

            if((NEAR_EAST_BOUNDARY < xb[0]) && (NEAR_WEST_BOUNDARY > xb[1]))
            {
                xb[1] = 1.0;

                ++numProcessedBoundaryPoints;

                glBegin(GL_QUADS);

                glTexCoord1d(fb[0]);
                glVertex3d(xb[0], yb[0], height);

                glTexCoord1d(fb[1]);
                glVertex3d(xb[1], yb[1], height);

                glTexCoord1d(fb[2]);
                glVertex3d(xb[2], yb[2], height);

                glTexCoord1d(fb[3]);
                glVertex3d(xb[3], yb[3], height);

                glEnd();

                continue;
            }
        }

        cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__
             << ", line: " << __LINE__ << endl;
        cout << "\tnum = " << num << endl;
        cout << "\tyb[1] = " << yb[1] << ", yb[2] = " << yb[2] << endl;
        cout << "\txb[1] = " << xb[1] << ", xb[2] = " << xb[2] << endl;
        cout << "\tyb[0] = " << yb[0] << ", yb[3] = " << yb[3] << endl;
        cout << "\txb[0] = " << xb[0] << ", xb[3] = " << xb[3] << endl;
    }

    if(numProcessedBoundaryPoints != _nBoundaryPoints)
    {
        cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
        cout << "\tnumProcessedBoundaryPoints = " << numProcessedBoundaryPoints
             << ", _nBoundaryPoints = " << _nBoundaryPoints << endl;
    }

    if(1 == lev)
        glDisable(GL_POLYGON_OFFSET_FILL);
    glDisable(GL_TEXTURE_1D);

    if(locator->on())
    {
        cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
        cout << "\tlocation = (" << locator->x() << ", " << locator->y() << ")" << endl;

      //_draw_cross(radius+ 0.01);
        if((NULL != nvoptions) && nvoptions->get_cb(NV_GRIDON))
            draw_plane_grids();
    }

    glPopMatrix();

    glEndList();
}

void CAMse2dViewer::_evaluate(double *var)
{
    int size;
    int n = 0;
    char vn[128];

    strcpy(vn, _varname.c_str());

    size = geometry->get_camse_ncol() * geometry->get_camse_lev();

    _valmax = var[0];
    _valmin = var[0];

    for(n = 1; n < size; ++n)
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

#if 0
    nvoptions->set_trueminimum(_valmin);
    nvoptions->set_truemaximum(_valmax);
#endif
}

void CAMse2dViewer::reset_texture1d(ColorTable *ct)
{
    colorTable = ct;

    glDisable(GL_TEXTURE_1D);

    texture1d->reset();
    texture1d->set_colors(ct->get_clen(), ct->get_cmap());
    texture1d->set_name(ct->get_name());
}

void CAMse2dViewer::draw_sphere_grids()
{
    int i, j, m, n;

    double radius = 1.001;

    GLfloat line_width = 1.0;

  //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\tncenters = " << ncenters << endl;

    _xSphere = geometry->get_xSphere();
    _ySphere = geometry->get_ySphere();
    _zSphere = geometry->get_zSphere();

    radius = 0.75 + 0.5 * ( 1.0 - (nvoptions->get_zsec() + 1.0) / lev);

    glPushMatrix();

  //earth->draw();

    glColor4f(0.0, 1.0, 1.0, 1.0);
  //glNormal3f(0.0, 0.0, -1.0);

    glLineWidth(line_width);

  //#pragma omp parallel for
    for(n = 0; n < ncenters; ++n)
    {
        j = n * ncorners;

        glBegin(GL_LINE_STRIP);
            for(i = 0; i < ncorners; ++i)
            {
                m = element_corners[j+i];
                glVertex3d(radius * _xSphere[m], radius * _ySphere[m], radius * _zSphere[m]);
            }

            m = element_corners[j];
            glVertex3d(radius * _xSphere[m], radius * _ySphere[m], radius * _zSphere[m]);
        glEnd();
    }
    glEnd();

    glPopMatrix();
}

void CAMse2dViewer::draw_plane_grids()
{
    int i, j, m, n, num;

    int numProcessedBoundaryPoints = 0;

    double height = 0.01;

    double xb[ncorners];
    double yb[ncorners];

    GLfloat line_width = 1.0;

  //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\tncenters = " << ncenters << endl;

    height = 0.8 * (0.5 - (nvoptions->get_zsec() + 1.0) / lev);

    glPushMatrix();

  //earth->draw_plane(0.0);

    glColor4f(1.0, 1.0, 0.0, 1.0);
  //glColor4f(0.0, 0.0, 0.0, 0.0);
    glNormal3f(0.0, 0.0, -1.0);

    glLineWidth(line_width);

    for(n = 0; n < ncenters; ++n)
    {
        if(_innerPoints[n])
        {
            j = n * ncorners;

            glBegin(GL_LINE_STRIP);
                for(i = 0; i < ncorners; ++i)
                {
                    m = element_corners[j+i];
                    glVertex3d(_xFlat[m], _yFlat[m], height);
                }

                m = element_corners[j];
                glVertex3d(_xFlat[m], _yFlat[m], height);
            glEnd();
        }
    }

  //handle South Pole
    for(num = 0; num < num_south_pole_cols; ++num)
    {
        n = south_pole_cols[num];

        j = n * ncorners;

        for(i = 0; i < ncorners; ++i)
        {
            m = element_corners[j+i];
            xb[i] = _xFlat[m];
            yb[i] = _yFlat[m];
        }

      //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__
      //     << ", line: " << __LINE__ << endl;
      //cout << "\txb[0] = " << xb[0] << ", xb[1] = " << xb[1] << ", xb[2] = " << xb[2] << ", xb[3] = " << xb[3] << endl;
      //cout << "\tyb[0] = " << yb[0] << ", yb[1] = " << yb[1] << ", yb[2] = " << yb[2] << ", yb[3] = " << yb[3] << endl;

        if(NEAR_SOUTH_POLE > yb[0])
        {
          //Point 0 at South pole
          //Make 2 quads: 0, 1, 0, 2 and 0, 2, 0, 3.

            ++numProcessedBoundaryPoints;

            glBegin(GL_LINE_STRIP);
                glVertex3d(xb[1], yb[0], height);
                glVertex3d(xb[1], yb[1], height);
                glVertex3d(xb[2], yb[2], height);
                glVertex3d(xb[2], yb[0], height);
                glVertex3d(xb[1], yb[0], height);
            glEnd();

            glBegin(GL_LINE_STRIP);
                glVertex3d(xb[2], yb[0], height);
                glVertex3d(xb[2], yb[2], height);
                glVertex3d(xb[3], yb[3], height);
                glVertex3d(xb[3], yb[0], height);
                glVertex3d(xb[2], yb[0], height);
            glEnd();

            continue;
        }
        else if(NEAR_SOUTH_POLE > yb[1])
        {
          //Point 1 at South pole
          //Make 2 quads: 1, 2, 3, 1 and 1, 3, 0, 1.

            if((NEAR_NEGATIVE_ZERO < xb[2]) && (NEAR_NEGATIVE_ZERO < xb[3]) && (NEAR_WEST_BOUNDARY > xb[0]))
                xb[0] = 1.0;

            ++numProcessedBoundaryPoints;

            glBegin(GL_LINE_STRIP);
                glVertex3d(xb[2], yb[1], height);
                glVertex3d(xb[2], yb[2], height);
                glVertex3d(xb[3], yb[3], height);
                glVertex3d(xb[3], yb[1], height);
                glVertex3d(xb[2], yb[1], height);
            glEnd();

            glBegin(GL_LINE_STRIP);
                glVertex3d(xb[3], yb[1], height);
                glVertex3d(xb[3], yb[3], height);
                glVertex3d(xb[0], yb[0], height);
                glVertex3d(xb[0], yb[1], height);
                glVertex3d(xb[3], yb[1], height);
            glEnd();

            continue;
        }
        else if(NEAR_SOUTH_POLE > yb[2])
        {
          //Point 2 at South pole
          //Make 2 quads: 2, 3, 0, 2 and 2, 0, 1, 2.

            ++numProcessedBoundaryPoints;

            glBegin(GL_LINE_STRIP);
                glVertex3d(xb[3], yb[2], height);
                glVertex3d(xb[3], yb[3], height);
                glVertex3d(xb[0], yb[0], height);
                glVertex3d(xb[0], yb[2], height);
                glVertex3d(xb[3], yb[2], height);
            glEnd();

            glBegin(GL_LINE_STRIP);
                glVertex3d(xb[0], yb[2], height);
                glVertex3d(xb[0], yb[0], height);
                glVertex3d(xb[1], yb[1], height);
                glVertex3d(xb[1], yb[2], height);
                glVertex3d(xb[0], yb[2], height);
            glEnd();

            continue;
        }
        else if(NEAR_SOUTH_POLE > yb[3])
        {
          //Point 3 at South pole
          //Make 2 quads: 3, 0, 1, 3 and 3, 1, 2, 3.

            ++numProcessedBoundaryPoints;

            glBegin(GL_LINE_STRIP);
                glVertex3d(xb[0], yb[3], height);
                glVertex3d(xb[0], yb[0], height);
                glVertex3d(xb[1], yb[1], height);
                glVertex3d(xb[1], yb[3], height);
                glVertex3d(xb[0], yb[3], height);
            glEnd();

            glBegin(GL_LINE_STRIP);
                glVertex3d(xb[1], yb[3], height);
                glVertex3d(xb[1], yb[1], height);
                glVertex3d(xb[2], yb[2], height);
                glVertex3d(xb[2], yb[3], height);
                glVertex3d(xb[1], yb[3], height);
            glEnd();

            continue;
        }
        else
        {
            cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__
                 << ", line: " << __LINE__ << endl;
            cout << "\txb[0] = " << xb[0] << ", xb[1] = " << xb[1] << ", xb[2] = " << xb[2] << ", xb[3] = " << xb[3] << endl;
            cout << "\tyb[0] = " << yb[0] << ", yb[1] = " << yb[1] << ", yb[2] = " << yb[2] << ", yb[3] = " << yb[3] << endl;
        }
    }

  //Handle North Pole
    for(num = 0; num < num_north_pole_cols; ++num)
    {
        n = north_pole_cols[num];

        j = n * ncorners;

        for(i = 0; i < ncorners; ++i)
        {
            m = element_corners[j+i];
            xb[i] = _xFlat[m];
            yb[i] = _yFlat[m];
        }

      //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__
      //     << ", line: " << __LINE__ << endl;
      //cout << "\txb[0] = " << xb[0] << ", xb[1] = " << xb[1] << ", xb[2] = " << xb[2] << ", xb[3] = " << xb[3] << endl;
      //cout << "\tyb[0] = " << yb[0] << ", yb[1] = " << yb[1] << ", yb[2] = " << yb[2] << ", yb[3] = " << yb[3] << endl;

        if(NEAR_NORTH_POLE < yb[0])
        {
          //Point 0 is at North Pole.
          //Make 2 quads: 0, 1, 0, 2 and 0, 2, 0, 3.

            if((NEAR_NEGATIVE_ZERO < xb[2]) && (NEAR_NEGATIVE_ZERO < xb[3]) && (NEAR_WEST_BOUNDARY > xb[1]))
                xb[1] = 1.0;

            ++numProcessedBoundaryPoints;

            glBegin(GL_LINE_STRIP);
                glVertex3d(xb[1], yb[0], height);
                glVertex3d(xb[1], yb[1], height);
                glVertex3d(xb[2], yb[2], height);
                glVertex3d(xb[2], yb[0], height);
                glVertex3d(xb[1], yb[0], height);
            glEnd();

            glBegin(GL_LINE_STRIP);
                glVertex3d(xb[2], yb[0], height);
                glVertex3d(xb[2], yb[2], height);
                glVertex3d(xb[3], yb[3], height);
                glVertex3d(xb[3], yb[0], height);
                glVertex3d(xb[2], yb[0], height);
            glEnd();

            continue;
        }
        else if(NEAR_NORTH_POLE < yb[1])
        {
          //Point 1 is at North Pole.
          //Make 2 quads: 1, 0, 1, 2 and 1, 2, 1, 3.

            ++numProcessedBoundaryPoints;

            glBegin(GL_LINE_STRIP);
                glVertex3d(xb[2], yb[1], height);
                glVertex3d(xb[2], yb[2], height);
                glVertex3d(xb[3], yb[3], height);
                glVertex3d(xb[3], yb[1], height);
                glVertex3d(xb[2], yb[1], height);
            glEnd();

            glBegin(GL_LINE_STRIP);
                glVertex3d(xb[3], yb[1], height);
                glVertex3d(xb[3], yb[3], height);
                glVertex3d(xb[0], yb[0], height);
                glVertex3d(xb[0], yb[1], height);
                glVertex3d(xb[3], yb[1], height);
            glEnd();

            continue;
        }
        else if(NEAR_NORTH_POLE < yb[2])
        {
          //Point 2 is at North Pole.
          //Make 2 quads: 2, 0, 2, 1 and 2, 1, 2, 3. 

            ++numProcessedBoundaryPoints;

            glBegin(GL_LINE_STRIP);
                glVertex3d(xb[3], yb[2], height);
                glVertex3d(xb[3], yb[3], height);
                glVertex3d(xb[0], yb[0], height);
                glVertex3d(xb[0], yb[2], height);
                glVertex3d(xb[3], yb[2], height);
            glEnd();

            glBegin(GL_LINE_STRIP);
                glVertex3d(xb[0], yb[2], height);
                glVertex3d(xb[0], yb[0], height);
                glVertex3d(xb[1], yb[1], height);
                glVertex3d(xb[1], yb[2], height);
                glVertex3d(xb[0], yb[2], height);
            glEnd();

            continue;
        }
        else if(NEAR_NORTH_POLE < yb[3])
        {
          //Point 3 is at North Pole.
          //Make 2 quads: 3, 0, 3, 1 and 3, 1, 3, 2.

            ++numProcessedBoundaryPoints;

            glBegin(GL_LINE_STRIP);
                glVertex3d(xb[0], yb[3], height);
                glVertex3d(xb[0], yb[0], height);
                glVertex3d(xb[1], yb[1], height);
                glVertex3d(xb[1], yb[3], height);
                glVertex3d(xb[0], yb[3], height);
            glEnd();

            glBegin(GL_LINE_STRIP);
                glVertex3d(xb[1], yb[3], height);
                glVertex3d(xb[1], yb[1], height);
                glVertex3d(xb[2], yb[2], height);
                glVertex3d(xb[2], yb[3], height);
                glVertex3d(xb[1], yb[3], height);
            glEnd();

            continue;
        }
        else
        {
            cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__
                 << ", line: " << __LINE__ << endl;
            cout << "\txb[0] = " << xb[0] << ", xb[1] = " << xb[1] << ", xb[2] = " << xb[2] << ", xb[3] = " << xb[3] << endl;
            cout << "\tyb[0] = " << yb[0] << ", yb[1] = " << yb[1] << ", yb[2] = " << yb[2] << ", yb[3] = " << yb[3] << endl;
        }
    }

  //The middle boundary
    for(num = 0; num < num_boundary_cols; ++num)
    {
        n = boundary_cols[num];

        j = n * ncorners;

        for(i = 0; i < ncorners; ++i)
        {
            m = element_corners[j+i];
            xb[i] = _xFlat[m];
            yb[i] = _yFlat[m];
        }

        if((NEAR_POSITIVE_ZERO > xb[0]) && (NEAR_POSITIVE_ZERO > xb[1]))
        {
            if((NEAR_EAST_BOUNDARY < xb[2]) && (NEAR_EAST_BOUNDARY < xb[3]))
            {
                xb[2] = -1.0;
                xb[3] = -1.0;

                ++numProcessedBoundaryPoints;

                glBegin(GL_LINE_STRIP);
                    glVertex3d(xb[0], yb[0], height);
                    glVertex3d(xb[1], yb[1], height);
                    glVertex3d(xb[2], yb[2], height);
                    glVertex3d(xb[3], yb[3], height);
                    glVertex3d(xb[0], yb[0], height);
                glEnd();

                continue;
            }

            if((NEAR_EAST_BOUNDARY < xb[2]) && (NEAR_WEST_BOUNDARY > xb[3])) 
            {
                xb[2] = -1.0;

                ++numProcessedBoundaryPoints;

                glBegin(GL_LINE_STRIP);
                    glVertex3d(xb[0], yb[0], height);
                    glVertex3d(xb[1], yb[1], height);
                    glVertex3d(xb[2], yb[2], height);
                    glVertex3d(xb[3], yb[3], height);
                    glVertex3d(xb[0], yb[0], height);
                glEnd();

                continue;
            }

            if((NEAR_WEST_BOUNDARY > xb[2]) && (NEAR_EAST_BOUNDARY < xb[3]))
            {
                xb[3] = -1.0;

                ++numProcessedBoundaryPoints;

                glBegin(GL_LINE_STRIP);
                    glVertex3d(xb[0], yb[0], height);
                    glVertex3d(xb[1], yb[1], height);
                    glVertex3d(xb[2], yb[2], height);
                    glVertex3d(xb[3], yb[3], height);
                    glVertex3d(xb[0], yb[0], height);
                glEnd();

                continue;
            }
        }

        if((NEAR_POSITIVE_ZERO > xb[2]) && (NEAR_POSITIVE_ZERO > xb[3]))
        {
            if((NEAR_EAST_BOUNDARY < xb[0]) && (NEAR_EAST_BOUNDARY < xb[1])) 
            {
                xb[0] = -1.0;
                xb[1] = -1.0;

                ++numProcessedBoundaryPoints;

                glBegin(GL_LINE_STRIP);
                    glVertex3d(xb[0], yb[0], height);
                    glVertex3d(xb[1], yb[1], height);
                    glVertex3d(xb[2], yb[2], height);
                    glVertex3d(xb[3], yb[3], height);
                    glVertex3d(xb[0], yb[0], height);
                glEnd();

                continue;
            }

            if((NEAR_WEST_BOUNDARY > xb[0]) && (NEAR_EAST_BOUNDARY < xb[1])) 
            {
                xb[1] = -1.0;

                ++numProcessedBoundaryPoints;

                glBegin(GL_LINE_STRIP);
                    glVertex3d(xb[0], yb[0], height);
                    glVertex3d(xb[1], yb[1], height);
                    glVertex3d(xb[2], yb[2], height);
                    glVertex3d(xb[3], yb[3], height);
                    glVertex3d(xb[0], yb[0], height);
                glEnd();

                continue;
            }

            if((NEAR_EAST_BOUNDARY < xb[0]) && (NEAR_WEST_BOUNDARY > xb[1]))
            {
                xb[0] = -1.0;

                ++numProcessedBoundaryPoints;

                glBegin(GL_LINE_STRIP);
                    glVertex3d(xb[0], yb[0], height);
                    glVertex3d(xb[1], yb[1], height);
                    glVertex3d(xb[2], yb[2], height);
                    glVertex3d(xb[3], yb[3], height);
                    glVertex3d(xb[0], yb[0], height);
                glEnd();

                continue;
            }
        }

        if((NEAR_NEGATIVE_ZERO < xb[2]) && (NEAR_NEGATIVE_ZERO < xb[3]))
        {
            if((NEAR_WEST_BOUNDARY > xb[0]) && (NEAR_WEST_BOUNDARY > xb[1]))
            {
                xb[0] = 1.0;
                xb[1] = 1.0;

                ++numProcessedBoundaryPoints;

                glBegin(GL_LINE_STRIP);
                    glVertex3d(xb[0], yb[0], height);
                    glVertex3d(xb[1], yb[1], height);
                    glVertex3d(xb[2], yb[2], height);
                    glVertex3d(xb[3], yb[3], height);
                    glVertex3d(xb[0], yb[0], height);
                glEnd();

                continue;
            }

            if((NEAR_WEST_BOUNDARY > xb[0]) && (NEAR_EAST_BOUNDARY < xb[1]))
            {
                xb[0] = 1.0;

                ++numProcessedBoundaryPoints;

                glBegin(GL_LINE_STRIP);
                    glVertex3d(xb[0], yb[0], height);
                    glVertex3d(xb[1], yb[1], height);
                    glVertex3d(xb[2], yb[2], height);
                    glVertex3d(xb[3], yb[3], height);
                    glVertex3d(xb[0], yb[0], height);
                glEnd();

                continue;
            }

            if((NEAR_EAST_BOUNDARY < xb[0]) && (NEAR_WEST_BOUNDARY > xb[1]))
            {
                xb[1] = 1.0;

                ++numProcessedBoundaryPoints;

                glBegin(GL_LINE_STRIP);
                    glVertex3d(xb[0], yb[0], height);
                    glVertex3d(xb[1], yb[1], height);
                    glVertex3d(xb[2], yb[2], height);
                    glVertex3d(xb[3], yb[3], height);
                    glVertex3d(xb[0], yb[0], height);
                glEnd();

                continue;
            }
        }

        cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__
             << ", line: " << __LINE__ << endl;
        cout << "\tnum = " << num << endl;
        cout << "\tyb[1] = " << yb[1] << ", yb[2] = " << yb[2] << endl;
        cout << "\txb[1] = " << xb[1] << ", xb[2] = " << xb[2] << endl;
        cout << "\tyb[0] = " << yb[0] << ", yb[3] = " << yb[3] << endl;
        cout << "\txb[0] = " << xb[0] << ", xb[3] = " << xb[3] << endl;
    }

    if(numProcessedBoundaryPoints != _nBoundaryPoints)
    {
        cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
        cout << "\tnumProcessedBoundaryPoints = " << numProcessedBoundaryPoints
             << ", _nBoundaryPoints = " << _nBoundaryPoints << endl;
    }

    glPopMatrix();
}

void CAMse2dViewer::_display_Yflat_plane(int ys)
{
    int i, j, k, m, n;

    double latc = (double) ys;
    double yc = latc / 180.0;
    double sv = 1.0;
    double height[lev];

    bool all_above;
    bool all_below;

    GLfloat line_width = 1.0;

    int numBoundaryPoints = 0;
    int numProcessedPoints = 0;

    if((-80.0 > latc) || (80.0 < latc))
       return;

  //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\t_varname: <" << _varname << ">" << endl;

    for(k = 0; k < lev; ++k)
        height[k] = 0.8 * (0.5 - ((float) k + 1.0) / lev);

    sv = 1.0 / (_valmax - _valmin);

    ycl = glGenLists(1);
  //glNewList(ycl, GL_COMPILE);
    glNewList(ycl, GL_COMPILE_AND_EXECUTE);
    lister->set_yid(ys+90, ycl);

  //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\t_valmin = " << _valmin << ", _valmax = " << _valmax << ", sv = " << sv << endl;
  //cout << "\tycl = " << ycl << endl;
  //cout << "\tys = " << ys << ", latc = " << latc << ", yc = " << yc << endl;

    glPushMatrix();

    glColor4f(1.0, 1.0, 1.0, 1.0);
  //glColor4f(0.0, 0.0, 0.0, 0.0);

    glEnable(GL_TEXTURE_1D);
    glBindTexture(GL_TEXTURE_1D, texture1d->get_textureID());

    for(n = 0; n < ncenters; ++n)
    {
        all_above = true;
        all_below = true;

        j = n * ncorners;

        for(i = 0; i < ncorners; ++i)
        {
            m = element_corners[j+i];
            if(latc > lat[m])
                all_below = false;
            if(latc < lat[m])
                all_above = false;
        }

        if(all_above || all_below)
            continue;

        ++numBoundaryPoints;

        handle_selectedYflat_quad(latc, j, sv, height, numProcessedPoints);
    }

    glDisable(GL_TEXTURE_1D);

    line_width = 2.0;
    glLineWidth(line_width);

    glColor3f(1.0, 1.0, 0.0);

    glBegin(GL_LINE_STRIP);
        glVertex3d(-1.0, yc, height[0]);
        glVertex3d( 1.0, yc, height[0]);
        glVertex3d( 1.0, yc, height[lev-1]);
        glVertex3d(-1.0, yc, height[lev-1]);
        glVertex3d(-1.0, yc, height[0]);
    glEnd();

    glPopMatrix();

    glEndList();

    if(numProcessedPoints != numBoundaryPoints)
    {
        cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__
             << ", line: " << __LINE__ << endl;
        cout << "\nProcessed " << numProcessedPoints << " of " << numBoundaryPoints << endl;
    }
}

double CAMse2dViewer::linInterp(double xc, double x0, double x1,
                                double y0, double y1, double &dx)
{
    double xv, dv, y;

    dv = x1 - x0;
    xv = xc - x0;

    if(1.0e-10 < fabs(dv))
        dx = xv/dv;
    else
        dx = 0.5;

    y = (1.0 - dx) * y0 + dx * y1;

    return y;
}

void CAMse2dViewer::handle_selectedXsphere_quad(double clon, double* ylon,
                                                int nc, double sv,
                                                double* radius,
                                                int& numProcessedPoints)
{
    int i, m;

    double xlon[ncorners];
    double ylat[ncorners];

    int mc[ncorners];

    bool isAtPole = false;

    for(i = 0; i < ncorners; ++i)
    {
        m = element_corners[nc+i];
        mc[i] = element_corners[nc+i];
        ylat[i] = lat[m];
        xlon[i] = ylon[i];

        if((ylat[i] < -89.99) || (ylat[i] > 89.99))
            isAtPole = true;

        if(clon < 180.0)
        {
            if(xlon[i] > 359.9)
               xlon[i] -= 360.0;
        }

        if(clon > 180.0)
        {
            if(xlon[i] < 70.0)
               xlon[i] += 360.0;
        }
    }

    if(isAtPole)
    {
        if((ylat[0] < -89.99) || (ylat[0] > 89.99))
        {
            if(xlon[1] > 359.99) 
            {
                if((xlon[2] > 360.0) && (xlon[3] < 181.0))
                    xlon[2] -= 360.0;

                if((xlon[2] < 60.0) || (xlon[3] < 60.0))
                    xlon[1] = 0.0;
            }

            if((xlon[1] < clon) && (xlon[2] < clon) && (xlon[3] < clon))
            {
                ++numProcessedPoints;
                return;
            }

            if((xlon[1] > clon) && (xlon[2] > clon) && (xlon[3] > clon))
            {
                ++numProcessedPoints;
                return;
            }
               
            if((xlon[1] <= clon) && (xlon[2] >= clon))
            {
                process_selectedXsphere_quad(clon, sv,
                                             0, 0, 1, 2,
                                             xlon, ylat, radius, mc);

                ++numProcessedPoints;
                return;
            }

            if((xlon[1] >= clon) && (xlon[2] <= clon))
            {
                process_selectedXsphere_quad(clon, sv,
                                             0, 0, 2, 1,
                                             xlon, ylat, radius, mc);

                ++numProcessedPoints;
                return;
            }

            if((xlon[2] <= clon) && (xlon[3] >= clon))
            {
                process_selectedXsphere_quad(clon, sv,
                                             0, 0, 2, 3,
                                             xlon, ylat, radius, mc);

                ++numProcessedPoints;
                return;
            }

            if((xlon[3] <= clon) && (xlon[2] >= clon))
            {
                process_selectedXsphere_quad(clon, sv,
                                             0, 0, 3, 2,
                                             xlon, ylat, radius, mc);

                ++numProcessedPoints;
                return;
            }
        }

        if((ylat[1] < -89.99) || (ylat[1] > 89.99))
        {
            if(xlon[0] > 359.99)
            {
                if((xlon[3] > 360.0) && (xlon[2] < 181.0))
                    xlon[3] -= 360.0;

                if((xlon[2] < 60.0) || (xlon[3] < 60.0))
                    xlon[0] = 0.0;
            }

            if((xlon[0] < clon) && (xlon[2] < clon) && (xlon[3] < clon))
            {
                ++numProcessedPoints;
                return;
            }

            if((xlon[0] > clon) && (xlon[2] > clon) && (xlon[3] > clon))
            {
                ++numProcessedPoints;
                return;
            }
               
            if((xlon[0] <= clon) && (xlon[3] >= clon))
            {
                process_selectedXsphere_quad(clon, sv,
                                             1, 1, 0, 3,
                                             xlon, ylat, radius, mc);

                ++numProcessedPoints;
                return;
            }
               
            if((xlon[0] >= clon) && (xlon[3] <= clon))
            {
                process_selectedXsphere_quad(clon, sv,
                                             1, 1, 3, 0,
                                             xlon, ylat, radius, mc);

                ++numProcessedPoints;
                return;
            }

            if((xlon[2] >= clon) && (xlon[3] <= clon))
            {
                process_selectedXsphere_quad(clon, sv,
                                             1, 1, 3, 2,
                                             xlon, ylat, radius, mc);

                ++numProcessedPoints;
                return;
            }

            if((xlon[2] <= clon) && (xlon[3] >= clon))
            {
                process_selectedXsphere_quad(clon, sv,
                                             1, 1, 2, 3,
                                             xlon, ylat, radius, mc);

                ++numProcessedPoints;
                return;
            }
        }

        if((ylat[2] < -89.99) || (ylat[2] > 89.99))
        {
            if((xlon[0] < clon) && (xlon[1] < clon) && (xlon[3] < clon))
            {
                ++numProcessedPoints;
                return;
            }

            if((xlon[0] > clon) && (xlon[1] > clon) && (xlon[3] > clon))
            {
                ++numProcessedPoints;
                return;
            }

            if((xlon[0] <= clon) && (xlon[1] >= clon))
            {
                process_selectedXsphere_quad(clon, sv,
                                             2, 2, 0, 1,
                                             xlon, ylat, radius, mc);

                ++numProcessedPoints;
                return;
            }

            if((xlon[0] >= clon) && (xlon[1] <= clon))
            {
                process_selectedXsphere_quad(clon, sv,
                                             2, 2, 1, 0,
                                             xlon, ylat, radius, mc);

                ++numProcessedPoints;
                return;
            }

            if((xlon[0] <= clon) && (xlon[3] >= clon))
            {
                process_selectedXsphere_quad(clon, sv,
                                             2, 2, 0, 3,
                                             xlon, ylat, radius, mc);

                ++numProcessedPoints;
                return;
            }

            if((xlon[0] >= clon) && (xlon[3] <= clon))
            {
                process_selectedXsphere_quad(clon, sv,
                                             2, 2, 3, 0,
                                             xlon, ylat, radius, mc);

                ++numProcessedPoints;
                return;
            }
        }

        if((ylat[3] < -89.99) || (ylat[3] > 89.99))
        {
            if((xlon[0] < clon) && (xlon[1] < clon) && (xlon[2] < clon))
            {
                ++numProcessedPoints;
                return;
            }

            if((xlon[0] > clon) && (xlon[1] > clon) && (xlon[2] > clon))
            {
                ++numProcessedPoints;
                return;
            }

            if((xlon[0] <= clon) && (xlon[1] >= clon))
            {
                process_selectedXsphere_quad(clon, sv,
                                             3, 3, 0, 1,
                                             xlon, ylat, radius, mc);

                ++numProcessedPoints;
                return;
            }

            if((xlon[0] >= clon) && (xlon[1] <= clon))
            {
                process_selectedXsphere_quad(clon, sv,
                                             3, 3, 1, 0,
                                             xlon, ylat, radius, mc);

                ++numProcessedPoints;
                return;
            }

            if((xlon[1] <= clon) && (xlon[2] >= clon))
            {
                process_selectedXsphere_quad(clon, sv,
                                             3, 3, 1, 2,
                                             xlon, ylat, radius, mc);

                ++numProcessedPoints;
                return;
            }

            if((xlon[1] >= clon) && (xlon[2] <= clon))
            {
                process_selectedXsphere_quad(clon, sv,
                                             3, 3, 2, 1,
                                             xlon, ylat, radius, mc);

                ++numProcessedPoints;
                return;
            }
        }

        cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
        cout << "\tclon = " << clon << endl;
        cout << "\tylat[1] = " << ylat[1] << ", ylat[2] = " << ylat[2] << endl;
        cout << "\tylat[0] = " << ylat[0] << ", ylat[3] = " << ylat[3] << endl << endl;
        cout << "\txlon[1] = " << xlon[1] << ", xlon[2] = " << xlon[2] << endl;
        cout << "\txlon[0] = " << xlon[0] << ", xlon[3] = " << xlon[3] << endl;
    }

#if 0
    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
    cout << "\tclon = " << clon << endl;
    cout << "\txlon[1] = " << xlon[1] << ", xlon[2] = " << xlon[2] << endl;
    cout << "\txlon[0] = " << xlon[0] << ", xlon[3] = " << xlon[3] << endl << endl;
    cout << "\tylat[1] = " << ylat[1] << ", ylat[2] = " << ylat[2] << endl;
    cout << "\tylat[0] = " << ylat[0] << ", ylat[3] = " << ylat[3] << endl;
#endif

    if(xlon[0] <= clon)
    {
        if(xlon[1] <= clon)
        {
            if(xlon[2] <= clon)
            {
                if(xlon[3] <= clon)
                {
                    if(xlon[3] == clon)
                    {
                        if(xlon[2] == clon)
                        {
                            process_selectedXsphere_quad(clon, sv,
                                                         1, 2, 0, 3,
                                                         xlon, ylat, radius, mc);
                        }
                        else
                        {
                            process_selectedXsphere_quad(clon, sv,
                                                         0, 3, 2, 3,
                                                         xlon, ylat, radius, mc);
                        }

                        ++numProcessedPoints;
                        return;
                    }

                    if(xlon[2] == clon)
                    {
                        if(xlon[1] == clon)
                        {
                            process_selectedXsphere_quad(clon, sv,
                                                         0, 1, 3, 2,
                                                         xlon, ylat, radius, mc);
                        }
                        else
                        {
                            process_selectedXsphere_quad(clon, sv,
                                                         1, 2, 3, 2,
                                                         xlon, ylat, radius, mc);
                        }

                        ++numProcessedPoints;
                        return;
                    }

                    if(xlon[1] == clon)
                    {
                        if(xlon[0] == clon)
                        {
                            process_selectedXsphere_quad(clon, sv,
                                                         3, 0, 2, 1,
                                                         xlon, ylat, radius, mc);
                        }
                        else
                        {
                            process_selectedXsphere_quad(clon, sv,
                                                         0, 1, 2, 1,
                                                         xlon, ylat, radius, mc);
                        }
                        ++numProcessedPoints;
                        return;
                    }

                    if(xlon[0] == clon)
                    {
                        if(xlon[3] == clon)
                        {
                            process_selectedXsphere_quad(clon, sv,
                                                         1, 0, 2, 3,
                                                         xlon, ylat, radius, mc);
                        }
                        else
                        {
                            process_selectedXsphere_quad(clon, sv,
                                                         1, 0, 3, 0,
                                                         xlon, ylat, radius, mc);
                        }

                        ++numProcessedPoints;
                        return;
                    }

                  //If it reaches here, just ignore it.
                    ++numProcessedPoints;
                    return;

                    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
                    cout << "\tclon = " << clon << endl;
                    cout << "\tylat[1] = " << ylat[1] << ", ylat[2] = " << ylat[2] << endl;
                    cout << "\txlon[1] = " << xlon[1] << ", xlon[2] = " << xlon[2] << endl;
                    cout << "\tylat[0] = " << ylat[0] << ", ylat[3] = " << ylat[3] << endl;
                    cout << "\txlon[0] = " << xlon[0] << ", xlon[3] = " << xlon[3] << endl;
                }
                else
                {
                    process_selectedXsphere_quad(clon, sv,
                                                 0, 3, 2, 3,
                                                 xlon, ylat, radius, mc);

                    ++numProcessedPoints;
                    return;

                    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
                    cout << "\tclon = " << clon << endl;
                    cout << "\tylat[1] = " << ylat[1] << ", ylat[2] = " << ylat[2] << endl;
                    cout << "\tylat[0] = " << ylat[0] << ", ylat[3] = " << ylat[3] << endl;
                    cout << "\txlon[1] = " << xlon[1] << ", xlon[2] = " << xlon[2] << endl;
                    cout << "\txlon[0] = " << xlon[0] << ", xlon[3] = " << xlon[3] << endl;
                }
            }
            else
            {
                if(xlon[3] <= clon)
                {
                    process_selectedXsphere_quad(clon, sv,
                                                 1, 2, 3, 2,
                                                 xlon, ylat, radius, mc);

                    ++numProcessedPoints;
                    return;

                    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
                    cout << "\tclon = " << clon << endl;
                    cout << "\tylat[1] = " << ylat[1] << ", ylat[2] = " << ylat[2] << endl;
                    cout << "\tylat[0] = " << ylat[0] << ", ylat[3] = " << ylat[3] << endl;
                    cout << "\txlon[1] = " << xlon[1] << ", xlon[2] = " << xlon[2] << endl;
                    cout << "\txlon[0] = " << xlon[0] << ", xlon[3] = " << xlon[3] << endl;
                }
                else
                {
                    process_selectedXsphere_quad(clon, sv,
                                                 0, 3, 1, 2,
                                                 xlon, ylat, radius, mc);

                    ++numProcessedPoints;
                    return;

                    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
                    cout << "\tclon = " << clon << endl;
                    cout << "\tylat[1] = " << ylat[1] << ", ylat[2] = " << ylat[2] << endl;
                    cout << "\tylat[0] = " << ylat[0] << ", ylat[3] = " << ylat[3] << endl;
                    cout << "\txlon[1] = " << xlon[1] << ", xlon[2] = " << xlon[2] << endl;
                    cout << "\txlon[0] = " << xlon[0] << ", xlon[3] = " << xlon[3] << endl;
                }
            }
        }
        else
        {
            if(xlon[2] <= clon)
            {
                if(xlon[3] <= clon)
                {
                    if(xlon[1] > clon)
                    {
                        if((xlon[0] < 0.001) && (xlon[2] < 0.001) && (xlon[3] < 0.001))
                        {
                            ++numProcessedPoints;
                            return;
                        }
                    }

                    if(xlon[1] > 360.0)
                    {
                        if((xlon[0] <= clon) && (xlon[2] <= clon) && (xlon[3] <= clon))
                        {
                            ++numProcessedPoints;
                            return;
                        }
                    }

                    process_selectedXsphere_quad(clon, sv,
                                                 0, 1, 2, 1,
                                                 xlon, ylat, radius, mc);
    
                    ++numProcessedPoints;
                    return;

                    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
                    cout << "\tclon = " << clon << endl;
                    cout << "\tylat[1] = " << ylat[1] << ", ylat[2] = " << ylat[2] << endl;
                    cout << "\tylat[0] = " << ylat[0] << ", ylat[3] = " << ylat[3] << endl;
                    cout << "\txlon[1] = " << xlon[1] << ", xlon[2] = " << xlon[2] << endl;
                    cout << "\txlon[0] = " << xlon[0] << ", xlon[3] = " << xlon[3] << endl;
                }
                else
                {
                    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
                    cout << "\tclon = " << clon << endl;
                    cout << "\tylat[1] = " << ylat[1] << ", ylat[2] = " << ylat[2] << endl;
                    cout << "\txlon[1] = " << xlon[1] << ", xlon[2] = " << xlon[2] << endl;
                    cout << "\tylat[0] = " << ylat[0] << ", ylat[3] = " << ylat[3] << endl;
                    cout << "\txlon[0] = " << xlon[0] << ", xlon[3] = " << xlon[3] << endl;
                }
            }
            else
            {
                if(xlon[3] <= clon)
                {
                    if((xlon[0] < -1.0) && (xlon[1] > 300.0) &&
                       (xlon[3] < -1.0) && (xlon[2] > 300.0))
                    {
                        if(clon < 300.0)
                        {
                            ++numProcessedPoints;
                            return;
                        }
                    }

                    process_selectedXsphere_quad(clon, sv,
                                                 0, 1, 3, 2,
                                                 xlon, ylat, radius, mc);

                    ++numProcessedPoints;
                    return;

                    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
                    cout << "\tclon = " << clon << endl;
                    cout << "\tylat[1] = " << ylat[1] << ", ylat[2] = " << ylat[2] << endl;
                    cout << "\tylat[0] = " << ylat[0] << ", ylat[3] = " << ylat[3] << endl;
                    cout << "\txlon[1] = " << xlon[1] << ", xlon[2] = " << xlon[2] << endl;
                    cout << "\txlon[0] = " << xlon[0] << ", xlon[3] = " << xlon[3] << endl;
                }
                else
                {
                    process_selectedXsphere_quad(clon, sv,
                                                 0, 1, 0, 3,
                                                 xlon, ylat, radius, mc);

                    ++numProcessedPoints;
                    return;

                    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
                    cout << "\tclon = " << clon << endl;
                    cout << "\tylat[1] = " << ylat[1] << ", ylat[2] = " << ylat[2] << endl;
                    cout << "\txlon[1] = " << xlon[1] << ", xlon[2] = " << xlon[2] << endl;
                    cout << "\tylat[0] = " << ylat[0] << ", ylat[3] = " << ylat[3] << endl;
                    cout << "\txlon[0] = " << xlon[0] << ", xlon[3] = " << xlon[3] << endl;
                }
            }
        }
    }
    else
    {
        if(xlon[1] <= clon)
        {
            if(xlon[2] <= clon)
            {
                if(xlon[3] <= clon)
                {
                    if(xlon[0] > clon)
                    {
                        if((xlon[1] < 0.001) && (xlon[2] < 0.001) && (xlon[3] < 0.001))
                        {
                            ++numProcessedPoints;
                            return;
                        }
                    }

                    process_selectedXsphere_quad(clon, sv,
                                                 1, 0, 3, 0,
                                                 xlon, ylat, radius, mc);

                    ++numProcessedPoints;
                    return;

                    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
                    cout << "\tclon = " << clon << endl;
                    cout << "\tylat[1] = " << ylat[1] << ", ylat[2] = " << ylat[2] << endl;
                    cout << "\txlon[1] = " << xlon[1] << ", xlon[2] = " << xlon[2] << endl;
                    cout << "\tylat[0] = " << ylat[0] << ", ylat[3] = " << ylat[3] << endl;
                    cout << "\txlon[0] = " << xlon[0] << ", xlon[3] = " << xlon[3] << endl;
                }
                else
                {
                    process_selectedXsphere_quad(clon, sv, 
                                                 1, 0, 2, 3,
                                                 xlon, ylat, radius, mc);

                    ++numProcessedPoints;
                    return;

                    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
                    cout << "\tclon = " << clon << endl;
                    cout << "\tylat[1] = " << ylat[1] << ", ylat[2] = " << ylat[2] << endl;
                    cout << "\txlon[1] = " << xlon[1] << ", xlon[2] = " << xlon[2] << endl;
                    cout << "\tylat[0] = " << ylat[0] << ", ylat[3] = " << ylat[3] << endl;
                    cout << "\txlon[0] = " << xlon[0] << ", xlon[3] = " << xlon[3] << endl;
                }
            }
            else
            {
                if(xlon[3] <= clon) 
                {
                    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
                    cout << "\tclon = " << clon << endl;
                    cout << "\tylat[1] = " << ylat[1] << ", ylat[2] = " << ylat[2] << endl;
                    cout << "\txlon[1] = " << xlon[1] << ", xlon[2] = " << xlon[2] << endl;
                    cout << "\tylat[0] = " << ylat[0] << ", ylat[3] = " << ylat[3] << endl;
                    cout << "\txlon[0] = " << xlon[0] << ", xlon[3] = " << xlon[3] << endl;
                }
                else
                {
                    process_selectedXsphere_quad(clon, sv,
                                                 1, 0, 1, 2,
                                                 xlon, ylat, radius, mc);

                    ++numProcessedPoints;
                    return;

                    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
                    cout << "\tclon = " << clon << endl;
                    cout << "\tylat[1] = " << ylat[1] << ", ylat[2] = " << ylat[2] << endl;
                    cout << "\txlon[1] = " << xlon[1] << ", xlon[2] = " << xlon[2] << endl;
                    cout << "\tylat[0] = " << ylat[0] << ", ylat[3] = " << ylat[3] << endl;
                    cout << "\txlon[0] = " << xlon[0] << ", xlon[3] = " << xlon[3] << endl;
                }
            }
        }
        else
        {
            if(xlon[2] <= clon)
            {
                if(xlon[3] <= clon)
                {
                    if((xlon[0] > 359.99) && (xlon[1] > 359.99))
                    {
                        if((xlon[2] <= clon) && (xlon[3] <= clon))
                        {
                            ++numProcessedPoints;
                            return;
                        }
                    }

                    if((xlon[2] < 0.001) && (xlon[3] < 0.001))
                    {
                        if((xlon[0] > clon) && (xlon[1] > clon))
                        {
                            ++numProcessedPoints;
                            return;
                        }
                    }

                    process_selectedXsphere_quad(clon, sv,
                                                 2, 1, 3, 0,
                                                 xlon, ylat, radius, mc);

                    ++numProcessedPoints;
                    return;

                    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
                    cout << "\tclon = " << clon << endl;
                    cout << "\tylat[1] = " << ylat[1] << ", ylat[2] = " << ylat[2] << endl;
                    cout << "\txlon[1] = " << xlon[1] << ", xlon[2] = " << xlon[2] << endl;
                    cout << "\tylat[0] = " << ylat[0] << ", ylat[3] = " << ylat[3] << endl;
                    cout << "\txlon[0] = " << xlon[0] << ", xlon[3] = " << xlon[3] << endl;
                }
                else
                {
                    if(xlon[2] < 0.001)
                    {
                        if((xlon[0] > clon) && (xlon[1] > clon) && (xlon[3] > clon))
                        {
                            ++numProcessedPoints;
                            return;
                        }
                    }

                    if(xlon[2] <  clon)
                    {
                        if((xlon[0] > 359.99) && (xlon[1] > 359.99) && (xlon[3] > 359.99))
                        {
                            ++numProcessedPoints;
                            return;
                        }
                    }

                    process_selectedXsphere_quad(clon, sv,
                                                 2, 1, 2, 3,
                                                 xlon, ylat, radius, mc);

                    ++numProcessedPoints;
                    return;

                    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
                    cout << "\tclon = " << clon << endl;
                    cout << "\tylat[1] = " << ylat[1] << ", ylat[2] = " << ylat[2] << endl;
                    cout << "\txlon[1] = " << xlon[1] << ", xlon[2] = " << xlon[2] << endl;
                    cout << "\tylat[0] = " << ylat[0] << ", ylat[3] = " << ylat[3] << endl;
                    cout << "\txlon[0] = " << xlon[0] << ", xlon[3] = " << xlon[3] << endl;
                }
            }
            else
            {
                if(xlon[3] <= clon) 
                {
                    if(xlon[3] < 0.001)
                    {
                        if((xlon[0] > clon) && (xlon[1] > clon) && (xlon[2] > clon))
                        {
                            ++numProcessedPoints;
                            return;
                        }
                    }

                    if((xlon[0] > 359.99) && (xlon[1] > 359.99) && (xlon[2] > 359.99))
                    {
                        ++numProcessedPoints;
                        return;
                    }


                    process_selectedXsphere_quad(clon, sv,
                                                 3, 0, 3, 2,
                                                 xlon, ylat, radius, mc);

                    ++numProcessedPoints;
                    return;

                    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
                    cout << "\tclon = " << clon << endl;
                    cout << "\tylat[1] = " << ylat[1] << ", ylat[2] = " << ylat[2] << endl;
                    cout << "\tylat[0] = " << ylat[0] << ", ylat[3] = " << ylat[3] << endl;
                    cout << "\txlon[1] = " << xlon[1] << ", xlon[2] = " << xlon[2] << endl;
                    cout << "\txlon[0] = " << xlon[0] << ", xlon[3] = " << xlon[3] << endl;
                }
                else
                {
                  //Do not think we need to do anything if it reaches here.
                  //They are more likely bogus.
                    ++numProcessedPoints;
                    return;

                    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
                    cout << "\tclon = " << clon << endl;
                    cout << "\tylat[1] = " << ylat[1] << ", ylat[2] = " << ylat[2] << endl;
                    cout << "\tylat[0] = " << ylat[0] << ", ylat[3] = " << ylat[3] << endl;
                    cout << "\txlon[1] = " << xlon[1] << ", xlon[2] = " << xlon[2] << endl;
                    cout << "\txlon[0] = " << xlon[0] << ", xlon[3] = " << xlon[3] << endl;
                }
            }
        }
    }
}

void CAMse2dViewer::process_selectedXsphere_quad(double clon, double sv,
                                                 int il0, int ih0, 
                                                 int il1, int ih1,
                                                 double* xlon, double* ylat, double* radius,
                                                 int* mc)
{
    int k;
    double fact;

    double val;
    double clat[2];
    double alon[2];

    if(il0 == ih0)
    {
        alon[0] = 0.5;
        clat[0] = ylat[il0];
    }
    else
        clat[0] = linInterp(clon, xlon[il0], xlon[ih0], ylat[il0], ylat[ih0], alon[0]);

    clat[1] = linInterp(clon, xlon[il1], xlon[ih1], ylat[il1], ylat[ih1], alon[1]);

#if 0
    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
    cout << "\tclon = " << clon << ", clat[0] = " << clat[0] << ", clat[1] = " << clat[1] << endl;
    cout << "\tylat[il0] = " << ylat[il0] << ", ylat[ih0] = " << ylat[ih0] << endl;
    cout << "\txlon[il0] = " << xlon[il0] << ", xlon[ih0] = " << xlon[ih0] << endl;
    cout << "\tylat[il1] = " << ylat[il1] << ", ylat[ih1] = " << ylat[ih1] << endl;
    cout << "\txlon[il1] = " << xlon[il1] << ", xlon[ih1] = " << xlon[ih1] << endl;
#endif

    glBegin(GL_QUAD_STRIP);
        for(k = 0; k < lev; ++k)
        {
            val = (1.0 - alon[0]) * pltvar[mc[il0]+k*ncol]
                       + alon[0]  * pltvar[mc[ih0]+k*ncol]; 
            fact = sv * (val - _valmin);
            _lonlat2xyz(clon, clat[0], radius[k], fact);

            val = (1.0 - alon[1]) * pltvar[mc[il1]+k*ncol]
                       + alon[1]  * pltvar[mc[ih1]+k*ncol]; 
            fact = sv * (val - _valmin);
            _lonlat2xyz(clon, clat[1], radius[k], fact);
        }
    glEnd();
}

void CAMse2dViewer::handle_selectedYsphere_quad(double clat, double* ylat,
                                                int nc, double sv,
                                                double* radius,
                                                int& numProcessedPoints)
{
    int i, m;
    int mc[ncorners];

    bool up = false;
    bool down = false;

    double xlon[ncorners];

    for(i = 0; i < ncorners; ++i)
    {
        m = element_corners[nc+i];
        mc[i] = element_corners[nc+i];
        xlon[i] = lon[m];

        if(xlon[i] > 300.0)
            up = true;
        if(xlon[i] < 60.0)
            down = true;
    }

#if 0
    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
    cout << "\tclat = " << clat << endl;
    cout << "\tylat[1] = " << ylat[1] << ", ylat[2] = " << ylat[2] << endl;
    cout << "\tylat[0] = " << ylat[0] << ", ylat[3] = " << ylat[3] << endl << endl;
    cout << "\txlon[1] = " << xlon[1] << ", xlon[2] = " << xlon[2] << endl;
    cout << "\txlon[0] = " << xlon[0] << ", xlon[3] = " << xlon[3] << endl;
#endif

    if(up && down)
    {
        for(i = 0; i < ncorners; ++i)
        {
            if(xlon[i] < 60.0)
               xlon[i] += 360.0;
        }
    }

    if(ylat[0] >= clat)
    {
        if(ylat[1] >= clat)
        {
            if(ylat[2] >= clat)
            {
                if(ylat[3] >= clat)
                {
                    if(ylat[0] == clat)
                    {
                        if((ylat[1] >= clat) && (ylat[3] >= clat))
                        {
                            process_selectedYsphere_quad(clat, sv,
                                                         0, 1, 0, 3,
                                                         xlon, ylat, radius, mc);

                            ++numProcessedPoints;
                            return;
                        }
                    }

                    if(ylat[1] == clat)
                    {
                        if((ylat[0] >= clat) && (ylat[2] >= clat))
                        {
                            process_selectedYsphere_quad(clat, sv,
                                                         1, 0, 2, 1,
                                                         xlon, ylat, radius, mc);

                            ++numProcessedPoints;
                            return;
                        }
                    }

                    if(ylat[2] == clat)
                    {
                        if((ylat[1] >= clat) && (ylat[3] >= clat))
                        {
                            process_selectedYsphere_quad(clat, sv,
                                                         2, 1, 2, 3,
                                                         xlon, ylat, radius, mc);

                            ++numProcessedPoints;
                            return;
                        }
                    }

                    if(ylat[3] == clat)
                    {
                        if((ylat[0] >= clat) && (ylat[2] >= clat))
                        {
                            process_selectedYsphere_quad(clat, sv,
                                                         3, 0, 3, 2,
                                                         xlon, ylat, radius, mc);

                            ++numProcessedPoints;
                            return;
                        }
                    }

                    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
                    cout << "\tclat = " << clat << endl;
                    cout << "\tylat[1] = " << ylat[1] << ", ylat[2] = " << ylat[2] << endl;
                    cout << "\tylat[0] = " << ylat[0] << ", ylat[3] = " << ylat[3] << endl << endl;
                    cout << "\txlon[1] = " << xlon[1] << ", xlon[2] = " << xlon[2] << endl;
                    cout << "\txlon[0] = " << xlon[0] << ", xlon[3] = " << xlon[3] << endl;
                }
                else
                {
                  //if((xlon[0] >= clon) && (xlon[2] >= clon))
                  //{
                        process_selectedYsphere_quad(clat, sv,
                                                     3, 0, 3, 2,
                                                     xlon, ylat, radius, mc);

                        ++numProcessedPoints;
                        return;
                  //}

                    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
                    cout << "\tclat = " << clat << endl;
                    cout << "\tylat[1] = " << ylat[1] << ", ylat[2] = " << ylat[2] << endl;
                    cout << "\txlon[1] = " << xlon[1] << ", xlon[2] = " << xlon[2] << endl;
                    cout << "\tylat[0] = " << ylat[0] << ", ylat[3] = " << ylat[3] << endl;
                    cout << "\txlon[0] = " << xlon[0] << ", xlon[3] = " << xlon[3] << endl;
                }
            }
            else
            {
                if(ylat[3] >= clat)
                {
                  //if((xlon[0] >= clon) && (xlon[2] >= clon))
                  //{
                        process_selectedYsphere_quad(clat, sv,
                                                     2, 1, 2, 3,
                                                     xlon, ylat, radius, mc);

                        ++numProcessedPoints;
                        return;
                  //}

                    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
                    cout << "\tclat = " << clat << endl;
                    cout << "\tylat[1] = " << ylat[1] << ", ylat[2] = " << ylat[2] << endl;
                    cout << "\txlon[1] = " << xlon[1] << ", xlon[2] = " << xlon[2] << endl;
                    cout << "\tylat[0] = " << ylat[0] << ", ylat[3] = " << ylat[3] << endl;
                    cout << "\txlon[0] = " << xlon[0] << ", xlon[3] = " << xlon[3] << endl;
                }
                else
                {
                  //if((xlon[0] >= clon) && (xlon[2] >= clon))
                  //{
                        process_selectedYsphere_quad(clat, sv,
                                                     3, 0, 2, 1,
                                                     xlon, ylat, radius, mc);

                        ++numProcessedPoints;
                        return;
                  //}

                    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
                    cout << "\tclat = " << clat << endl;
                    cout << "\tylat[1] = " << ylat[1] << ", ylat[2] = " << ylat[2] << endl;
                    cout << "\txlon[1] = " << xlon[1] << ", xlon[2] = " << xlon[2] << endl;
                    cout << "\tylat[0] = " << ylat[0] << ", ylat[3] = " << ylat[3] << endl;
                    cout << "\txlon[0] = " << xlon[0] << ", xlon[3] = " << xlon[3] << endl;
                }
            }
        }
        else
        {
            if(ylat[2] >= clat)
            {
                if(ylat[3] >= clat)
                {
                  //if((xlon[0] >= clon) && (xlon[2] >= clon))
                  //{
                        process_selectedYsphere_quad(clat, sv,
                                                     1, 0, 1, 2,
                                                     xlon, ylat, radius, mc);

                        ++numProcessedPoints;
                        return;
                  //}

                    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
                    cout << "\tclat = " << clat << endl;
                    cout << "\tylat[1] = " << ylat[1] << ", ylat[2] = " << ylat[2] << endl;
                    cout << "\txlon[1] = " << xlon[1] << ", xlon[2] = " << xlon[2] << endl;
                    cout << "\tylat[0] = " << ylat[0] << ", ylat[3] = " << ylat[3] << endl;
                    cout << "\txlon[0] = " << xlon[0] << ", xlon[3] = " << xlon[3] << endl;
                }
                else
                {
                    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
                    cout << "\tclat = " << clat << endl;
                    cout << "\tylat[1] = " << ylat[1] << ", ylat[2] = " << ylat[2] << endl;
                    cout << "\txlon[1] = " << xlon[1] << ", xlon[2] = " << xlon[2] << endl;
                    cout << "\tylat[0] = " << ylat[0] << ", ylat[3] = " << ylat[3] << endl;
                    cout << "\txlon[0] = " << xlon[0] << ", xlon[3] = " << xlon[3] << endl;
                }
            }
            else
            {
                if(ylat[3] >= clat)
                {
                  //if((xlon[0] >= clon) && (xlon[2] >= clon))
                  //{
                        process_selectedYsphere_quad(clat, sv,
                                                     1, 0, 2, 3,
                                                     xlon, ylat, radius, mc);

                        ++numProcessedPoints;
                        return;
                  //}

                    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
                    cout << "\tclat = " << clat << endl;
                    cout << "\tylat[1] = " << ylat[1] << ", ylat[2] = " << ylat[2] << endl;
                    cout << "\txlon[1] = " << xlon[1] << ", xlon[2] = " << xlon[2] << endl;
                    cout << "\tylat[0] = " << ylat[0] << ", ylat[3] = " << ylat[3] << endl;
                    cout << "\txlon[0] = " << xlon[0] << ", xlon[3] = " << xlon[3] << endl;
                }
                else
                {
                  //if((xlon[0] >= clon) && (xlon[2] >= clon))
                  //{
                        process_selectedYsphere_quad(clat, sv,
                                                     1, 0, 3, 0,
                                                     xlon, ylat, radius, mc);

                        ++numProcessedPoints;
                        return;
                  //}

                    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
                    cout << "\tclat = " << clat << endl;
                    cout << "\tylat[1] = " << ylat[1] << ", ylat[2] = " << ylat[2] << endl;
                    cout << "\txlon[1] = " << xlon[1] << ", xlon[2] = " << xlon[2] << endl;
                    cout << "\tylat[0] = " << ylat[0] << ", ylat[3] = " << ylat[3] << endl;
                    cout << "\txlon[0] = " << xlon[0] << ", xlon[3] = " << xlon[3] << endl;
                }
            }
        }
    }
    else
    {
        if(ylat[1] >= clat)
        {
            if(ylat[2] >= clat)
            {
                if(ylat[3] >= clat)
                {
                  //if((xlon[0] >= clon) && (xlon[2] >= clon))
                  //{
                        process_selectedYsphere_quad(clat, sv,
                                                     0, 1, 0, 3,
                                                     xlon, ylat, radius, mc);

                        ++numProcessedPoints;
                        return;
                  //}

                    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
                    cout << "\tclat = " << clat << endl;
                    cout << "\tylat[1] = " << ylat[1] << ", ylat[2] = " << ylat[2] << endl;
                    cout << "\txlon[1] = " << xlon[1] << ", xlon[2] = " << xlon[2] << endl;
                    cout << "\tylat[0] = " << ylat[0] << ", ylat[3] = " << ylat[3] << endl;
                    cout << "\txlon[0] = " << xlon[0] << ", xlon[3] = " << xlon[3] << endl;
                }
                else
                {
                  //if((xlon[0] >= clon) && (xlon[2] >= clon))
                  //{
                        process_selectedYsphere_quad(clat, sv,
                                                     0, 1, 3, 2,
                                                     xlon, ylat, radius, mc);

                        ++numProcessedPoints;
                        return;
                  //}

                    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
                    cout << "\tclat = " << clat << endl;
                    cout << "\tylat[1] = " << ylat[1] << ", ylat[2] = " << ylat[2] << endl;
                    cout << "\txlon[1] = " << xlon[1] << ", xlon[2] = " << xlon[2] << endl;
                    cout << "\tylat[0] = " << ylat[0] << ", ylat[3] = " << ylat[3] << endl;
                    cout << "\txlon[0] = " << xlon[0] << ", xlon[3] = " << xlon[3] << endl;
                }
            }
            else
            {
                if(ylat[3] >= clat)
                {
                    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
                    cout << "\tclat = " << clat << endl;
                    cout << "\tylat[1] = " << ylat[1] << ", ylat[2] = " << ylat[2] << endl;
                    cout << "\txlon[1] = " << xlon[1] << ", xlon[2] = " << xlon[2] << endl;
                    cout << "\tylat[0] = " << ylat[0] << ", ylat[3] = " << ylat[3] << endl;
                    cout << "\txlon[0] = " << xlon[0] << ", xlon[3] = " << xlon[3] << endl;
                }
                else
                {
                  //if((xlon[0] >= clon) && (xlon[2] >= clon))
                  //{
                        process_selectedYsphere_quad(clat, sv,
                                                     0, 1, 2, 1,
                                                     xlon, ylat, radius, mc);

                        ++numProcessedPoints;
                        return;
                  //}

                    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
                    cout << "\tclat = " << clat << endl;
                    cout << "\tylat[1] = " << ylat[1] << ", ylat[2] = " << ylat[2] << endl;
                    cout << "\txlon[1] = " << xlon[1] << ", xlon[2] = " << xlon[2] << endl;
                    cout << "\tylat[0] = " << ylat[0] << ", ylat[3] = " << ylat[3] << endl;
                    cout << "\txlon[0] = " << xlon[0] << ", xlon[3] = " << xlon[3] << endl;
                }
            }
        }
        else
        {
            if(ylat[2] >= clat)
            {
                if(ylat[3] >= clat)
                {
                  //if((xlon[0] >= clon) && (xlon[2] >= clon))
                  //{
                        process_selectedYsphere_quad(clat, sv,
                                                     0, 3, 1, 2,
                                                     xlon, ylat, radius, mc);

                        ++numProcessedPoints;
                        return;
                  //}

                    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
                    cout << "\tclat = " << clat << endl;
                    cout << "\tylat[1] = " << ylat[1] << ", ylat[2] = " << ylat[2] << endl;
                    cout << "\txlon[1] = " << xlon[1] << ", xlon[2] = " << xlon[2] << endl;
                    cout << "\tylat[0] = " << ylat[0] << ", ylat[3] = " << ylat[3] << endl;
                    cout << "\txlon[0] = " << xlon[0] << ", xlon[3] = " << xlon[3] << endl;
                }
                else
                {
                  //if((xlon[0] >= clon) && (xlon[2] >= clon))
                  //{
                        process_selectedYsphere_quad(clat, sv,
                                                     1, 2, 3, 2,
                                                     xlon, ylat, radius, mc);

                        ++numProcessedPoints;
                        return;
                  //}

                    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
                    cout << "\tclat = " << clat << endl;
                    cout << "\tylat[1] = " << ylat[1] << ", ylat[2] = " << ylat[2] << endl;
                    cout << "\txlon[1] = " << xlon[1] << ", xlon[2] = " << xlon[2] << endl;
                    cout << "\tylat[0] = " << ylat[0] << ", ylat[3] = " << ylat[3] << endl;
                    cout << "\txlon[0] = " << xlon[0] << ", xlon[3] = " << xlon[3] << endl;
                }
            }
            else
            {
                if(ylat[3] >= clat)
                {
                  //if((xlon[0] >= clon) && (xlon[2] >= clon))
                  //{
                        process_selectedYsphere_quad(clat, sv,
                                                     0, 3, 2, 3,
                                                     xlon, ylat, radius, mc);

                        ++numProcessedPoints;
                        return;
                  //}

                    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
                    cout << "\tclat = " << clat << endl;
                    cout << "\tylat[1] = " << ylat[1] << ", ylat[2] = " << ylat[2] << endl;
                    cout << "\txlon[1] = " << xlon[1] << ", xlon[2] = " << xlon[2] << endl;
                    cout << "\tylat[0] = " << ylat[0] << ", ylat[3] = " << ylat[3] << endl;
                    cout << "\txlon[0] = " << xlon[0] << ", xlon[3] = " << xlon[3] << endl;
                }
                else
                {
                    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
                    cout << "\tclat = " << clat << endl;
                    cout << "\tylat[1] = " << ylat[1] << ", ylat[2] = " << ylat[2] << endl;
                    cout << "\txlon[1] = " << xlon[1] << ", xlon[2] = " << xlon[2] << endl;
                    cout << "\tylat[0] = " << ylat[0] << ", ylat[3] = " << ylat[3] << endl;
                    cout << "\txlon[0] = " << xlon[0] << ", xlon[3] = " << xlon[3] << endl;
                }
            }
        }
    }
}

void CAMse2dViewer::process_selectedYsphere_quad(double clat, double sv,
                                                 int il0, int ih0, 
                                                 int il1, int ih1,
                                                 double* xlon, double* ylat, double* radius,
                                                 int* mc)
{
    int k;
    double fact;

    double val;
    double alat[2];
    double clon[2];

    if(il0 == ih0)
    {
        alat[0] = 0.5;
        clon[0] = xlon[il0];
    }
    else
        clon[0] = linInterp(clat, ylat[il0], ylat[ih0], xlon[il0], xlon[ih0], alat[0]);

    clon[1] = linInterp(clat, ylat[il1], ylat[ih1], xlon[il1], xlon[ih1], alat[1]);

#if 0
    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
    cout << "\tclat = " << clat << ", clon[0] = " << clon[0] << ", clon[1] = " << clon[1] << endl;
    cout << "\txlon[il0] = " << xlon[il0] << ", xlon[ih0] = " << xlon[ih0] << endl;
    cout << "\txlon[il1] = " << xlon[il1] << ", xlon[ih1] = " << xlon[ih1] << endl << endl;
    cout << "\tylat[il0] = " << ylat[il0] << ", ylat[ih0] = " << ylat[ih0] << endl;
    cout << "\tylat[il1] = " << ylat[il1] << ", ylat[ih1] = " << ylat[ih1] << endl;
#endif

    glBegin(GL_QUAD_STRIP);
        for(k = 0; k < lev; ++k)
        {
            val = (1.0 - alat[0]) * pltvar[mc[il0]+k*ncol]
                       + alat[0]  * pltvar[mc[ih0]+k*ncol]; 
            fact = sv * (val - _valmin);
            _lonlat2xyz(clon[0], clat, radius[k], fact);

            val = (1.0 - alat[1]) * pltvar[mc[il1]+k*ncol]
                       + alat[1]  * pltvar[mc[ih1]+k*ncol]; 
            fact = sv * (val - _valmin);
            _lonlat2xyz(clon[1], clat, radius[k], fact);
        }
    glEnd();
}

void CAMse2dViewer::_sphereXplane(int xs)
{
    int i, j, m, n;

    double sv = 1.0;

    double xlon[ncorners];
    double clon = (double) xs;

    bool shift_low = false;
    bool shift_high = false;
    bool all_left, all_rite;

    int numProcessedPoints = 0;
    int numBoundaryPoints = 0;

    double radius[lev];

    if(xs < 30)
        shift_low = true;
    if(xs > 330)
        shift_high = true;

    for(n = 0; n < lev; ++n)
        radius[n] = 0.725 + 0.5 * ( 1.0 - (n + 1.0) / lev);

    sv = 1.0 / (_valmax - _valmin);

    xcl = glGenLists(1);

  //glNewList(xcl, GL_COMPILE);
    glNewList(xcl, GL_COMPILE_AND_EXECUTE);
    lister->set_xid(xs, xcl);

  //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\t_valmin = " << _valmin << ", _valmax = " << _valmax << ", sv = " << sv << endl;
  //cout << "\txcl = " << xcl << endl;

    glPushMatrix();

    glColor4f(1.0, 1.0, 1.0, 1.0);
  //glColor4f(0.0, 0.0, 0.0, 0.0);

    glEnable(GL_TEXTURE_1D);
    glBindTexture(GL_TEXTURE_1D, texture1d->get_textureID());

    numBoundaryPoints = 0;
    numProcessedPoints = 0;

  //#pragma omp parallel for
    for(n = 0; n < ncenters; ++n)
    {
        all_left = true;
        all_rite = true;

        j = n * ncorners;
        for(i = 0; i < ncorners; ++i)
        {
            m = element_corners[j+i];

            xlon[i] = lon[m];

            if(shift_high)
            {
                if(xlon[i] < 30.0)
                   xlon[i] += 360.0;
            }

            if(shift_low)
            {
                if(xlon[i] > 330.0)
                   xlon[i] -= 360.0;
            }

            if(xlon[i] >= clon)
                all_left = false;
            if(xlon[i] <= clon)
                all_rite = false;
        }

        if(all_left || all_rite)
            continue;

        ++numBoundaryPoints;

        handle_selectedXsphere_quad(clon, xlon, j, sv,
                                    radius, numProcessedPoints);
    }

    if(numProcessedPoints != numBoundaryPoints)
    {
        cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__
             << ", line: " << __LINE__ << endl;
        cout << "\nProcessed " << numProcessedPoints << " of " << numBoundaryPoints << endl;
        cout << "\tclon = " << clon << endl;
    }

    glDisable(GL_TEXTURE_1D);

    glPopMatrix();

    glEndList();
}

void CAMse2dViewer::_sphereYplane(int ys)
{
    int i, j, m, n;

    double sv = 1.0;

    double clat = (double) ys;
    double ylat[ncorners];

    bool all_below, all_above;

    int numProcessedPoints = 0;
    int numBoundaryPoints = 0;

    double radius[lev];

    for(n = 0; n < lev; ++n)
        radius[n] = 0.725 + 0.5 * ( 1.0 - (n + 1.0) / lev);

  //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\t_varname: <" << _varname << ">, lev = " << lev << endl;
  //cout << "\tncenters = " << ncenters << ", nvoptions->get_zsec() = " << nvoptions->get_zsec() << endl;

    sv = 1.0 / (_valmax - _valmin);

    ycl = glGenLists(1);
  //glNewList(ycl, GL_COMPILE);
    glNewList(ycl, GL_COMPILE_AND_EXECUTE);
    lister->set_yid(ys+90, ycl);

  //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\t_valmin = " << _valmin << ", _valmax = " << _valmax << ", sv = " << sv << endl;
  //cout << "\tycl = " << ycl << endl;

    glPushMatrix();

    glColor4f(1.0, 1.0, 1.0, 1.0);
  //glColor4f(0.0, 0.0, 0.0, 0.0);

    glEnable(GL_TEXTURE_1D);
    glBindTexture(GL_TEXTURE_1D, texture1d->get_textureID());

    numBoundaryPoints = 0;
    numProcessedPoints = 0;

  //#pragma omp parallel for
    for(n = 0; n < ncenters; ++n)
    {
        all_below = true;
        all_above = true;

        j = n * ncorners;
        for(i = 0; i < ncorners; ++i)
        {
            m = element_corners[j+i];

            ylat[i] = lat[m];

            if(ylat[i] < clat)
                all_below = false;
            if(ylat[i] > clat)
                all_above = false;
        }

        if(all_below || all_above)
            continue;

        ++numBoundaryPoints;

        handle_selectedYsphere_quad(clat, ylat, j, sv,
                                    radius, numProcessedPoints);
    }

    glDisable(GL_TEXTURE_1D);

    glPopMatrix();

    glEndList();

    if(numProcessedPoints != numBoundaryPoints)
    {
        cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__
             << ", line: " << __LINE__ << endl;
        cout << "\nProcessed " << numProcessedPoints << " of " << numBoundaryPoints << endl;
        cout << "\tclat = " << clat << endl;
    }
}

void CAMse2dViewer::process_selectedYflat_quad(double yc, double sv,
                                               int il0, int ih0, int il1, int ih1,
                                               double* x, double* y, double* height,
                                               int* mc)
{
    int k;
    double fact;

    double val;
    double dy[2];
    double xc[2];

    if((x[0] == 1.0) && (x[3] < -0.75) &&
       (x[1] == 1.0) && (x[2] < -0.75))
    {
        x[0] = -1.0;
        x[1] = -1.0;
    }

    if((x[2] == 1.0) && (x[1] < -0.75) &&
       (x[3] == 1.0) && (x[0] < -0.75))
    {
        x[2] = -1.0;
        x[3] = -1.0;
    }

    if(il0 == ih0)
    {
        dy[0] = 0.5;
        xc[0] = x[il0];
    }
    else
        xc[0] = linInterp(yc, y[il0], y[ih0], x[il0], x[ih0], dy[0]);

    xc[1] = linInterp(yc, y[il1], y[ih1], x[il1], x[ih1], dy[1]);

#if 0
    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
    cout << "\tyc = " << yc << ", xc[0] = " << xc[0] << ", xc[1] = " << xc[1] << endl;
    cout << "\tx[il0] = " << x[il0] << ", x[ih0] = " << x[ih0] << endl;
    cout << "\tx[il1] = " << x[il1] << ", x[ih1] = " << x[ih1] << endl << endl;
    cout << "\ty[il0] = " << y[il0] << ", y[ih0] = " << y[ih0] << endl;
    cout << "\ty[il1] = " << y[il1] << ", y[ih1] = " << y[ih1] << endl;
#endif

    glBegin(GL_QUAD_STRIP);
        for(k = 0; k < lev; ++k)
        {
            val = (1.0 - dy[0]) * pltvar[mc[il0]+k*ncol]
                       + dy[0]  * pltvar[mc[ih0]+k*ncol]; 
            fact = sv * (val - _valmin);
            glTexCoord1d(fact);
            glVertex3d(xc[0], yc, height[k]);

            val = (1.0 - dy[1]) * pltvar[mc[il1]+k*ncol]
                       + dy[1]  * pltvar[mc[ih1]+k*ncol]; 
            fact = sv * (val - _valmin);
            glTexCoord1d(fact);
            glVertex3d(xc[1], yc, height[k]);
        }
    glEnd();
}

void CAMse2dViewer::handle_selectedYflat_quad(double clat,
                                              int nc, double sv,
                                              double* height,
                                              int& numProcessedPoints)
{
    int i, m;
    int mc[ncorners];

    bool up = false;
    bool down = false;

    double xlon[ncorners];
    double ylat[ncorners];
    double x[ncorners];
    double y[ncorners];
    double yc = clat / 180.0;

    for(i = 0; i < ncorners; ++i)
    {
        m = element_corners[nc+i];
        mc[i] = element_corners[nc+i];
        xlon[i] = lon[m];
        ylat[i] = lat[m];

        x[i] = _xFlat[m];
        y[i] = _yFlat[m];

        if(xlon[i] > 300.0)
            up = true;
        if(xlon[i] < 60.0)
            down = true;
    }

#if 0
    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
    cout << "\tclat = " << clat << endl;
    cout << "\tylat[1] = " << ylat[1] << ", ylat[2] = " << ylat[2] << endl;
    cout << "\tylat[0] = " << ylat[0] << ", ylat[3] = " << ylat[3] << endl << endl;
    cout << "\txlon[1] = " << xlon[1] << ", xlon[2] = " << xlon[2] << endl;
    cout << "\txlon[0] = " << xlon[0] << ", xlon[3] = " << xlon[3] << endl;
#endif

    if(up && down)
    {
        for(i = 0; i < ncorners; ++i)
        {
            if(xlon[i] < 60.0)
               xlon[i] += 360.0;
        }
    }

    if(ylat[0] >= clat)
    {
        if(ylat[1] >= clat)
        {
            if(ylat[2] >= clat)
            {
                if(ylat[3] >= clat)
                {
                    if(ylat[0] == clat)
                    {
                        if((ylat[1] >= clat) && (ylat[3] >= clat))
                        {
                            process_selectedYflat_quad(yc, sv,
                                                       0, 1, 0, 3,
                                                       x, y, height, mc);

                            ++numProcessedPoints;
                            return;
                        }
                    }

                    if(ylat[1] == clat)
                    {
                        if((ylat[0] >= clat) && (ylat[2] >= clat))
                        {
                            process_selectedYflat_quad(yc, sv,
                                                       1, 0, 2, 1,
                                                       x, y, height, mc);

                            ++numProcessedPoints;
                            return;
                        }
                    }

                    if(ylat[2] == clat)
                    {
                        if((ylat[1] >= clat) && (ylat[3] >= clat))
                        {
                            process_selectedYflat_quad(yc, sv,
                                                       2, 1, 2, 3,
                                                       x, y, height, mc);

                            ++numProcessedPoints;
                            return;
                        }
                    }

                    if(ylat[3] == clat)
                    {
                        if((ylat[0] >= clat) && (ylat[2] >= clat))
                        {
                            process_selectedYflat_quad(yc, sv,
                                                       3, 0, 3, 2,
                                                       x, y, height, mc);

                            ++numProcessedPoints;
                            return;
                        }
                    }

                    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
                    cout << "\tclat = " << clat << endl;
                    cout << "\tylat[1] = " << ylat[1] << ", ylat[2] = " << ylat[2] << endl;
                    cout << "\tylat[0] = " << ylat[0] << ", ylat[3] = " << ylat[3] << endl << endl;
                    cout << "\txlon[1] = " << xlon[1] << ", xlon[2] = " << xlon[2] << endl;
                    cout << "\txlon[0] = " << xlon[0] << ", xlon[3] = " << xlon[3] << endl;
                }
                else
                {
                    process_selectedYflat_quad(yc, sv,
                                               3, 0, 3, 2,
                                               x, y, height, mc);

                    ++numProcessedPoints;
                    return;

                    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
                    cout << "\tclat = " << clat << endl;
                    cout << "\tylat[1] = " << ylat[1] << ", ylat[2] = " << ylat[2] << endl;
                    cout << "\txlon[1] = " << xlon[1] << ", xlon[2] = " << xlon[2] << endl;
                    cout << "\tylat[0] = " << ylat[0] << ", ylat[3] = " << ylat[3] << endl;
                    cout << "\txlon[0] = " << xlon[0] << ", xlon[3] = " << xlon[3] << endl;
                }
            }
            else
            {
                if(ylat[3] >= clat)
                {
                    process_selectedYflat_quad(yc, sv,
                                               2, 1, 2, 3,
                                               x, y, height, mc);

                    ++numProcessedPoints;
                    return;

                    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
                    cout << "\tclat = " << clat << endl;
                    cout << "\tylat[1] = " << ylat[1] << ", ylat[2] = " << ylat[2] << endl;
                    cout << "\txlon[1] = " << xlon[1] << ", xlon[2] = " << xlon[2] << endl;
                    cout << "\tylat[0] = " << ylat[0] << ", ylat[3] = " << ylat[3] << endl;
                    cout << "\txlon[0] = " << xlon[0] << ", xlon[3] = " << xlon[3] << endl;
                }
                else
                {
                    process_selectedYflat_quad(yc, sv,
                                               3, 0, 2, 1,
                                               x, y, height, mc);

                    ++numProcessedPoints;
                    return;

                    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
                    cout << "\tclat = " << clat << endl;
                    cout << "\tylat[1] = " << ylat[1] << ", ylat[2] = " << ylat[2] << endl;
                    cout << "\txlon[1] = " << xlon[1] << ", xlon[2] = " << xlon[2] << endl;
                    cout << "\tylat[0] = " << ylat[0] << ", ylat[3] = " << ylat[3] << endl;
                    cout << "\txlon[0] = " << xlon[0] << ", xlon[3] = " << xlon[3] << endl;
                }
            }
        }
        else
        {
            if(ylat[2] >= clat)
            {
                if(ylat[3] >= clat)
                {
                    process_selectedYflat_quad(yc, sv,
                                               1, 0, 1, 2,
                                               x, y, height, mc);

                    ++numProcessedPoints;
                    return;

                    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
                    cout << "\tclat = " << clat << endl;
                    cout << "\tylat[1] = " << ylat[1] << ", ylat[2] = " << ylat[2] << endl;
                    cout << "\txlon[1] = " << xlon[1] << ", xlon[2] = " << xlon[2] << endl;
                    cout << "\tylat[0] = " << ylat[0] << ", ylat[3] = " << ylat[3] << endl;
                    cout << "\txlon[0] = " << xlon[0] << ", xlon[3] = " << xlon[3] << endl;
                }
                else
                {
                    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
                    cout << "\tclat = " << clat << endl;
                    cout << "\tylat[1] = " << ylat[1] << ", ylat[2] = " << ylat[2] << endl;
                    cout << "\txlon[1] = " << xlon[1] << ", xlon[2] = " << xlon[2] << endl;
                    cout << "\tylat[0] = " << ylat[0] << ", ylat[3] = " << ylat[3] << endl;
                    cout << "\txlon[0] = " << xlon[0] << ", xlon[3] = " << xlon[3] << endl;
                }
            }
            else
            {
                if(ylat[3] >= clat)
                {
                    process_selectedYflat_quad(yc, sv,
                                               1, 0, 2, 3,
                                               x, y, height, mc);

                    ++numProcessedPoints;
                    return;

                    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
                    cout << "\tclat = " << clat << endl;
                    cout << "\tylat[1] = " << ylat[1] << ", ylat[2] = " << ylat[2] << endl;
                    cout << "\txlon[1] = " << xlon[1] << ", xlon[2] = " << xlon[2] << endl;
                    cout << "\tylat[0] = " << ylat[0] << ", ylat[3] = " << ylat[3] << endl;
                    cout << "\txlon[0] = " << xlon[0] << ", xlon[3] = " << xlon[3] << endl;
                }
                else
                {
                    process_selectedYflat_quad(yc, sv,
                                               1, 0, 3, 0,
                                               x, y, height, mc);

                    ++numProcessedPoints;
                    return;

                    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
                    cout << "\tclat = " << clat << endl;
                    cout << "\tylat[1] = " << ylat[1] << ", ylat[2] = " << ylat[2] << endl;
                    cout << "\txlon[1] = " << xlon[1] << ", xlon[2] = " << xlon[2] << endl;
                    cout << "\tylat[0] = " << ylat[0] << ", ylat[3] = " << ylat[3] << endl;
                    cout << "\txlon[0] = " << xlon[0] << ", xlon[3] = " << xlon[3] << endl;
                }
            }
        }
    }
    else
    {
        if(ylat[1] >= clat)
        {
            if(ylat[2] >= clat)
            {
                if(ylat[3] >= clat)
                {
                    process_selectedYflat_quad(yc, sv,
                                               0, 1, 0, 3,
                                               x, y, height, mc);

                    ++numProcessedPoints;
                    return;

                    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
                    cout << "\tclat = " << clat << endl;
                    cout << "\tylat[1] = " << ylat[1] << ", ylat[2] = " << ylat[2] << endl;
                    cout << "\txlon[1] = " << xlon[1] << ", xlon[2] = " << xlon[2] << endl;
                    cout << "\tylat[0] = " << ylat[0] << ", ylat[3] = " << ylat[3] << endl;
                    cout << "\txlon[0] = " << xlon[0] << ", xlon[3] = " << xlon[3] << endl;
                }
                else
                {
                    process_selectedYflat_quad(yc, sv,
                                               0, 1, 3, 2,
                                               x, y, height, mc);

                    ++numProcessedPoints;
                    return;

                    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
                    cout << "\tclat = " << clat << endl;
                    cout << "\tylat[1] = " << ylat[1] << ", ylat[2] = " << ylat[2] << endl;
                    cout << "\txlon[1] = " << xlon[1] << ", xlon[2] = " << xlon[2] << endl;
                    cout << "\tylat[0] = " << ylat[0] << ", ylat[3] = " << ylat[3] << endl;
                    cout << "\txlon[0] = " << xlon[0] << ", xlon[3] = " << xlon[3] << endl;
                }
            }
            else
            {
                if(ylat[3] >= clat)
                {
                    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
                    cout << "\tclat = " << clat << endl;
                    cout << "\tylat[1] = " << ylat[1] << ", ylat[2] = " << ylat[2] << endl;
                    cout << "\txlon[1] = " << xlon[1] << ", xlon[2] = " << xlon[2] << endl;
                    cout << "\tylat[0] = " << ylat[0] << ", ylat[3] = " << ylat[3] << endl;
                    cout << "\txlon[0] = " << xlon[0] << ", xlon[3] = " << xlon[3] << endl;
                }
                else
                {
                    process_selectedYflat_quad(yc, sv,
                                               0, 1, 2, 1,
                                               x, y, height, mc);

                    ++numProcessedPoints;
                    return;

                    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
                    cout << "\tclat = " << clat << endl;
                    cout << "\tylat[1] = " << ylat[1] << ", ylat[2] = " << ylat[2] << endl;
                    cout << "\txlon[1] = " << xlon[1] << ", xlon[2] = " << xlon[2] << endl;
                    cout << "\tylat[0] = " << ylat[0] << ", ylat[3] = " << ylat[3] << endl;
                    cout << "\txlon[0] = " << xlon[0] << ", xlon[3] = " << xlon[3] << endl;
                }
            }
        }
        else
        {
            if(ylat[2] >= clat)
            {
                if(ylat[3] >= clat)
                {
                    process_selectedYflat_quad(yc, sv,
                                               0, 3, 1, 2,
                                               x, y, height, mc);

                    ++numProcessedPoints;
                    return;

                    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
                    cout << "\tclat = " << clat << endl;
                    cout << "\tylat[1] = " << ylat[1] << ", ylat[2] = " << ylat[2] << endl;
                    cout << "\txlon[1] = " << xlon[1] << ", xlon[2] = " << xlon[2] << endl;
                    cout << "\tylat[0] = " << ylat[0] << ", ylat[3] = " << ylat[3] << endl;
                    cout << "\txlon[0] = " << xlon[0] << ", xlon[3] = " << xlon[3] << endl;
                }
                else
                {
                    process_selectedYflat_quad(yc, sv,
                                               1, 2, 3, 2,
                                               x, y, height, mc);

                    ++numProcessedPoints;
                    return;

                    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
                    cout << "\tclat = " << clat << endl;
                    cout << "\tylat[1] = " << ylat[1] << ", ylat[2] = " << ylat[2] << endl;
                    cout << "\txlon[1] = " << xlon[1] << ", xlon[2] = " << xlon[2] << endl;
                    cout << "\tylat[0] = " << ylat[0] << ", ylat[3] = " << ylat[3] << endl;
                    cout << "\txlon[0] = " << xlon[0] << ", xlon[3] = " << xlon[3] << endl;
                }
            }
            else
            {
                if(ylat[3] >= clat)
                {
                    process_selectedYflat_quad(yc, sv,
                                               0, 3, 2, 3,
                                               x, y, height, mc);

                    ++numProcessedPoints;
                    return;

                    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
                    cout << "\tclat = " << clat << endl;
                    cout << "\tylat[1] = " << ylat[1] << ", ylat[2] = " << ylat[2] << endl;
                    cout << "\txlon[1] = " << xlon[1] << ", xlon[2] = " << xlon[2] << endl;
                    cout << "\tylat[0] = " << ylat[0] << ", ylat[3] = " << ylat[3] << endl;
                    cout << "\txlon[0] = " << xlon[0] << ", xlon[3] = " << xlon[3] << endl;
                }
                else
                {
                    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
                    cout << "\tclat = " << clat << endl;
                    cout << "\tylat[1] = " << ylat[1] << ", ylat[2] = " << ylat[2] << endl;
                    cout << "\txlon[1] = " << xlon[1] << ", xlon[2] = " << xlon[2] << endl;
                    cout << "\tylat[0] = " << ylat[0] << ", ylat[3] = " << ylat[3] << endl;
                    cout << "\txlon[0] = " << xlon[0] << ", xlon[3] = " << xlon[3] << endl;
                }
            }
        }
    }
}

void CAMse2dViewer::_display_Xflat_plane(int xs)
{
    int i, j, m, n;

    double sv = 1.0;

    double xlon[ncorners];
    double clon = (double) xs;

    bool shift_low = false;
    bool shift_high = false;
    bool all_left, all_rite;

    int numProcessedPoints = 0;
    int numBoundaryPoints = 0;

    double height[lev];

    if(xs < 30)
        shift_low = true;
    if(xs > 330)
        shift_high = true;

    for(n = 0; n < lev; ++n)
        height[n] = 0.8 * ( 0.5 - (n + 1.0) / lev);

    sv = 1.0 / (_valmax - _valmin);

    xcl = glGenLists(1);
  //glNewList(xcl, GL_COMPILE);
    glNewList(xcl, GL_COMPILE_AND_EXECUTE);
    lister->set_xid(xs, xcl);

  //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\t_varname: <" << _varname << ">, xs = " << xs << endl;
  //cout << "\t_valmin = " << _valmin << ", _valmax = " << _valmax << ", sv = " << sv << endl;
  //cout << "\txcl = " << xcl << endl;

    glPushMatrix();

    glColor4f(1.0, 1.0, 1.0, 1.0);
  //glColor4f(0.0, 0.0, 0.0, 0.0);

    glEnable(GL_TEXTURE_1D);
    glBindTexture(GL_TEXTURE_1D, texture1d->get_textureID());

    numBoundaryPoints = 0;
    numProcessedPoints = 0;

  //#pragma omp parallel for
    for(n = 0; n < ncenters; ++n)
    {
        all_left = true;
        all_rite = true;

        j = n * ncorners;
        for(i = 0; i < ncorners; ++i)
        {
            m = element_corners[j+i];

            xlon[i] = lon[m];

            if(shift_high)
            {
                if(xlon[i] < 30.0)
                   xlon[i] += 360.0;
            }

            if(shift_low)
            {
                if(xlon[i] > 330.0)
                   xlon[i] -= 360.0;
            }

            if(xlon[i] >= clon)
                all_left = false;
            if(xlon[i] <= clon)
                all_rite = false;
        }

        if(all_left || all_rite)
            continue;

        ++numBoundaryPoints;

        handle_selectedXflat_quad(clon, xlon, j, sv,
                                  height, numProcessedPoints);
    }

    if(numProcessedPoints != numBoundaryPoints)
    {
        cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__
             << ", line: " << __LINE__ << endl;
        cout << "\nProcessed " << numProcessedPoints << " of " << numBoundaryPoints << endl;
        cout << "\tclon = " << clon << endl;
    }

    glDisable(GL_TEXTURE_1D);

    glPopMatrix();

    glEndList();
}

void CAMse2dViewer::handle_selectedXflat_quad(double clon, double* xlon,
                                              int nc, double sv,
                                              double* height,
                                              int& numProcessedPoints)
{
    int i, m;
    int mc[ncorners];
    bool isAtPole = false;
    double x[ncorners];
    double y[ncorners];
    double ylat[ncorners];
    double xc = clon / 180.0;

    if(xc > 1.0)
       xc -= 2.0;

    for(i = 0; i < ncorners; ++i)
    {
        m = element_corners[nc+i];
        mc[i] = element_corners[nc+i];
        ylat[i] = lat[m];
        x[i] = _xFlat[m];
        y[i] = _yFlat[m];

        if((ylat[i] < -89.99) || (ylat[i] > 89.99))
            isAtPole = true;

        if(clon < 180.0)
        {
            if(xlon[i] > 359.9)
               xlon[i] -= 360.0;
        }

        if(clon > 180.0)
        {
            if(xlon[i] < 70.0)
               xlon[i] += 360.0;
        }
    }

    if(isAtPole)
    {
        if((ylat[0] < -89.99) || (ylat[0] > 89.99))
        {
            if(xlon[1] > 359.99) 
            {
                if((xlon[2] > 360.0) && (xlon[3] < 181.0))
                    xlon[2] -= 360.0;

                if((xlon[2] < 60.0) || (xlon[3] < 60.0))
                    xlon[1] = 0.0;
            }

            if((xlon[1] < clon) && (xlon[2] < clon) && (xlon[3] < clon))
            {
                ++numProcessedPoints;
                return;
            }

            if((xlon[1] > clon) && (xlon[2] > clon) && (xlon[3] > clon))
            {
                ++numProcessedPoints;
                return;
            }
               
            if((xlon[1] <= clon) && (xlon[2] >= clon))
            {
                process_selectedXflat_quad(xc, sv,
                                           0, 0, 1, 2,
                                           x, y, height, mc);

                ++numProcessedPoints;
                return;
            }

            if((xlon[1] >= clon) && (xlon[2] <= clon))
            {
                process_selectedXflat_quad(xc, sv,
                                           0, 0, 2, 1,
                                           x, y, height, mc);

                ++numProcessedPoints;
                return;
            }

            if((xlon[2] <= clon) && (xlon[3] >= clon))
            {
                process_selectedXflat_quad(xc, sv,
                                           0, 0, 2, 3,
                                           x, y, height, mc);

                ++numProcessedPoints;
                return;
            }

            if((xlon[3] <= clon) && (xlon[2] >= clon))
            {
                process_selectedXflat_quad(xc, sv,
                                           0, 0, 3, 2,
                                           x, y, height, mc);

                ++numProcessedPoints;
                return;
            }
        }

        if((ylat[1] < -89.99) || (ylat[1] > 89.99))
        {
            if(xlon[0] > 359.99)
            {
                if((xlon[3] > 360.0) && (xlon[2] < 181.0))
                    xlon[3] -= 360.0;

                if((xlon[2] < 60.0) || (xlon[3] < 60.0))
                    xlon[0] = 0.0;
            }

            if((xlon[0] < clon) && (xlon[2] < clon) && (xlon[3] < clon))
            {
                ++numProcessedPoints;
                return;
            }

            if((xlon[0] > clon) && (xlon[2] > clon) && (xlon[3] > clon))
            {
                ++numProcessedPoints;
                return;
            }
               
            if((xlon[0] <= clon) && (xlon[3] >= clon))
            {
                process_selectedXflat_quad(xc, sv,
                                           1, 1, 0, 3,
                                           x, y, height, mc);

                ++numProcessedPoints;
                return;
            }
               
            if((xlon[0] >= clon) && (xlon[3] <= clon))
            {
                process_selectedXflat_quad(xc, sv,
                                           1, 1, 3, 0,
                                           x, y, height, mc);

                ++numProcessedPoints;
                return;
            }

            if((xlon[2] >= clon) && (xlon[3] <= clon))
            {
                process_selectedXflat_quad(xc, sv,
                                           1, 1, 3, 2,
                                           x, y, height, mc);

                ++numProcessedPoints;
                return;
            }

            if((xlon[2] <= clon) && (xlon[3] >= clon))
            {
                process_selectedXflat_quad(xc, sv,
                                           1, 1, 2, 3,
                                           x, y, height, mc);

                ++numProcessedPoints;
                return;
            }
        }

        if((ylat[2] < -89.99) || (ylat[2] > 89.99))
        {
            if((xlon[0] < clon) && (xlon[1] < clon) && (xlon[3] < clon))
            {
                ++numProcessedPoints;
                return;
            }

            if((xlon[0] > clon) && (xlon[1] > clon) && (xlon[3] > clon))
            {
                ++numProcessedPoints;
                return;
            }

            if((xlon[0] <= clon) && (xlon[1] >= clon))
            {
                process_selectedXflat_quad(xc, sv,
                                           2, 2, 0, 1,
                                           x, y, height, mc);

                ++numProcessedPoints;
                return;
            }

            if((xlon[0] >= clon) && (xlon[1] <= clon))
            {
                process_selectedXflat_quad(xc, sv,
                                           2, 2, 1, 0,
                                           x, y, height, mc);

                ++numProcessedPoints;
                return;
            }

            if((xlon[0] <= clon) && (xlon[3] >= clon))
            {
                process_selectedXflat_quad(xc, sv,
                                           2, 2, 0, 3,
                                           x, y, height, mc);

                ++numProcessedPoints;
                return;
            }

            if((xlon[0] >= clon) && (xlon[3] <= clon))
            {
                process_selectedXflat_quad(xc, sv,
                                           2, 2, 3, 0,
                                           x, y, height, mc);

                ++numProcessedPoints;
                return;
            }
        }

        if((ylat[3] < -89.99) || (ylat[3] > 89.99))
        {
            if((xlon[0] < clon) && (xlon[1] < clon) && (xlon[2] < clon))
            {
                ++numProcessedPoints;
                return;
            }

            if((xlon[0] > clon) && (xlon[1] > clon) && (xlon[2] > clon))
            {
                ++numProcessedPoints;
                return;
            }

            if((xlon[0] <= clon) && (xlon[1] >= clon))
            {
                process_selectedXflat_quad(xc, sv,
                                           3, 3, 0, 1,
                                           x, y, height, mc);

                ++numProcessedPoints;
                return;
            }

            if((xlon[0] >= clon) && (xlon[1] <= clon))
            {
                process_selectedXflat_quad(xc, sv,
                                           3, 3, 1, 0,
                                           x, y, height, mc);

                ++numProcessedPoints;
                return;
            }

            if((xlon[1] <= clon) && (xlon[2] >= clon))
            {
                process_selectedXflat_quad(xc, sv,
                                           3, 3, 1, 2,
                                           x, y, height, mc);

                ++numProcessedPoints;
                return;
            }

            if((xlon[1] >= clon) && (xlon[2] <= clon))
            {
                process_selectedXflat_quad(xc, sv,
                                           3, 3, 2, 1,
                                           x, y, height, mc);

                ++numProcessedPoints;
                return;
            }
        }

        cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
        cout << "\tclon = " << clon << endl;
        cout << "\tylat[1] = " << ylat[1] << ", ylat[2] = " << ylat[2] << endl;
        cout << "\tylat[0] = " << ylat[0] << ", ylat[3] = " << ylat[3] << endl << endl;
        cout << "\txlon[1] = " << xlon[1] << ", xlon[2] = " << xlon[2] << endl;
        cout << "\txlon[0] = " << xlon[0] << ", xlon[3] = " << xlon[3] << endl;
    }

#if 0
    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
    cout << "\tclon = " << clon << endl;
    cout << "\txlon[1] = " << xlon[1] << ", xlon[2] = " << xlon[2] << endl;
    cout << "\txlon[0] = " << xlon[0] << ", xlon[3] = " << xlon[3] << endl << endl;
    cout << "\tylat[1] = " << ylat[1] << ", ylat[2] = " << ylat[2] << endl;
    cout << "\tylat[0] = " << ylat[0] << ", ylat[3] = " << ylat[3] << endl;
#endif

    if(xlon[0] <= clon)
    {
        if(xlon[1] <= clon)
        {
            if(xlon[2] <= clon)
            {
                if(xlon[3] <= clon)
                {
                    if(xlon[3] == clon)
                    {
                        if(xlon[2] == clon)
                        {
                            process_selectedXflat_quad(xc, sv,
                                                       1, 2, 0, 3,
                                                       x, y, height, mc);
                        }
                        else
                        {
                            process_selectedXflat_quad(xc, sv,
                                                       0, 3, 2, 3,
                                                       x, y, height, mc);
                        }

                        ++numProcessedPoints;
                        return;
                    }

                    if(xlon[2] == clon)
                    {
                        if(xlon[1] == clon)
                        {
                            process_selectedXflat_quad(xc, sv,
                                                       0, 1, 3, 2,
                                                       x, y, height, mc);
                        }
                        else
                        {
                            process_selectedXflat_quad(xc, sv,
                                                       1, 2, 3, 2,
                                                       x, y, height, mc);
                        }

                        ++numProcessedPoints;
                        return;
                    }

                    if(xlon[1] == clon)
                    {
                        if(xlon[0] == clon)
                        {
                            process_selectedXflat_quad(xc, sv,
                                                       3, 0, 2, 1,
                                                       x, y, height, mc);
                        }
                        else
                        {
                            process_selectedXflat_quad(xc, sv,
                                                       0, 1, 2, 1,
                                                       x, y, height, mc);
                        }
                        ++numProcessedPoints;
                        return;
                    }

                    if(xlon[0] == clon)
                    {
                        if(xlon[3] == clon)
                        {
                            process_selectedXflat_quad(xc, sv,
                                                       1, 0, 2, 3,
                                                       x, y, height, mc);
                        }
                        else
                        {
                            process_selectedXflat_quad(xc, sv,
                                                       1, 0, 3, 0,
                                                       x, y, height, mc);
                        }

                        ++numProcessedPoints;
                        return;
                    }

                  //If it reaches here, just ignore it.
                    ++numProcessedPoints;
                    return;

                    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
                    cout << "\tclon = " << clon << endl;
                    cout << "\tylat[1] = " << ylat[1] << ", ylat[2] = " << ylat[2] << endl;
                    cout << "\txlon[1] = " << xlon[1] << ", xlon[2] = " << xlon[2] << endl;
                    cout << "\tylat[0] = " << ylat[0] << ", ylat[3] = " << ylat[3] << endl;
                    cout << "\txlon[0] = " << xlon[0] << ", xlon[3] = " << xlon[3] << endl;
                }
                else
                {
                    process_selectedXflat_quad(xc, sv,
                                               0, 3, 2, 3,
                                               x, y, height, mc);

                    ++numProcessedPoints;
                    return;

                    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
                    cout << "\tclon = " << clon << endl;
                    cout << "\tylat[1] = " << ylat[1] << ", ylat[2] = " << ylat[2] << endl;
                    cout << "\tylat[0] = " << ylat[0] << ", ylat[3] = " << ylat[3] << endl;
                    cout << "\txlon[1] = " << xlon[1] << ", xlon[2] = " << xlon[2] << endl;
                    cout << "\txlon[0] = " << xlon[0] << ", xlon[3] = " << xlon[3] << endl;
                }
            }
            else
            {
                if(xlon[3] <= clon)
                {
                    process_selectedXflat_quad(xc, sv,
                                               1, 2, 3, 2,
                                               x, y, height, mc);

                    ++numProcessedPoints;
                    return;

                    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
                    cout << "\tclon = " << clon << endl;
                    cout << "\tylat[1] = " << ylat[1] << ", ylat[2] = " << ylat[2] << endl;
                    cout << "\tylat[0] = " << ylat[0] << ", ylat[3] = " << ylat[3] << endl;
                    cout << "\txlon[1] = " << xlon[1] << ", xlon[2] = " << xlon[2] << endl;
                    cout << "\txlon[0] = " << xlon[0] << ", xlon[3] = " << xlon[3] << endl;
                }
                else
                {
                    process_selectedXflat_quad(xc, sv,
                                               0, 3, 1, 2,
                                               x, y, height, mc);

                    ++numProcessedPoints;
                    return;

                    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
                    cout << "\tclon = " << clon << endl;
                    cout << "\tylat[1] = " << ylat[1] << ", ylat[2] = " << ylat[2] << endl;
                    cout << "\tylat[0] = " << ylat[0] << ", ylat[3] = " << ylat[3] << endl;
                    cout << "\txlon[1] = " << xlon[1] << ", xlon[2] = " << xlon[2] << endl;
                    cout << "\txlon[0] = " << xlon[0] << ", xlon[3] = " << xlon[3] << endl;
                }
            }
        }
        else
        {
            if(xlon[2] <= clon)
            {
                if(xlon[3] <= clon)
                {
                    if(xlon[1] > clon)
                    {
                        if((xlon[0] < 0.001) && (xlon[2] < 0.001) && (xlon[3] < 0.001))
                        {
                            ++numProcessedPoints;
                            return;
                        }
                    }

                    if(xlon[1] > 360.0)
                    {
                        if((xlon[0] <= clon) && (xlon[2] <= clon) && (xlon[3] <= clon))
                        {
                            ++numProcessedPoints;
                            return;
                        }
                    }

                    process_selectedXflat_quad(xc, sv,
                                               0, 1, 2, 1,
                                               x, y, height, mc);
    
                    ++numProcessedPoints;
                    return;

                    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
                    cout << "\tclon = " << clon << endl;
                    cout << "\tylat[1] = " << ylat[1] << ", ylat[2] = " << ylat[2] << endl;
                    cout << "\tylat[0] = " << ylat[0] << ", ylat[3] = " << ylat[3] << endl;
                    cout << "\txlon[1] = " << xlon[1] << ", xlon[2] = " << xlon[2] << endl;
                    cout << "\txlon[0] = " << xlon[0] << ", xlon[3] = " << xlon[3] << endl;
                }
                else
                {
                    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
                    cout << "\tclon = " << clon << endl;
                    cout << "\tylat[1] = " << ylat[1] << ", ylat[2] = " << ylat[2] << endl;
                    cout << "\txlon[1] = " << xlon[1] << ", xlon[2] = " << xlon[2] << endl;
                    cout << "\tylat[0] = " << ylat[0] << ", ylat[3] = " << ylat[3] << endl;
                    cout << "\txlon[0] = " << xlon[0] << ", xlon[3] = " << xlon[3] << endl;
                }
            }
            else
            {
                if(xlon[3] <= clon)
                {
                    if((xlon[0] < -1.0) && (xlon[1] > 300.0) &&
                       (xlon[3] < -1.0) && (xlon[2] > 300.0))
                    {
                        if(clon < 300.0)
                        {
                            ++numProcessedPoints;
                            return;
                        }
                    }

                    process_selectedXflat_quad(xc, sv,
                                               0, 1, 3, 2,
                                               x, y, height, mc);

                    ++numProcessedPoints;
                    return;

                    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
                    cout << "\tclon = " << clon << endl;
                    cout << "\tylat[1] = " << ylat[1] << ", ylat[2] = " << ylat[2] << endl;
                    cout << "\tylat[0] = " << ylat[0] << ", ylat[3] = " << ylat[3] << endl;
                    cout << "\txlon[1] = " << xlon[1] << ", xlon[2] = " << xlon[2] << endl;
                    cout << "\txlon[0] = " << xlon[0] << ", xlon[3] = " << xlon[3] << endl;
                }
                else
                {
                    process_selectedXflat_quad(xc, sv,
                                               0, 1, 0, 3,
                                               x, y, height, mc);

                    ++numProcessedPoints;
                    return;

                    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
                    cout << "\tclon = " << clon << endl;
                    cout << "\tylat[1] = " << ylat[1] << ", ylat[2] = " << ylat[2] << endl;
                    cout << "\txlon[1] = " << xlon[1] << ", xlon[2] = " << xlon[2] << endl;
                    cout << "\tylat[0] = " << ylat[0] << ", ylat[3] = " << ylat[3] << endl;
                    cout << "\txlon[0] = " << xlon[0] << ", xlon[3] = " << xlon[3] << endl;
                }
            }
        }
    }
    else
    {
        if(xlon[1] <= clon)
        {
            if(xlon[2] <= clon)
            {
                if(xlon[3] <= clon)
                {
                    if(xlon[0] > clon)
                    {
                        if((xlon[1] < 0.001) && (xlon[2] < 0.001) && (xlon[3] < 0.001))
                        {
                            ++numProcessedPoints;
                            return;
                        }
                    }

                    process_selectedXflat_quad(xc, sv,
                                               1, 0, 3, 0,
                                               x, y, height, mc);

                    ++numProcessedPoints;
                    return;

                    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
                    cout << "\tclon = " << clon << endl;
                    cout << "\tylat[1] = " << ylat[1] << ", ylat[2] = " << ylat[2] << endl;
                    cout << "\txlon[1] = " << xlon[1] << ", xlon[2] = " << xlon[2] << endl;
                    cout << "\tylat[0] = " << ylat[0] << ", ylat[3] = " << ylat[3] << endl;
                    cout << "\txlon[0] = " << xlon[0] << ", xlon[3] = " << xlon[3] << endl;
                }
                else
                {
                    process_selectedXflat_quad(xc, sv, 
                                               1, 0, 2, 3,
                                               x, y, height, mc);

                    ++numProcessedPoints;
                    return;

                    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
                    cout << "\tclon = " << clon << endl;
                    cout << "\tylat[1] = " << ylat[1] << ", ylat[2] = " << ylat[2] << endl;
                    cout << "\txlon[1] = " << xlon[1] << ", xlon[2] = " << xlon[2] << endl;
                    cout << "\tylat[0] = " << ylat[0] << ", ylat[3] = " << ylat[3] << endl;
                    cout << "\txlon[0] = " << xlon[0] << ", xlon[3] = " << xlon[3] << endl;
                }
            }
            else
            {
                if(xlon[3] <= clon) 
                {
                    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
                    cout << "\tclon = " << clon << endl;
                    cout << "\tylat[1] = " << ylat[1] << ", ylat[2] = " << ylat[2] << endl;
                    cout << "\txlon[1] = " << xlon[1] << ", xlon[2] = " << xlon[2] << endl;
                    cout << "\tylat[0] = " << ylat[0] << ", ylat[3] = " << ylat[3] << endl;
                    cout << "\txlon[0] = " << xlon[0] << ", xlon[3] = " << xlon[3] << endl;
                }
                else
                {
                    process_selectedXflat_quad(xc, sv,
                                               1, 0, 1, 2,
                                               x, y, height, mc);

                    ++numProcessedPoints;
                    return;

                    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
                    cout << "\tclon = " << clon << endl;
                    cout << "\tylat[1] = " << ylat[1] << ", ylat[2] = " << ylat[2] << endl;
                    cout << "\txlon[1] = " << xlon[1] << ", xlon[2] = " << xlon[2] << endl;
                    cout << "\tylat[0] = " << ylat[0] << ", ylat[3] = " << ylat[3] << endl;
                    cout << "\txlon[0] = " << xlon[0] << ", xlon[3] = " << xlon[3] << endl;
                }
            }
        }
        else
        {
            if(xlon[2] <= clon)
            {
                if(xlon[3] <= clon)
                {
                    if((xlon[0] > 359.99) && (xlon[1] > 359.99))
                    {
                        if((xlon[2] <= clon) && (xlon[3] <= clon))
                        {
                            ++numProcessedPoints;
                            return;
                        }
                    }

                    if((xlon[2] < 0.001) && (xlon[3] < 0.001))
                    {
                        if((xlon[0] > clon) && (xlon[1] > clon))
                        {
                            ++numProcessedPoints;
                            return;
                        }
                    }

                    process_selectedXflat_quad(xc, sv,
                                               2, 1, 3, 0,
                                               x, y, height, mc);

                    ++numProcessedPoints;
                    return;

                    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
                    cout << "\tclon = " << clon << endl;
                    cout << "\tylat[1] = " << ylat[1] << ", ylat[2] = " << ylat[2] << endl;
                    cout << "\txlon[1] = " << xlon[1] << ", xlon[2] = " << xlon[2] << endl;
                    cout << "\tylat[0] = " << ylat[0] << ", ylat[3] = " << ylat[3] << endl;
                    cout << "\txlon[0] = " << xlon[0] << ", xlon[3] = " << xlon[3] << endl;
                }
                else
                {
                    if(xlon[2] < 0.001)
                    {
                        if((xlon[0] > clon) && (xlon[1] > clon) && (xlon[3] > clon))
                        {
                            ++numProcessedPoints;
                            return;
                        }
                    }

                    if(xlon[2] <  clon)
                    {
                        if((xlon[0] > 359.99) && (xlon[1] > 359.99) && (xlon[3] > 359.99))
                        {
                            ++numProcessedPoints;
                            return;
                        }
                    }

                    process_selectedXflat_quad(xc, sv,
                                               2, 1, 2, 3,
                                               x, y, height, mc);

                    ++numProcessedPoints;
                    return;

                    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
                    cout << "\tclon = " << clon << endl;
                    cout << "\tylat[1] = " << ylat[1] << ", ylat[2] = " << ylat[2] << endl;
                    cout << "\txlon[1] = " << xlon[1] << ", xlon[2] = " << xlon[2] << endl;
                    cout << "\tylat[0] = " << ylat[0] << ", ylat[3] = " << ylat[3] << endl;
                    cout << "\txlon[0] = " << xlon[0] << ", xlon[3] = " << xlon[3] << endl;
                }
            }
            else
            {
                if(xlon[3] <= clon) 
                {
                    if(xlon[3] < 0.001)
                    {
                        if((xlon[0] > clon) && (xlon[1] > clon) && (xlon[2] > clon))
                        {
                            ++numProcessedPoints;
                            return;
                        }
                    }

                    if((xlon[0] > 359.99) && (xlon[1] > 359.99) && (xlon[2] > 359.99))
                    {
                        ++numProcessedPoints;
                        return;
                    }


                    process_selectedXflat_quad(xc, sv,
                                               3, 0, 3, 2,
                                               x, y, height, mc);

                    ++numProcessedPoints;
                    return;

                    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
                    cout << "\tclon = " << clon << endl;
                    cout << "\tylat[1] = " << ylat[1] << ", ylat[2] = " << ylat[2] << endl;
                    cout << "\tylat[0] = " << ylat[0] << ", ylat[3] = " << ylat[3] << endl;
                    cout << "\txlon[1] = " << xlon[1] << ", xlon[2] = " << xlon[2] << endl;
                    cout << "\txlon[0] = " << xlon[0] << ", xlon[3] = " << xlon[3] << endl;
                }
                else
                {
                  //Do not think we need to do anything if it reaches here.
                  //They are more likely bogus.
                    ++numProcessedPoints;
                    return;

                    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
                    cout << "\tclon = " << clon << endl;
                    cout << "\tylat[1] = " << ylat[1] << ", ylat[2] = " << ylat[2] << endl;
                    cout << "\tylat[0] = " << ylat[0] << ", ylat[3] = " << ylat[3] << endl;
                    cout << "\txlon[1] = " << xlon[1] << ", xlon[2] = " << xlon[2] << endl;
                    cout << "\txlon[0] = " << xlon[0] << ", xlon[3] = " << xlon[3] << endl;
                }
            }
        }
    }
}

void CAMse2dViewer::process_selectedXflat_quad(double xc, double sv,
                                               int il0, int ih0, 
                                               int il1, int ih1,
                                               double* x, double* y, double* height,
                                               int* mc)
{
    int k;
    double fact;

    double val;
    double yc[2];
    double xa[2];

    if(il0 == ih0)
    {
        xa[0] = 0.5;
        yc[0] = y[il0];
    }
    else
        yc[0] = linInterp(xc, x[il0], x[ih0], y[il0], y[ih0], xa[0]);

    yc[1] = linInterp(xc, x[il1], x[ih1], y[il1], y[ih1], xa[1]);

#if 0
    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
    cout << "\txc = " << xc << ", height[0] = " << height[0] << ", height[" << lev-1 << "] = " << height[lev-1] << endl;
    cout << "\tyc[0] = " << yc[0] << endl;
    cout << "\tyc[1] = " << yc[1] << endl;
  //cout << "\ty[" << il0 << "] = " << y[il0] << ", y[" << ih0 << "] = " << y[ih0] << endl;
  //cout << "\ty[" << il1 << "] = " << y[il1] << ", y[" << ih1 << "] = " << y[ih1] << endl;
  //cout << "\tx[" << il0 << "] = " << x[il0] << ", x[" << ih0 << "] = " << x[ih0] << endl;
  //cout << "\tx[" << il1 << "] = " << x[il1] << ", x[" << ih1 << "] = " << x[ih1] << endl;
#endif

    glBegin(GL_QUAD_STRIP);
        for(k = 0; k < lev; ++k)
        {
            val = (1.0 - xa[0]) * pltvar[mc[il0]+k*ncol]
                       + xa[0]  * pltvar[mc[ih0]+k*ncol]; 
            fact = sv * (val - _valmin);
            glTexCoord1d(fact);
            glVertex3d(xc, yc[0], height[k]);

            val = (1.0 - xa[1]) * pltvar[mc[il1]+k*ncol]
                       + xa[1]  * pltvar[mc[ih1]+k*ncol]; 
            fact = sv * (val - _valmin);
            glTexCoord1d(fact);
            glVertex3d(xc, yc[1], height[k]);
        }
    glEnd();
}

void CAMse2dViewer::_sphereBump()
{
    int i, j, k, m, n;

    double sv = 1.0;
    double fact;
    double radius = 1.001;

    double amp = 0.1;
    double offset = 0.5;

    k = nvoptions->get_zsec();

  //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\t_varname: <" << _varname << ">, lev = " << lev << endl;
  //cout << "\tncenters = " << ncenters << ", nvoptions->get_zsec() = " << nvoptions->get_zsec() << endl;


    if(1 < lev)
    {
        radius = 0.725 + 0.5 * ( 1.0 - (k + 1.0) / lev);
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

  //glPopMatrix();

  //glPushMatrix();

    glDisable(GL_LIGHTING);

    glColor4f(1.0, 1.0, 1.0, 1.0);
  //glColor4f(0.0, 0.0, 0.0, 0.0);

  //glEnable(GL_TEXTURE_1D);
  //glBindTexture(GL_TEXTURE_1D, texture1d->get_textureID());

    glBegin(GL_QUADS);
  //#pragma omp parallel for
    for(n = 0; n < ncenters; ++n)
    {
        j = n * ncorners;
        for(i = 0; i < ncorners; ++i)
        {
            m = element_corners[j+i];
            fact = sv * (pltvar[m+k*ncol] - _valmin);
            _lonlat2xyz(lon[m], lat[m], radius + amp * (fact - offset), fact);
        }
    }
    glEnd();

    glDisable(GL_TEXTURE_1D);

    glPopMatrix();

    glEndList();
}

void CAMse2dViewer::_flatBump()
{
    int i, j, k, m, n, num;

    int numProcessedBoundaryPoints = 0;

    double sv = 1.0;
    double alpha, fact;
    double height = 0.00;

    double xb[ncorners];
    double yb[ncorners];
    double vb[ncorners];
    double fb[ncorners];

    double amp = 0.2;
    double offset = 0.5;

    k = nvoptions->get_zsec();

    if(1 < lev)
        height = 0.8 * (0.5 - (k + 1.0) / lev);
    else
        height = 0.001;

  //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\t_varname: <" << _varname << ">, lev = " << lev << endl;
  //cout << "\tnvoptions->get_zsec() = " << nvoptions->get_zsec() << endl;
  //cout << "\tcurrent_timelevel = " << current_timelevel << endl;

    sv = 1.0 / (_valmax - _valmin);

  //zcl = glGenLists(1);
  //glNewList(zcl, GL_COMPILE);
  //glNewList(zcl, GL_COMPILE_AND_EXECUTE);
  //lister->set_zid(k, zcl);

  //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\t_valmin = " << _valmin << ", _valmax = " << _valmax << ", sv = " << sv << endl;
  //cout << "\tzcl = " << zcl << endl;

    glPopMatrix();

    glClearColor(1.0, 1.0, 1.0, 1.0);

  //OpenGL should normalize normal vectors
    glEnable(GL_NORMALIZE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

  //glShadeModel(GL_SMOOTH);

    earth->bump_plane(height - 0.1);

    glPopMatrix();

    glPushMatrix();

  //glEnable(GL_TEXTURE_1D);
  //glBindTexture(GL_TEXTURE_1D, texture1d->get_textureID());

    glNormal3f(0.0, 0.0, -1.0);
    glBegin(GL_QUADS);
    for(n = 0; n < ncenters; ++n)
    {
        if(_innerPoints[n])
        {
            j = n * ncorners;

            for(i = 0; i < ncorners; ++i)
            {
                m = element_corners[j+i];
                fact = sv * (pltvar[m+k*ncol] - _valmin);

                if(nvoptions->get_cb(NV_BUMPON))
                {
                    alpha = 1.125 * fact;
                    if(alpha < 0.1)
                        alpha = 0.0;
                    if(alpha > 1.0)
                        alpha = 1.0;
                    glColor4d(1.5*fact, 1.5*fact, 1.5*fact, alpha);
                }
                else
                    glTexCoord1d(fact);

                glVertex3d(_xFlat[m], _yFlat[m], height + amp * (fact - offset));
            }
        }
      //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << ", n = " << n << endl;
    }
    glEnd();

  //handle South Pole
    for(num = 0; num < num_south_pole_cols; ++num)
    {
        n = south_pole_cols[num];

        j = n * ncorners;

        for(i = 0; i < ncorners; ++i)
        {
            m = element_corners[j+i];
            vb[i] = pltvar[m+k*ncol];
          //fb[i] = sv * (vb[i] - _valmin);
            fb[i] = 1.0 - sv * (vb[i] - _valmin);

            xb[i] = _xFlat[m];
            yb[i] = _yFlat[m];
        }

      //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << ", num = " << num << endl;

      //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__
      //     << ", line: " << __LINE__ << endl;
      //cout << "\txb[0] = " << xb[0] << ", xb[1] = " << xb[1] << ", xb[2] = " << xb[2] << ", xb[3] = " << xb[3] << endl;
      //cout << "\tyb[0] = " << yb[0] << ", yb[1] = " << yb[1] << ", yb[2] = " << yb[2] << ", yb[3] = " << yb[3] << endl;

        if(NEAR_SOUTH_POLE > yb[0])
        {
          //Point 0 at South pole
          //Make 2 quads: 0, 1, 0, 2 and 0, 2, 0, 3.

            ++numProcessedBoundaryPoints;

            glBegin(GL_QUAD_STRIP);

            glTexCoord1d(fb[0]);
            glVertex3d(xb[1], yb[0], height);

            glTexCoord1d(fb[1]);
            glVertex3d(xb[1], yb[1], height);

            glTexCoord1d(fb[0]);
            glVertex3d(xb[2], yb[0], height);

            glTexCoord1d(fb[2]);
            glVertex3d(xb[2], yb[2], height);

            glTexCoord1d(fb[0]);
            glVertex3d(xb[3], yb[0], height);

            glTexCoord1d(fb[3]);
            glVertex3d(xb[3], yb[3], height);

            glEnd();

            continue;
        }
        else if(NEAR_SOUTH_POLE > yb[1])
        {
          //Point 1 at South pole
          //Make 2 quads: 1, 0, 1, 2 and 1, 2, 1, 3.

            if((NEAR_NEGATIVE_ZERO < xb[2]) && (NEAR_NEGATIVE_ZERO < xb[3]) && (NEAR_WEST_BOUNDARY > xb[0]))
                xb[0] = 1.0;

            ++numProcessedBoundaryPoints;

            glBegin(GL_QUAD_STRIP);

            glTexCoord1d(fb[1]);
            glVertex3d(xb[0], yb[1], height);

            glTexCoord1d(fb[0]);
            glVertex3d(xb[0], yb[0], height);

            glTexCoord1d(fb[1]);
            glVertex3d(xb[3], yb[1], height);

            glTexCoord1d(fb[3]);
            glVertex3d(xb[3], yb[3], height);

            glTexCoord1d(fb[1]);
            glVertex3d(xb[2], yb[1], height);

            glTexCoord1d(fb[2]);
            glVertex3d(xb[2], yb[2], height);

            glEnd();

            continue;
        }
        else if(NEAR_SOUTH_POLE > yb[2])
        {
          //Point 2 at South pole
          //Make 2 quads: 2, 0, 2, 1 and 2, 1, 2, 3.

            ++numProcessedBoundaryPoints;

            glBegin(GL_QUAD_STRIP);

            glTexCoord1d(fb[2]);
            glVertex3d(xb[1], yb[2], height);

            glTexCoord1d(fb[1]);
            glVertex3d(xb[1], yb[1], height);

            glTexCoord1d(fb[2]);
            glVertex3d(xb[0], yb[2], height);

            glTexCoord1d(fb[0]);
            glVertex3d(xb[0], yb[0], height);

            glTexCoord1d(fb[2]);
            glVertex3d(xb[3], yb[2], height);

            glTexCoord1d(fb[3]);
            glVertex3d(xb[3], yb[3], height);

            glEnd();

            continue;
        }
        else if(NEAR_SOUTH_POLE > yb[3])
        {
          //Point 3 at South pole
          //Make 2 quads: 3, 0, 3, 1 and 3, 1, 3, 2.

            ++numProcessedBoundaryPoints;

            glBegin(GL_QUAD_STRIP);

            glTexCoord1d(fb[3]);
            glVertex3d(xb[2], yb[3], height);

            glTexCoord1d(fb[2]);
            glVertex3d(xb[2], yb[2], height);

            glTexCoord1d(fb[3]);
            glVertex3d(xb[1], yb[3], height);

            glTexCoord1d(fb[1]);
            glVertex3d(xb[1], yb[1], height);

            glTexCoord1d(fb[3]);
            glVertex3d(xb[0], yb[3], height);

            glTexCoord1d(fb[0]);
            glVertex3d(xb[0], yb[0], height);

            glEnd();

            continue;
        }
        else
        {
            cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__
                 << ", line: " << __LINE__ << endl;
            cout << "\txb[0] = " << xb[0] << ", xb[1] = " << xb[1] << ", xb[2] = " << xb[2] << ", xb[3] = " << xb[3] << endl;
            cout << "\tyb[0] = " << yb[0] << ", yb[1] = " << yb[1] << ", yb[2] = " << yb[2] << ", yb[3] = " << yb[3] << endl;
        }
    }

  //Handle North Pole
    for(num = 0; num < num_north_pole_cols; ++num)
    {
        n = north_pole_cols[num];

        j = n * ncorners;

        for(i = 0; i < ncorners; ++i)
        {
            m = element_corners[j+i];
            vb[i] = pltvar[m+k*ncol];
          //fb[i] = sv * (vb[i] - _valmin);
            fb[i] = 1.0 - sv * (vb[i] - _valmin);

            xb[i] = _xFlat[m];
            yb[i] = _yFlat[m];
        }

      //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << ", num = " << num << endl;

      //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__
      //     << ", line: " << __LINE__ << endl;
      //cout << "\txb[0] = " << xb[0] << ", xb[1] = " << xb[1] << ", xb[2] = " << xb[2] << ", xb[3] = " << xb[3] << endl;
      //cout << "\tyb[0] = " << yb[0] << ", yb[1] = " << yb[1] << ", yb[2] = " << yb[2] << ", yb[3] = " << yb[3] << endl;

        if(NEAR_NORTH_POLE < yb[0])
        {
          //Point 0 is at North Pole.
          //Make 2 quads: 0, 1, 0, 2 and 0, 2, 0, 3.

            if((NEAR_NEGATIVE_ZERO < xb[2]) && (NEAR_NEGATIVE_ZERO < xb[3]) && (NEAR_WEST_BOUNDARY > xb[1]))
                xb[1] = 1.0;

            ++numProcessedBoundaryPoints;

            glBegin(GL_QUAD_STRIP);

            glTexCoord1d(fb[0]);
            glVertex3d(xb[1], yb[0], height);

            glTexCoord1d(fb[1]);
            glVertex3d(xb[1], yb[1], height);

            glTexCoord1d(fb[0]);
            glVertex3d(xb[2], yb[0], height);

            glTexCoord1d(fb[2]);
            glVertex3d(xb[2], yb[2], height);

            glTexCoord1d(fb[0]);
            glVertex3d(xb[3], yb[0], height);

            glTexCoord1d(fb[3]);
            glVertex3d(xb[3], yb[3], height);

            glEnd();

            continue;
        }
        else if(NEAR_NORTH_POLE < yb[1])
        {
          //Point 1 is at North Pole.
          //Make 2 quads: 1, 0, 1, 2 and 1, 2, 1, 3.

            ++numProcessedBoundaryPoints;

            glBegin(GL_QUAD_STRIP);

            glTexCoord1d(fb[1]);
            glVertex3d(xb[0], yb[1], height);

            glTexCoord1d(fb[0]);
            glVertex3d(xb[0], yb[0], height);

            glTexCoord1d(fb[1]);
            glVertex3d(xb[3], yb[1], height);

            glTexCoord1d(fb[3]);
            glVertex3d(xb[3], yb[3], height);

            glTexCoord1d(fb[1]);
            glVertex3d(xb[2], yb[1], height);

            glTexCoord1d(fb[2]);
            glVertex3d(xb[2], yb[2], height);

            glEnd();

            continue;
        }
        else if(NEAR_NORTH_POLE < yb[2])
        {
          //Point 2 is at North Pole.
          //Make 2 quads: 2, 0, 2, 1 and 2, 1, 2, 3. 

            ++numProcessedBoundaryPoints;

            glBegin(GL_QUAD_STRIP);

            glTexCoord1d(fb[2]);
            glVertex3d(xb[1], yb[2], height); 

            glTexCoord1d(fb[1]);
            glVertex3d(xb[1], yb[1], height); 

            glTexCoord1d(fb[2]);
            glVertex3d(xb[0], yb[2], height);

            glTexCoord1d(fb[0]);
            glVertex3d(xb[0], yb[0], height);

            glTexCoord1d(fb[2]);
            glVertex3d(xb[3], yb[2], height); 

            glTexCoord1d(fb[3]);
            glVertex3d(xb[3], yb[3], height);

            glEnd();

            continue;
        }
        else if(NEAR_NORTH_POLE < yb[3])
        {
          //Point 3 is at North Pole.
          //Make 2 quads: 3, 0, 3, 1 and 3, 1, 3, 2.

            ++numProcessedBoundaryPoints;

            glBegin(GL_QUAD_STRIP);

            glTexCoord1d(fb[3]);
            glVertex3d(xb[2], yb[3], height);

            glTexCoord1d(fb[2]);
            glVertex3d(xb[2], yb[2], height);

            glTexCoord1d(fb[3]);
            glVertex3d(xb[1], yb[3], height);

            glTexCoord1d(fb[1]);
            glVertex3d(xb[1], yb[1], height);

            glTexCoord1d(fb[3]);
            glVertex3d(xb[0], yb[3], height);

            glTexCoord1d(fb[0]);
            glVertex3d(xb[0], yb[0], height);

            glEnd();

            continue;
        }
        else
        {
            cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__
                 << ", line: " << __LINE__ << endl;
            cout << "\txb[0] = " << xb[0] << ", xb[1] = " << xb[1] << ", xb[2] = " << xb[2] << ", xb[3] = " << xb[3] << endl;
            cout << "\tyb[0] = " << yb[0] << ", yb[1] = " << yb[1] << ", yb[2] = " << yb[2] << ", yb[3] = " << yb[3] << endl;
        }
    }

  //The middle boundary
    for(num = 0; num < num_boundary_cols; ++num)
    {
        n = boundary_cols[num];

        j = n * ncorners;

        for(i = 0; i < ncorners; ++i)
        {
            m = element_corners[j+i];
            vb[i] = pltvar[m+k*ncol];
          //fb[i] = sv * (vb[i] - _valmin);
            fb[i] = 1.0 - sv * (vb[i] - _valmin);

            xb[i] = _xFlat[m];
            yb[i] = _yFlat[m];
        }

      //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << ", num = " << num << endl;

        if((NEAR_POSITIVE_ZERO > xb[0]) && (NEAR_POSITIVE_ZERO > xb[1]))
        {
            if((NEAR_EAST_BOUNDARY < xb[2]) && (NEAR_EAST_BOUNDARY < xb[3]))
            {
                xb[2] = -1.0;
                xb[3] = -1.0;

                ++numProcessedBoundaryPoints;

                glBegin(GL_QUADS);

                glTexCoord1d(fb[0]);
                glVertex3d(xb[0], yb[0], height);

                glTexCoord1d(fb[1]);
                glVertex3d(xb[1], yb[1], height);

                glTexCoord1d(fb[2]);
                glVertex3d(xb[2], yb[2], height);

                glTexCoord1d(fb[3]);
                glVertex3d(xb[3], yb[3], height);

                glEnd();

                continue;
            }

            if((NEAR_EAST_BOUNDARY < xb[2]) && (NEAR_WEST_BOUNDARY > xb[3])) 
            {
                xb[2] = -1.0;

                ++numProcessedBoundaryPoints;

                glBegin(GL_QUADS);

                glTexCoord1d(fb[0]);
                glVertex3d(xb[0], yb[0], height);

                glTexCoord1d(fb[1]);
                glVertex3d(xb[1], yb[1], height);

                glTexCoord1d(fb[2]);
                glVertex3d(xb[2], yb[2], height);

                glTexCoord1d(fb[3]);
                glVertex3d(xb[3], yb[3], height);

                glEnd();

                continue;
            }

            if((NEAR_WEST_BOUNDARY > xb[2]) && (NEAR_EAST_BOUNDARY < xb[3]))
            {
                xb[3] = -1.0;

                ++numProcessedBoundaryPoints;

                glBegin(GL_QUADS);

                glTexCoord1d(fb[0]);
                glVertex3d(xb[0], yb[0], height);

                glTexCoord1d(fb[1]);
                glVertex3d(xb[1], yb[1], height);

                glTexCoord1d(fb[2]);
                glVertex3d(xb[2], yb[2], height);

                glTexCoord1d(fb[3]);
                glVertex3d(xb[3], yb[3], height);

                glEnd();

                continue;
            }
        }

        if((NEAR_POSITIVE_ZERO > xb[2]) && (NEAR_POSITIVE_ZERO > xb[3]))
        {
            if((NEAR_EAST_BOUNDARY < xb[0]) && (NEAR_EAST_BOUNDARY < xb[1])) 
            {
                xb[0] = -1.0;
                xb[1] = -1.0;

                ++numProcessedBoundaryPoints;

                glBegin(GL_QUADS);

                glTexCoord1d(fb[0]);
                glVertex3d(xb[0], yb[0], height);

                glTexCoord1d(fb[1]);
                glVertex3d(xb[1], yb[1], height);

                glTexCoord1d(fb[2]);
                glVertex3d(xb[2], yb[2], height);

                glTexCoord1d(fb[3]);
                glVertex3d(xb[3], yb[3], height);

                glEnd();

                continue;
            }

            if((NEAR_WEST_BOUNDARY > xb[0]) && (NEAR_EAST_BOUNDARY < xb[1])) 
            {
                xb[1] = -1.0;

                ++numProcessedBoundaryPoints;

                glBegin(GL_QUADS);

                glTexCoord1d(fb[0]);
                glVertex3d(xb[0], yb[0], height);

                glTexCoord1d(fb[1]);
                glVertex3d(xb[1], yb[1], height);

                glTexCoord1d(fb[2]);
                glVertex3d(xb[2], yb[2], height);

                glTexCoord1d(fb[3]);
                glVertex3d(xb[3], yb[3], height);

                glEnd();

                continue;
            }

            if((NEAR_EAST_BOUNDARY < xb[0]) && (NEAR_WEST_BOUNDARY > xb[1]))
            {
                xb[0] = -1.0;

                ++numProcessedBoundaryPoints;

                glBegin(GL_QUADS);

                glTexCoord1d(fb[0]);
                glVertex3d(xb[0], yb[0], height);

                glTexCoord1d(fb[1]);
                glVertex3d(xb[1], yb[1], height);

                glTexCoord1d(fb[2]);
                glVertex3d(xb[2], yb[2], height);

                glTexCoord1d(fb[3]);
                glVertex3d(xb[3], yb[3], height);

                glEnd();

                continue;
            }
        }

        if((NEAR_NEGATIVE_ZERO < xb[2]) && (NEAR_NEGATIVE_ZERO < xb[3]))
        {
            if((NEAR_WEST_BOUNDARY > xb[0]) && (NEAR_WEST_BOUNDARY > xb[1]))
            {
                xb[0] = 1.0;
                xb[1] = 1.0;

                ++numProcessedBoundaryPoints;

                glBegin(GL_QUADS);

                glTexCoord1d(fb[0]);
                glVertex3d(xb[0], yb[0], height);

                glTexCoord1d(fb[1]);
                glVertex3d(xb[1], yb[1], height);

                glTexCoord1d(fb[2]);
                glVertex3d(xb[2], yb[2], height);

                glTexCoord1d(fb[3]);
                glVertex3d(xb[3], yb[3], height);

                glEnd();

                continue;
            }

            if((NEAR_WEST_BOUNDARY > xb[0]) && (NEAR_EAST_BOUNDARY < xb[1]))
            {
                xb[0] = 1.0;

                ++numProcessedBoundaryPoints;

                glBegin(GL_QUADS);

                glTexCoord1d(fb[0]);
                glVertex3d(xb[0], yb[0], height);

                glTexCoord1d(fb[1]);
                glVertex3d(xb[1], yb[1], height);

                glTexCoord1d(fb[2]);
                glVertex3d(xb[2], yb[2], height);

                glTexCoord1d(fb[3]);
                glVertex3d(xb[3], yb[3], height);

                glEnd();

                continue;
            }

            if((NEAR_EAST_BOUNDARY < xb[0]) && (NEAR_WEST_BOUNDARY > xb[1]))
            {
                xb[1] = 1.0;

                ++numProcessedBoundaryPoints;

                glBegin(GL_QUADS);

                glTexCoord1d(fb[0]);
                glVertex3d(xb[0], yb[0], height);

                glTexCoord1d(fb[1]);
                glVertex3d(xb[1], yb[1], height);

                glTexCoord1d(fb[2]);
                glVertex3d(xb[2], yb[2], height);

                glTexCoord1d(fb[3]);
                glVertex3d(xb[3], yb[3], height);

                glEnd();

                continue;
            }
        }

        cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__
             << ", line: " << __LINE__ << endl;
        cout << "\tnum = " << num << endl;
        cout << "\tyb[1] = " << yb[1] << ", yb[2] = " << yb[2] << endl;
        cout << "\txb[1] = " << xb[1] << ", xb[2] = " << xb[2] << endl;
        cout << "\tyb[0] = " << yb[0] << ", yb[3] = " << yb[3] << endl;
        cout << "\txb[0] = " << xb[0] << ", xb[3] = " << xb[3] << endl;
    }

    if(numProcessedBoundaryPoints != _nBoundaryPoints)
    {
        cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
        cout << "\tnumProcessedBoundaryPoints = " << numProcessedBoundaryPoints
             << ", _nBoundaryPoints = " << _nBoundaryPoints << endl;
    }

    glDisable(GL_TEXTURE_1D);

    glPopMatrix();

  //glEndList();
}

void CAMse2dViewer::_draw_cross(double radius)
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

