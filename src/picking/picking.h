#ifndef _Picking_H

#include <stdio.h>
#include <stdlib.h>

#include <QtOpenGL>

#include <iostream>

#define MaxHits	10

using namespace std;

class Picking
{
    public:
        Picking();
       ~Picking();

        void set_screensize(int w, int h);
        void initialize(int x, int y);

        void loadName(int n);
        void pushName(int n);
        void popName() { glPopName(); };

        void set_mode(GLenum m) { mode = m; };
        void processHits();
        void printHits();

        GLenum get_mode() { return mode; };

        GLint get_nhits() { return nhits; };
        GLuint** get_hitlist() { return hitlist; };

    private:
        int screenWidth;
        int screenHeight;
        int selected;

        GLenum mode;
        GLint viewport[4];
        GLint nhits;
        GLuint* buffer;
        GLuint** hitlist;

        size_t presize;
        size_t maxsize;
        size_t cursize;

        void _gen_hitlist();
};
#endif

