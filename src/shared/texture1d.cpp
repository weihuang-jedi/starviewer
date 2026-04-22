#include <QOpenGLWidget>

#include <iostream>
#include <vector>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include <texture1d.h>

Texture1d::Texture1d()
{
  //cout << "\tEnter function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
    _length = 0;
    _colors = NULL;
    _opacity = 1.0;
    _textureID = 0;
    _name = string("unknown");
  //cout << "\tLeave function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
}

Texture1d::~Texture1d()
{
  //cout << "\tEnter function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
    _free_colors();
  //cout << "\tLeave function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
}

void Texture1d::_free_colors()
{
    if(0 != _length)
    {
        delete [] _colors;
    }
}

void Texture1d::reset()
{
    if(_textureID)
    {
        if(glIsTexture(_textureID))
            glDeleteTextures(1, &_textureID);

        _textureID = 0;
    }

    if(NULL != _colors)
    {
        delete [] _colors;
    }

    _length = 0;
    _colors = NULL;
    _opacity = 1.0;
}

void Texture1d::set_colors(int clen, float *cmap)
{
    int i, n = 0;

    _length = clen - 2;
  //_length = clen;

    _free_colors();

    _colors = new float[4 * _length];

  //cout << "Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__
  //     << ", file: <" << __FILE__ << ">" << endl;
  //cout << "\t_length = " << _length << endl;

    for(n = 0; n < _length; ++n)
    {
      //cout << "\t_colors[" << n << "] = ";
        for(i = 0; i < 3; ++i)
        {
            _colors[4*n + i] = cmap[3*(n+2) + i];

          //if(i)
          //    cout << ", " << _colors[4*n + i];
          //else
          //    cout << "{" << _colors[4*n + i];
        }

        _colors[4*n + 3] = 0.75;
      //_colors[4*n + 3] = (float) n / _length;
      //cout << ", " << _colors[4*n + 3] << "}," << endl;
    }

    _setup();
}

void Texture1d::set_colors4pop(int clen, float *cmap)
{
    int i, n = 0;
    int olen = clen - 3;

    _length = 1.11112 * olen;

    _free_colors();

    _colors = new float[4 * _length];

  //cout << "Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__
  //     << ", file: <" << __FILE__ << ">" << endl;
  //cout << "\t_length = " << _length << endl;

    for(n = 0; n < olen; ++n)
    {
      //cout << "\t_colors[" << n << "] = ";
        for(i = 0; i < 3; ++i)
        {
            _colors[4*n + i] = cmap[3*(n+3) + i];
        }

        _colors[4*n + 3] = 1.0;
    }

    for(n = olen; n < _length; ++n)
    {
        _colors[4*n    ] = 0.2;
        _colors[4*n + 1] = 0.2;
        _colors[4*n + 2] = 0.2;
        _colors[4*n + 3] = 1.0;
    }

    _setup();
}

void Texture1d::set_opacity(float o)
{
    int n = 0;

    if(o == _opacity)
        return;

    _opacity = o;

    for(n = 0; n < _length; ++n)
    {
        _colors[4*n + 3] = o;
    }

    _setup();
}

void Texture1d::_setup()
{
    int i, n;

    float colorMap[_length][4];

  //cout << "Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__
  //     << ", file: <" << __FILE__ << ">" << endl;
  //cout << "\t_length = " << _length << endl;

    for(n = 0; n < _length; ++n)
    {
        for(i = 0; i < 4; ++i)
        {
            colorMap[n][i] = _colors[4*n + i];
        }
    }

    if(_textureID)
    {
        if(glIsTexture(_textureID))
            glDeleteTextures(1, &_textureID);
    }

  //glEnable(GL_TEXTURE_1D);

  //Generate a texture name
    glGenTextures(1, &_textureID);

    glBindTexture(GL_TEXTURE_1D, _textureID);

  //Set pixel storage mode 
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  //float testColors[3][4] = { {0.0, 0.0, 1.0, 1.0}, {1.0, 1.0, 1.0, 1.0}, {1.0, 0.0, 0.0, 1.0}};
  //glTexImage1D(GL_TEXTURE_1D, 0, 4, 3, 0, GL_RGBA, GL_FLOAT, testColors);

    glTexImage1D(GL_TEXTURE_1D, 0, 4, _length, 0, GL_RGBA, GL_FLOAT, colorMap);
  //glTexImage1D(GL_TEXTURE_1D, 0, 4, _length, 0, GL_RGBA, GL_FLOAT, &_colors);

    glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

  //glDisable(GL_TEXTURE_1D);
}

void Texture1d::set_gradient_opacity()
{
    int n = 0;
    float scl = 1.0 / _length;

    for(n = 0; n < _length; ++n)
    {
        _colors[4*n + 3] = scl * n;
    }

    _setup();
}

