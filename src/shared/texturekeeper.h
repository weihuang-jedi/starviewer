#ifndef _TextureKeeper_H
#define _TextureKeeper_H

//$Id:

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

using namespace std;

class TextureKeeper
{
    public:
        TextureKeeper();
       ~TextureKeeper();

        void set_xid(int n, unsigned int id);
        void set_yid(int n, unsigned int id);
        void set_zid(int n, unsigned int id);

        unsigned int get_xid(int n);
        unsigned int get_yid(int n);
        unsigned int get_zid(int n);

        void reset();
        void setup(int nx, int ny, int nz);
        void reinitialize(int nx, int ny, int nz);

    private:
        unsigned int* _xid;
        unsigned int* _yid;
        unsigned int* _zid;

        int _nx;
        int _ny;
        int _nz;
};
#endif

