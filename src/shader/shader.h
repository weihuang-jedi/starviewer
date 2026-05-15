#ifndef _Shader_H
#define _Shader_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include <string>

#ifndef PI
#define PI   3.1415926535897932
#endif
#ifndef ARC
#define ARC  PI/180.0
#endif

#define Cos(th) cos(ARC*(th))
#define Sin(th) sin(ARC*(th))

using namespace std;

class Shader
{
    public:
        Shader();
       ~Shader();

    private:
        int id[2];

        float _x, _y, _z, _w;
        float _r, _g, _b, _a;

      //These routines are copied from CSCIX239. Spring, 2013 CU Boulder.
      //void printShaderLog(int obj,const char *srcFile);
      //void fatal(const char* format , ...);
      //void readText(const char *textFile);
        void errCheck(const char *where);
        int  createShaderProg(const char *VertFile,const char *FragFile);
        void createShader(int prog,const GLenum type,const char *srcFile);
        void printProgramLog(int obj);

        void draw();
        void _draw_ball(float rad);
        void _ballVertex(float lon, float lat, float rad);
};
#endif

