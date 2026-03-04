#include <QOpenGLWidget>
#include "axis.h"

//
//  Constructor
//
//Axis::Axis(QWidget* parent) : QGLWidget(parent)
Axis::Axis()
{
    _setup();
}

//Axis::~Axis()
//{
//}

void Axis::draw()
{
   double arrow_length = _ratio * _axis_length;

   _x_axis(_axis_length, arrow_length, _arrow_angle);
   _y_axis(_axis_length, arrow_length, _arrow_angle);
   _z_axis(_axis_length, arrow_length, _arrow_angle);
}

void Axis::drawOneLine(double x1, double y1, double z1,
                       double x2, double y2, double z2)
{
    glBegin(GL_LINES);
        glVertex3f(x1, y1, z1);
        glVertex3f(x2, y2, z2);
    glEnd();
}

void Axis::_setup()
{
   _ratio = 0.05;
   _axis_length = 1.2;
   _arrow_angle = 30.0;
}

void Axis::_x_axis(double x, double length, double angle)
{
    double arc = GL_PI/180.0;
    double d_l = length * cos(angle * arc);
    double d_h = length * sin(angle * arc);
    double x1  = x - d_l;
    double y1  = + d_h;
    double y2  = - d_h;
    double z1  = + d_h;
    double z2  = - d_h;

    GLfloat line_width = 1.5;

    glLineWidth(line_width);

    glColor4f(1.0, 0.0, 0.0, 0.75);

    drawOneLine(0.0, 0.0, 0.0, x, 0.0, 0.0);

  //glEnable (GL_LINE_STIPPLE);
  //    glLineStipple (1, 0x0101);  /*dotted*/
  //    drawOneLine(0.0, 0.0, 0.0, -x, 0.0, 0.0);
  //glDisable (GL_LINE_STIPPLE);

    glBegin(GL_POLYGON);
        glVertex3d(  x, 0.0, 0.0);
        glVertex3d(  x1, y1, 0.0);
        glVertex3d(  x1, y2, 0.0);
    glEnd();

    glBegin(GL_POLYGON);
        glVertex3d(  x,  0.0, 0.0);
        glVertex3d(  x1, 0.0, z1);
        glVertex3d(  x1, 0.0, z2);
    glEnd();

    renderText(x1, 0.0, z2 - d_h, "X");
}

void Axis::_y_axis(double y, double length, double angle)
{
    double arc = GL_PI/180.0;
    double d_l = length * cos(angle * arc);
    double d_h = length * sin(angle * arc);
    double y1  = y - d_l;
    double x1  = + d_h;
    double x2  = - d_h;
    double z1  = + d_h;
    double z2  = - d_h;

    GLfloat line_width = 1.5;

    glLineWidth(line_width);

    glColor4f(0.0, 0.0, 1.0, 0.75);

    drawOneLine(0.0, 0.0, 0.0, 0.0, y, 0.0);

  //glEnable (GL_LINE_STIPPLE);
  //    glLineStipple (1, 0x0101);  /*dotted*/
  //    drawOneLine(0.0, 0.0, 0.0, 0.0, -y, 0.0);
  //glDisable (GL_LINE_STIPPLE);

    glBegin(GL_POLYGON);
        glVertex3d(0.0,  y, 0.0);
        glVertex3d( x1, y1, 0.0);
        glVertex3d( x2, y1, 0.0);
    glEnd();

    glBegin(GL_POLYGON);
        glVertex3d(0.0, y,  0.0);
        glVertex3d(0.0, y1, z1);
        glVertex3d(0.0, y1, z2);
    glEnd();

  //renderText(x2 - d_h, y1, 0.0, "Y");
}

void Axis::_z_axis(double z, double length, double angle)
{
    double arc = GL_PI/180.0;
    double d_l = length * cos(angle * arc);
    double d_h = length * sin(angle * arc);
    double z1  = z - d_l;
    double x1  = + d_h;
    double x2  = - d_h;
    double y1  = + d_h;
    double y2  = - d_h;

    GLfloat line_width = 1.5;

    glLineWidth(line_width);

    glColor4f(0.0, 1.0, 0.0, 0.75);

    drawOneLine(0.0, 0.0, 0.0, 0.0, 0.0, z);

  //glEnable (GL_LINE_STIPPLE);
  //    glLineStipple (1, 0x0101);  /*dotted*/
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

  //renderText(x2 - d_h, 0.0, z1, "Z");
}

