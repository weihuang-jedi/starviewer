#include "cone.h"

#include <QtOpenGL>

Cone::Cone()
{
    xc = NULL;
    yc = NULL;

    color[0] = 0.0;
    color[1] = 0.0;
    color[2] = 1.0;
    color[3] = 1.0;

    set_scale(1.0, 1.0, 1.0);
    set_rotation(0.0, 0.0, 0.0, 0.0);
    set_translation(0.0, 0.0, 0.0);
    set_shape(1.0, 1.0);
    set_sides(36);
}

Cone::~Cone()
{
    free(xc);
    free(yc);
}

void Cone::set_rotation(double a, double x, double y, double z)
{
    angle = a;
    rx = x;
    ry = y;
    rz = z;
}

void Cone::set_scale(double x, double y, double z)
{
    sx = x;
    sy = y;
    sz = z;
}

void Cone::set_translation(double x, double y, double z)
{
    tx = x;
    ty = y;
    tz = z;
}

void Cone::set_shape(double h, double r)
{
    height = h;
    radius = r;
  //height = 1.1*h;
  //radius = 1.01*r;
}

void Cone::set_color(float c[4])
{
    memcpy(color, c, 4*sizeof(float));
}

void Cone::set_sides(int n)
{
    int i;
    double angle;
    double delta = 2.0 * 3.1415926535897932 / number_of_sides;

  //cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">";
  //cout << "\tline: " << __LINE__;
  //cout << "\tfile: <" << __FILE__ << ">" << endl;
  //cout << "\tn: <" << n << ">" << endl;
  //cout << "\theight = " << height << ", radius = " << radius << endl;

    if(n != number_of_sides)
    {
        number_of_sides = n;

        if(NULL == xc)
        {
            xc = (double *)calloc(number_of_sides + 1, sizeof(double));
            yc = (double *)calloc(number_of_sides + 1, sizeof(double));
        }
        else
        {
            xc = (double *)realloc(xc, (number_of_sides + 1) * sizeof(double));
            yc = (double *)realloc(yc, (number_of_sides + 1) * sizeof(double));
        }

        assert(xc);
        assert(yc);
    }
    
    for(i = 0; i < number_of_sides; i++)
    {
        angle = delta * i;
        xc[i] = radius * cos(angle);
        yc[i] = radius * sin(angle);

      //cout << "\ti = " << i << ", x = " << xc[i] << ", y = " << yc[i] << endl;
    }

    xc[number_of_sides] = xc[0];
    yc[number_of_sides] = yc[0];

}

void Cone::draw()
{
    int i;

    glPushMatrix();

    glTranslatef(tx, ty, tz);
    glScalef(sx, sy, sz);
    glRotatef(angle, rx, ry, rz);

    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1,1);

    glColor4fv(color);

    glBegin(GL_TRIANGLES);
        for(i = 0; i < number_of_sides; i++)
        {
            glVertex3d(0.0,     0.0,     0.0);
            glVertex3d(xc[i],   yc[i],   0.0);
            glVertex3d(xc[i+1], yc[i+1], 0.0);

            glVertex3d(0.0,     0.0,     height);
            glVertex3d(xc[i],   yc[i],   0.0);
            glVertex3d(xc[i+1], yc[i+1], 0.0);
        }
    glEnd();

    glDisable(GL_POLYGON_OFFSET_FILL);

    glPopMatrix();
}

