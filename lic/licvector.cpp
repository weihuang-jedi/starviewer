#include <math.h>
#include <stdio.h>

#include "licvector.h"

LICvector::LICvector(const double cx, const double cy, const double cz)
{
    coord[0] = cx;
    coord[1] = cy;
    coord[2] = cz;
}

LICvector::LICvector(const LICvector &v)
{
    coord[0] = v.coord[0];
    coord[1] = v.coord[1];
    coord[2] = v.coord[2];
}

LICvector LICvector::operator=(const LICvector &v)
{
    coord[0] = v.coord[0];
    coord[1] = v.coord[1];
    coord[2] = v.coord[2];

    return *this;
}

LICvector LICvector::operator*(const LICvector &v1)
{
    return LICvector(   coord[1] * v1.coord[2] -
                  v1.coord[1] *    coord[2],
                     coord[2] * v1.coord[0] -
                  v1.coord[2] *    coord[0],
                     coord[0] * v1.coord[1] -
                  v1.coord[0] *    coord[1]);
}

LICvector LICvector::operator*(const double c)
{
    return LICvector(coord[0]*c,
                  coord[1]*c,
                  coord[2]*c);
}

LICvector LICvector::operator+=(const LICvector &v1)
{
    coord[0] += v1.coord[0];
    coord[1] += v1.coord[1];
    coord[2] += v1.coord[2];

    return *this;
}

LICvector LICvector::operator-=(const LICvector &v1)
{
    coord[0] -= v1.coord[0];
    coord[1] -= v1.coord[1];
    coord[2] -= v1.coord[2];

    return *this;
}

LICvector LICvector::operator*=(const double c)
{
    coord[0] *= c;
    coord[1] *= c;
    coord[2] *= c;

    return *this;
}

LICvector LICvector::operator/=(double d)
{
    double c = 1./d;
    
    coord[0] *= c;
    coord[1] *= c;
    coord[2] *= c;

    return *this;
}

LICvector LICvector::operator+(const LICvector &v1)
{
    return LICvector(coord[0] + v1.coord[0],
                  coord[1] + v1.coord[1],
                  coord[2] + v1.coord[2]);
}

LICvector LICvector::operator-(const LICvector &v1)
{
    return LICvector(coord[0] - v1.coord[0],
                  coord[1] - v1.coord[1],
                  coord[2] - v1.coord[2]);
}

LICvector LICvector::operator-()
{
    return LICvector(-coord[0],
                  -coord[1],
                  -coord[2]);
}

LICvector LICvector::operator/(double d)
{
    double c = 1./d;

    return LICvector(c*coord[0],
                  c*coord[1],
                  c*coord[2]);
}

double LICvector::operator,(const LICvector &v1)
{
    return (v1.coord[0] * coord[0] +
            v1.coord[1] * coord[1] +
            v1.coord[2] * coord[2]);
}

double LICvector::sizesqd()
{
    return (coord[0]*coord[0] +
            coord[1]*coord[1] +
            coord[2]*coord[2]);
}

double LICvector::size()
{
    return sqrt(sizesqd());
}

LICvector LICvector::unit()
{
    return (*this)/size();
}

void LICvector::Print()
{
    fprintf(stderr, "\tcoord(%lf,%lf,%lf)\n", coord[0], coord[1], coord[2]);
}

int LICvector::iszero()
{
    if(sizesqd() < EPSZERO) 
        return 1;
    else
        return 0;
}

double distance(const LICvector v1, const LICvector v2)
{
    LICvector v = v1;
    v -= v2;
    return v.size();
}

double dsqrd(const LICvector v1, const LICvector v2)
{
    LICvector v = v1;
    v -= v2;
    return v.sizesqd();
}

