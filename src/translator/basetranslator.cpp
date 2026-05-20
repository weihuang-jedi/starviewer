#include <QtOpenGL>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h> // Ensure this header is included

#include "basetranslator.h"

#if 0
void gluPerspective(double fovy,double aspect, double zNear, double zFar)
{
 // Start in projection mode.
 glMatrixMode(GL_PROJECTION);
 glLoadIdentity();
 double xmin, xmax, ymin, ymax;
 ymax = zNear * tan(fovy * M_PI / 360.0);
 ymin = -ymax;
 xmin = ymin * aspect;
 xmax = ymax * aspect;
 glFrustum(xmin, xmax, ymin, ymax, zNear, zFar);
}
#endif

//Constructor
BaseTranslator::BaseTranslator(ColorTable* ct, NVOptions* opt, QWidget* parent)
              : QGLWidget(parent)
{
  //cout << "\tEnter functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;

    colorTable = ct;
    nvoptions = opt;

    _initialize();

    makeCurrent();

    _time = 0.0;

    deg2rad = 3.1415926535897932 / 180.0;
    d2r = 3.14159 / 180.0;

    nx = 1;
    ny = 1;
    nz = 1;
    nt = 1;

    _isDummy = false;

    _curFile = 0;
    _maxFile = 1;

    _glbTime = 0;
    _preTime = 0;
    _curTime = 0;
    _maxTime = 1;
    _nTimes = NULL;

#ifdef OUTHISTORY
    outf.open("./.nv_history");

    if(! outf.is_open())
    {
        outf.close();
        throw(errno);
    }
#endif

    _jpgNotSaved = true;
    _saveJpg = false;
    _startSave = false;

    locator = nullptr;
    light = nullptr;

  //cout << "\tLeave functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
}

BaseTranslator::~BaseTranslator()
{
}

void BaseTranslator::set_filename(QString flnm)
{
    _filename = flnm.toStdString();
}

void BaseTranslator::_initialize()
{
    xRot = 0;
    yRot = 0;
    zRot = 0;

    mouse = 0;          //  Mouse movement

    aspect = 1.0;           //  Aspect ratio
    fovy   = 20.0;
    zNear  = 0.001;
    zFar   = 100.0;

    dim = 0.75;          //  World dimension

    m_d_left_plane = -dim;
    m_d_right_plane = dim;
    m_d_bottom_plane = -dim;
    m_d_top_plane = dim;
    m_d_near_plane = -dim;
    m_d_far_plane = dim;

    ori_xRot = xRot;
    ori_yRot = yRot;
    ori_zRot = zRot;
    ori_dim  = dim;
    ori_fovy = fovy;
    ori_zFar = zFar; 
    ori_zNear = zNear;
}

/********************************************************************/
/*************************  Set parameters  *************************/
/********************************************************************/

void BaseTranslator::selectProjection(int p)
{
    nvoptions->set_cb(NV_STATUS_CHANGED, true);
    if(p)
        nvoptions->set_cb(NV_ISPERSPECTIVE, true);
    else
        nvoptions->set_cb(NV_ISPERSPECTIVE, false);

    updateGL();   //  Request redisplay
}

//axis on/off
void BaseTranslator::selectAxis(int f)
{
    nvoptions->set_cb(NV_STATUS_CHANGED, true);
    if(f)
        nvoptions->set_cb(NV_AXISON, true);
    else
        nvoptions->set_cb(NV_AXISON, false);

    updateGL();   //  Request redisplay
}

//Reset view angle
void BaseTranslator::reset(void)
{
    _initialize();
    updateGL();   //  Request redisplay
}

/******************************************************************/
/*************************  Mouse Events  *************************/
/******************************************************************/
//
//  Mouse pressed
//
void BaseTranslator::mousePressEvent(QMouseEvent* e)
{
   mouse = true;
   pos = e->pos();  //  Remember mouse location
}

//
//  Mouse released
//
void BaseTranslator::mouseReleaseEvent(QMouseEvent* e)
{

    if(mouse && (e->pos() == pos))
    {
      //cout << "\nIn functions: <" << __PRETTY_FUNCTION__
      //     << ">, line: " << __LINE__
      //     << ", file: <" << __FILE__ << ">" << endl;
      //cout << "\te->pos().x() = " << e->pos().x() << endl;
      //cout << "\te->pos().y() = " << e->pos().y() << endl;

        if(NULL != nvoptions)
        {
            nvoptions->set_clicked(true);
            nvoptions->set_position(e->pos());
        }
    }

    mouse = false;
}

//
//  Mouse moved
//
void BaseTranslator::mouseMoveEvent(QMouseEvent* e)
{
    if(mouse)
    {
        QPoint d = e->pos()-pos;  //  Change in mouse location

        zRot = (zRot+d.x())%360;  //  BaseTranslator y movement to elevation
        xRot = (xRot+d.y())%360;  //  BaseTranslator x movement to azimuth
        pos = e->pos();           //  Remember new location

        updateGL();               //  Request redisplay
    }
}

//
//  Mouse wheel
//
void BaseTranslator::wheelEvent(QWheelEvent* e)
{
   if(e->angleDelta().y() < 0)
   {
     //Zoom out
       if(nvoptions->get_cb(NV_ISPERSPECTIVE))
       {
           if(fovy < 100.0)
               fovy *= 1.05;
       }
       else
       {
           if(dim < 100.0)
               dim *= 1.05;
       }
   }
   else
   {
     //Zoom in
       if(nvoptions->get_cb(NV_ISPERSPECTIVE))
       {
           if(fovy > 0.05)
               fovy *= 0.95;
       }
       else
       {
           if(dim > 0.025)
               dim *= 0.95;
       }
   }

   update();   //  Request redisplay
}

void BaseTranslator::keyPressEvent(QKeyEvent *event)
{
    cout << "\nIn functions: <" << __PRETTY_FUNCTION__
         << ">, line: " << __LINE__
         << ", file: <" << __FILE__ << ">" << endl;
    cout << "\tevent->key() = " << event->key() << endl;

    if(event->key() == Qt::Key_Escape)
    {
        cout << "You pressed ESC\n" << endl;
    }
    else if(event->key() == Qt::Key_A)
    {
        zRot += 5;
        if(360 < zRot)
            zRot -= 360;
    }
    else if(event->key() == Qt::Key_C)
    {
        zRot -= 5;
        if(0 > zRot)
            zRot += 360;
    }

    updateGL();
}

void BaseTranslator::keyReleaseEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Escape)
    {
        cout << "You released ESC\n" << endl;
    }

    updateGL();
}

/*******************************************************************/
/*************************  OpenGL Events  *************************/
/*******************************************************************/

//
//  Initialize
//
void BaseTranslator::initializeGL()
{
  //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  //glEnable(GL_DEPTH_TEST); //  Enable Z-buffer depth testing
    setMouseTracking(true);  //  Ask for mouse events
}

//
//  Window is resized
//
void BaseTranslator::resizeGL(int width, int height)
{
  //cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
  //Window aspect ration
    aspect = (width && height) ? width / (float)height : 1;

    sWidth = width;
    sHeight = height;

    if(NULL != nvoptions)
    {
        nvoptions->set_screenWidth(width);
        nvoptions->set_screenHeight(height);
    }

  //Viewport is whole screen
    glViewport(0,0,width,height);

  //Set projection
  //project();
}

//Set projection
void BaseTranslator::set_modelview()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

   //Switch to manipulating the model matrix
     glMatrixMode(GL_MODELVIEW);

   //Undo previous transformations
     glLoadIdentity();
}

//Set projection
void BaseTranslator::project()
{
  //glMatrixMode(GL_PROJECTION);
  //glLoadIdentity();

    if(nvoptions->get_cb(NV_ISPERSPECTIVE))
    {
       /*Perspective transformation*/
         gluPerspective(fovy, aspect, zNear, zFar);
    }
    else
    {
        /*Orthogonal projection*/
#if 1
        if(aspect > 1.0)
            glOrtho(m_d_left_plane*aspect, m_d_right_plane*aspect, m_d_bottom_plane, m_d_top_plane, m_d_near_plane, m_d_far_plane);
        else
            glOrtho(m_d_left_plane, m_d_right_plane, m_d_bottom_plane*aspect, m_d_top_plane*aspect, m_d_near_plane, m_d_far_plane);
#else
         if(aspect > 1.0)
             glOrtho(-dim*aspect, dim*aspect, -dim, dim, -dim, dim);
         else
             glOrtho(-dim, dim, -dim/aspect, dim/aspect, -dim, dim);
#endif
    }

   //Switch to manipulating the model matrix
   //glMatrixMode(GL_MODELVIEW);

   //Undo previous transformations
   //glLoadIdentity();
}

void BaseTranslator::x_axis(double x, double length, double angle)
{
    double d_l = length * cos(angle * deg2rad);
    double d_h = length * sin(angle * deg2rad);
    double x1  = x - d_l;
    double y1  = + d_h;
    double y2  = - d_h;
    double z1  = + d_h;
    double z2  = - d_h;

    GLfloat line_width = 1.5;

  //cout << "Functions: <" << __PRETTY_FUNCTION__
  //     << ">, line: " << __LINE__
  //     << ", file: <" << __FILE__ << ">" << endl;

    glLineWidth(line_width);

    glColor4f(1.0, 0.0, 0.0, 0.75);

    glEnable (GL_LINE_STIPPLE);

  //glLineStipple (1, 0x0101); // dotted
    glLineStipple (1, 0x00FF); // dashed

    glBegin(GL_LINES);
        glVertex3f( -x, 0.0, 0.0);
        glVertex3f(0.0, 0.0, 0.0);
    glEnd();

    glDisable (GL_LINE_STIPPLE);

    glBegin(GL_LINES);
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(  x, 0.0, 0.0);
    glEnd();

    glBegin(GL_TRIANGLES);
        glVertex3d(  x, 0.0, 0.0);
        glVertex3d(  x1, y1, 0.0);
        glVertex3d(  x1, y2, 0.0);

        glVertex3d(  x,  0.0, 0.0);
        glVertex3d(  x1, 0.0, z1);
        glVertex3d(  x1, 0.0, z2);
    glEnd();

    renderText(x1, 0.0, z2 - d_h, "X");
}

void BaseTranslator::y_axis(double y, double length, double angle)
{
    double d_l = length * cos(angle * deg2rad);
    double d_h = length * sin(angle * deg2rad);
    double y1  = y - d_l;
    double x1  = + d_h;
    double x2  = - d_h;
    double z1  = + d_h;
    double z2  = - d_h;

    GLfloat line_width = 1.5;

  //cout << "Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;

    glLineWidth(line_width);

    glColor4f(0.0, 1.0, 0.0, 0.75);

    glEnable (GL_LINE_STIPPLE);

  //glLineStipple (1, 0x0101); // dotted
    glLineStipple (1, 0x00FF); // dashed 
    
    glBegin(GL_LINES);
        glVertex3f(0.0,  -y, 0.0);
        glVertex3f(0.0, 0.0, 0.0);
    glEnd();

    glDisable (GL_LINE_STIPPLE);

    glBegin(GL_LINES);
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(0.0,   y, 0.0);
    glEnd();

    glBegin(GL_TRIANGLES);
        glVertex3d(0.0,  y, 0.0);
        glVertex3d( x1, y1, 0.0);
        glVertex3d( x2, y1, 0.0);

        glVertex3d(0.0, y,  0.0);
        glVertex3d(0.0, y1, z1);
        glVertex3d(0.0, y1, z2);
    glEnd();

    renderText(x2 - d_h, y1, 0.0, "Y");
}

void BaseTranslator::z_axis(double z, double length, double angle)
{
    double d_l = length * cos(angle * deg2rad);
    double d_h = length * sin(angle * deg2rad);
    double z1  = z - d_l;
    double x1  = + d_h;
    double x2  = - d_h;
    double y1  = + d_h;
    double y2  = - d_h;

    GLfloat line_width = 1.5;

  //cout << "Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;

    glLineWidth(line_width);

    glColor4f(0.0, 0.0, 1.0, 0.75);

    glEnable (GL_LINE_STIPPLE);

  //glLineStipple (1, 0x0101); // dotted
    glLineStipple (1, 0x00FF); // dashed
    
    glBegin(GL_LINES);
        glVertex3f(0.0, 0.0,  -z);
        glVertex3f(0.0, 0.0, 0.0);
    glEnd();

    glDisable (GL_LINE_STIPPLE);

    glBegin(GL_LINES);
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(0.0, 0.0,   z);
    glEnd();

    glBegin(GL_TRIANGLES);
        glVertex3d(0.0, 0.0, z);
        glVertex3d( x1, 0.0, z1);
        glVertex3d( x2, 0.0, z1);

        glVertex3d(0.0, 0.0, z);
        glVertex3d(0.0,  y1, z1);
        glVertex3d(0.0,  y2, z1);
    glEnd();

    renderText(x2 - d_h, 0.0, z1, "Z");
}

//select save image
void BaseTranslator::selectSaveImage()
{
   SaveImage();
}

void BaseTranslator::SaveImage(char *flnm)
{
    bool withAlpha = true;
  //Copy from OpenGL
    QImage *tempImage = new QImage(grabFrameBuffer(withAlpha));

    if(tempImage)
    {
      //tempImage->save("test.bmp", "BMP");
        tempImage->save(flnm, "PNG");
      //Cleaning memory
        delete tempImage;
    }
}

void BaseTranslator::update_frame()
{
    updateGL();
}

void BaseTranslator::selectBackground(int f)
{
    nvoptions->set_cb(NV_STATUS_CHANGED, true);
    if(f)
        nvoptions->set_cb(NV_BGBLACK, true);
    else
        nvoptions->set_cb(NV_BGBLACK, false);

    updateGL();
}

//shader on/off
void BaseTranslator::selectShader(int f)
{
    nvoptions->set_cb(NV_STATUS_CHANGED, true);
    if(f)
        nvoptions->set_cb(NV_SHADERON, true);
    else
        nvoptions->set_cb(NV_SHADERON, false);

    updateGL();
}

void BaseTranslator::_set_current_time()
{
    int n;

    _curFile = 0;
    _curTime = _glbTime;

    for(n = 0; n < _maxFile; ++n)
    {
        if(_curTime < _nTimes[n])
        {
           _curFile = n;
           break;
        }
        _curTime -= _nTimes[n];
    }

    if(_curTime >= _glbTime)
        _curTime = _glbTime;

    if(_curFile >= _maxFile)
        _curFile = _maxFile;
}

//Draw color bar
void BaseTranslator::drawColorBar()
{
    if(nvoptions->get_cb(NV_COLORBARON))
    {
        _displayColorBar();
    }
}

void BaseTranslator::setBackgroundColor()
{
    if(nvoptions->get_cb(NV_BGBLACK))
    {
      //Set the background color black.
        glClearColor(0.0, 0.0, 0.0, 1.0);
    }
    else
    {
      //Set the forekground color white.
        glClearColor(1.0, 1.0, 1.0, 1.0);
    }
}

void BaseTranslator::setLabelColor()
{
    if(nvoptions->get_cb(NV_BGBLACK))
    {
      //Set the label color black.
        glColor4f(1.0, 1.0, 1.0, 1.0);
    }
    else
    {
      //Set the label color white.
        glColor4f(0.0, 0.0, 0.0, 1.0);
    }
}

void BaseTranslator::setViewOptions()
{
    float eX = 0.0;
    float eY = 0.0;
    float eZ = 7.5;
    double dimsize = 1.0;

    cout << "Enter Functions: <" << __PRETTY_FUNCTION__
         << ">, line: " << __LINE__
         << ", file: <" << __FILE__ << ">" << endl;

    if(locator && locator->on())
    {
        cout << "line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;

        if(nvoptions->get_cb(NV_ISPERSPECTIVE))
        {
            fovy = pow(0.9525, (double) locator->z()) * locator->get_fovy();
            zNear = pow(1.115, (double) locator->z()) * locator->get_znear();
            zFar = pow(1.1125, (double) locator->z()) * locator->get_zfar();
        }
        else
        {
            dimsize = pow(0.9525, (double) locator->z()) * locator->get_dim();
            m_d_left_plane = -dimsize;
            m_d_right_plane = dimsize;
            m_d_bottom_plane = -dimsize;
            m_d_top_plane = dimsize;
        }

        cout << "line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;

        project();

        cout << "line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;

      //eZ = (0.99 * locator->z() + 1.0) * locator->get_height();
      //eZ = 4.0 * locator->get_height();

        if(nvoptions->get_cb(NV_ISPERSPECTIVE))
        {
            gluLookAt(eX, eY, eZ,
                      0.0, 0.0, 0.0,
                      0.0, 1.0, 0.0);
            glTranslatef( 0.0, 0.0, -0.125);
        }

        if(nvoptions->get_cb(NV_FLATON))
        {
            eX = (double)locator->x()/180.0 - 1.0;
            eY = (double)locator->y()/180.0;
            glTranslated(-eX, -eY, 0.0);
        }
        else
        {
            eX = (double)locator->x();
            eY = (double)locator->y();

           //Set rotation
             glRotated(eY, 1,0,0);
             glRotated(-eX, 0,1,0);
        }

        cout << "line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    }
    else
    {
        cout << "line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;

        project();

        if(nvoptions->get_cb(NV_ISPERSPECTIVE))
        {
            gluLookAt(eX, eY, eZ,
                      0.0, 0.0, 0.0,
                      0.0, 1.0, 0.0);
        }

        cout << "line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;

        if(! nvoptions->get_cb(NV_PIXELON))
        {
          //Set rotation
            glRotated(xRot, 1,0,0);
            glRotated(zRot, 0,1,0);
        }

        cout << "line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    }

    cout << "Leave Functions: <" << __PRETTY_FUNCTION__
         << ">, line: " << __LINE__
         << ", file: <" << __FILE__ << ">" << endl;
}

void BaseTranslator::drawAxis()
{
    double len = 1.5;

  //if(_axisOn)
    if(nvoptions->get_cb(NV_AXISON))
    {
        x_axis(1.50*len, 0.05*len, 30.0);
        y_axis(len, 0.05*len, 30.0);
        z_axis(len, 0.05*len, 30.0);
    }
}

void BaseTranslator::selectX2(int n)
{
    nvoptions->set_cb(NV_STATUS_CHANGED, true);
    if(n)
        nvoptions->set_cb(NV_HASX2, true);
    else
        nvoptions->set_cb(NV_HASX2, false);
}

void BaseTranslator::selectY2(int n)
{
    nvoptions->set_cb(NV_STATUS_CHANGED, true);
    if(n)
        nvoptions->set_cb(NV_HASY2, true);
    else
        nvoptions->set_cb(NV_HASY2, false);
}

void BaseTranslator::selectZ2(int n)
{
    nvoptions->set_cb(NV_STATUS_CHANGED, true);
    if(n)
        nvoptions->set_cb(NV_HASZ2, true);
    else
        nvoptions->set_cb(NV_HASZ2, false);
}

void BaseTranslator::selectT2(int n)
{
    nvoptions->set_cb(NV_STATUS_CHANGED, true);
    if(n)
        nvoptions->set_cb(NV_HAST2, true);
    else
        nvoptions->set_cb(NV_HAST2, false);
}

void BaseTranslator::setValueX(int n)
{
    nvoptions->set_cb(NV_STATUS_CHANGED, true);
    if(nvoptions->get_cb(NV_HASX2))
        nvoptions->set_xsec2(n);
    else
        nvoptions->set_xsec(n);

    updateGL();
}

void BaseTranslator::setValueY(int n)
{
  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\tn = " << n << endl;

    nvoptions->set_cb(NV_STATUS_CHANGED, true);
    if(nvoptions->get_cb(NV_HASY2))
        nvoptions->set_ysec2(n);
    else
        nvoptions->set_ysec(n);

    updateGL();
}

void BaseTranslator::setValueZ(int n)
{
    nvoptions->set_cb(NV_STATUS_CHANGED, true);
    if(nvoptions->get_cb(NV_HASZ2))
        nvoptions->set_zsec2(n);
    else
        nvoptions->set_zsec(n);

    updateGL();
}

void BaseTranslator::setValueT(int n)
{
    _glbTime = n;

    _set_current_time();

    nvoptions->set_cb(NV_STATUS_CHANGED, true);
    if(nvoptions->get_cb(NV_HAST2))
        nvoptions->set_tsec2(n);
    else
        nvoptions->set_tsec(n);

    updateGL();
}

//animation on/off
void BaseTranslator::selectAnimation(int n)
{
    nvoptions->set_cb(NV_STATUS_CHANGED, true);
    if(n)
        nvoptions->set_cb(NV_ANIMATIONON, true);
    else
        nvoptions->set_cb(NV_ANIMATIONON, false);

    updateGL();
}

//saveJPG on/off
void BaseTranslator::selectJPG4Movie(int n)
{
    nvoptions->set_cb(NV_STATUS_CHANGED, true);
    if(n)
    {
      //_startSave = true;
        _saveJpg = true;
        nvoptions->set_cb(NV_SAVEJPG, true);
    }
    else
    {
      //_startSave = false;
        _saveJpg = false;
        nvoptions->set_cb(NV_SAVEJPG, false);
    }

    updateGL();
}

//isoSurface on/off
void BaseTranslator::selectIsoSurface(int n)
{
    nvoptions->set_cb(NV_STATUS_CHANGED, true);
    if(n)
        nvoptions->set_cb(NV_ISOSURFACEON, true);
    else
        nvoptions->set_cb(NV_ISOSURFACEON, false);

    updateGL();
}

//vector on/off
void BaseTranslator::selectVector(int n)
{
    nvoptions->set_cb(NV_STATUS_CHANGED, true);
    if(n)
        nvoptions->set_cb(NV_VECTORON, true);
    else
        nvoptions->set_cb(NV_VECTORON, false);

    updateGL();
}

//LIC on/off
void BaseTranslator::selectLIC(int n)
{
  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\tn = " << n << endl;

    nvoptions->set_cb(NV_STATUS_CHANGED, true);
    if(n)
        nvoptions->set_cb(NV_LICON, true);
    else
        nvoptions->set_cb(NV_LICON, false);

    updateGL();
}

//isoOnly on/off
void BaseTranslator::selectIsoOnly(int n)
{
  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\tn = " << n << endl;

    nvoptions->set_cb(NV_STATUS_CHANGED, true);
    if(n)
        nvoptions->set_cb(NV_ISOSURFACEONLY, true);
    else
        nvoptions->set_cb(NV_ISOSURFACEONLY, false);

    updateGL();
}

//vecOnly on/off
void BaseTranslator::selectVecOnly(int n)
{
  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\tn = " << n << endl;

    nvoptions->set_cb(NV_STATUS_CHANGED, true);
    if(n)
        nvoptions->set_cb(NV_VECTORONLY, true);
    else
        nvoptions->set_cb(NV_VECTORONLY, false);

    updateGL();
}

//LIConly on/off
void BaseTranslator::selectLIConly(int n)
{
  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\tn = " << n << endl;

    nvoptions->set_cb(NV_STATUS_CHANGED, true);
    if(n)
        nvoptions->set_cb(NV_LICONLY, true);
    else
        nvoptions->set_cb(NV_LICONLY, false);

    updateGL();
}

//vec3D on/off
void BaseTranslator::selectVec3D(int n)
{
  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\tn = " << n << endl;

    nvoptions->set_cb(NV_STATUS_CHANGED, true);
    if(n)
        nvoptions->set_cb(NV_VEC3D, true);
    else
        nvoptions->set_cb(NV_VEC3D, false);

    updateGL();
}

//LIC3D on/off
void BaseTranslator::selectLIC3D(int n)
{
  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\tn = " << n << endl;

    nvoptions->set_cb(NV_STATUS_CHANGED, true);
    if(n)
        nvoptions->set_cb(NV_LIC3D, true);
    else
        nvoptions->set_cb(NV_LIC3D, false);

    updateGL();
}

//colorbar on/off
void BaseTranslator::selectColorBar(int f)
{
    nvoptions->set_cb(NV_STATUS_CHANGED, true);
    if(f)
    {
        nvoptions->set_cb(NV_COLORBARON, true);
    }
    else
        nvoptions->set_cb(NV_COLORBARON, false);

    updateGL();   //  Request redisplay
}

//MarchingCube on/off
void BaseTranslator::selectMCub(int f)
{
    nvoptions->set_cb(NV_RESET, true);
    nvoptions->set_cb(NV_STATUS_CHANGED, true);

    if(f)
        nvoptions->set_cb(NV_MCUBON, true);
    else
        nvoptions->set_cb(NV_MCUBON, false);

    updateGL();   //  Request redisplay
}

//bump on/off
void BaseTranslator::selectBump(int f)
{
    nvoptions->set_cb(NV_RESET, true);
    nvoptions->set_cb(NV_STATUS_CHANGED, true);

    if(f)
        nvoptions->set_cb(NV_BUMPON, true);
    else
        nvoptions->set_cb(NV_BUMPON, false);

    updateGL();   //  Request redisplay
}

//flat on/off
void BaseTranslator::selectFlat(int f)
{
    nvoptions->set_cb(NV_RESET, true);
    nvoptions->set_cb(NV_STATUS_CHANGED, true);

    if(f)
        nvoptions->set_cb(NV_FLATON, true);
    else
        nvoptions->set_cb(NV_FLATON, false);

    updateGL();
}

//sphere on/off
void BaseTranslator::selectSphere(int f)
{
    nvoptions->set_cb(NV_RESET, true);
    nvoptions->set_cb(NV_STATUS_CHANGED, true);

    if(f)
        nvoptions->set_cb(NV_SPHEREON, true);
    else
        nvoptions->set_cb(NV_SPHEREON, false);

    updateGL();
}

//onmap on/off
void BaseTranslator::selectOnMap(int f)
{
    nvoptions->set_cb(NV_RESET, true);
    nvoptions->set_cb(NV_STATUS_CHANGED, true);

    if(f)
        nvoptions->set_cb(NV_ONMAPON, true);
    else
        nvoptions->set_cb(NV_ONMAPON, false);

    updateGL();
}

//title on/off
void BaseTranslator::selectTitle(int f)
{
    nvoptions->set_cb(NV_STATUS_CHANGED, true);
    if(f)
        nvoptions->set_cb(NV_TITLEON, true);
    else
        nvoptions->set_cb(NV_TITLEON, false);

    updateGL();
}

//coastline on/off
void BaseTranslator::selectCoastLine(int f)
{
    nvoptions->set_cb(NV_STATUS_CHANGED, true);
    if(f)
        nvoptions->set_cb(NV_COASTLINEON, true);
    else
        nvoptions->set_cb(NV_COASTLINEON, false);

    updateGL();
}

void BaseTranslator::writeHeader()
{
  //cout << "Enter Function: " << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\tnvoptions->get_cb(NV_TITLEON) = " << nvoptions->get_cb(NV_TITLEON) << endl;
  //cout << "\t_varname.c_str() = " << _varname.c_str() << endl;
  //cout << "\t_timeinfo.c_str() = " << _timeinfo.c_str() << endl;
  //cout << "\t_title.c_str() = " << _title.c_str() << endl;
  //cout << "\t_position.c_str() = " << _position.c_str() << endl;
    if(nvoptions->get_cb(NV_TITLEON))
    {
#if 0
      //QFont font("Times", 24, QFont::Bold);
        QFont font("DejaVu Sans", 24, QFont::Bold);
        font.setStyleStrategy(QFont::NoAntialias); // Try to bypass the texture blender

        // Inside writeHeader()
        const GLubyte* renderer = glGetString(GL_RENDERER);
        const GLubyte* version = glGetString(GL_VERSION);
        const GLubyte* vendor = glGetString(GL_VENDOR);

        cout << "GL Renderer: " << renderer << endl;
        cout << "GL Version:  " << version << endl;
        cout << "GL Vendor:   " << vendor << endl;

        // Inside your function
        Display *dpy = glXGetCurrentDisplay();
        if (dpy) {
            int major, minor;
            if (glXQueryVersion(dpy, &major, &minor)) {
                cout << "GLX Version: " << major << "." << minor << endl;
            }
        } else {
            cout << "No active X11 Display found via GLX." << endl;
        }
#endif
      //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      //qglColor(Qt::blue); // Set text color
      //cout << "In Function: " << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
      //cout << "Has problem to display var name:" << _varname << endl;
      //cout << "and timeinfo:" << _timeinfo << endl;
      //renderText(30, 30, _varname.c_str(), QFont("DejaVu", 16));
      //renderText(30, 60, _timeinfo.c_str(), QFont("DejaVu", 16));

      //QPainter painter(this);
      //painter.setPen(Qt::blue);
      //painter.setFont(QFont("Time", 16));
      //painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
      //painter.drawText(30, 30, _varname.c_str());
      //painter.drawText(30, 60, _timeinfo.c_str());
      //painter.end();

      //renderText(30.0, 30.0, 0.0, _varname.c_str());
      //renderText(30.0, 60.0, 0.0, _timeinfo.c_str());
      //renderText(30.0, 30.0, 0.0, _title.c_str());
      //renderText(30.0, 90.0,0.0,  _position.c_str());
    }
}

void BaseTranslator::writeVarInfo()
{
    emit info(_varinfo);
}

void BaseTranslator::writeFrameInfo()
{
    emit frame(_frameinfo);
}

void BaseTranslator::nextFrame()
{
    ++_glbTime;
    if(_glbTime >= _maxTime)
       _glbTime = 0;

    _set_current_time();

    updateGL();
}

void BaseTranslator::backFrame()
{
    --_glbTime;
    if(_glbTime < 0)
       _glbTime = _maxTime - 1;

    _set_current_time();

    updateGL();
}

void BaseTranslator::writeMouseMsg()
{
    emit mouse_msg(_mouseinfo);
}

void BaseTranslator::writeLocatorMsg()
{
    emit locator_msg(_locatorinfo);
}

void BaseTranslator::selectClearDataview()
{
    nvoptions->set_cb(NV_STATUS_CHANGED, true);
    nvoptions->set_cb(NV_CLEARDATAVIEW, true);
}

void BaseTranslator::selectDataview(int n)
{
    nvoptions->set_cb(NV_STATUS_CHANGED, true);
    if(n)
    {
        nvoptions->set_cb(NV_DATAVIEWON, true);
    }
    else
    {
        nvoptions->set_cb(NV_DATAVIEWON, true);
    }
}

//subset on/off
void BaseTranslator::selectSubset(int n)
{
  //cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">" << endl;
  //cout << "\tfile: <" << __FILE__ << ">" << ", line: " << __LINE__ << endl;
  //cout << "\tn = " << n << endl;

    nvoptions->set_cb(NV_RESET, true);
    nvoptions->set_cb(NV_STATUS_CHANGED, true);
    if(n)
        nvoptions->set_cb(NV_SUBSET, true);
    else
        nvoptions->set_cb(NV_SUBSET, false);

    updateGL();
}

//minmax on/off
void BaseTranslator::selectMinMax(int n)
{
  //cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">" << endl;
  //cout << "\tfile: <" << __FILE__ << ">" << ", line: " << __LINE__ << endl;
  //cout << "\tn = " << n << endl;

    nvoptions->set_cb(NV_STATUS_CHANGED, true);
    if(n)
    {
        nvoptions->set_cb(NV_HASMINMAX, true);
        nvoptions->set_cb(NV_RESET, true);
    }
    else
        nvoptions->set_cb(NV_HASMINMAX, false);

    updateGL();
}

//minmax on/off
void BaseTranslator::save_status()
{
#ifdef OUTHISTORY
    int n = 0;

    outf << xRot << "\t";
    outf << yRot << "\t";
    outf << zRot << "\t";
    outf << dim << "\t";
    outf << fovy << "\t";
    outf << aspect << "\t";
    outf << zNear << "\t";
    outf << zFar << "\t";
    outf << mouse << "\t";
    outf << pos.x() << "\t";
    outf << pos.y() << "\t";

    outf << _curFile << "\t";
    outf << _glbTime << "\t";
    outf << _curTime << "\t";
    outf << _maxTime << "\t";

    outf << sWidth << "\t";
    outf << sHeight << "\t";

    for(n = 0; n < NV_NUMBEROFCBS; ++n)
        outf << nvoptions->get_cb((NV_cbType) n) << "\t";
    outf << endl;
#endif
}

void BaseTranslator::_backupStatus()
{
    cur_xRot = xRot;
    cur_yRot = yRot;
    cur_zRot = zRot;
    cur_dim  = dim;
    cur_fovy = fovy;
    cur_aspect = aspect;
    cur_zNear = zNear;
    cur_zFar = zFar;

    xRot = ori_xRot;
    yRot = ori_yRot;
    zRot = ori_zRot;
    dim  = ori_dim;
    fovy = ori_fovy;
    aspect = ori_aspect;
    zNear = ori_zNear;
    zFar = ori_zFar;
}

void BaseTranslator::_restoreStatus()
{
    xRot = cur_xRot;
    yRot = cur_yRot;
    zRot = cur_zRot;
    dim  = cur_dim;
    fovy = cur_fovy;
    aspect = cur_aspect;
    zNear = cur_zNear;
    zFar = cur_zFar;
}

void BaseTranslator::_displayColorBar()
{
    int maxLev = 5;
    int n, mstep;
    double a, d, s, v;
    double x, y, x1, x2, y1, y2;
    char buf[12];
    char format[8];

    int clen = colorTable->get_clen() - 2;
    float* cmap = colorTable->get_cmap();

    _backupStatus();

  //Reset transformations
    glLoadIdentity();

    a = 1.90;
    s = 1.0 / clen;
    d = (_maxval - _minval) * s;

  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__
  //     << ", function: " << __PRETTY_FUNCTION__ << endl;
  //cout << "\t_minval = " << _minval << ", _maxval = " << _maxval << endl;
  //cout << "\tclen = " << clen << ", maxLev = " << maxLev << endl;

    glPushMatrix();

    glNormal3d(0.0, 0.0, -1.0);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__
  //     << ", function: " << __PRETTY_FUNCTION__ << endl;

    if(_maxval > 100.0)
        strcpy(format, "%11.0f");
    else if(_maxval > 10.0)
        strcpy(format, "%11.1f");
    else if(_maxval > 1.0)
        strcpy(format, "%11.2f");
    else if(_maxval > 0.1)
        strcpy(format, "%11.3f");
    else if(_maxval > 0.01)
        strcpy(format, "%11.4f");
    else if(_maxval > 0.001)
        strcpy(format, "%11.5f");
    else if(_maxval > 0.0001)
        strcpy(format, "%11.6f");
    else
        strcpy(format, "%g");
    
    y1 = -0.900;
    y2 = -0.850;
    for(n = 0; n < clen; ++n)
    {
        x1 = a * (s * n - 0.5);
        x2 = a * (s * (n + 1) - 0.5);
      //y1 = a * (s * n - 0.5);
      //y2 = a * (s * (n + 1) - 0.5);

        glColor4f(cmap[3*(n+2)], cmap[3*(n+2)+1], cmap[3*(n+2)+2], 1.0);
        glBegin(GL_QUADS);
            glVertex2d(x1, y1);
            glVertex2d(x2, y1);
            glVertex2d(x2, y2);
            glVertex2d(x1, y2);
        glEnd();

      //cout << "\tNo " << n << ": x1 = " << x1 << ", y1 = " << y1;
      //cout << "\tcolor: " << n << " = (" << cmap[3*(n+2)];
      //cout << ", " << cmap[3*(n+2)+1];
      //cout << ", " << cmap[3*(n+2)+2] << ")" << endl;
    }

    if(nvoptions->get_cb(NV_BGBLACK))
    {
      //Set the forekground color white.
        glColor4f(1.0, 1.0, 1.0, 1.0);
    }
    else
    {
      //Set the background color red.
        glColor4f(1.0, 0.0, 0.0, 1.0);
    }

    mstep = clen/(maxLev - 1);

  //QFont font("Times", 12, QFont::Bold);
  //QFont font("DejaVu Sans", 12, QFont::Bold);
    QFont font("DejaVu", 12, QFont::Bold);

    a *= 0.95;
    y = -0.975;
    for(n = 0; n <= clen; n += mstep)
    {
        memset(buf, 0, 12);

        v = _minval + n * d;
        sprintf(buf, format, v);

        x = a * (s * n - 0.55);
      //renderText(x, y, 0.0, buf, QFont("Times", 15, QFont::Bold));
        renderText(x, y, 0.0, buf);

      //cout << "\tNo " << n << ": x = " << x << ", y = " << y << ", buf = " << buf << endl;
    }

    glPopMatrix();

    _restoreStatus();
}

void BaseTranslator::updateSliders()
{
    sliderNspinX->set_max(10);
    sliderNspinX->setValue(0);
    sliderNspinX->set_tickinterval(1);
    sliderNspinX->set_step(1);

    sliderNspinY->set_max(10);
    sliderNspinY->setValue(0);
    sliderNspinY->set_tickinterval(1);
    sliderNspinY->set_step(1);

    sliderNspinZ->set_max(10);
    sliderNspinZ->setValue(0);
    sliderNspinZ->set_tickinterval(1);
    sliderNspinZ->set_step(1);

    sliderNspinT->set_max(10);
    sliderNspinT->setValue(0);
    sliderNspinT->set_tickinterval(10);
    sliderNspinT->set_step(1);
}

bool BaseTranslator::FileExists(char *filename)
{
    struct stat fileInfo;
    return stat(filename, &fileInfo) == 0;
}

void BaseTranslator::SaveImage()
{
    int n;

    char flnm[128];

    strcpy(flnm, "test_image_0000.png");

    for(n = 0; n < 10000; ++n)
    {
        sprintf(flnm, "img_%4.4d.png", n);
        if(! FileExists(flnm))
            break;
    }

    SaveImage(flnm);
}

void BaseTranslator::SaveJpg()
{
    SaveImage();
}

void BaseTranslator::SaveJpg(int n)
{
    char flnm[128];

    sprintf(flnm, "img_%4.4d.png", n);
    SaveImage(flnm);
}

void BaseTranslator::set_locator(Locator* l)
{
    locator = l;

    if(locator)
    {
        locator->set_dim(dim);
        locator->set_fovy(fovy);
        locator->set_zfar(zFar);
        locator->set_znear(zNear);
    }
}

void BaseTranslator::set_light(Light* l)
{
    light = l;
}

