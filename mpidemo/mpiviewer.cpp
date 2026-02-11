//$Id: mpiviewer.cpp 4863 2013-12-18 23:11:35Z starviewer $

#include <QtOpenGL>

#include "mpiviewer.h"

MPIViewer::MPIViewer()
{
    int i, j, n = 0;

    step = 0;

    setup(4, 3, 5, 1);

    single_processor = new MPIProcessor();
    processor = new MPIProcessor[nprocs];

    single_processor->set_global_size(nx, ny, nz);
    single_processor->decomposition(1, 1);

    for(j = 0; j < nrows; ++j)
    {
        for(i = 0; i < ncols; ++i)
        {
          //cout << "\nfile: <" << __FILE__ << ">";
          //cout << "\tFunctions: <" << __PRETTY_FUNCTION__ << ">";
          //cout << "\tline: " << __LINE__ << endl;
          //cout << "\tSetup processor: " << n << endl;

            processor[n].set_global_size(nx, ny, nz);
            processor[n].decomposition(ncols, nrows);
            processor[n].set_halo_width(2);
            processor[n].set_numb_procs(nprocs);
            processor[n].set_rank(n, i, j);

            ++n;
        }
    }
}

MPIViewer::~MPIViewer()
{
    delete single_processor;
    delete [] processor;
}

void MPIViewer::setup(int col, int row, int sxy, int sz)
{
    ncols = col;
    nrows = row;
    nprocs = nrows * ncols;

    nx = sxy * ncols;
    ny = sxy * nrows;
    nz = sz * nprocs;
}

void MPIViewer::draw()
{
    int n;

  //cout << "\nfile: <" << __FILE__ << ">";
  //cout << "\tFunctions: <" << __PRETTY_FUNCTION__ << ">";
  //cout << "\tline: " << __LINE__ << endl;
  //cout << "\tstep = " << step << endl;

  //Clear screen and Z-buffer
  //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  //Enable Z-buffering in OpenGL
  //glEnable(GL_DEPTH_TEST);

  //glPushMatrix();

    switch(step)
    {
        case 0:
             single_processor->draw_global_domain();
             break;
        case 1:
             for(n = 0; n < nprocs; ++n)
                 processor[n].draw_decomposed();
             break;
        case 2:
        case 6:
        case 9:
        case 12:
             for(n = 0; n < nprocs; ++n)
                 processor[n].draw_without_halo();
             break;
        case 3:
             for(n = 0; n < nprocs; ++n)
                 processor[n].draw_with_halo();
             break;
        case 4:
             for(n = 0; n < nprocs; ++n)
                 processor[n].draw_row_exchanged();
             break;
        case 5:
             for(n = 0; n < nprocs; ++n)
                 processor[n].draw_with_halo_filled();
             break;
        case 7:
             for(n = 0; n < nprocs; ++n)
                 processor[n].draw_xtranspose_whole();
             break;
        case 8:
             for(n = 0; n < nprocs; ++n)
                 processor[n].draw_xtransposed();
             break;
        case 10:
             for(n = 0; n < nprocs; ++n)
                 processor[n].draw_ytranspose_whole();
             break;
        case 11:
             for(n = 0; n < nprocs; ++n)
                 processor[n].draw_ytransposed();
             break;
        default:
             cout << "\nfile: <" << __FILE__ << ">";
             cout << "\tFunctions: <" << __PRETTY_FUNCTION__ << ">";
             cout << "\tline: " << __LINE__ << endl;
             cout << "\tDo not know how to process step: " << step << endl;
    }

  //glPopMatrix();
}

