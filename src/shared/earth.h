#ifndef EARTH_H
#define EARTH_H

#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>

#include <QOpenGLFunctions>
#include <QtOpenGL/QGLWidget>
#include <QString>
#include <QImage>
#include <QDebug>

#include <netcdf>
#include <cmath>
#include <vector>

#include "ncreader.h"

using namespace std;

class Earth : public QGLWidget, protected QOpenGLFunctions
{
    public:
        Earth();         //  Constructor
        Earth(const char *flnm);         //  Constructor
        Earth(const char *flnm, ncReader* nchandler);         //  Constructor
        ~Earth();        //  Destructor

        void draw();
        void draw(float r);
        void bump();
        void bump(float r);
        void draw_plane(float z);
        void bump_plane(float z);

        void initializeGL();
        void set_radius(double r) { radius = r; };
        void set_texture_id(GLuint id) {_texture_id = id;};
        GLuint get_texture_id() {return _texture_id;};

    private:
        char _bmpflnm[1024];
        char _topoflnm[1024];
        GLuint _texture_id;

        double deg2arc;
        double radius;

        int nlon, nlat;
        vector<float> lon;
        vector<float> lat;
        vector<float> ter;
        float  maxhgt;
        float  minhgt;
        void read_terrain();

        void _loadTexBMP();
        void _Vertex(int th, int ph);
        void _bumpVertex(float th, float ph, double radius);
        void _fatal(const char* format , ...);
        void _errCheck(const char *where, const char *fl, int ln);
};
#endif

