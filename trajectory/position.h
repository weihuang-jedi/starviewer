#ifndef _Position_H
#define _Position_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define EPSZERO 1E-12

class Position
{
    public:
        Position(const double cx=0, const double cy=0, const double cz=0);
        Position(const Position &v);

        Position operator=(const Position &v);
        Position operator*(const Position &v1);
        Position operator*(const double c);
        Position operator+=(const Position &v1);
        Position operator-=(const Position &v1);
        Position operator*=(const double c);
        Position operator/=(double c);
        Position operator+(const Position &v1);
        Position operator-(const Position &v1);
        Position operator-();
        Position operator/(double c);

        Position unit();

        void init(const double cx=0, const double cy=0, const double cz=0);
        void set_value(double d) { v = d; };
        void set_outbound(bool v) { outbound = v; };

        double get_value() { return v; };
        double get_length() { return length; };
        bool is_outbound() { return outbound; };

        double x, y, z;
        double length;

    private:
        bool outbound;
        double v;
};

double distance(const Position v1, const Position v2);
double dotproduct(const Position v1, const Position v2);

#endif

