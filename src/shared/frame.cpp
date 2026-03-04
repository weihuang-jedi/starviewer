#include <frame.h>

Frame::Frame()
{
}

Frame::~Frame()
{
}

void Frame::draw()
{
    double xmin = 0.0;
    double xmax = (double) _nx;
    double ymin = 0.0;
    double ymax = (double) _ny;
    double zmin = 0.0;
    double zmax = (double) _phmax;

    glPushMatrix();

    glRotatef(-90.0, 1.0, 0.0, 0.0);

    glScalef(_sx, _sy, _sz);

    glTranslatef(-_tx, -_ty, 0.0);

    glColor3f(1, 1, 1);

    glBegin(GL_LINES);
      //The bottom
        glVertex3d(xmin,ymin,zmin);
        glVertex3d(xmax,ymin,zmin);

        glVertex3d(xmax,ymin,zmin);
        glVertex3d(xmax,ymax,zmin);

        glVertex3d(xmax,ymax,zmin);
        glVertex3d(xmin,ymax,zmin);

        glVertex3d(xmin,ymax,zmin);
        glVertex3d(xmin,ymin,zmin);

      //The top
        glVertex3d(xmin,ymin,zmax);
        glVertex3d(xmax,ymin,zmax);

        glVertex3d(xmax,ymin,zmax);
        glVertex3d(xmax,ymax,zmax);

        glVertex3d(xmax,ymax,zmax);
        glVertex3d(xmin,ymax,zmax);

        glVertex3d(xmin,ymax,zmax);
        glVertex3d(xmin,ymin,zmax);

      //4 vertical edges
        glVertex3d(xmin,ymin,zmin);
        glVertex3d(xmin,ymin,zmax);

        glVertex3d(xmax,ymin,zmin);
        glVertex3d(xmax,ymin,zmax);

        glVertex3d(xmax,ymax,zmin);
        glVertex3d(xmax,ymax,zmax);

        glVertex3d(xmin,ymax,zmin);
        glVertex3d(xmin,ymax,zmax);
    glEnd();

    glPopMatrix();
}

