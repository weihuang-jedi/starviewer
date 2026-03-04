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
};
#endif

