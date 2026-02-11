#ifndef _MPIPROCESSOR_H_
#define _MPIPROCESSOR_H_

//$Id: mpiprocessor.h 4863 2013-12-18 23:11:35Z starviewer $

#include "mpicolor.h"
#include "mpicube.h"

struct _xyz_struct
{
    int x;
    int y;
    int z;
};

struct _decomposion_struct
{
    int row;
    int col;
    int lev;
};

struct _shift_struct
{
    float x;
    float y;
    float z;
};

typedef struct _xyz_struct pointstruct;
typedef struct _decomposion_struct decomposion_struct;
typedef struct _shift_struct shiftstruct;

class MPIProcessor
{
    public:
        MPIProcessor();
       ~MPIProcessor();

        void draw();
        void draw_with_halo_filled();
        void draw_row_exchanged();
        void draw_with_halo();
        void draw_without_halo();
        void draw_decomposed();
        void draw_global_domain();
        void decomposition(int col, int row);

        void draw_xtranspose_whole();
        void draw_xtransposed();

        void draw_ytranspose_whole();
        void draw_ytransposed();

        void set_global_size(int nx, int ny, int nz);
        void set_halo_width(int n) { halo_width = n; };
        void set_numb_procs(int n) { numb_procs = n; };
        void set_rank(int n, int nc, int nr);

    private:
        MPICube* cube;
        MPIColor* mpi_color;
        float* c4v;
        float* xc4v;
        float* yc4v;

        int numb_procs, myrank;
        int halo_width;
        int ic, jc, kc;

        float scale;
        float delta;

        decomposion_struct xPlane, yPlane, zPlane;
        decomposion_struct xRank, yRank, zRank;
        pointstruct global_size;
        pointstruct xStart, yStart, zStart;
        pointstruct xSize, ySize, zSize;
        shiftstruct shift;

        void drawAregion(pointstruct start, pointstruct region, shiftstruct shift);
        void xfactor(int n, int base);
        void yfactor(int n, int base);
};
#endif

