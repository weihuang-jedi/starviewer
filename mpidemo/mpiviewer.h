#ifndef _MPIVIEWER_H
#define _MPIVIEWER_H

//$Id: mpiviewer.h 4855 2013-12-17 18:06:31Z starviewer $

#include <iostream>
#include <vector>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "mpiprocessor.h"

using namespace std;

class MPIViewer
{
    public:
        MPIViewer();
       ~MPIViewer();

        void draw();

        void set_step(int n) { step = n; };
        void setup(int row, int col, int sxy = 10, int sz = 1);

    protected:
        MPIProcessor* processor;
        MPIProcessor* single_processor;

        int step;
        int nx, ny, nz;
        int nprocs, nrows, ncols;
};
#endif

