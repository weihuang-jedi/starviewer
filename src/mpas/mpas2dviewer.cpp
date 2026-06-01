//$Id: mpas2dviewer.cpp 5194 2014-08-18 19:51:52Z starviewer $

#include <QtOpenGL>

#include <mpas2dviewer.h>

#define	MINIDIST	1.0e-6

#define	GL_PI		3.1415926535897932
#define	ARC2DEG		(180.0/GL_PI)
#define DEG2ARC		(GL_PI/180.0)

#define LOWBOUND	45.0
#define HIGHBOUND	315.0

#define NEAR_NORTH_POLE	(90.0 - MINIDIST)
#define NEAR_SOUTH_POLE	(MINIDIST - 90.0)

#define	BLANKRANGE	9
#define FAR_NORTH	(90 - BLANKRANGE)
#define FAR_SOUTH	(BLANKRANGE - 90)

MPAS2dViewer::MPAS2dViewer(ColorTable *ct, NVOptions* opt)
{
    colorTable = ct;
    nvoptions = opt;

    coastline = new CoastLine();

    texture1d = new Texture1d();
    texture1d->set_colors(ct->get_clen(), ct->get_cmap());
    texture1d->set_name(ct->get_name());

    locator = NULL;

    lister = new Lister();
    lister->setup(361, 181, 121);

    evaluator = new Evaluator();

    _var = NULL;
    _zlist = NULL;

    nvoptions->set_xsec(360);
    nvoptions->set_xsec2(360);
    nvoptions->set_ysec(180);
    nvoptions->set_ysec2(180);
    nvoptions->set_zsec(0);
}

MPAS2dViewer::~MPAS2dViewer()
{
    if(NULL != locator)
        locator->turnOff();
    delete lister;
    delete coastline;
    delete texture1d;
    delete evaluator;
}

void MPAS2dViewer::set_geometry(MPASGeometry *gm)
{
    geometry = gm;

    _initialize();
}

void MPAS2dViewer::setup(string vn, float *var)
{
    size_t gridsize;

    reset();

    _varname  = vn;
    _var = var;

  //gridsize = geometry->get_nt() * geometry->get_nCells() * geometry->get_nVertLevels();
  //evaluator->set_value(gridsize, _var);

    gridsize = geometry->get_nCells() * geometry->get_nVertLevels();
  //evaluator->set_value(gridsize, &_var[3*gridsize]);
    evaluator->set_value(gridsize, _var);

    _minval = evaluator->get_min();
    _maxval = evaluator->get_max();

  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\tgeometry->get_nt() = " << geometry->get_nt() << endl;
  //cout << "\t_minval = " << _minval << ", _maxval = " << _maxval << endl;

    nvoptions->set_trueminimum(_minval);
    nvoptions->set_truemaximum(_maxval);
  //_workMinVal = _minval;
  //_workMaxVal = _maxval;
}

void MPAS2dViewer::reset()
{
    lister->reinitialize(361, 181, geometry->get_nVertLevels()+1);
}

void MPAS2dViewer::_initialize()
{
    lister->reinitialize(361, 181, geometry->get_nVertLevels()+1);
}

void MPAS2dViewer::draw()
{
    zcl = lister->get_zid(nvoptions->get_zsec());
    ycl = lister->get_yid(nvoptions->get_ysec()+90);
    xcl = lister->get_xid(nvoptions->get_xsec());

    if(nvoptions->get_cb(NV_RESET))
    {
        _subsetVertices.clear();
        nvoptions->set_cb(NV_RESET, false);
        reset();
    }

    if(nvoptions->get_cb(NV_HASMINMAX))
    {
        _workMinVal = nvoptions->get_userminimum();
        _workMaxVal = nvoptions->get_usermaximum();
    }
    else
    {
        _workMinVal = nvoptions->get_trueminimum();
        _workMaxVal = nvoptions->get_truemaximum();
    }

    if(nvoptions->get_cb(NV_MPAS_SUBSET))
    {
        if((geometry->get_nVertLevels() > nvoptions->get_zsec()) && (0 <= nvoptions->get_zsec()))
        {
            if(zcl)
                glCallList(zcl);
            else
            {
                if(nvoptions->get_cb(NV_SUBSET))
                {
                    if(nvoptions->get_cb(NV_FLATON))
                        _flatSubset();
                    else
                        _sphereSubset();
                }
            }
        }

        return;
    }

  //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\t_varname: <" << _varname << ">, zcl = " << zcl << ", xcl = " << xcl << endl;

    if(nvoptions->get_cb(NV_FLATON))
    {
        if((geometry->get_nVertLevels() > nvoptions->get_zsec()) && (0 <= nvoptions->get_zsec()))
        {
            if(zcl)
                glCallList(zcl);
            else
            {
                if(nvoptions->get_cb(NV_SUBSET))
                    _flatSubset();
                else
                    _flatDisplay();
            }
        }

        if(geometry->get_nVertLevels() > 1)
        {
            if((360 > nvoptions->get_xsec()) && (0 <= nvoptions->get_xsec()))
            {
              //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
              //cout << "\t_varname: <" << _varname << ">, xcl = " << xcl << endl;

                if(xcl)
                    glCallList(xcl);
                else
                    _flatDisplayXsec();
            }

            if((FAR_NORTH > nvoptions->get_ysec()) && (FAR_SOUTH < nvoptions->get_ysec()))
            {
              //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
              //cout << "\t_varname: <" << _varname << ">, ycl = " << ycl << endl;

                if(ycl)
                    glCallList(ycl);
                else
                    _flatDisplayYsec();
            }
        }
    }
    else
    {
        if((geometry->get_nVertLevels() > nvoptions->get_zsec()) && (0 <= nvoptions->get_zsec()))
        {
            if(zcl)
                glCallList(zcl);
            else
            {
                if(nvoptions->get_cb(NV_SUBSET))
                    _sphereSubset();
                else
                    _sphereDisplay();
            }
        }

        if(geometry->get_nVertLevels() > 1)
        {
            if((360 > nvoptions->get_xsec()) && (0 <= nvoptions->get_xsec()))
            {
              //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
              //cout << "\t_varname: <" << _varname << ">, xcl = " << xcl << endl;

                if(xcl)
                    glCallList(xcl);
                else
                    _sphereDisplayXsec();
            }

            if((FAR_NORTH > nvoptions->get_ysec()) && (FAR_SOUTH < nvoptions->get_ysec()))
            {
              //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
              //cout << "\t_varname: <" << _varname << ">, ycl = " << ycl << endl;

                if(ycl)
                    glCallList(ycl);
                else
                    _sphereDisplayYsec();
            }
        }
    }
}

pnt3d MPAS2dViewer::nvop(pnt3d p, double height)
{
    pnt3d np;

    np.x = p.x / height;
    np.y = p.y / height;
    np.z = p.z / height;

    return np;
}

void MPAS2dViewer::_sphereDisplay()
{
    zcl = 0;
    int c, v, i, k, n;

    int nCells         = geometry->get_nCells();
    int nVertLevels    = geometry->get_nVertLevels();
    int nVertices      = geometry->get_nVertices();
    int vertexDegree   = geometry->get_vertexDegree();
    int* cellsOnVertex = geometry->get_cellsOnVertex();

    double sv = 1.0;
    double fact;
    float radius = 1.001;

    pnt3d pnt;
    pnt3d norm;

    float* current_var_ptr;

    k = nvoptions->get_zsec();

  //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\t_varname: <" << _varname << ">" << endl;
  //cout << "\tnVertLevels = " << nVertLevels << ", nvoptions->get_zsec() = " << nvoptions->get_zsec() << endl;
  //cout << "\tnvoptions->get_tsec() = " << nvoptions->get_tsec() << ", vertexDegree = " << vertexDegree << endl;
  //cout << "\tnCells = " << nCells << ", nVertices = " << nVertices << endl;

    current_var_ptr = &_var[nCells * nVertLevels * nvoptions->get_tsec()];

    zcl = lister->get_zid(k);

    if(0 < zcl)
        return;

    radius = 1.001 + 0.5 * (float) k / (float) nVertLevels;

    locator->set_height(radius);

    sv = 1.0 / (_workMaxVal - _workMinVal);

    zcl = glGenLists(1);
  //glNewList(zcl, GL_COMPILE);
    glNewList(zcl, GL_COMPILE_AND_EXECUTE);

  //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\tzcl = " << zcl << endl;

    glPushMatrix();

    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1, 1);

    glColor3d(1.0, 1.0, 1.0);

    glNormal3d(0.0, 0.0, 1.0);

    glEnable(GL_TEXTURE_1D);
    glBindTexture(GL_TEXTURE_1D, texture1d->get_textureID());

    glBegin(GL_TRIANGLES);
    for(v = 0; v < nVertices; ++v)
    {
        for(i = 0; i < vertexDegree; i++)
        {
            c = cellsOnVertex[i+v*vertexDegree];
            n = nVertLevels * c + k;
            fact = sv * (current_var_ptr[n] - _workMinVal);

            pnt = geometry->polar2xyz(c, radius);

            norm = nvop(pnt, radius);
            glNormal3d(norm.x, norm.y, norm.z);
            glTexCoord1d(fact);
            glVertex3d(pnt.x, pnt.y, pnt.z);
        }
    }
    glEnd();

    coastline->drawOnSphere(radius+0.01);

    glDisable(GL_POLYGON_OFFSET_FILL);
    glDisable(GL_TEXTURE_1D);

#if 0
    if(locator->on())
    {
      //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
      //cout << "\tlocation = (" << locator->x() << ", " << locator->y() << ")" << endl;

        _draw_cross(radius + 0.01);

        if((NULL != nvoptions) && nvoptions->get_cb(NV_GRIDON))
            _draw_triangle(radius + 0.01);
    }
#endif

    glPopMatrix();

    glEndList();

    lister->set_zid(k, zcl);
}

void MPAS2dViewer::_flatDisplay()
{
    zcl = 0;
    int c, v, i, k, n, cp1, cp2;

    int nVertLevels    = geometry->get_nVertLevels();
    int nVertices      = geometry->get_nVertices();
    int nCells         = geometry->get_nCells();
    int vertexDegree   = geometry->get_vertexDegree();
    int* cellsOnVertex = geometry->get_cellsOnVertex();

    float sv = 1.0;
    float fact;
    float height = 0.0;

    float x[5];
    float y[5];
  //float z[5];
    float val[5];
    float dx;
  //float dy;

    float* current_var_ptr;

    pnt2d pnt;
    pnt2d pnt0;
    pnt2d pnt1;
    pnt2d pnt2;

    k = nvoptions->get_zsec();

    current_var_ptr = &_var[nCells * nVertLevels * nvoptions->get_tsec()];

    zcl = lister->get_zid(k);

    if(0 < zcl)
        return;

    zcl = glGenLists(1);
  //glNewList(zcl, GL_COMPILE);
    glNewList(zcl, GL_COMPILE_AND_EXECUTE);

    if(1 < nVertLevels)
        height = (float) k / (float) nVertLevels - 0.5;
    else
        height = 0.0;

    locator->set_height(height);

    _innerPoints = geometry->get_innerPoints();
    _nBoundaryPoints = geometry->get_nBoundaryPoints();
    _boundaryType = geometry->get_boundaryType();
    _boundaryPoints = geometry->get_boundaryPoints();

    sv = 1.0 / (_workMaxVal - _workMinVal);

  //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\t_varname: <" << _varname << ">" << endl;
  //cout << "\tnVertices = " << nVertices << ", nvoptions->get_zsec() = " << nvoptions->get_zsec() << endl;
  //cout << "\t_nBoundaryPoints = " << _nBoundaryPoints << endl;

    glPushMatrix();

    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1, 1);

    glColor3f(1.0, 1.0, 1.0);

    glNormal3f(0.0, 0.0, 1.0);

    glEnable(GL_TEXTURE_1D);
    glBindTexture(GL_TEXTURE_1D, texture1d->get_textureID());

    glBegin(GL_TRIANGLES);
    for(v = 0; v < nVertices; ++v)
    {
        if(_innerPoints[v])
        {
            for(i = 0; i < vertexDegree; i++)
            {
                c = cellsOnVertex[i+v*vertexDegree];
                n = nVertLevels * c + k;
                fact = sv * (current_var_ptr[n] - _workMinVal);
                glTexCoord1d(fact);

                pnt = geometry->latlon2xy(c);
                glVertex3d(pnt.x, pnt.y, height);
            }
        }
    }

  //Handling triangles across longitude 0.
  //  _boundaryType            _boundaryType
  //         west                        west
  //   -2    lon 0               -1      lon 0
  //      0 .  |                           |  . 0
  //        |\ |                           | /|
  //        | \|                           |/ |
  //        |  + 1                       1 +  |
  //        |  |\                         /|  |
  //        |  | . 2                   2 . |  |
  //        |  |/                         \|  |
  //        |  + 3                       3 +  |
  //        | /|                           |\ |
  //        |/ |                           | \|
  //      4 .  |                           |  . 4
  //
  //    2                         1
  //         east                        east

    for(v = 0; v < _nBoundaryPoints; ++v)
    {
        c = _boundaryPoints[3*v];
        cp1 = _boundaryPoints[3*v+1];
        cp2 = _boundaryPoints[3*v+2];

        pnt0 = geometry->latlon2xy(c);
        pnt1 = geometry->latlon2xy(cp1);
        pnt2 = geometry->latlon2xy(cp2);

        switch (_boundaryType[v])
        {
            case -2:
                 x[0] = pnt0.x - 2.0;
                 x[2] = pnt2.x;
                 x[4] = pnt1.x - 2.0;

                 y[0] = pnt0.y;
                 y[2] = pnt2.y;
                 y[4] = pnt1.y;

                 val[0] = current_var_ptr[nVertLevels * c + k];
                 val[2] = current_var_ptr[nVertLevels * cp2 + k];
                 val[4] = current_var_ptr[nVertLevels * cp1 + k];

                 x[1] = -1.0;
                 x[3] = -1.0;

                 dx = x[1] - x[0];
                 y[1] = (1.0 - dx) * y[0] + dx * y[2];
                 val[1] = (1.0 - dx) * val[0] + dx * val[2];
                 dx = x[3] - x[4];
                 y[3] = (1.0 - dx) * y[4] + dx * y[2];
                 val[3] = (1.0 - dx) * val[4] + dx * val[2];

               //Handle triangle 1,2,3
                 fact = sv * (val[1] - _workMinVal);
                 glTexCoord1d(fact);
                 glVertex3d(x[1], y[1], height);

                 fact = sv * (val[2] - _workMinVal);
                 glTexCoord1d(fact);
                 glVertex3d(x[2], y[2], height);

                 fact = sv * (val[3] - _workMinVal);
                 glTexCoord1d(fact);
                 glVertex3d(x[3], y[3], height);

               //Handle triangle 0,1,3, 1, 3, 4
               //change their coords.
                 for(n = 0; n < 5; ++n)
                 {
                     x[n] += 2.0;
                 }

                 break;

            case -1:
                 x[0] = pnt0.x;
                 x[2] = pnt2.x - 2.0;
                 x[4] = pnt1.x;

                 y[0] = pnt0.y;
                 y[2] = pnt2.y;
                 y[4] = pnt1.y;

                 val[0] = current_var_ptr[nVertLevels * c + k];
                 val[2] = current_var_ptr[nVertLevels * cp2 + k];
                 val[4] = current_var_ptr[nVertLevels * cp1 + k];

                 x[1] = -1.0;
                 x[3] = -1.0;

                 dx = x[0] - x[1];
                 y[1] = (1.0 - dx) * y[0] + dx * y[2];
                 val[1] = (1.0 - dx) * val[0] + dx * val[2];
                 dx = x[4] - x[3];
                 y[3] = (1.0 - dx) * y[4] + dx * y[2];
                 val[3] = (1.0 - dx) * val[4] + dx * val[2];

               //Handle triangle 1,2,3
                 fact = sv * (val[1] - _workMinVal);
                 glTexCoord1d(fact);
                 glVertex3d(x[1]+2.0, y[1], height);

                 fact = sv * (val[2] - _workMinVal);
                 glTexCoord1d(fact);
                 glVertex3d(x[2]+2.0, y[2], height);

                 fact = sv * (val[3] - _workMinVal);
                 glTexCoord1d(fact);
                 glVertex3d(x[3]+2.0, y[3], height);

                 break;

            case 1:
                 x[2] = pnt0.x;
                 x[0] = pnt1.x + 2.0;
                 x[4] = pnt2.x + 2.0;

                 y[2] = pnt0.y;
                 y[0] = pnt1.y;
                 y[4] = pnt2.y;

                 val[2] = current_var_ptr[nVertLevels * c + k];
                 val[0] = current_var_ptr[nVertLevels * cp1 + k];
                 val[4] = current_var_ptr[nVertLevels * cp2 + k];

                 x[1] = 1.0;
                 x[3] = 1.0;

                 dx = x[0] - x[1];
                 y[1] = (1.0 - dx) * y[0] + dx * y[2];
                 val[1] = (1.0 - dx) * val[0] + dx * val[2];
                 dx = x[4] - x[3];
                 y[3] = (1.0 - dx) * y[4] + dx * y[2];
                 val[3] = (1.0 - dx) * val[4] + dx * val[2];

               //Handle triangle 1,2,3
                 fact = sv * (val[1] - _workMinVal);
                 glTexCoord1d(fact);
                 glVertex3d(x[1], y[1], height);

                 fact = sv * (val[2] - _workMinVal);
                 glTexCoord1d(fact);
                 glVertex3d(x[2], y[2], height);

                 fact = sv * (val[3] - _workMinVal);
                 glTexCoord1d(fact);
                 glVertex3d(x[3], y[3], height);

               //Handle triangle 0,1,3, 1, 3, 4
               //change their coords.
                 for(n = 0; n < 5; ++n)
                 {
                     x[n] -= 2.0;
                 }

                 break;

            case 2:
                 x[0] = pnt0.x;
                 x[4] = pnt1.x;
                 x[2] = pnt2.x + 2.0;

                 y[0] = pnt0.y;
                 y[4] = pnt1.y;
                 y[2] = pnt2.y;

                 val[0] = current_var_ptr[nVertLevels * c + k];
                 val[4] = current_var_ptr[nVertLevels * cp1 + k];
                 val[2] = current_var_ptr[nVertLevels * cp2 + k];

                 x[1] = 1.0;
                 x[3] = 1.0;

                 dx = x[1] - x[0];
                 y[1] = (1.0 - dx) * y[0] + dx * y[2];
                 val[1] = (1.0 - dx) * val[0] + dx * val[2];
                 dx = x[3] - x[4];
                 y[3] = (1.0 - dx) * y[4] + dx * y[2];
                 val[3] = (1.0 - dx) * val[4] + dx * val[2];

               //Handle triangle 1,2,3
                 fact = sv * (val[1] - _workMinVal);
                 glTexCoord1d(fact);
                 glVertex3d(x[1]-2.0, y[1], height);

                 fact = sv * (val[2] - _workMinVal);
                 glTexCoord1d(fact);
                 glVertex3d(x[2]-2.0, y[2], height);

                 fact = sv * (val[3] - _workMinVal);
                 glTexCoord1d(fact);
                 glVertex3d(x[3]-2.0, y[3], height);

                 break;

            default:
                 cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
                 cout << "\tUnknown _boundaryType[" << v << " = " << _boundaryType[v] << endl;
                 break;
        }

      //Triangle 0, 1, 3
        fact = sv * (val[0] - _workMinVal);
        glTexCoord1d(fact);
        glVertex3d(x[0], y[0], height);

        fact = sv * (val[1] - _workMinVal);
        glTexCoord1d(fact);
        glVertex3d(x[1], y[1], height);

        fact = sv * (val[3] - _workMinVal);
        glTexCoord1d(fact);
        glVertex3d(x[3], y[3], height);

      //Triangle 0, 3, 4
        fact = sv * (val[0] - _workMinVal);
        glTexCoord1d(fact);
        glVertex3d(x[0], y[0], height);

        fact = sv * (val[3] - _workMinVal);
        glTexCoord1d(fact);
        glVertex3d(x[3], y[3], height);

        fact = sv * (val[4] - _workMinVal);
        glTexCoord1d(fact);
        glVertex3d(x[4], y[4], height);
    }
    glEnd();

    coastline->drawOnPlane(height+0.01);

    glDisable(GL_POLYGON_OFFSET_FILL);
    glDisable(GL_TEXTURE_1D);

    glPopMatrix();

    glEndList();

    lister->set_zid(k, zcl);
}

void MPAS2dViewer::reset_texture1d(ColorTable *ct)
{
    colorTable = ct;

    texture1d->reset();
    texture1d->set_colors(ct->get_clen(), ct->get_cmap());
    texture1d->set_name(ct->get_name());
}

void MPAS2dViewer::_sphereDisplayXsec()
{
    xcl = 0;
    int c, v, i, k, n;

  //int nCells         = geometry->get_nCells();
    int nVertLevels    = geometry->get_nVertLevels();
    int nVertices      = geometry->get_nVertices();
    int vertexDegree   = geometry->get_vertexDegree();
    int* cellsOnVertex = geometry->get_cellsOnVertex();

    double sv = 1.0;
    double fact;
    double height[nVertLevels];

    double* lon = NULL;
    double* lat = NULL;

    float tlon[3];
    float tlat[3];

    double clon = (double) nvoptions->get_xsec();
    double alon = DEG2ARC * clon;

    double alat[3];
    float dx;

    float val[3][nVertLevels];
    float cval[3][nVertLevels];

    bool need_shift_low  = false;
    bool need_shift_high = false;

    pnt3d pnt0;
    pnt3d pnt1;

    int idx[3];

    xcl = lister->get_xid(nvoptions->get_xsec());

    if(0 < xcl)
        return;

    xcl = glGenLists(1);
  //glNewList(xcl, GL_COMPILE);
    glNewList(xcl, GL_COMPILE_AND_EXECUTE);

  //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\t_varname: <" << _varname << ">" << endl;
  //cout << "\tnVertLevels = " << nVertLevels << ", nvoptions->get_xsec() = " << nvoptions->get_xsec() << endl;

    for(k = 0; k < nVertLevels; ++k)
        height[k] = 1.001 + 0.5 * (float) k / (float) nVertLevels;

    if(LOWBOUND > clon)
        need_shift_low = true;
    if(HIGHBOUND < clon)
        need_shift_high = true;

    lon = geometry->get_lonCell();
    lat = geometry->get_latCell();

    sv = 1.0 / (_workMaxVal - _workMinVal);

  //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\txcl = " << xcl << endl;

    glPushMatrix();

    glColor3f(1.0, 1.0, 1.0);

    glNormal3f(0.0, 0.0, -1.0);

    glEnable(GL_TEXTURE_1D);
    glBindTexture(GL_TEXTURE_1D, texture1d->get_textureID());

    for(v = 0; v < nVertices; ++v)
    {
        for(i = 0; i < vertexDegree; i++)
        {
            c = cellsOnVertex[i+v*vertexDegree];

            idx[i] = nVertLevels * c;
            tlon[i] = ARC2DEG * lon[c];
            tlat[i] = ARC2DEG * lat[c];

            if(NEAR_NORTH_POLE < tlat[i])
                tlon[i] = clon;
            if(NEAR_SOUTH_POLE > tlat[i])
                tlon[i] = clon;
        }

        if(need_shift_low)
        {
            for(i = 0; i < vertexDegree; i++)
            {
                if(HIGHBOUND < tlon[i])
                   tlon[i] -= 360.0;

                if(NEAR_NORTH_POLE < tlat[i])
                    tlon[i] = clon;
                if(NEAR_SOUTH_POLE > tlat[i])
                    tlon[i] = clon;
            }
        }

        if(need_shift_high)
        {
            for(i = 0; i < vertexDegree; i++)
            {
                if(LOWBOUND > tlon[i])
                   tlon[i] += 360.0;

                if(NEAR_NORTH_POLE < tlat[i])
                    tlon[i] = clon;
                if(NEAR_SOUTH_POLE > tlat[i])
                    tlon[i] = clon;
            }
        }

        if((tlon[0] > clon) && (tlon[1] > clon) && (tlon[2] > clon))
            continue;
        if((tlon[0] < clon) && (tlon[1] < clon) && (tlon[2] < clon))
            continue;

        if((180.0 < fabs(tlon[1]-tlon[0])) ||
           (180.0 < fabs(tlon[2]-tlon[0])) ||
           (180.0 < fabs(tlon[2]-tlon[1])))
        {
          //cout << "\n\tclon = " << clon << endl;

          //for(i = 0; i < vertexDegree; ++i)
          //{
          //    cout << "\ttlat[" << i << "] = " << tlat[i]
          //         << "\ttlon[" << i << "] = " << tlon[i] << endl;
          //}
            continue;
        }

        for(i = 0; i < vertexDegree; ++i)
        {
          //cout << "\ttlat[" << i << "] = " << tlat[i]
          //     << "\ttlon[" << i << "] = " << tlon[i] << endl;

            n = idx[i];
            for(k = 0; k < nVertLevels; ++k)
            {
                val[i][k] = _var[n+k];
            }
        }

        n = 0;

        if(((tlon[0] <= clon) && (tlon[1] >= clon)) ||
           ((tlon[0] >= clon) && (tlon[1] <= clon)))
        {
            fact = tlon[1] - tlon[0];
            if(MINIDIST < fabs(fact))
                dx = (clon - tlon[0]) / fact;
            else
                dx = 0.5;

            alat[n] = (1.0 - dx) * tlat[0] + dx * tlat[1];

            for(k = 0; k < nVertLevels; ++k)
            {
                cval[n][k] = (1.0 - dx) * val[0][k] + dx * val[1][k];
            }

          //cout << "\talat[" << n << "] = " << alat[n] << endl;
            alat[n] *= DEG2ARC;

            ++n;
        }

        if(((tlon[0] <= clon) && (tlon[2] >= clon)) ||
           ((tlon[0] >= clon) && (tlon[2] <= clon)))
        {
            fact = tlon[2] - tlon[0];
            if(MINIDIST < fabs(fact))
                dx = (clon - tlon[0]) / fact;
            else
                dx = 0.5;

            alat[n] = (1.0 - dx) * tlat[0] + dx * tlat[2];

            for(k = 0; k < nVertLevels; ++k)
            {
                cval[n][k] = (1.0 - dx) * val[0][k] + dx * val[2][k];
            }

          //cout << "\talat[" << n << "] = " << alat[n] << endl;
            alat[n] *= DEG2ARC;

            ++n;
        }

        if(((tlon[1] <= clon) && (tlon[2] >= clon)) ||
           ((tlon[1] >= clon) && (tlon[2] <= clon)))
        {
            fact = tlon[2] - tlon[1];
            if(MINIDIST < fabs(fact))
                dx = (clon - tlon[1]) / fact;
            else
                dx = 0.5;

            alat[n] = (1.0 - dx) * tlat[1] + dx * tlat[2];

            for(k = 0; k < nVertLevels; ++k)
            {
                cval[n][k] = (1.0 - dx) * val[1][k] + dx * val[2][k];
            }

          //cout << "\talat[" << n << "] = " << alat[n] << endl;
            alat[n] *= DEG2ARC;

            ++n;
        }

        glBegin(GL_QUAD_STRIP);
        for(k = 0; k < nVertLevels; ++k)
        {
            fact = sv * (cval[0][k] - _workMinVal);
            glTexCoord1d(fact);
            pnt0 = polar2xyz(alon, alat[0], height[k]);
            glVertex3d(pnt0.x, pnt0.y, pnt0.z);

          //cout << "\tpnt0.x = " << pnt0.x
          //     << ", pnt0.y = " << pnt0.y
          //     << ", pnt0.z = " << pnt0.z << endl;

            fact = sv * (cval[1][k] - _workMinVal);
            glTexCoord1d(fact);
            pnt1 = polar2xyz(alon, alat[1], height[k]);
            glVertex3d(pnt1.x, pnt1.y, pnt1.z);

          //cout << "\tpnt1.x = " << pnt1.x
          //     << ", pnt1.y = " << pnt1.y
          //     << ", pnt1.z = " << pnt1.z << endl;
        }
        glEnd();
    }

    glDisable(GL_TEXTURE_1D);

    glPopMatrix();

    glEndList();

    lister->set_xid(nvoptions->get_xsec(), xcl);
}

void MPAS2dViewer::_sphereDisplayYsec()
{
    ycl = 0;
    int c, v, i, k, n;
    int ml, mh;

  //int nCells         = geometry->get_nCells();
    int nVertLevels    = geometry->get_nVertLevels();
    int nVertices      = geometry->get_nVertices();
    int vertexDegree   = geometry->get_vertexDegree();
    int* cellsOnVertex = geometry->get_cellsOnVertex();

    double sv = 1.0;
    double fact;
    double height[nVertLevels];

    double* lon = NULL;
    double* lat = NULL;

    double tlon[3];
    double tlat[3];

    double clat = (float) nvoptions->get_ysec();
    double alat = DEG2ARC * clat;

    double alon[3];
    float dy;

    float val[3][nVertLevels];
    float cval[3][nVertLevels];

    pnt3d pnt0;
    pnt3d pnt1;

    int idx[3];

    ycl = lister->get_yid(nvoptions->get_ysec()+90);

    if(0 < ycl)
        return;

    ycl = glGenLists(1);
  //glNewList(ycl, GL_COMPILE);
    glNewList(ycl, GL_COMPILE_AND_EXECUTE);

  //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\t_varname: <" << _varname << ">" << endl;
  //cout << "\tnVertLevels = " << nVertLevels << ", nvoptions->get_ysec() = " << nvoptions->get_ysec() << endl;

    for(k = 0; k < nVertLevels; ++k)
        height[k] = 1.001 + 0.5 * (float) k / (float) nVertLevels;

    lon = geometry->get_lonCell();
    lat = geometry->get_latCell();

    sv = 1.0 / (_workMaxVal - _workMinVal);

  //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\tycl = " << ycl << endl;

    glPushMatrix();

    glColor3f(1.0, 1.0, 1.0);

    glNormal3f(0.0, 0.0, 1.0);

    glEnable(GL_TEXTURE_1D);
    glBindTexture(GL_TEXTURE_1D, texture1d->get_textureID());

    for(v = 0; v < nVertices; ++v)
    {
        n = 0;
        for(i = 0; i < vertexDegree; i++)
        {
            c = cellsOnVertex[i+v*vertexDegree];

            idx[i] = nVertLevels * c;
            tlon[i] = ARC2DEG * lon[c];
            tlat[i] = ARC2DEG * lat[c];

            if(clat < tlat[i])
                ++n;
            if(clat > tlat[i])
                --n;
        }

        if((3 == n) || ((-3) == n))
            continue;

      //cout << "\nclat = " << clat << endl;

        ml = 0;
        mh = 0;
        for(i = 0; i < vertexDegree; ++i)
        {
          //cout << "\ttlat[" << i << "] = " << tlat[i]
          //     << "\ttlon[" << i << "] = " << tlon[i] << endl;

            n = idx[i];
            for(k = 0; k < nVertLevels; ++k)
            {
                val[i][k] = _var[n+k];
            }

            if(LOWBOUND > tlon[i])
                ++ml;
            if(HIGHBOUND < tlon[i])
                ++mh;
        }

        if(ml && mh)
        {
            for(i = 0; i < vertexDegree; ++i)
            {
                if(LOWBOUND > tlon[i])
                    tlon[i] += 360.0;
            }
        }

        n = 0;

        if(((tlat[0] <= clat) && (tlat[1] >= clat)) ||
           ((tlat[0] >= clat) && (tlat[1] <= clat)))
        {
            fact = tlat[1] - tlat[0];
            if(MINIDIST < fabs(fact))
                dy = (clat - tlat[0]) / fact;
            else
                dy = 0.5;

            alon[n] = (1.0 - dy) * tlon[0] + dy * tlon[1];

            for(k = 0; k < nVertLevels; ++k)
            {
                cval[n][k] = (1.0 - dy) * val[0][k] + dy * val[1][k];
            }

          //cout << "\talon[" << n << "] = " << alon[n] << endl;
            alon[n] *= DEG2ARC;

            ++n;
        }

        if(((tlat[0] <= clat) && (tlat[2] >= clat)) ||
           ((tlat[0] >= clat) && (tlat[2] <= clat)))
        {
            fact = tlat[2] - tlat[0];
            if(MINIDIST < fabs(fact))
                dy = (clat - tlat[0]) / fact;
            else
                dy = 0.5;

            alon[n] = (1.0 - dy) * tlon[0] + dy * tlon[2];

            for(k = 0; k < nVertLevels; ++k)
            {
                cval[n][k] = (1.0 - dy) * val[0][k] + dy * val[2][k];
            }

          //cout << "\talon[" << n << "] = " << alon[n] << endl;
            alon[n] *= DEG2ARC;

            ++n;
        }

        if(((tlat[1] <= clat) && (tlat[2] >= clat)) ||
           ((tlat[1] >= clat) && (tlat[2] <= clat)))
        {
            fact = tlat[2] - tlat[1];
            if(MINIDIST < fabs(fact))
                dy = (clat - tlat[1]) / fact;
            else
                dy = 0.5;

            alon[n] = (1.0 - dy) * tlon[1] + dy * tlon[2];

            for(k = 0; k < nVertLevels; ++k)
            {
                cval[n][k] = (1.0 - dy) * val[1][k] + dy * val[2][k];
            }

          //cout << "\talon[" << n << "] = " << alon[n] << endl;
            alon[n] *= DEG2ARC;

            ++n;
        }

        glBegin(GL_QUAD_STRIP);
        for(k = 0; k < nVertLevels; ++k)
        {
            fact = sv * (cval[0][k] - _workMinVal);
            glTexCoord1d(fact);
            pnt0 = polar2xyz(alon[0], alat, height[k]);
            glVertex3d(pnt0.x, pnt0.y, pnt0.z);

          //cout << "\tpnt0.x = " << pnt0.x
          //     << ", pnt0.y = " << pnt0.y
          //     << ", pnt0.z = " << pnt0.z << endl;

            fact = sv * (cval[1][k] - _workMinVal);
            glTexCoord1d(fact);
            pnt1 = polar2xyz(alon[1], alat, height[k]);
            glVertex3d(pnt1.x, pnt1.y, pnt1.z);

          //cout << "\tpnt1.x = " << pnt1.x
          //     << ", pnt1.y = " << pnt1.y
          //     << ", pnt1.z = " << pnt1.z << endl;
        }
        glEnd();
    }

    glDisable(GL_TEXTURE_1D);

    glPopMatrix();

    glEndList();

    lister->set_yid(nvoptions->get_ysec()+90, ycl);
}

pnt3d MPAS2dViewer::polar2xyz(double lon, double lat, double r)
{
   pnt3d pnt;
   double delt = r * cos(lat);

   pnt.x = delt * sin(lon);
   pnt.y =    r * sin(lat);
   pnt.z = delt * cos(lon);

   return pnt;
}

pnt3d MPAS2dViewer::polar2flat(double lon, double lat, double h)
{
   pnt3d pnt;

   pnt.x = lon/180.0 - 1.0;
   pnt.y = lat/180.0;
   pnt.z = h;

   return pnt;
}

void MPAS2dViewer::_flatDisplayXsec()
{
    xcl = 0;
    int c, v, i, k, n;

  //int nCells         = geometry->get_nCells();
    int nVertLevels    = geometry->get_nVertLevels();
    int nVertices      = geometry->get_nVertices();
    int vertexDegree   = geometry->get_vertexDegree();
    int* cellsOnVertex = geometry->get_cellsOnVertex();

    double sv = 1.0;
    double fact;
    double height[nVertLevels];

    double* lon = NULL;
    double* lat = NULL;

    double tlon[3];
    double tlat[3];

    double clon = (float) nvoptions->get_xsec();
    double alon = clon;

    double alat[3];
    float dx;

    float val[3][nVertLevels];
    float cval[3][nVertLevels];

    bool need_shift_low  = false;
    bool need_shift_high = false;

    pnt3d pnt0;
    pnt3d pnt1;

    int idx[3];

    xcl = lister->get_xid(nvoptions->get_xsec());

    if(0 < xcl)
        return;

    xcl = glGenLists(1);
  //glNewList(xcl, GL_COMPILE);
    glNewList(xcl, GL_COMPILE_AND_EXECUTE);

  //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\t_varname: <" << _varname << ">" << endl;
  //cout << "\tnVertLevels = " << nVertLevels << ", nvoptions->get_xsec() = " << nvoptions->get_xsec() << endl;

    for(k = 0; k < nVertLevels; ++k)
        height[k] = 0.75 * (float) k / (float) nVertLevels - 0.5;

    if(LOWBOUND > clon)
        need_shift_low = true;
    if(HIGHBOUND < clon)
        need_shift_high = true;

    lon = geometry->get_lonCell();
    lat = geometry->get_latCell();

    sv = 1.0 / (_workMaxVal - _workMinVal);

  //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\txcl = " << xcl << endl;

    glPushMatrix();

    glColor3f(1.0, 1.0, 1.0);

    glNormal3f(1.0, 0.0, 0.0);

    glEnable(GL_TEXTURE_1D);
    glBindTexture(GL_TEXTURE_1D, texture1d->get_textureID());

    for(v = 0; v < nVertices; ++v)
    {
        for(i = 0; i < vertexDegree; i++)
        {
            c = cellsOnVertex[i+v*vertexDegree];

            idx[i] = nVertLevels * c;
            tlon[i] = ARC2DEG * lon[c];
            tlat[i] = ARC2DEG * lat[c];

            if(NEAR_NORTH_POLE < tlat[i])
                tlon[i] = clon;
            if(NEAR_SOUTH_POLE > tlat[i])
                tlon[i] = clon;
        }

        if(need_shift_low)
        {
            for(i = 0; i < vertexDegree; i++)
            {
                if(HIGHBOUND < tlon[i])
                   tlon[i] -= 360.0;

                if(NEAR_NORTH_POLE < tlat[i])
                    tlon[i] = clon;
                if(NEAR_SOUTH_POLE > tlat[i])
                    tlon[i] = clon;
            }
        }

        if(need_shift_high)
        {
            for(i = 0; i < vertexDegree; i++)
            {
                if(LOWBOUND > tlon[i])
                   tlon[i] += 360.0;

                if(NEAR_NORTH_POLE < tlat[i])
                    tlon[i] = clon;
                if(NEAR_SOUTH_POLE > tlat[i])
                    tlon[i] = clon;
            }
        }

        if((tlon[0] > clon) && (tlon[1] > clon) && (tlon[2] > clon))
            continue;
        if((tlon[0] < clon) && (tlon[1] < clon) && (tlon[2] < clon))
            continue;

        if((180.0 < fabs(tlon[1]-tlon[0])) ||
           (180.0 < fabs(tlon[2]-tlon[0])) ||
           (180.0 < fabs(tlon[2]-tlon[1])))
        {
          //cout << "\n\tclon = " << clon << endl;

          //for(i = 0; i < vertexDegree; ++i)
          //{
          //    cout << "\ttlat[" << i << "] = " << tlat[i]
          //         << "\ttlon[" << i << "] = " << tlon[i] << endl;
          //}
            continue;
        }

        for(i = 0; i < vertexDegree; ++i)
        {
          //cout << "\ttlat[" << i << "] = " << tlat[i]
          //     << "\ttlon[" << i << "] = " << tlon[i] << endl;

            n = idx[i];
            for(k = 0; k < nVertLevels; ++k)
            {
                val[i][k] = _var[n+k];
            }
        }

        n = 0;

        if(((tlon[0] <= clon) && (tlon[1] >= clon)) ||
           ((tlon[0] >= clon) && (tlon[1] <= clon)))
        {
            fact = tlon[1] - tlon[0];
            if(MINIDIST < fabs(fact))
                dx = (clon - tlon[0]) / fact;
            else
                dx = 0.5;

            alat[n] = (1.0 - dx) * tlat[0] + dx * tlat[1];

            for(k = 0; k < nVertLevels; ++k)
            {
                cval[n][k] = (1.0 - dx) * val[0][k] + dx * val[1][k];
            }

            ++n;
        }

        if(((tlon[0] <= clon) && (tlon[2] >= clon)) ||
           ((tlon[0] >= clon) && (tlon[2] <= clon)))
        {
            fact = tlon[2] - tlon[0];
            if(MINIDIST < fabs(fact))
                dx = (clon - tlon[0]) / fact;
            else
                dx = 0.5;

            alat[n] = (1.0 - dx) * tlat[0] + dx * tlat[2];

            for(k = 0; k < nVertLevels; ++k)
            {
                cval[n][k] = (1.0 - dx) * val[0][k] + dx * val[2][k];
            }

            ++n;
        }

        if(((tlon[1] <= clon) && (tlon[2] >= clon)) ||
           ((tlon[1] >= clon) && (tlon[2] <= clon)))
        {
            fact = tlon[2] - tlon[1];
            if(MINIDIST < fabs(fact))
                dx = (clon - tlon[1]) / fact;
            else
                dx = 0.5;

            alat[n] = (1.0 - dx) * tlat[1] + dx * tlat[2];

            for(k = 0; k < nVertLevels; ++k)
            {
                cval[n][k] = (1.0 - dx) * val[1][k] + dx * val[2][k];
            }

            ++n;
        }

        glBegin(GL_QUAD_STRIP);
        for(k = 0; k < nVertLevels; ++k)
        {
            fact = sv * (cval[0][k] - _workMinVal);
            glTexCoord1d(fact);
            pnt0 = polar2flat(alon, alat[0], height[k]);
            glVertex3d(pnt0.x, pnt0.y, pnt0.z);

          //cout << "\tpnt0.x = " << pnt0.x
          //     << ", pnt0.y = " << pnt0.y
          //     << ", pnt0.z = " << pnt0.z << endl;

            fact = sv * (cval[1][k] - _workMinVal);
            glTexCoord1d(fact);
            pnt1 = polar2flat(alon, alat[1], height[k]);
            glVertex3d(pnt1.x, pnt1.y, pnt1.z);

          //cout << "\tpnt1.x = " << pnt1.x
          //     << ", pnt1.y = " << pnt1.y
          //     << ", pnt1.z = " << pnt1.z << endl;
        }
        glEnd();
    }

    glDisable(GL_TEXTURE_1D);

    glPopMatrix();

    glEndList();

    lister->set_xid(nvoptions->get_xsec(), xcl);
}

void MPAS2dViewer::_flatDisplayYsec()
{
    ycl = 0;
    int c, v, i, k, n;
    int ml, mh;

  //int nCells         = geometry->get_nCells();
    int nVertLevels    = geometry->get_nVertLevels();
    int nVertices      = geometry->get_nVertices();
    int vertexDegree   = geometry->get_vertexDegree();
    int* cellsOnVertex = geometry->get_cellsOnVertex();

    double sv = 1.0;
    double fact;
    double height[nVertLevels];

    double* lon = NULL;
    double* lat = NULL;

    double tlon[3];
    double tlat[3];

    double clat = (float) nvoptions->get_ysec();
    double alat = clat;

    double alon[3];
    float dy;

    float val[3][nVertLevels];
    float cval[3][nVertLevels];

    pnt3d pnt0;
    pnt3d pnt1;

    int idx[3];

    ycl = lister->get_yid(nvoptions->get_ysec()+90);

    if(0 < ycl)
        return;

    ycl = glGenLists(1);
  //glNewList(ycl, GL_COMPILE);
    glNewList(ycl, GL_COMPILE_AND_EXECUTE);

  //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\t_varname: <" << _varname << ">" << endl;
  //cout << "\tnVertLevels = " << nVertLevels << ", nvoptions->get_ysec() = " << nvoptions->get_ysec() << endl;

    for(k = 0; k < nVertLevels; ++k)
        height[k] = 0.75 * (float) k / (float) nVertLevels - 0.5;

    lon = geometry->get_lonCell();
    lat = geometry->get_latCell();

    sv = 1.0 / (_workMaxVal - _workMinVal);

  //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\tycl = " << ycl << endl;

    glPushMatrix();

    glColor3f(1.0, 1.0, 1.0);

    glNormal3f(0.0, 1.0, 0.0);

    glEnable(GL_TEXTURE_1D);
    glBindTexture(GL_TEXTURE_1D, texture1d->get_textureID());

    for(v = 0; v < nVertices; ++v)
    {
        n = 0;
        for(i = 0; i < vertexDegree; i++)
        {
            c = cellsOnVertex[i+v*vertexDegree];

            idx[i] = nVertLevels * c;
            tlon[i] = ARC2DEG * lon[c];
            tlat[i] = ARC2DEG * lat[c];

            if(clat < tlat[i])
                ++n;
            if(clat > tlat[i])
                --n;
        }

        if((3 == n) || ((-3) == n))
            continue;

      //cout << "\nclat = " << clat << endl;

        ml = 0;
        mh = 0;
        for(i = 0; i < vertexDegree; ++i)
        {
          //cout << "\ttlat[" << i << "] = " << tlat[i]
          //     << "\ttlon[" << i << "] = " << tlon[i] << endl;

            n = idx[i];
            for(k = 0; k < nVertLevels; ++k)
            {
                val[i][k] = _var[n+k];
            }

            if(LOWBOUND > tlon[i])
                ++ml;
            if(HIGHBOUND < tlon[i])
                ++mh;
        }

        if(ml && mh)
        {
            for(i = 0; i < vertexDegree; ++i)
            {
                if(LOWBOUND > tlon[i])
                    tlon[i] += 360.0;
            }
        }

        n = 0;

        if(((tlat[0] <= clat) && (tlat[1] >= clat)) ||
           ((tlat[0] >= clat) && (tlat[1] <= clat)))
        {
            fact = tlat[1] - tlat[0];
            if(MINIDIST < fabs(fact))
                dy = (clat - tlat[0]) / fact;
            else
                dy = 0.5;

            alon[n] = (1.0 - dy) * tlon[0] + dy * tlon[1];

            for(k = 0; k < nVertLevels; ++k)
            {
                cval[n][k] = (1.0 - dy) * val[0][k] + dy * val[1][k];
            }

            ++n;
        }

        if(((tlat[0] <= clat) && (tlat[2] >= clat)) ||
           ((tlat[0] >= clat) && (tlat[2] <= clat)))
        {
            fact = tlat[2] - tlat[0];
            if(MINIDIST < fabs(fact))
                dy = (clat - tlat[0]) / fact;
            else
                dy = 0.5;

            alon[n] = (1.0 - dy) * tlon[0] + dy * tlon[2];

            for(k = 0; k < nVertLevels; ++k)
            {
                cval[n][k] = (1.0 - dy) * val[0][k] + dy * val[2][k];
            }

            ++n;
        }

        if(((tlat[1] <= clat) && (tlat[2] >= clat)) ||
           ((tlat[1] >= clat) && (tlat[2] <= clat)))
        {
            fact = tlat[2] - tlat[1];
            if(MINIDIST < fabs(fact))
                dy = (clat - tlat[1]) / fact;
            else
                dy = 0.5;

            alon[n] = (1.0 - dy) * tlon[1] + dy * tlon[2];

            for(k = 0; k < nVertLevels; ++k)
            {
                cval[n][k] = (1.0 - dy) * val[1][k] + dy * val[2][k];
            }

            ++n;
        }

        glBegin(GL_QUAD_STRIP);
        for(k = 0; k < nVertLevels; ++k)
        {
            fact = sv * (cval[0][k] - _workMinVal);
            glTexCoord1d(fact);
            pnt0 = polar2flat(alon[0], alat, height[k]);
            glVertex3d(pnt0.x, pnt0.y, pnt0.z);

          //cout << "\tpnt0.x = " << pnt0.x
          //     << ", pnt0.y = " << pnt0.y
          //     << ", pnt0.z = " << pnt0.z << endl;

            fact = sv * (cval[1][k] - _workMinVal);
            glTexCoord1d(fact);
            pnt1 = polar2flat(alon[1], alat, height[k]);
            glVertex3d(pnt1.x, pnt1.y, pnt1.z);

          //cout << "\tpnt1.x = " << pnt1.x
          //     << ", pnt1.y = " << pnt1.y
          //     << ", pnt1.z = " << pnt1.z << endl;
        }
        glEnd();
    }

    glDisable(GL_TEXTURE_1D);

    glPopMatrix();

    glEndList();

    lister->set_yid(nvoptions->get_ysec()+90, ycl);
}

void MPAS2dViewer::_draw_cross(double radius)
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

void MPAS2dViewer::_draw_triangle(double radius)
{
    int c, i, v;
    int nVertices      = geometry->get_nVertices();
    int vertexDegree   = geometry->get_vertexDegree();
    int* cellsOnVertex = geometry->get_cellsOnVertex();

    pnt3d pnt;
  //pnt3d norm;

    glPushMatrix();

  //Draw lines 
    glColor3f(0.0, 1.0, 1.0);

    for(v = 0; v < nVertices; ++v)
    {
        glBegin ( GL_LINE_STRIP );
        for(i = 0; i < vertexDegree; i++)
        {
            c = cellsOnVertex[i+v*vertexDegree];
            pnt = geometry->polar2xyz(c, radius);
          //norm = nvop(pnt, radius);
          //glNormal3d(norm.x, norm.y, norm.z);
            glVertex3d(pnt.x, pnt.y, pnt.z);
        }

        c = cellsOnVertex[v*vertexDegree];
        pnt = geometry->polar2xyz(c, radius);
      //norm = nvop(pnt, radius);
      //glNormal3d(norm.x, norm.y, norm.z);
        glVertex3d(pnt.x, pnt.y, pnt.z);

        glEnd();
    }

    glPopMatrix();
}

void MPAS2dViewer::_flatSubset()
{
    zcl = 0;
    int c, v, i, k, n;
    unsigned int m;

    int nVertLevels    = geometry->get_nVertLevels();
    int nVertices      = geometry->get_nVertices();
    int nCells         = geometry->get_nCells();
    int vertexDegree   = geometry->get_vertexDegree();
    int* cellsOnVertex = geometry->get_cellsOnVertex();

    double sv = 1.0;
    double fact;
    double height = 0.0;

    float* current_var_ptr;

    int nwest, neast, nsouth, nnorth;
    float west, east, south, north;
    float xc, yc;
    pnt2d pnt;

    k = nvoptions->get_zsec();

    current_var_ptr = &_var[nCells * nVertLevels * nvoptions->get_tsec()];

    zcl = lister->get_zid(k);

    if(0 < zcl)
        return;

    zcl = glGenLists(1);
  //glNewList(zcl, GL_COMPILE);
    glNewList(zcl, GL_COMPILE_AND_EXECUTE);

    if(1 < nVertLevels)
        height = (float) k / (float) nVertLevels - 0.5;
    else
        height = 0.0;

    locator->set_height(height);

    sv = 1.0 / (_workMaxVal - _workMinVal);

  //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\tnVertLevels = " << nVertLevels << endl;
  //cout << "\tnvoptions->get_cb(NV_SUBSET): <" << nvoptions->get_cb(NV_SUBSET) << ">" << endl;
  //cout << "\tnvoptions->get_minlon() = " << nvoptions->get_minlon() << endl;
  //cout << "\tnvoptions->get_maxlon() = " << nvoptions->get_maxlon() << endl;
  //cout << "\tnvoptions->get_minlat() = " << nvoptions->get_minlat() << endl;
  //cout << "\tnvoptions->get_maxlat() = " << nvoptions->get_maxlat() << endl;

    west = nvoptions->get_minlon()/180.0 - 1.0;
    east = nvoptions->get_maxlon()/180.0 - 1.0;
    south = nvoptions->get_minlat()/180.0 - 0.5;
    north = nvoptions->get_maxlat()/180.0 - 0.5;

    if(west > east)
    {
        xc = west;
        west = east;
        east = xc;
    }

    if(south > north)
    {
        yc = south;
        south = north;
        north = yc;
    }

    fact = nvoptions->get_maxlon() - nvoptions->get_minlon();
    if(80.0 < fact)
    {
        fact -= 80.0;
        fact /= 360.0;
        west += fact;
        east -= fact;
    }

    fact = nvoptions->get_maxlat() - nvoptions->get_minlat();
    if(50.0 < fact)
    {
        fact -= 50.0;
        fact /= 360.0;
        south += fact;
        north -= fact;
    }

    xc = 0.5*(west + east);
    yc = 0.5*(south + north);
 
    glTranslatef(-xc, -yc, 0.0);

    if(0 == _subsetVertices.size())
    {
        for(v = 0; v < nVertices; ++v)
        {
            neast = 0;
            nwest = 0;
            nsouth = 0;
            nnorth = 0;

            for(i = 0; i < vertexDegree; i++)
            {
                c = cellsOnVertex[i+v*vertexDegree];
                pnt = geometry->latlon2xy(c);

                if(west > pnt.x)
                    ++nwest;
                if(east < pnt.x)
                    ++neast;
                if(south > pnt.y)
                    ++nsouth;
                if(north < pnt.y)
                    ++nnorth;
            }

            if(vertexDegree == nwest)
                continue;
            if(vertexDegree == neast)
                continue;
            if(vertexDegree == (nwest + neast))
                continue;
            if(vertexDegree == nsouth)
                continue;
            if(vertexDegree == nnorth)
                continue;

            _subsetVertices.push_back(v);
        }
    }

    glPushMatrix();

    glColor3f(1.0, 1.0, 1.0);

    glNormal3f(0.0, 0.0, 1.0);

    glEnable(GL_TEXTURE_1D);
    glBindTexture(GL_TEXTURE_1D, texture1d->get_textureID());

    glBegin(GL_TRIANGLES);
    for(m = 0; m < _subsetVertices.size(); ++m)
    {
        v = _subsetVertices[m];

        for(i = 0; i < vertexDegree; i++)
        {
            c = cellsOnVertex[i+v*vertexDegree];
            n = nVertLevels * c + k;
            fact = sv * (current_var_ptr[n] - _workMinVal);
            glTexCoord1d(fact);

            pnt = geometry->latlon2xy(c);
            glVertex3d(pnt.x, pnt.y, height);
        }
    }
    glEnd();

    glDisable(GL_TEXTURE_1D);

    coastline->drawOnPlane(height+0.001);

    glPopMatrix();

    glEndList();

    lister->set_zid(k, zcl);
}

void MPAS2dViewer::_sphereSubset()
{
    zcl = 0;
    int c, v, i, k, n;
    unsigned int m;

    int nVertLevels    = geometry->get_nVertLevels();
    int nVertices      = geometry->get_nVertices();
    int nCells         = geometry->get_nCells();
    int vertexDegree   = geometry->get_vertexDegree();
    int* cellsOnVertex = geometry->get_cellsOnVertex();

    double sv = 1.0;
    double fact;
    double height = 1.001;

    float* current_var_ptr;

    int nwest, neast, nsouth, nnorth;
    float west, east, south, north;
    float xc, yc;

    double* lon = geometry->get_lonCell();
    double* lat = geometry->get_latCell();

    pnt3d pnt;
    pnt3d norm;

    k = nvoptions->get_zsec();

    current_var_ptr = &_var[nCells * nVertLevels * nvoptions->get_tsec()];

    zcl = lister->get_zid(k);

    if(0 < zcl)
        return;

    zcl = glGenLists(1);
  //glNewList(zcl, GL_COMPILE);
    glNewList(zcl, GL_COMPILE_AND_EXECUTE);

    height = 1.001 + 0.5 * (float) k / (float) nVertLevels;

    locator->set_height(height);

    sv = 1.0 / (_workMaxVal - _workMinVal);

  //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\tnvoptions->get_cb(NV_SUBSET): <" << nvoptions->get_cb(NV_SUBSET) << ">" << endl;
  //cout << "\tnvoptions->get_minlon() = " << nvoptions->get_minlon() << endl;
  //cout << "\tnvoptions->get_maxlon() = " << nvoptions->get_maxlon() << endl;
  //cout << "\tnvoptions->get_minlat() = " << nvoptions->get_minlat() << endl;
  //cout << "\tnvoptions->get_maxlat() = " << nvoptions->get_maxlat() << endl;

    west = nvoptions->get_minlon() * ARC;
    east = nvoptions->get_maxlon() * ARC;
    south = nvoptions->get_minlat() * ARC;
    north = nvoptions->get_maxlat() * ARC;

    if(west > east)
    {
        xc = west;
        west = east;
        east = xc;
    }

    if(south > north)
    {
        yc = south;
        south = north;
        north = yc;
    }

    fact = nvoptions->get_maxlon() - nvoptions->get_minlon();
    if(80.0 < fact)
    {
        fact -= 80.0;
        fact *= ARC;
        west += 0.5 * fact;
        east -= 0.5 * fact;
    }

    fact = nvoptions->get_maxlat() - nvoptions->get_minlat();
    if(50.0 < fact)
    {
        fact -= 50.0;
        fact /= ARC;
        south += 0.5 * fact;
        north -= 0.5 * fact;
    }

    xc = 0.5*(west + east);
    yc = 0.5*(south + north);
 
    fact = height * cos(yc);

    pnt.x = fact * sin(xc);
    pnt.y = height * sin(yc);
    pnt.z = fact * cos(xc);

    glTranslatef(-pnt.x, -pnt.y, -pnt.z);

  //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\t_varname: <" << _varname << ">" << endl;
  //cout << "\tnVertices = " << nVertices << ", nvoptions->get_zsec() = " << nvoptions->get_zsec() << endl;

    if(0 == _subsetVertices.size())
    {
        for(v = 0; v < nVertices; ++v)
        {
            neast = 0;
            nwest = 0;
            nsouth = 0;
            nnorth = 0;

            for(i = 0; i < vertexDegree; i++)
            {
                c = cellsOnVertex[i+v*vertexDegree];

                if(west > lon[c])
                    ++nwest;
                if(east < lon[c])
                    ++neast;
                if(south > lat[c])
                    ++nsouth;
                if(north < lat[c])
                    ++nnorth;
            }

            if(vertexDegree == nwest)
                continue;
            if(vertexDegree == neast)
                continue;
            if(vertexDegree == (nwest + neast))
                continue;
            if(vertexDegree == nsouth)
                continue;
            if(vertexDegree == nnorth)
                continue;

            _subsetVertices.push_back(v);
        }
    }

    glPushMatrix();

    glColor3f(1.0, 1.0, 1.0);

    glNormal3f(0.0, 0.0, 1.0);

    glEnable(GL_TEXTURE_1D);
    glBindTexture(GL_TEXTURE_1D, texture1d->get_textureID());

    glBegin(GL_TRIANGLES);
    for(m = 0; m < _subsetVertices.size(); ++m)
    {
        v = _subsetVertices[m];

        for(i = 0; i < vertexDegree; i++)
        {
            c = cellsOnVertex[i+v*vertexDegree];
            n = nVertLevels * c + k;
            fact = sv * (current_var_ptr[n] - _workMinVal);

            pnt = geometry->polar2xyz(c, height);

            norm = nvop(pnt, height);
            glNormal3d(norm.x, norm.y, norm.z);
            glTexCoord1d(fact);
            glVertex3d(pnt.x, pnt.y, pnt.z);
        }
    }
    glEnd();

    glDisable(GL_TEXTURE_1D);

    coastline->drawOnPlane(height+0.001);

    glPopMatrix();

    glEndList();

    lister->set_zid(k, zcl);
}

void MPAS2dViewer::draw_sphere_grids()
{
    int c, v, i, k, n;

    int nCells         = geometry->get_nCells();
    int nVertLevels    = geometry->get_nVertLevels();
    int nVertices      = geometry->get_nVertices();
    int vertexDegree   = geometry->get_vertexDegree();
    int* cellsOnVertex = geometry->get_cellsOnVertex();

    float radius;

    pnt3d pnt;
    pnt3d norm;

    k = nvoptions->get_zsec();

    radius = 1.020 + 0.5 * (float) k / (float) nVertLevels;

    locator->set_height(radius);

  //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;

    glPushMatrix();

    glColor3d(1.0, 1.0, 0.0);

    glBegin(GL_POLYGON);
    for(v = 0; v < nVertices; ++v)
    {
        for(i = 0; i < vertexDegree; i++)
        {
            c = cellsOnVertex[i+v*vertexDegree];
            pnt = geometry->polar2xyz(c, radius);

          //norm = nvop(pnt, height);
          //glNormal3d(norm.x, norm.y, norm.z);
            glVertex3d(pnt.x, pnt.y, pnt.z);
        }
    }
    glEnd();

#if 0
    coastline->draw(radius + 0.01);

    if(locator->on())
    {
      //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
      //cout << "\tlocation = (" << locator->x() << ", " << locator->y() << ")" << endl;

        _draw_cross(height + 0.01);

        if((NULL != nvoptions) && nvoptions->get_cb(NV_GRIDON))
            _draw_triangle(radius + 0.01);
    }
#endif

    glPopMatrix();
}

void MPAS2dViewer::draw_plane_grids()
{
    int c, v, i, k, n, cp1, cp2;

    int nVertLevels    = geometry->get_nVertLevels();
    int nVertices      = geometry->get_nVertices();
    int nCells         = geometry->get_nCells();
    int vertexDegree   = geometry->get_vertexDegree();
    int* cellsOnVertex = geometry->get_cellsOnVertex();

    float x[5];
    float y[5];
    float dx;
    float height;

    pnt2d pnt;
    pnt2d pnt0;
    pnt2d pnt1;
    pnt2d pnt2;

    k = nvoptions->get_zsec();

    if(1 < nVertLevels)
        height = (float) k / (float) nVertLevels - 0.5;
    else
        height = 0.0;

    locator->set_height(height);

    _innerPoints = geometry->get_innerPoints();
    _nBoundaryPoints = geometry->get_nBoundaryPoints();
    _boundaryType = geometry->get_boundaryType();
    _boundaryPoints = geometry->get_boundaryPoints();

    glPushMatrix();

    glColor3f(1.0, 1.0, 1.0);

    glNormal3f(0.0, 0.0, 1.0);

    for(v = 0; v < nVertices; ++v)
    {
        if(_innerPoints[v])
        {
            glBegin(GL_POLYGON);
            for(i = 0; i < vertexDegree; i++)
            {
                c = cellsOnVertex[i+v*vertexDegree];
                pnt = geometry->latlon2xy(c);
                glVertex3d(pnt.x, pnt.y, height);
            }
            glEnd();
        }
    }

    for(v = 0; v < _nBoundaryPoints; ++v)
    {
        c = _boundaryPoints[3*v];
        cp1 = _boundaryPoints[3*v+1];
        cp2 = _boundaryPoints[3*v+2];

        pnt0 = geometry->latlon2xy(c);
        pnt1 = geometry->latlon2xy(cp1);
        pnt2 = geometry->latlon2xy(cp2);

        switch (_boundaryType[v])
        {
            case -2:
                 x[0] = pnt0.x - 2.0;
                 x[2] = pnt2.x;
                 x[4] = pnt1.x - 2.0;

                 y[0] = pnt0.y;
                 y[2] = pnt2.y;
                 y[4] = pnt1.y;

                 x[1] = -1.0;
                 x[3] = -1.0;

                 dx = x[1] - x[0];
                 y[1] = (1.0 - dx) * y[0] + dx * y[2];
                 dx = x[3] - x[4];
                 y[3] = (1.0 - dx) * y[4] + dx * y[2];

               //Handle triangle 1,2,3
                 glBegin(GL_POLYGON);
                 glVertex3d(x[1], y[1], height);
                 glVertex3d(x[2], y[2], height);
                 glVertex3d(x[3], y[3], height);
                 glEnd();

               //Handle triangle 0,1,3, 1, 3, 4
               //change their coords.
                 for(n = 0; n < 5; ++n)
                 {
                     x[n] += 2.0;
                 }

                 break;

            case -1:
                 x[0] = pnt0.x;
                 x[2] = pnt2.x - 2.0;
                 x[4] = pnt1.x;

                 y[0] = pnt0.y;
                 y[2] = pnt2.y;
                 y[4] = pnt1.y;

                 x[1] = -1.0;
                 x[3] = -1.0;

                 dx = x[0] - x[1];
                 y[1] = (1.0 - dx) * y[0] + dx * y[2];
                 dx = x[4] - x[3];
                 y[3] = (1.0 - dx) * y[4] + dx * y[2];

               //Handle triangle 1,2,3
                 glBegin(GL_POLYGON);
                 glVertex3d(x[1]+2.0, y[1], height);
                 glVertex3d(x[2]+2.0, y[2], height);
                 glVertex3d(x[3]+2.0, y[3], height);
                 glEnd();

                 break;

            case 1:
                 x[2] = pnt0.x;
                 x[0] = pnt1.x + 2.0;
                 x[4] = pnt2.x + 2.0;

                 y[2] = pnt0.y;
                 y[0] = pnt1.y;
                 y[4] = pnt2.y;

                 x[1] = 1.0;
                 x[3] = 1.0;

                 dx = x[0] - x[1];
                 y[1] = (1.0 - dx) * y[0] + dx * y[2];
                 dx = x[4] - x[3];
                 y[3] = (1.0 - dx) * y[4] + dx * y[2];

               //Handle triangle 1,2,3
                 glBegin(GL_POLYGON);
                 glVertex3d(x[1], y[1], height);
                 glVertex3d(x[2], y[2], height);
                 glVertex3d(x[3], y[3], height);
                 glEnd();

               //Handle triangle 0,1,3, 1, 3, 4
               //change their coords.
                 for(n = 0; n < 5; ++n)
                 {
                     x[n] -= 2.0;
                 }

                 break;

            case 2:
                 x[0] = pnt0.x;
                 x[4] = pnt1.x;
                 x[2] = pnt2.x + 2.0;

                 y[0] = pnt0.y;
                 y[4] = pnt1.y;
                 y[2] = pnt2.y;

                 x[1] = 1.0;
                 x[3] = 1.0;

                 dx = x[1] - x[0];
                 y[1] = (1.0 - dx) * y[0] + dx * y[2];
                 dx = x[3] - x[4];
                 y[3] = (1.0 - dx) * y[4] + dx * y[2];

               //Handle triangle 1,2,3
                 glBegin(GL_POLYGON);
                 glVertex3d(x[1]-2.0, y[1], height);
                 glVertex3d(x[2]-2.0, y[2], height);
                 glVertex3d(x[3]-2.0, y[3], height);
                 glEnd();

                 break;

            default:
                 cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
                 cout << "\tUnknown _boundaryType[" << v << " = " << _boundaryType[v] << endl;
                 break;
        }

      //Polygon 0, 1, 3, 4
        glBegin(GL_POLYGON);
        glVertex3d(x[0], y[0], height);
        glVertex3d(x[1], y[1], height);
        glVertex3d(x[3], y[3], height);
        glVertex3d(x[4], y[4], height);
        glEnd();
    }

    coastline->drawOnPlane(height+0.001);

    glPopMatrix();
}

