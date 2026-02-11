//$Id: vtk2dviewer.cpp 4865 2014-01-01 15:49:23Z starviewer $

#include <QtOpenGL>

#include "vtk2dviewer.h"

#define VTK_PI		3.1415926535897932
#define ARC2DEG		(180.0/VTK_PI)
#define DEG2ARC		(VTK_PI/180.0)

VTK2dViewer::VTK2dViewer(ColorTable *ct, NVOptions* opt)
{
    colorTable = ct;
    nvoptions = opt;

    locator = NULL;

    lister = new Lister();
    lister->setup(1, 1, 121);

    _zlist = NULL;

    _minval = 1.0e20;
    _maxval = -1.0e20;

    _colorlen = colorTable->get_clen() - 3;
    _colormap = colorTable->get_cmap();
}

VTK2dViewer::~VTK2dViewer()
{
    locator->turnOff();
    delete lister;
}

void VTK2dViewer::set_reader(VTKReader *r)
{
    reader = r;

    polyData = reader->getPolyData();

    _initialize();
}

void VTK2dViewer::setup(string vn)
{
    reset();

    _varname  = vn;

  //_set_minmax();
}

void VTK2dViewer::reset()
{
    lister->reinitialize(1, 1, 121);
}

void VTK2dViewer::_initialize()
{
    lister->reinitialize(1, 1, 121);
}

void VTK2dViewer::draw()
{
    int lvl = nvoptions->get_zsec();
    GLuint zcl = lister->get_zid(lvl);

  //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\t_varname: <" << _varname << ">, zcl = " << zcl << endl;
  //cout << "\tlvl = " << lvl << endl;

#if 0
    _draw_grid();
#else
    if(NULL == polyData)
        return;

    if(zcl)
        glCallList(zcl);
    else
        _display();
#endif
}

void VTK2dViewer::_draw_cross(double dist)
{
    double x, y, z;
    double lon = (double) locator->x();
    double lat = (double) locator->y();
    
    glLineWidth(2.0);
    glColor4d(1.0, 0.0, 1.0, 0.95);

    x = dist * cos(lon * ARC);
    y = dist * sin(lat * ARC);
    z = 0.0;

    glBegin(GL_LINE_STRIP);
        glVertex3f(x-0.02, y, z);
        glVertex3f(x+0.02, y, z);

        glVertex3f(x, y-0.02, z);
        glVertex3f(x, y+0.02, z);
    glEnd();
}

void VTK2dViewer::_set_minmax()
{
#if 1
    _minval = -1.0;
    _maxval = 1.0;
#else
    vtkIdType m;

    double* pntvalues = NULL;

    _minval = 1.0e20;
    _maxval = -1.0e20;

    for(m = 0; m < polyData->GetNumberOfPoints(); ++m)
    {
        pntvalues = polyData->GetPoint(m);

        if(_minval > pntvalues[2])
           _minval = pntvalues[2];
        if(_maxval < pntvalues[2])
           _maxval = pntvalues[2];

        if(pntvalues[2] > 0.0)
            cout << "\tPoint " << m << ": {" << pntvalues[0] << ", " << pntvalues[1] << ", " << pntvalues[2] << "}" << endl;
        if(pntvalues[3] > 1.0e-20)
            cout << "\tPoint " << m << ": {" << pntvalues[0] << ", " << pntvalues[1] << ", " << pntvalues[3] << "}" << endl;
    }

    if(0.001 > (_maxval - _minval))
        _maxval = _minval + 0.001;
#endif

    cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
    cout << "\t_minval = " << _minval << ", _maxval = " << _maxval << endl;
}

void VTK2dViewer::_draw_rect(double x[4], double y[4], double z[4])
{
    int n;

    glLineWidth(1.0);

    glBegin(GL_LINE_STRIP);
        for(n = 0; n < 4; ++n)
            glVertex3f(x[n], y[n], z[n]);

        glVertex3f(x[0], y[0], z[n]);
    glEnd();
}

void VTK2dViewer::_draw_grid()
{
  //int lvl = nvoptions->get_zsec();
  //double x[4], y[4], z[4];

    cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;

    glPushMatrix();

  //Draw lines 
    glColor3f(0.0, 1.0, 1.0);

    glPopMatrix();
}

void VTK2dViewer::_set_color(double val)
{
    float color[4];
    int idx = 0;

  //if((val > _lowbound) && ( val < _upbound))
  //{
        idx = (int) ((((float)val - _minval) * _colorlen) / (_maxval - _minval));

        color[0] = _colormap[3*(idx+3)];
        color[1] = _colormap[3*(idx+3) + 1];
        color[2] = _colormap[3*(idx+3) + 2];
      //color[3] = 0.25 + (val - _minval) / (_maxval - _minval);
        color[3] = 0.005 + 0.75 * (val - _minval) / (_maxval - _minval);
  //}
  //else
  //{
  //    color[0] = 0.95;
  //    color[1] = 0.95;
  //    color[2] = 0.95;
  //    color[3] = 0.125;
  //}

    glColor4fv(color);
}

void VTK2dViewer::_fill_quad(double lon[4], double lat[4], double rad[4], double v[4])
{
    int n;
    double x, y;

    glBegin(GL_QUADS);
        for(n = 0; n < 4; ++n)
        {
            _set_color(v[n]);
            _latlon2xy(lat[n], lon[n], x, y);
            glVertex3d(x, y, rad[n]);
        }
    glEnd();
}

void VTK2dViewer::_display()
{
    int lvl = nvoptions->get_zsec();
    vtkIdType m, n;
    GLuint zcl = 0;

    double x[4], y[4], z[4], v[4];

    double* pntvalues = NULL;

    vtkCell *cell = NULL;

    vtkIdList* idlist = NULL;
    vtkIdType npts;

    zcl = glGenLists(1);

  //cout << "\nfile: " << __FILE__;
  //cout << "\tline: " << __LINE__;
  //cout << "\tfunction: " << __PRETTY_FUNCTION__ << endl;
  //cout << "\tzcl = " << zcl << endl;

    glNewList(zcl, GL_COMPILE_AND_EXECUTE);

  //OpenGL should normalize normal vectors
  //glEnable(GL_NORMALIZE);

    glEnable(GL_BLEND);
  //glBlendFunc(GL_SRC_ALPHA,GL_ONE);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

  //glShadeModel(GL_SMOOTH);

  //glDisable(GL_CULL_FACE);
  //glEnable(GL_CULL_FACE);

    glPushMatrix();

  //glDisable(GL_CULL_FACE);

  //glNormal3f(0.0, 0.0, -1.0);

    cout << "\nfile: " << __FILE__;
    cout << "\tline: " << __LINE__;
    cout << "\tfunction: " << __PRETTY_FUNCTION__ << endl;

    cout << "polyData has " << polyData->GetNumberOfPoints() << " points." << endl;
    cout << "polyData has " << polyData->GetNumberOfCells() << " cells." << endl;

    _set_minmax();
#if 0
    for(m = 0; m < polyData->GetNumberOfPoints(); ++m)
    {
        pntvalues = polyData->GetPoint(m);
    }
#endif

    for(m = 0; m < polyData->GetNumberOfCells(); ++m)
    {
        cell = polyData->GetCell(m);

      //cout << "cell " << m << " has " << cell->GetNumberOfPoints() << " points." << endl;

        idlist = cell->GetPointIds();

        for(n = 0; n < cell->GetNumberOfPoints(); ++n)
        {
            npts = idlist->GetId(n);

          //cout << "idlist->GetId(" << n << ") = " << npts << endl;

            pntvalues = polyData->GetPoint(npts);
                
            x[n] = pntvalues[0];
            y[n] = pntvalues[1];
            z[n] = 0.0;
            v[n] = pntvalues[2];

          //cout << "\tPoint " << n << ": {" << x[n] << ", " << y[n] << ", " << v[n] << "}" << endl;

            _fill_quad(x, y, z, v);
        }
    }

    glEndList();

    lister->set_zid(lvl, zcl);

    glPopMatrix();
}

void VTK2dViewer::_latlon2polar(double lat, double lon, double r, double &x, double &y, double &z)
{
    double delt = r * cos(lat * DEG2ARC);

    x = delt * sin(lon * DEG2ARC);
    y =    r * sin(lat * DEG2ARC);
    z = delt * cos(lon * DEG2ARC);
}

void VTK2dViewer::_latlon2xy(double lat, double lon, double &x, double &y)
{
    x = lon/180.0;
    y = lat/180.0;
}

