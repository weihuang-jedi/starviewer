#ifndef _Texture1D_H
#define _Texture1D_H

#include <QtOpenGL>

#ifdef LINUX
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>
#endif

#include <iostream>

#include <assert.h>

using namespace std;

class Texture1d
{
    public:
        Texture1d();
       ~Texture1d();

        void set_colors(int clen, float *cmap);
        void set_name(string name) { _name = name; };
        void set_opacity(float o);
        void set_gradient_opacity();

        string get_name() { return _name; };

        unsigned int get_textureID() { return _textureID; };

        void set_colors4pop(int clen, float *cmap);

        void reset();

    private:
        unsigned int _textureID; // Texture ID

        int   _length;
        float _opacity;
        float *_colors;

        string _name;

        void _setup();
        void _free_colors();
};
#endif

