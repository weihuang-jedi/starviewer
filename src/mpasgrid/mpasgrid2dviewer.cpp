#include <QtOpenGL>

#include <mpasgrid2dviewer.h>

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

MPASGrid2dViewer::MPASGrid2dViewer(ColorTable *ct, NVOptions* opt)
{
    cout << "Enter" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
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
    cout << "Leave" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
}

MPASGrid2dViewer::~MPASGrid2dViewer()
{
    if(NULL != locator)
        locator->turnOff();
    delete lister;
    delete coastline;
    delete texture1d;
    delete evaluator;
}

void MPASGrid2dViewer::set_geometry(MPASGeometry *gm)
{
    // cout << "Enter" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
    geometry = gm;

    _initialize();
    // cout << "Leave" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
}

void MPASGrid2dViewer::setup(string vn, int *var)
{
    size_t gridsize;
    cout << "Enter" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
    cout << "\tvarname: " << vn << endl;

    reset();

    _varname  = vn;
    _var = var;
    cout << "Leave" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
}

void MPASGrid2dViewer::reset()
{
    lister->reinitialize(361, 181, 1);
}

void MPASGrid2dViewer::_initialize()
{
    lister->reinitialize(361, 181, 1);
}

void MPASGrid2dViewer::draw()
{
    cout << "\nEnter" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
    zcl = lister->get_zid(nvoptions->get_zsec());

    if(nvoptions->get_cb(NV_RESET))
    {
        _subsetVertices.clear();
        nvoptions->set_cb(NV_RESET, false);
        reset();
    }

    if(nvoptions->get_cb(NV_MPASGRID_SUBSET))
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

    cout << "\t_varname: <" << _varname << ">, zcl = " << zcl << ", xcl = " << xcl << endl;
    cout << "\tnvoptions->get_cb(NV_FLATON) = " << nvoptions->get_cb(NV_FLATON) << endl;

    if(nvoptions->get_cb(NV_FLATON))
    {
        cout << "\t_varname: <" << _varname << ">, zcl = " << zcl << ", xcl = " << xcl << endl;
        cout << "\tgeometry->get_nVertLevels() = " << geometry->get_nVertLevels() << endl;
        cout << "\tnvoptions->get_zsec() = " << nvoptions->get_zsec() << endl;
        if((geometry->get_nVertLevels() > nvoptions->get_zsec()) && (0 <= nvoptions->get_zsec()))
        {
            if(zcl)
                glCallList(zcl);
            else
            {
                cout << "\t_varname: <" << _varname << ">, zcl = " << zcl << ", xcl = " << xcl << endl;
                cout << "\tnvoptions->get_cb(NV_SUBSET) = " << nvoptions->get_cb(NV_SUBSET) << endl;
                if(nvoptions->get_cb(NV_SUBSET))
                    _flatSubset();
                else
                    _flatDisplay();
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
    }
    // cout << "Leave" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
}

pnt3d MPASGrid2dViewer::nvop(pnt3d p, double height)
{
    pnt3d np;

    np.x = p.x / height;
    np.y = p.y / height;
    np.z = p.z / height;

    return np;
}

void MPASGrid2dViewer::_sphereDisplay()
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

    int* current_var_ptr;

    k = nvoptions->get_zsec();

    cout << "\nEnter " << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
    cout << "\t_varname: <" << _varname << ">" << endl;
    cout << "\tnVertLevels = " << nVertLevels << ", nvoptions->get_zsec() = " << nvoptions->get_zsec() << endl;
    cout << "\tnvoptions->get_tsec() = " << nvoptions->get_tsec() << ", vertexDegree = " << vertexDegree << endl;
    cout << "\tnCells = " << nCells << ", nVertices = " << nVertices << endl;

    current_var_ptr = &_var[nCells * nVertLevels * nvoptions->get_tsec()];

    zcl = lister->get_zid(k);

    if(0 < zcl)
        return;

    locator->set_height(radius);

    sv = 1.0 / (_workMaxVal - _workMinVal);

    zcl = glGenLists(1);
    glNewList(zcl, GL_COMPILE_AND_EXECUTE);

    cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
    cout << "\tzcl = " << zcl << endl;

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
            // fact = sv * (current_var_ptr[n] - _workMinVal);
            fact = (float) v / (float) nVertices;

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
    cout << "Leave " << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
}

void MPASGrid2dViewer::_flatDisplay()
{
    int c, v, i, k, n, cp1, cp2;

    int nVertLevels    = geometry->get_nVertLevels();
    int nVertices      = geometry->get_nVertices();
    int nCells         = geometry->get_nCells();
    int vertexDegree   = geometry->get_vertexDegree();
    int* cellsOnVertex = geometry->get_cellsOnVertex();

    float sv = 1.0;
    float fact;
    float height = 1.01;

    float x[5];
    float y[5];
  //float z[5];
    float val[5];
    float dx;
  //float dy;

    int* current_var_ptr;

    pnt2d pnt;
    pnt2d pnt0;
    pnt2d pnt1;
    pnt2d pnt2;

    cout << "\nEnter " << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
    zcl = 0;
    k = nvoptions->get_zsec();

    current_var_ptr = &_var[nCells * nVertLevels * nvoptions->get_tsec()];

    zcl = lister->get_zid(k);

    if(0 < zcl)
        return;

    zcl = glGenLists(1);
    glNewList(zcl, GL_COMPILE_AND_EXECUTE);

    locator->set_height(height);

    sv = 1.0 / (_workMaxVal - _workMinVal);

    cout << "\t" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
    cout << "\t_varname: <" << _varname << ">" << endl;
    cout << "\tnVertices = " << nVertices << ", nvoptions->get_zsec() = " << nvoptions->get_zsec() << endl;

    glPushMatrix();

    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1, 1);

    glColor3d(1.0, 1.0, 1.0);

    glNormal3d(0.0, 0.0, 1.0);

    glEnable(GL_TEXTURE_1D);
    glBindTexture(GL_TEXTURE_1D, texture1d->get_textureID());

    cout << "\t" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
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

    cout << "\t" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
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

    cout << "\t" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
    coastline->drawOnPlane(height+0.01);

    glDisable(GL_POLYGON_OFFSET_FILL);
    glDisable(GL_TEXTURE_1D);

    glPopMatrix();

    glEndList();

    lister->set_zid(k, zcl);
    cout << "Leave " << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
}

void MPASGrid2dViewer::reset_texture1d(ColorTable *ct)
{
    colorTable = ct;

    texture1d->reset();
    texture1d->set_colors(ct->get_clen(), ct->get_cmap());
    texture1d->set_name(ct->get_name());
}

pnt3d MPASGrid2dViewer::polar2xyz(double lon, double lat, double r)
{
   pnt3d pnt;
   double delt = r * cos(lat);

   pnt.x = delt * sin(lon);
   pnt.y =    r * sin(lat);
   pnt.z = delt * cos(lon);

   return pnt;
}

pnt3d MPASGrid2dViewer::polar2flat(double lon, double lat, double h)
{
   pnt3d pnt;

   pnt.x = lon/180.0 - 1.0;
   pnt.y = lat/180.0;
   pnt.z = h;

   return pnt;
}

void MPASGrid2dViewer::_draw_cross(double radius)
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

void MPASGrid2dViewer::_draw_triangle(double radius)
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

void MPASGrid2dViewer::_flatSubset()
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
    double height = 1.01;

    int* current_var_ptr;

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
    glNewList(zcl, GL_COMPILE_AND_EXECUTE);

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

void MPASGrid2dViewer::_sphereSubset()
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

    int* current_var_ptr;

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

void MPASGrid2dViewer::draw_sphere_grids()
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

    radius = 1.01;

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

void MPASGrid2dViewer::draw_plane_grids()
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
    float height = 1.001;

    pnt2d pnt;
    pnt2d pnt0;
    pnt2d pnt1;
    pnt2d pnt2;

    k = nvoptions->get_zsec();

    locator->set_height(height);

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

