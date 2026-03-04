#include "licvector.h"
#include "point.h"

Point::Point(const double cx, const double cy,
             const double cz, const double ch)
{
    coord[0] = cx;
    coord[1] = cy;
    coord[2] = cz;
    coord[3] = ch;
}

Point::Point(const Point &p)
{
    coord[0] = p.coord[0];
    coord[1] = p.coord[1];
    coord[2] = p.coord[2];
    coord[3] = p.coord[3];
}

LICvector Point::operator-(const Point &p)
{
    return LICvector(coord[0]-p.coord[0],
          coord[1]-p.coord[1],
          coord[2]-p.coord[2]
          );
}

LICvector Point::operator-(const LICvector &v)
{
    return LICvector(coord[0]-v.coord[0],
                  coord[1]-v.coord[1],
                  coord[2]-v.coord[2]);
}

Point Point::operator+(const LICvector &v)
{
    return Point(coord[0]+v.coord[0],
                 coord[1]+v.coord[1],
                 coord[2]+v.coord[2],
                 1.);
}

Point Point::operator+(const Point &p)
{
    return Point(coord[0]+p.coord[0],
                 coord[1]+p.coord[1],
                 coord[2]+p.coord[2],
                 1.);
}

Point Point::operator+=(const Point &p)
{
    coord[0] += p.coord[0];
    coord[1] += p.coord[1];
    coord[2] += p.coord[2];
    coord[3] = 1.;
    
    return (*this);
}

Point Point::operator+=(const LICvector &v)
{
    coord[0] += v.coord[0];
    coord[1] += v.coord[1];
    coord[2] += v.coord[2];
    coord[3] = 1.;

    return (*this);
}

Point Point::operator-=(const Point &p)
{
    coord[0] -= p.coord[0];
    coord[1] -= p.coord[1];
    coord[2] -= p.coord[2];
    coord[3] = 1.;

    return *this;
}

Point Point::ToBasis(const Point &c, const LICvector &u,
                     const LICvector &v, const LICvector &w)
{
    LICvector v1((*this)-c);

    return Point((v1,u),(v1,v),(v1,w));
}

Point Point::ToCanonical(const Point &c, const LICvector &u,
             const LICvector &v, const LICvector &w)
{
    return Point(coord[0]*u.coord[0]+
                 coord[1]*v.coord[0]+
                 coord[2]*w.coord[0],
                 coord[0]*u.coord[1]+
                 coord[1]*v.coord[1]+
                 coord[2]*w.coord[1],
                 coord[0]*u.coord[2]+
                 coord[1]*v.coord[2]+
                 coord[2]*w.coord[2]) + c;
}

Point Point::operator=(const LICvector &v)
{
    coord[0] = v.coord[0];
    coord[1] = v.coord[1];
    coord[2] = v.coord[2];
    coord[3] = 1.;
    
    return *this;
}

Point Point::operator=(const Point &p)
{
    coord[0] = p.coord[0];
    coord[1] = p.coord[1];
    coord[2] = p.coord[2];
    coord[3] = p.coord[3];

    return *this;
}

LICvector Point::operator*(const LICvector &v2)
{
    LICvector v1 = LICvector(coord[0],coord[1],coord[2]);
    return v1*v2;
}

Point Point::operator*(const double c)
{
    return Point(coord[0]*c,coord[1]*c,coord[2]*c,coord[3]);
}

Point Point::operator/(double c)
{
    double div = 1./c;
    return (*this)*div;
}
 
double Point::operator,(const LICvector &v)
{
    return (coord[0]*v.coord[0] +
            coord[1]*v.coord[1] +
            coord[2]*v.coord[2]);
}

double Point::operator,(const Point &p)
{
    return coord[0]*p.coord[0] +
           coord[1]*p.coord[1] +
           coord[2]*p.coord[2];
}

void Point::Print()
{
    printf("%lf %lf %lf %lf\n",coord[0],coord[1],coord[2],coord[3]);
}

double Point::size()
{
    double dist = sqrt(coord[0] * coord[0]
                     + coord[1] * coord[1]
                     + coord[2] * coord[2]);
    return dist;
}

void blend(const Point &p1, const Point &p2,
           const double t, Point &pr)
{
    double w1,w2;

    w1 = p1.coord[3]*(1.-t);
    w2 = p2.coord[3]*t;

    pr.coord[3] = w1+w2;

    w1 /= pr.coord[3];
    w2 = 1.-w1;

    pr.coord[0] = w1*p1.coord[0] + w2*p2.coord[0];
    pr.coord[1] = w1*p1.coord[1] + w2*p2.coord[1];
    pr.coord[2] = w1*p1.coord[2] + w2*p2.coord[2];
}

double distance(const Point &p1, const Point &p2)
{
    return sqrt((p1.coord[0]-p2.coord[0])*(p1.coord[0]-p2.coord[0])
              + (p1.coord[1]-p2.coord[1])*(p1.coord[1]-p2.coord[1])
              + (p1.coord[2]-p2.coord[2])*(p1.coord[2]-p2.coord[2]));
}

