#ifndef _STREAMLINE_H
#define _STREAMLINE_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "licvector.h"
#include "point.h"

#define Ht .5
#define MAXSTEPS	20

class StreamLine
{
    public:
        StreamLine();
       ~StreamLine();

        void GenStreamLine(int i, int j);

        int getPixel(const Point p, int &i, int &j);
        int get_maxsteps() { return maxsteps; };

        Point* get_fwd_points() { return fwd; };
        Point* get_bwd_points() { return bwd; };

        Point getCenter(int i, int j) { return Point(i+.5,j+.5); };

        LICvector getVector(const Point &p);

        void set_nx(int r) { nx = r; };
        void set_ny(int c) { ny = c; };
        void set_maxsteps(int ms);
        void set_origin(Point o) { origin = o; };
        void set_vecdata(LICvector **vd) { vecdata = vd; };
  
        inline Point &operator[](int m)
        {
            if(0 == m) 
                return origin;
            else if (0 < m)
                return fwd[m-1];
            else
                return bwd[-m-1];
        }
  
    private:
        int nx;
        int ny;
        int maxsteps;
        int presteps;

        double onethird;
        double onesixth;

        LICvector **vecdata;

        Point origin;
        Point* fwd;
        Point* bwd;
        void  RK(Point &, double);
};
#endif

