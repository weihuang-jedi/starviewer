#include <math.h>
#include <stdio.h>

#include "position.h"

Position::Position(const double cx, const double cy, const double cz)
{
    init(cx, cy, cz);
}

Position::Position(const Position &v)
{
    init(v.x, v.y, v.z);
}

void Position::init(const double cx, const double cy, const double cz)
{
    x = cx;
    y = cy;
    z = cz;

    length = sqrt(x*x + y*y + z*z);
}

Position Position::operator=(const Position &v)
{
    init(v.x, v.y, v.z);

    outbound = v.outbound;

    return *this;
}

Position Position::operator*(const Position &v1)
{
    return Position(y * v1.z - v1.y * z,
                    z * v1.x - v1.z * x,
                    x * v1.y - v1.x * y);
}

Position Position::operator*(const double c)
{
    return Position(x*c, y*c, z*c);
}

Position Position::operator+=(const Position &v1)
{
    x += v1.x;
    y += v1.y;
    z += v1.z;

    return *this;
}

Position Position::operator-=(const Position &v1)
{
    x -= v1.x;
    y -= v1.y;
    z -= v1.z;

    return *this;
}

Position Position::operator*=(const double c)
{
    x *= c;
    y *= c;
    z *= c;

    return *this;
}

Position Position::operator/=(double d)
{
    double c = 1./d;
    
    x *= c;
    y *= c;
    z *= c;

    return *this;
}

Position Position::operator+(const Position &v1)
{
    return Position(x + v1.x, y + v1.y, z + v1.z);
}

Position Position::operator-(const Position &v1)
{
    return Position(x - v1.x, y - v1.y, z - v1.z);
}

Position Position::operator-()
{
    return Position(-x, -y, -z);
}

Position Position::operator/(double d)
{
    double c = 1./d;

    return Position(c*x, c*y, c*z);
}

Position Position::unit()
{
    if(1.0e-6 > length)
        return Position(0.0, 0.0, 1.0);
    else
        return (*this)/length;
}

double distance(const Position v1, const Position v2)
{
    Position v = v1;
    v -= v2;
    return v.get_length();
}

double dotproduct(const Position v1, const Position v2)
{
    return (v1.x*v2.x + v1.y*v2.y + v1.z*v2.z);
}

