#include "interp.h"

Interp::Interp()
{
    _farwest = 1.0;
    _fareast = 1.0e20;
    _farsouth = 1.0;
    _farnorth = 1.0e20;
    _surface = 0.0;
    _overtop = 1.0e20;
}

Interp::~Interp()
{
}

double Interp::interpt(double t, double tleng, float v1, float v2)
{
    double dt = t / tleng;
    return ((1.0 - dt)*v1 + dt*v2);
}

double Interp::interpt(double t, double tleng, double v1, double v2)
{
    double dt = t / tleng;
    return ((1.0 - dt)*v1 + dt*v2);
}

double Interp::interpz(double z, float z_bot, float z_top, float v_bot, float v_top)
{
    double dz = (z - z_bot) / (z_top - z_bot);
    return ((1.0 - dz)*v_bot + dz*v_top);
}

double Interp::interpz(double z, float z_bot, float z_top, double v_bot, double v_top)
{
    double dz = (z - z_bot) / (z_top - z_bot);
    return ((1.0 - dz)*v_bot + dz*v_top);
}

double Interp::interpz(double z, double z_bot, double z_top, double v_bot, double v_top)
{
    double dz = (z - z_bot) / (z_top - z_bot);
    return ((1.0 - dz)*v_bot + dz*v_top);
}

double Interp::interp1d(double x, float v1, float v2)
{
    int ix = (int)x;
    double dx = x - ix;
    return ((1.0 - dx)*v1 + dx*v2);
}

double Interp::interp1d(double x, double v1, double v2)
{
    int ix = (int)x;
    double dx = x - ix;
    return ((1.0 - dx)*v1 + dx*v2);
}

double Interp::interp2d(double x, double y,
                        float v_west_south, float v_east_south,
                        float v_west_north, float v_east_north)
{
    double v_south = interp1d(x, v_west_south, v_east_south);
    double v_north = interp1d(x, v_west_north, v_east_north);
    return interp1d(y, v_south, v_north);
}

double Interp::interp2d(double x, double y,
                        double v_west_south, double v_east_south,
                        double v_west_north, double v_east_north)
{
    double v_south = interp1d(x, v_west_south, v_east_south);
    double v_north = interp1d(x, v_west_north, v_east_north);
    return interp1d(y, v_south, v_north);
}

