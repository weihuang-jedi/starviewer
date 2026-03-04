#ifndef _CYLINDER_H_
#define _CYLINDER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include <iostream>

using namespace std;

/*************************************************
 * The base point: x, y, z.
 *   +      /--+r1\
 *   |     /   |   \
 *   |    /    |    \
 * height/     |     \
 *   |  /      |      \
 *   | /       |       \
 *   +/--------+--r-2---\
 *          (x,y,z)
 * Assume the cylinder's larger circle centered at (x, y, z).
 * Then the cylinder is perpendiculer to x-y plane
 * (or the height is parallel to z-axis).
 */

class Cylinder
{
    public:
        Cylinder();
       ~Cylinder();

        void draw();
        void set_scale(double x, double y, double z);
        void set_rotation(double a, double x, double y, double z);
        void set_translation(double x, double y, double z);
        void set_shape(double h, double rd1, double rd2);
        void set_sides(int n);
        void set_color(float c[4]);
        int  get_instances() { return instance; };

    private:
        double rx, ry, rz;
        double sx, sy, sz;
        double tx, ty, tz;
        double angle;
        double height;
        double r1, r2;

        double* x1;
        double* y1;
        double* x2;
        double* y2;

        int number_of_sides;

        float color[4];

        void _initialize_sides();
        static int instance;
};
#endif

