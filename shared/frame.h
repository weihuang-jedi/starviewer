#ifndef FRAME_H
#define FRAME_H
#include <QOpenGLWidget>

using namespace std;

class Frame
{
    public:
        Frame();
       ~Frame();

        int get_nx() { return (_nx); };
        int get_ny() { return (_ny); };
        int get_nz() { return (_nz); };

        void draw();

    protected:
        int _nx;
        int _ny;
        int _nz;
        double _sx = 1.0;
        double _sy = 1.0;
        double _sz = 1.0;
        double _tx = 1.0;
        double _ty = 1.0;
        double _tz = 1.0;
        double _phmax = 50000.0;
};
#endif

