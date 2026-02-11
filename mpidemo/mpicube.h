#ifndef _MPICUBE_H_
#define _MPICUBE_H_

//$Id: mpicube.h 4859 2013-12-17 23:51:52Z starviewer $

#include <iostream>
#include <vector>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "nvoptions.h"

using namespace std;

class MPICube
{
    public:
        MPICube();
       ~MPICube();

        void reset();
        void set_origin(float x, float y, float z);
        void set_length(float d);
        void set_alpha(float a);
        void set_cubecolor(float c[4]);
        void set_linecolor(float c[4]);
        void draw();

    private:
        void drawcube();
        void drawline();
        float _d;
        float _x1, _y1, _z1;
        float _x2, _y2, _z2;
        float _cubecolor[4];
        float _linecolor[4];
};
#endif
 
