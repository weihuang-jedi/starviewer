#ifndef _Interp_H_
#define _Interp_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include <iostream>

using namespace std;

/*************************************************
 * The base point: x, y, z.
 *   +--------|--------+
 *   v1   dx  v  1.0-dx v2
 *  v = (1.0 - dx) * v1 + dx * v2
 */

class Interp
{
    public:
        Interp();
       ~Interp();

        double interpt(double t, double tleng, float v1, float v2);
        double interpt(double t, double tleng, double v1, double v2);
        double interpz(double z, float z_bot, float z_top, float v_bot, float v_top);
        double interpz(double z, float z_bot, float z_top, double v_bot, double v_top);
        double interpz(double z, double z_bot, double z_top, double v_bot, double v_top);
        double interp1d(double x, float v1, float v2);
        double interp1d(double x, double v1, double v2);
        double interp2d(double x, double y,
                        float v_west_south, float v_east_south,
                        float v_west_north, float v_east_north);
        double interp2d(double x, double y,
                        double v_west_south, double v_east_south,
                        double v_west_north, double v_east_north);

    private:
        double _farwest, _fareast;
        double _farsouth, _farnorth;
        double _surface, _overtop;
};
#endif

