#ifndef _VECTOR_H
#define _VECTOR_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define EPSZERO 1E-12

class Vector
{
    public:
        Vector(const double cx=0, const double cy=0, const double cz=0);
        Vector(const Vector &v);

        Vector operator=(const Vector &v);
        Vector operator*(const Vector &v1);
        Vector operator*(const double c);
        Vector operator+=(const Vector &v1);
        Vector operator-=(const Vector &v1);
        Vector operator*=(const double c);
        Vector operator/=(double c);
        Vector operator+(const Vector &v1);
        Vector operator-(const Vector &v1);
        Vector operator-();
        Vector operator/(double c);

        double operator,(const Vector &v1);
        double sizesqd();
        double size();
    
        void Print();

        int iszero();
    
        Vector unit();

        double coord[3];
};

double distance(const Vector v1, const Vector v2);
double dsqrd(const Vector v1, const Vector v2);

#endif

