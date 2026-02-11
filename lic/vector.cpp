#include <math.h>
#include <stdio.h>

#include "vector.h"

Vector::Vector(const double cx, const double cy, const double cz)
{
    coord[0] = cx;
    coord[1] = cy;
    coord[2] = cz;
}

Vector::Vector(const Vector &v)
{
    coord[0] = v.coord[0];
    coord[1] = v.coord[1];
    coord[2] = v.coord[2];
}

Vector Vector::operator=(const Vector &v)
{
    coord[0] = v.coord[0];
    coord[1] = v.coord[1];
    coord[2] = v.coord[2];

    return *this;
}

Vector Vector::operator*(const Vector &v1)
{
    return Vector(   coord[1] * v1.coord[2] -
                  v1.coord[1] *    coord[2],
                     coord[2] * v1.coord[0] -
                  v1.coord[2] *    coord[0],
                     coord[0] * v1.coord[1] -
                  v1.coord[0] *    coord[1]);
}

Vector Vector::operator*(const double c)
{
    return Vector(coord[0]*c,
                  coord[1]*c,
                  coord[2]*c);
}

Vector Vector::operator+=(const Vector &v1)
{
    coord[0] += v1.coord[0];
    coord[1] += v1.coord[1];
    coord[2] += v1.coord[2];

    return *this;
}

Vector Vector::operator-=(const Vector &v1)
{
    coord[0] -= v1.coord[0];
    coord[1] -= v1.coord[1];
    coord[2] -= v1.coord[2];

    return *this;
}

Vector Vector::operator*=(const double c)
{
    coord[0] *= c;
    coord[1] *= c;
    coord[2] *= c;

    return *this;
}

Vector Vector::operator/=(double d)
{
    double c = 1./d;
    
    coord[0] *= c;
    coord[1] *= c;
    coord[2] *= c;

    return *this;
}

Vector Vector::operator+(const Vector &v1)
{
    return Vector(coord[0] + v1.coord[0],
                  coord[1] + v1.coord[1],
                  coord[2] + v1.coord[2]);
}

Vector Vector::operator-(const Vector &v1)
{
    return Vector(coord[0] - v1.coord[0],
                  coord[1] - v1.coord[1],
                  coord[2] - v1.coord[2]);
}

Vector Vector::operator-()
{
    return Vector(-coord[0],
                  -coord[1],
                  -coord[2]);
}

Vector Vector::operator/(double d)
{
    double c = 1./d;

    return Vector(c*coord[0],
                  c*coord[1],
                  c*coord[2]);
}

double Vector::operator,(const Vector &v1)
{
    return (v1.coord[0] * coord[0] +
            v1.coord[1] * coord[1] +
            v1.coord[2] * coord[2]);
}

double Vector::sizesqd()
{
    return (coord[0]*coord[0] +
            coord[1]*coord[1] +
            coord[2]*coord[2]);
}

double Vector::size()
{
    return sqrt(sizesqd());
}

Vector Vector::unit()
{
    return (*this)/size();
}

void Vector::Print()
{
    fprintf(stderr, "\tcoord(%lf,%lf,%lf)\n", coord[0], coord[1], coord[2]);
}

int Vector::iszero()
{
    if(sizesqd() < EPSZERO) 
        return 1;
    else
        return 0;
}

double distance(const Vector v1, const Vector v2)
{
    Vector v = v1;
    v -= v2;
    return v.size();
}

double dsqrd(const Vector v1, const Vector v2)
{
    Vector v = v1;
    v -= v2;
    return v.sizesqd();
}

