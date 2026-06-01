#ifndef POINT3D_H
#define POINT3D_H

struct _point3d
{
    double x;
    double y;
    double z;
};

typedef struct _point3d pnt3d;

void get_2d_norm(pnt3d &pn, pnt3d pnt[]);
#endif
