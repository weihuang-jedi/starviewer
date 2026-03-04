//$Id: pixel_viewer.h 4833 2013-11-05 15:49:31Z starviewer $

#ifndef _Pixel_Viewer_H
#define _Pixel_Viewer_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include <string>

#include <QString>

#include "colorTable.h"
#include "evaluator.h"
#include "nvoptions.h"
#include "geometry.h"

#include <QOpenGLFunctions>
#include <QtOpenGL/QGLWidget> // or QOpenGLWidget

using namespace std;

typedef enum
{
    NV_FRAG_NONE = 0,
    NV_FRAG_COPY,
    NV_FRAG_SMOOTH,
    NV_FRAG_SHARPEN,
    NV_FRAG_BLUR,
    NV_FRAG_EROSION,
    NV_FRAG_DILATION,
    NV_FRAG_LAPLACIAN,
    NV_FRAG_PREWITT,
    NV_FRAG_SOBEL,
    NV_FRAG_BLACKWHITE,
    NV_FRAG_NUMBERS,
} NV_FRAG_Type;

class Pixel_Viewer : public QGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
    public:
        Pixel_Viewer(NVOptions* opt);       	// Constructor
       ~Pixel_Viewer();		// Destructor

        void draw();

        void reset();
        void setup(string vn, double* var);
        void set_geometry(Geometry* gm) { geometry = gm; };
        void set_colorTable(ColorTable* ct);

        double get_minval() { return vMinimum; };
        double get_maxval() { return vMaximum; };

        void set_fillValue(double v) { fillValue = v; };
        void set_hasFillValue(bool v) { hasFillValue = v; };

    protected:
        void initializeGL() override; // Must be overridden to initialize functions

    private:
        ColorTable* colorTable;
        Evaluator* evaluator;
        NVOptions* nvoptions;
        Geometry* geometry;

        unsigned int img;
        unsigned int textureID;

        float* colorMap;
        int numberofcolors;

        double* _var;
        double* pltvar;

        double* lon;
        double* lat;
        double* lev;

        double fillValue;
        bool hasFillValue;

        int nx, ny, nz;
        int numberShaderPasses;
        double vMinimum, vMaximum, scale, zScale;
        string name;

        int shader[NV_FRAG_NUMBERS];
        int shaderID;

        float _screenWidth, _screenHeight;

        float dX, dY;

      //These routines are copied from CSCIX239. Spring, 2013 CU Boulder.
        void errCheck(const char *where);
        int  createShaderProg(const char *VertFile,const char *FragFile);
        void createShader(int prog,const GLenum type,const char *srcFile);
        void printProgramLog(int obj);

        void _initialize();
        void _parameter_setup();
        void _createTexture(int zs);
        void _setColor(double f, unsigned char* color4f);
        void _setup_shader(NV_FRAG_Type type, const char* name);
};
#endif

