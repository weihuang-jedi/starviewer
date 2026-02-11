#ifndef TEXTURE3D_H
#define TEXTURE3D_H

#include <QtOpenGL>

#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <math.h>

using namespace std;

class Texture3d
{
    public:
        Texture3d();
       ~Texture3d();

      //void b2r() { _textureID = _centID; };
        void w2r() { _textureID = _sideID; };
      //void rain() { _textureID = _rainID; };

        int     get_numlvl() { return _texturesize; };
        float **get_colvec();

        unsigned int get_textureID() { return _textureID; };

    private:
        unsigned int _textureID; // Texture ID
      //unsigned int _centID;    // centralized Texture ID
        unsigned int _sideID;    // sided Texture ID
      //unsigned int _rainID;    // rain Texture ID

        int _texturesize;

        void _side_texture();
};
#endif

