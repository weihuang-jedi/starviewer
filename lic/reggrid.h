#ifndef _REGGRID_H
#define _REGGRID_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "licvector.h"
#include "point.h"
#include "streamline.h"
#include "colorTable.h"

class RegGrid
{
    public:
        RegGrid(ColorTable* ct);
       ~RegGrid();
  
        void setup(int nx, int ny);

        int get_nx() { return ny; };
        int get_ny() { return ny; };
        int get_numvalid() { return numvalid; };
        int get_maxsteps() { return maxsteps; };

        int getPixel(const Point p, int &i, int &j);
        int validpt(Point &p);

        double ComputeI(StreamLine &s, int &numvalid);
        double ComputeIFwd(StreamLine &s, double &I, int m, int &numvalid);
        double ComputeIBwd(StreamLine &s, double &I, int m, int &numvalid);
        double getT(Point &p);

        LICvector getLICvector(const Point &p);
  
        void set_maxsteps(int ms) { maxsteps = ms; };
        void set_maxvecmag(double v) { maxvecmag = v; };
        void Print();
        void Normalize();

        int **hitdata;
        int **texdata;

        double **Idata;

        LICvector **vecdata;

    private:
        ColorTable* colorTable;

        int nx;
        int ny;
        int maxsteps;

        int numvalid;

        double maxvecmag;
};
#endif

