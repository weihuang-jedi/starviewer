#ifndef _Light_H
#define _Light_H

#include "ball.h"


#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include <string>
#include <iostream>

#ifdef LINUX
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>
#endif

#include <QtOpenGL/QGLWidget>
#include <QtCore/qstring.h>
#include <QtGui/QKeyEvent>
#include <QtCore/QTimer>

using namespace std;

class Light
{
    public:
        Light();
       ~Light();

        void set_ambient(float r, float g, float b, float a);
        void set_diffuse(float r, float g, float b, float a);
        void set_emission(float r, float g, float b, float a);
        void set_specular(float r, float g, float b, float a);
        void set_shinyness(float s);

        void set_longitude(float lon);
        void set_latitude(float lat);
        void set_altitude(float alt);

        void turnOn() { lightOn = true; }
        void turnOff() { lightOn = false; }
        void set_drawBall(bool b) { drawBall = b; }

        void draw();

        void set_ballsize(float s);

        float* get_ambient() { return ambient; }
        float* get_diffuse() { return diffuse; }
        float* get_emission() { return emission; }
        float* get_specular() { return specular; }

        float* get_position() { return position; }

        float get_shinyness() { return shinyness[0]; }

        float get_longitude() { return longitude; }
        float get_latitude() { return latitude; }
        float get_altitude() { return altitude; }

        bool on() { return lightOn; }
        bool get_drawBall() { return drawBall; }

    private:
        Ball *ball;

        float longitude; // Light longitude
        float latitude;  // Light latitude
        float altitude;  // Light latitude
 
      //Light position and colors
        float ambient[4];
        float diffuse[4];
        float emission[4];
        float specular[4];

        float position[4];

        float shinyness[1];

        float size;

        bool lightOn;
        bool drawBall;

        void set_position();
};
#endif

