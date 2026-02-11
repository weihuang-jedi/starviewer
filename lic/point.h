#ifndef _POINT_H
#define _POINT_H

#include "licvector.h"

class Point
{
    public:
        Point(const double cx=0, const double cy=0,
              const double cz=0, const double ch=1);
        Point(const Point &p);

        LICvector operator-(const Point &p);
        LICvector operator-(const LICvector &v);
        LICvector operator*(const LICvector &v2);

        Point operator+(const LICvector &v);
        Point operator+(const Point &p);
        Point operator*(const double c);
        Point operator/(double c);

        Point operator+=(const Point &p);
        Point operator+=(const LICvector &v);
        Point operator-=(const Point &p);
        Point operator=(const LICvector &v);
        Point operator=(const Point &p);

        Point ToBasis(const Point &c, const LICvector &u,
                      const LICvector &v, const LICvector &w);
        Point ToCanonical(const Point &c, const LICvector &u,
                          const LICvector &v, const LICvector &w);

        double operator,(const LICvector &v);
        double operator,(const Point &p);

        double size();

        void Print();

        inline operator LICvector()
        {
	    return (LICvector(coord[0],coord[1],coord[2]));
        }
    
        double coord[4];
};

void blend(const Point &p1, const Point &p2,
           const double t, Point &pr);
double distance(const Point &p1, const Point &p2);
#endif

