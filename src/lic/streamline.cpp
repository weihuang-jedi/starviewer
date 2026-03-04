//$Id: streamline.cpp 4833 2013-11-05 15:49:31Z starviewer $

#include <QtOpenGL>

#include <iostream>

#include "streamline.h"

StreamLine::StreamLine()
{
    maxsteps = MAXSTEPS;
    presteps = maxsteps;

    onethird = 1.0/3.0;
    onesixth = 1.0/6.0;

    fwd = new Point[2*maxsteps];
    bwd = new Point[2*maxsteps];
}

StreamLine::~StreamLine()
{
    delete [] fwd;
    delete [] bwd;
}

void StreamLine::set_maxsteps(int ms)
{
    maxsteps = ms;

    if(ms != presteps)
    {
        delete [] fwd;
        delete [] bwd;

        fwd = new Point[2*maxsteps];
        bwd = new Point[2*maxsteps];
    }

    presteps = maxsteps;
}

void StreamLine::GenStreamLine(int i, int j)
{
    int k;

    Point b,f;

  //std::cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">, line: "
  //     << __LINE__ << ", file: <" << __FILE__ << ">" << std::endl;
  //std::cout << "\tStart: (" << i << ", " << j << "):" << std::endl;

    origin = f = b = getCenter(i,j);

    for(k = 0; k < get_maxsteps(); ++k)
    {
        RK(f,Ht);
        fwd[k] = f;

      //printf("f:");
      //f.Print();

        RK(b,-Ht);
        bwd[k] = b;

      //printf("b:");
      //b.Print();
    }
}

int StreamLine::getPixel(const Point p, int &i, int &j)
{
    i = (int)p.coord[0];
    j = (int)p.coord[1];

    if((i >= 0) && (i < nx) && (j >= 0) && (j < ny))
        return 1;
    else
        return 0;
}

LICvector StreamLine::getVector(const Point &p)
{
    int i,j;

    if(getPixel(p,i,j))
    {
        return vecdata[j][i];
    }

    return(LICvector(0,0,0));
}

void StreamLine::RK(Point &p, double h)
{
    LICvector v;
    LICvector k1,k2,k3,k4;

    v = getVector(p);
    if (!v.iszero())
        v = v.unit();
  
    k1 = v*h;
    v = getVector(p+k1*.5);
    if (!v.iszero())
        v = v.unit();
  
    k2 = v*h;
    v = getVector(p+k2*.5);
    if (!v.iszero())
        v = v.unit();
  
    k3 = v*h;
    v = getVector(p+k3);
    if (!v.iszero())
        v = v.unit();
  
    k4 = v*h;
    p += k1*onesixth + k2*onethird + k3*onethird + k4*onesixth;
}

