#ifndef _Topography_H
#define _Topography_H

//$Id: topography.h 4875 2014-01-05 19:48:41Z starviewer $

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <string.h>

#ifdef LINUX
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>
#endif

#include <QtOpenGL/QGLWidget>
#include <QString>

#include <iostream>
#include <string>

#include "nvoptions.h"

using namespace std;

class Topography
{
    public:
        Topography();	//  Constructor
        ~Topography();	//  Destructor

        void draw();
        void draw(double r);
        void drawAregion(double z);
        void draw_plane(double z);
        void draw_plane_grid(double z);

        void set_radius(double r) { _radius = r; };
        void set_nvoptions(NVOptions* opt) { nvoptions = opt; };

        int get_level() { return _level; };

        double* get_area() { return _area; };

        GLuint get_tid(int n) { return _tid[n]; };

    private:
        NVOptions* nvoptions;

        string _filename;
        GLuint _tid[4];
        GLuint _clist[4];

        double _dlon, _dlat;
        double _cntlon;
        double _minlon, _maxlon;
        double _minlat, _maxlat;
        double _xwest, _xeast;
        double _ysouth, _ynorth;
        double _xcenter, _ycenter;
        double _area[4];

        int _number_tiles;
        int _level;
        int _nlon, _nlat;
        int _width, _height;
        int _image_width, _image_height;

        double _deg2arc;
        double _radius;
        double _oneover;

        void _loadImage();
        void _loadAregion();
        void _checkImage(QImage t);

        void _Vertex(int th, int ph);
        void _lonlat2xy(double lon, double lat, double &x, double &y);

        void _setupRegionInfo();
        void _generateTexture(double slon, double elon, double slat, double elat,
                                  int n, unsigned char* bits);
        void _loadAregion(double slon, double elon, double slat, double elat,
                          int ntile);
        void _drawAregion(double slon, double elon, double slat, double elat,
                          double z, int ntile);
};
#endif

