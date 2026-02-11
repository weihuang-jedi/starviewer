#include "texture3d.h"

static float SideTextures[7][4] = 
{
    {1.0, 1.0, 1.0, 0.0},    //White, invisible
    {1.0, 1.0, 1.0, 0.2},    //White
    {0.0, 1.0, 1.0, 0.3},    //Cyan
    {0.0, 0.0, 1.0, 0.4},    //Blue
    {1.0, 1.0, 0.0, 0.5},    //Yellow
    {1.0, 0.0, 0.0, 0.6},    //Red
    {1.0, 0.0, 1.0, 0.7}     //Magenta
};

Texture3d::Texture3d()
{
    _side_texture();
}

Texture3d::~Texture3d()
{
}

void Texture3d::_side_texture()
{
    int s, t, r, n, m, idx;

    float *textures;

    _texturesize = sizeof(SideTextures) / sizeof(SideTextures[0]);

    textures = (float *)malloc(4 * _texturesize * _texturesize * _texturesize * sizeof(float));
    assert(textures);

    n = 0;
    for(r = 0; r < _texturesize; r++)
    {
        for(t = 0; t < _texturesize; t++)
        {
            for(s = 0; s < _texturesize; s++)
            {
                idx = (int) sqrtf((float)(s*s + t*t + r*r));
                if(idx >= (_texturesize - 1))
                   idx = _texturesize - 1;

                for(m = 0; m < 4; m++)
                {
                    textures[n] = SideTextures[idx][m];
                    ++n;
                }
            }
        }
    }

    glEnable(GL_TEXTURE_3D);        // enable 3d texturing

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);

  //our texture colors will replace the untextured colors
  //glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

  //request 1 texture name from OpenGL
    glGenTextures(1, &_textureID);

  //tell OpenGL we're going to be setting up the texture name it gave us
    glBindTexture(GL_TEXTURE_3D, _textureID);

  //when this texture needs to be shrunk to fit on small polygons,
  //use linear interpolation of the texels to determine the color
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  //when this texture needs to be magnified to fit on a big polygon,
  //use linear interpolation of the texels to determine the color
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  //we want the texture to repeat over the S axis,
  //so if we specify coordinates out of range we still get textured.
  //glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);

  //same as above for T axis
  //glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  //same as above for R axis
  //glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);

  //this is a 3d texture, level 0 (max detail),
  //GL should store it in RGB8 format, its _width*_height*_depth in size,
  //it doesnt have a border, we're giving it to GL in RGB format
  //as a series of floas, and texels is where the texel data is.
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, _texturesize, _texturesize, _texturesize, 0,
                 GL_RGBA, GL_FLOAT, textures);

  //glBindTexture(GL_TEXTURE_3D, _textureID);

    glDisable(GL_TEXTURE_1D);

    _sideID = _textureID;

    free(textures);
}

float **Texture3d::get_colvec()
{
    int n;
    float **cv;

    cv = (float **) malloc(_texturesize * sizeof(float *));
    for(n = 0; n < _texturesize; n++)
    {
        cv[n] = (float *) malloc(4 * sizeof(float));
    }

    for(n = 0; n < _texturesize; n++)
    {
        memcpy(cv[n], SideTextures[n], 4 * sizeof(float));
    }

    return cv;
}

