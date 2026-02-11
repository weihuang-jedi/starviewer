#ifndef _Ball_H
#define _Ball_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include <string>

#ifdef LINUX
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>
#endif

#include "util.h"

using namespace std;

class Ball
{
    public:
        Ball();	       	//  Constructor
       ~Ball();		//  Destructor

        void set_radius(float r) { radius = r; };

        void draw();
        void draw(float rad);

    private:
        float radius;

        void _ballVertex(float lon, float lat, float rad);
};
#endif

