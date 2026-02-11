#include "marker.h"

//
//  Constructor
//
Marker::Marker(QWidget* parent)
             : QGLWidget(parent)
{
    _setup();
}

Marker::~Marker()
{
}

void Marker::_setup()
{
   _length = 12.0;
   _height = 1.0;

   _varname = string("notset");
   _timestr = string("notset");

   set_nc(0);
}

void Marker::draw()
{
    double x1  = 0.0;
    double y1  = 0.0;
    double y2  = 0.0;
    double z1  = 0.0;
    double z2  = 0.0;

    GLfloat line_width = 1.5;

    glLineWidth(line_width);

    glColor4f(1.0, 0.0, 0.0, 0.75);

    renderText(0.0, 0.0, 0.0, _varname.c_str());

/*
  //glEnable (GL_LINE_STIPPLE);
  //    glLineStipple (1, 0x0101);  //dotted
  //    drawOneLine(0.0, 0.0, 0.0, 0.0, 0.0, -z);
  //glDisable (GL_LINE_STIPPLE);

    glBegin(GL_POLYGON);
        glVertex3d(0.0, 0.0, z);
        glVertex3d( x1, 0.0, z1);
        glVertex3d( x2, 0.0, z1);
    glEnd();

    glBegin(GL_POLYGON);
        glVertex3d(0.0, 0.0, z);
        glVertex3d(0.0,  y1, z1);
        glVertex3d(0.0,  y2, z1);
    glEnd();
*/
}

