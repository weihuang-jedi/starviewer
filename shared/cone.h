#ifndef _CONE_H_
#define _CONE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include <iostream>

using namespace std;

/*************************************************
 * The base point: x, y, z.
 *   -       ^
 *   |     / | \
 * height /  |  \
 *   |   /   |   \
 *   |  /    |    \
 *   | /     |     \
 *   |/------+radius\
 *         (x,y,z)
 * Assume the cone's biggest circle centered at (x, y, z).
 * Then the cone is perpendiculer to x-y plane
 * (or the height is parallel to z-axis).
 */

class Cone
{
    public:
        Cone();
       ~Cone();

        void draw();
        void set_scale(double x, double y, double z);
        void set_rotation(double a, double x, double y, double z);
        void set_translation(double x, double y, double z);
        void set_shape(double h, double r);
        void set_sides(int n);
        void set_color(float c[4]);

    private:
        double rx, ry, rz;
        double sx, sy, sz;
        double tx, ty, tz;
        double angle;
        double height;
        double radius;

        double* xc;
        double* yc;

        int number_of_sides;

        float color[4];
};
#endif

