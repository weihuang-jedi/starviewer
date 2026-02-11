#ifndef _Lister_H
#define _Lister_H

//$Id:

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

using namespace std;

class Lister
{
    public:
        Lister();
       ~Lister();

        void set_xid(int n, GLuint id);
        void set_yid(int n, GLuint id);
        void set_zid(int n, GLuint id);

        GLuint get_xid(int n);
        GLuint get_yid(int n);
        GLuint get_zid(int n);

        void reset();
        void setup(int nx, int ny, int nz);
        void reinitialize(int nx, int ny, int nz);

    private:
        GLuint* _xid;
        GLuint* _yid;
        GLuint* _zid;

        int _nx;
        int _ny;
        int _nz;
};
#endif

