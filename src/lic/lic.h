#ifndef _LineIntegralConvolution_H
#define _LineIntegralConvolution_H

#include <QtOpenGL>

#include "colorTable.h"
#include "nvoptions.h"
#include "lister.h"
#include "licvector.h"
#include "point.h"
#include "noise.h"
#include "streamline.h"
#include "reggrid.h"

using namespace std;

#define minNumHits 12

class LineIntegralConvolution
{
    public:
        LineIntegralConvolution(ColorTable* ct, NVOptions* opt);
       ~LineIntegralConvolution();

        void setup(int ix, int iy, int iz,
                   float* u, float* v, float*w);

        void draw();

        void set_colorTable(ColorTable *ct) { colorTable = ct; };

        void set_stepsize(int n) { _stepsize = n; };
        void set_maxvalue(float s) { _maxvalue = s; };

    private:
        ColorTable* colorTable;
        NVOptions* nvoptions;
        Lister* lister;
        RegGrid* grid;
        Noise* noise;

        int xcl;
        int ycl;
        int zcl;
        
        double maxvecmag;

        float* _u;
        float* _v;
        float* _w;

        void _display_Xplane();
        void _display_Yplane();
        void _display_Zplane();

        int _nx, _ny, _nz;
        int _stepsize;

        int    _colorlen;
        float* _colormap;
        float _wings;
        float _maxvalue;
        float _xStart, _yStart, _zStart;
        float _xyDelt, _zDelt;

        void _parameter_setup();

        void _set_color(Point p);
        void _set_color(double spd);

        void _point2xyz(Point p, double &x, double &y, double &z);

        void _draw_line(Point p1, Point p2);
        void _draw_line(Point* p, int n);

        void _generate_lic();
        void _setup_zPlane();

        void scaleNtranslate();

        bool first_time;
};
#endif

