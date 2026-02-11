#ifndef _LICVECTOR_H
#define _LICVECTOR_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define EPSZERO 1E-12

class LICvector
{
    public:
        LICvector(const double cx=0, const double cy=0, const double cz=0);
        LICvector(const LICvector &v);

        LICvector operator=(const LICvector &v);
        LICvector operator*(const LICvector &v1);
        LICvector operator*(const double c);
        LICvector operator+=(const LICvector &v1);
        LICvector operator-=(const LICvector &v1);
        LICvector operator*=(const double c);
        LICvector operator/=(double c);
        LICvector operator+(const LICvector &v1);
        LICvector operator-(const LICvector &v1);
        LICvector operator-();
        LICvector operator/(double c);

        double operator,(const LICvector &v1);
        double sizesqd();
        double size();
    
        void Print();

        int iszero();
    
        LICvector unit();

        double coord[3];
};

double distance(const LICvector v1, const LICvector v2);
double dsqrd(const LICvector v1, const LICvector v2);

#endif

