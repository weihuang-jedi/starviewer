#ifndef _Selecting_H
#define _Selecting_H

#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>

#include <QtOpenGL>

using namespace std;

#define BUFSIZE 512

class Selecting
{
    public:
        Selecting();
       ~Selecting();

        void set_mode(GLenum m) { mode = m; };
        GLenum get_mode() { return mode; };
        GLint get_nhits() { return nhits; };

        void processHits();
        void start(int button, int state, GLuint x, GLuint y);

    private:
        GLenum mode;
        GLuint nhits;

        GLuint* buffer;

        GLuint presize;
        GLuint bufsize;

        GLuint width, height;
        GLuint viewport[4];
};
#endif

