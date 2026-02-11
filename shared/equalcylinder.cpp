#include "equalcylinder.h"
#include <QtOpenGL>

EqualCylinder::EqualCylinder()
{
    color[0] = 0.0;
    color[1] = 0.0;
    color[2] = 1.0;
    color[3] = 1.0;

    number_of_sides = 36;
    set_sides();
    set_shape(1.0, 1.0);
    set_scale(1.0, 1.0);
    set_rotation(0.0, 0.0, 0.0, 0.0);
    set_translation(0.0, 0.0, 0.0);

    ++instance;
}

EqualCylinder::~EqualCylinder()
{
    --instance;

  //cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">";
  //cout << "\tline: " << __LINE__;
  //cout << "\tfile: <" << __FILE__ << ">" << endl;
  //cout << "\tinstance = " << instance << endl;
}

void EqualCylinder::set_rotation(double a, double x, double y, double z)
{
    angle = a;
    rx = x;
    ry = y;
    rz = z;
}

void EqualCylinder::set_scale(double h, double r)
{
    sx = r;
    sy = r;
    sz = h;
}

void EqualCylinder::set_translation(double x, double y, double z)
{
    tx = x;
    ty = y;
    tz = z;
}

void EqualCylinder::set_shape(double h, double r)
{
    int i;
    double angle, delta;

    height = h;
    radius = r;

  //cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">";
  //cout << "\tline: " << __LINE__;
  //cout << "\tfile: <" << __FILE__ << ">" << endl;
  //cout << "\tinstance = " << instance << endl;
  //cout << "\theight = " << height << ", radius = " << radius << endl;

    delta = 2.0 * 3.1415926535897932 / number_of_sides;
    
    for(i = 0; i < number_of_sides; i++)
    {
        angle = delta * i;
        x1[i] = radius * cos(angle);
        y1[i] = radius * sin(angle);

      //cout << "\t\tx1[" << i << "] = " << x1[i] << ", y1[" << i << "] = " << y1[i] << endl;
    }

    x1[number_of_sides] = x1[0];
    y1[number_of_sides] = y1[0];
}

void EqualCylinder::set_color(float c[4])
{
    memcpy(color, c, 4*sizeof(float));
}

void EqualCylinder::set_sides()
{
    int i;
    double angle, delta;

  //cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">";
  //cout << "\tline: " << __LINE__;
  //cout << "\tfile: <" << __FILE__ << ">" << endl;
  //cout << "\tn: <" << n << ">" << endl;
  //cout << "\theight = " << height << ", r1 = " << r1 << ", r2 = " << r2 << endl;

    number_of_sides = NUMBER_OF_SIDES;

    delta = 2.0 * 3.1415926535897932 / number_of_sides;
    
    for(i = 0; i < number_of_sides; i++)
    {
        angle = delta * i;
        x1[i] = radius * cos(angle);
        y1[i] = radius * sin(angle);
    }

    x1[number_of_sides] = x1[0];
    y1[number_of_sides] = y1[0];
}

void EqualCylinder::draw()
{
    int i;

  //cout << "\nEnter functions: <" << __PRETTY_FUNCTION__ << ">";
  //cout << "\tline: " << __LINE__;
  //cout << "\tfile: <" << __FILE__ << ">" << endl;
  //cout << "\tnumber_of_sides = " << number_of_sides << endl;

    glPushMatrix();

    glTranslatef(tx, ty, tz);
    glScalef(sx, sy, sz);
    glRotatef(angle, rx, ry, rz);

    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1,1);

    glColor4fv(color);

    glBegin(GL_QUAD_STRIP);
        for(i = 0; i <= number_of_sides; i++)
        {
            glVertex3d(x1[i],   y1[i],   0.0);
            glVertex3d(x1[i],   y1[i],   height);
        }
    glEnd();

    glDisable(GL_POLYGON_OFFSET_FILL);

    glPopMatrix();

  //cout << "\tinstance = " << instance << endl;
  //cout << "\nLeave functions: <" << __PRETTY_FUNCTION__ << ">";
  //cout << "\tline: " << __LINE__;
  //cout << "\tfile: <" << __FILE__ << ">\n" << endl;
}

int EqualCylinder::instance = 0;

