//$Id: mpicube.cpp 5341 2015-02-27 23:24:55Z starviewer $

#include "mpicube.h"

MPICube::MPICube()
{
    reset();
}

MPICube::~MPICube()
{
}

void MPICube::reset()
{
    _x1 = 0.0;
    _y1 = 0.0;
    _z1 = 0.0;
    _d = 1.0;

    _x2 = _x1 + _d;
    _y2 = _y1 + _d;
    _x2 = _z1 + _d;

    _cubecolor[0] = 0.50;
    _cubecolor[1] = 0.50;
    _cubecolor[2] = 0.50;
    _cubecolor[3] = 0.25;

    _linecolor[0] = 0.0;
    _linecolor[1] = 0.125;
    _linecolor[2] = 0.875;
    _linecolor[3] = 0.5;
}

void MPICube::set_cubecolor(float c[4])
{
    _cubecolor[0] = c[0];
    _cubecolor[1] = c[1];
    _cubecolor[2] = c[2];
    _cubecolor[3] = c[3];
}

void MPICube::set_linecolor(float c[4])
{
    _linecolor[0] = c[0];
    _linecolor[1] = c[1];
    _linecolor[2] = c[2];
    _linecolor[3] = c[3];
}

void MPICube::set_origin(float x, float y, float z)
{
    _x1 = x;
    _y1 = y;
    _z1 = z;

    _x2 = _x1 + _d;
    _y2 = _y1 + _d;
    _z2 = _z1 + _d;
}

void MPICube::set_length(float d)
{
    _d = d;
}

void MPICube::set_alpha(float a)
{
    _cubecolor[3] = a;
}

void MPICube::draw()
{
  //cout << "\nfile: <" << __FILE__ << ">";
  //cout << "\tFunctions: <" << __PRETTY_FUNCTION__ << ">";
  //cout << "\tline: " << __LINE__ << endl;
  //cout << "\tx, y, z = " << _x << ", " << _y << ", " << _z << endl;

    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1, 1);
    drawcube();
    glDisable(GL_POLYGON_OFFSET_FILL);

    drawline();
}

void MPICube::drawcube()
{
  //cout << "\nfile: <" << __FILE__ << ">";
  //cout << "\tFunctions: <" << __PRETTY_FUNCTION__ << ">";
  //cout << "\tline: " << __LINE__ << endl;
  //cout << "\tx, y, z = " << _x << ", " << _y << ", " << _z << endl;

    glColor4fv(_cubecolor);

  //Front
    glBegin(GL_QUADS);
    glNormal3f(0, 0, 1);
    glVertex3f(_x1, _y1, _z2);
    glVertex3f(_x2, _y1, _z2);
    glVertex3f(_x2, _y2, _z2);
    glVertex3f(_x1, _y2, _z2);
    glEnd();

  //Back
    glBegin(GL_QUADS);
    glNormal3f(0, 0, -1);
    glVertex3f(_x2, _y1, _z1);
    glVertex3f(_x1, _y1, _z1);
    glVertex3f(_x1, _y2, _z1);
    glVertex3f(_x2, _y2, _z1);
    glEnd();

  //Right
    glBegin(GL_QUADS);
    glNormal3f(1, 0, 0);
    glVertex3f(_x2, _y1, _z2);
    glVertex3f(_x2, _y1, _z1);
    glVertex3f(_x2, _y2, _z1);
    glVertex3f(_x2, _y2, _z2);
    glEnd();

  //Left
    glBegin(GL_QUADS);
    glNormal3f(-1, 0, 0);
    glVertex3f(_x1, _y1, _z1);
    glVertex3f(_x1, _y1, _z2);
    glVertex3f(_x1, _y2, _z2);
    glVertex3f(_x1, _y2, _z1);
    glEnd();

  //Top
    glBegin(GL_QUADS);
    glNormal3f(0, 1, 0);
    glVertex3f(_x1, _y2, _z2);
    glVertex3f(_x2, _y2, _z2);
    glVertex3f(_x2, _y2, _z1);
    glVertex3f(_x1, _y2, _z1);
    glEnd();

  //Bottom
    glBegin(GL_QUADS);
    glNormal3f(0, -1, 0);
    glVertex3f(_x1, _y1, _z1);
    glVertex3f(_x2, _y1, _z1);
    glVertex3f(_x2, _y1, _z2);
    glVertex3f(_x1, _y1, _z2);
    glEnd();
}

void MPICube::drawline()
{
  //cout << "\nfile: <" << __FILE__ << ">";
  //cout << "\tFunctions: <" << __PRETTY_FUNCTION__ << ">";
  //cout << "\tline: " << __LINE__ << endl;
  //cout << "\tx, y, z = " << _x << ", " << _y << ", " << _z << endl;

    glLineWidth(4.0);
    glColor4fv(_linecolor);

  //Front
    glBegin(GL_LINE_STRIP);
    glVertex3f(_x1, _y1, _z2+0.001);
    glVertex3f(_x2, _y1, _z2+0.001);
    glVertex3f(_x2, _y2, _z2+0.001);
    glVertex3f(_x1, _y2, _z2+0.001);
    glVertex3f(_x1, _y1, _z2+0.001);
    glEnd();

  //Back
    glBegin(GL_LINE_STRIP);
    glVertex3f(_x2, _y1, _z1-0.001);
    glVertex3f(_x1, _y1, _z1-0.001);
    glVertex3f(_x1, _y2, _z1-0.001);
    glVertex3f(_x2, _y2, _z1-0.001);
    glVertex3f(_x2, _y1, _z1-0.001);
    glEnd();

  //Right
    glBegin(GL_LINE_STRIP);
    glVertex3f(_x2+0.001, _y1, _z2);
    glVertex3f(_x2+0.001, _y1, _z1);
    glVertex3f(_x2+0.001, _y2, _z1);
    glVertex3f(_x2+0.001, _y2, _z2);
    glVertex3f(_x2+0.001, _y1, _z2);
    glEnd();

  //Left
    glBegin(GL_LINE_STRIP);
    glVertex3f(_x1-0.001, _y1, _z1);
    glVertex3f(_x1-0.001, _y1, _z2);
    glVertex3f(_x1-0.001, _y2, _z2);
    glVertex3f(_x1-0.001, _y2, _z1);
    glVertex3f(_x1-0.001, _y1, _z1);
    glEnd();

  //Top
    glBegin(GL_LINE_STRIP);
    glVertex3f(_x1, _y2+0.001, _z2);
    glVertex3f(_x2, _y2+0.001, _z2);
    glVertex3f(_x2, _y2+0.001, _z1);
    glVertex3f(_x1, _y2+0.001, _z1);
    glVertex3f(_x1, _y2+0.001, _z2);
    glEnd();

  //Bottom
    glBegin(GL_LINE_STRIP);
    glVertex3f(_x1, _y1-0.001, _z1);
    glVertex3f(_x2, _y1-0.001, _z1);
    glVertex3f(_x2, _y1-0.001, _z2);
    glVertex3f(_x1, _y1-0.001, _z2);
    glVertex3f(_x1, _y1-0.001, _z1);
    glEnd();
}
 
