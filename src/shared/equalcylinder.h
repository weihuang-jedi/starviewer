#ifndef _EQUALCYLINDER_H_
#define _EQUALCYLINDER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include <iostream>

using namespace std;

/*************************************************
 * The base point: x, y, z.
 *   +      |------+--r1--|
 *   |      |      |      |
 *   |      |      |      |
 * height   |      |      |
 *   |      |      |      |
 *   |      |      |      |
 *   +      |------+--r1--|
 *              (x,y,z)
 * Assume the cylinder's larger circle centered at (x, y, z).
 * Then the cylinder is perpendiculer to x-y plane
 * (or the height is parallel to z-axis).
 */

#define	NUMBER_OF_SIDES	36

class EqualCylinder
{
    public:
        EqualCylinder();
       ~EqualCylinder();

        void draw();
        void set_scale(double h, double r);
        void set_rotation(double a, double x, double y, double z);
        void set_translation(double x, double y, double z);
        void set_shape(double h, double r);
        void set_color(float c[4]);
        int  get_instances() { return instance; };

    private:
        double rx, ry, rz;
        double sx, sy, sz;
        double tx, ty, tz;
        double angle;
        double height;
        double radius;

        double x1[NUMBER_OF_SIDES + 1];
        double y1[NUMBER_OF_SIDES + 1];

        int number_of_sides;

        float color[4];

        void set_sides();

        static int instance;
};
#endif

