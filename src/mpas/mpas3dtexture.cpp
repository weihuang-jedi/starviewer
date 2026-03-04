#include <QtOpenGL>

#include "mpas3dtexture.h"

//The vertice cylinder
//
//            5 +-----------------+ 4
//              |\               /|
//              | \             / |
//              |  \           /  |
//              |   \         /   |
//              |    \       /    |
//            2 +-----\-----/-----+ 1
//               \     \   /     /
//                \     \ /     /
//                 \   3 +     /
//                  \    |    /
//                   \   |   /
//                    \  |  /
//                     \ | /
//                      \|/
//                    0  +
//

//static const int triangleIndex[12][3] =
//{
//   {0, 1, 2}, {1, 2, 3}, {2, 3, 0}, {3, 0, 1},
//   {1, 2, 3}, {2, 3, 4}, {3, 4, 1}, {4, 1, 2},
//   {2, 3, 4}, {3, 4, 5}, {4, 5, 2}, {5, 2, 3}
//};

static const int triangleIndex[12][3] =
{
   {0, 1, 2}, {1, 2, 3}, {2, 3, 0}, {3, 0, 1},
   {1, 2, 3}, {2, 3, 4}, {3, 4, 1}, {4, 1, 2},
   {2, 3, 4}, {3, 4, 5}, {4, 5, 2}, {5, 2, 3}
};

MPAS3dTexture::MPAS3dTexture()
{
    _display_layers = 0;
    _texture1d = new Texture1d();
}

MPAS3dTexture::~MPAS3dTexture()
{
    free(_display_list);

    delete _texture1d;
}

void MPAS3dTexture::set_geompas(GeoMPAS *geompas)
{
    _geompas = geompas;

    _display_layers = 5;

    _display_list = (int *) calloc(_geompas->get_nt(), sizeof(int));
    assert(_display_list);

    reset();
}

void MPAS3dTexture::reset()
{
    int n;
    for(n = 0; n < _geompas->get_nt(); n++)
    {
        if(_display_list[n])
           glDeleteLists(_display_list[n], 1);
        _display_list[n] = 0;
    }
}

void MPAS3dTexture::_compile_display_list(int tl)
{
    int i, v;

    int nz             = _geompas->get_nz();
    int nVertices      = _geompas->get_nVertices();
    int vertexDegree   = _geompas->get_vertexDegree();
    int *cellsOnVertex = _geompas->get_cellsOnVertex();

    int vertice[3];

    _alfa = 0.05;

    _leveler->reset_nl(_display_layers);

    _desired_level = _leveler->get_levels();

    _beta = 0.90 / (_desired_level[_display_layers - 1] - _desired_level[0]);

    _display_list[tl] = glGenLists(1);
    glNewList(_display_list[tl], GL_COMPILE);

    glPushMatrix();

    glColor3f(1,1,1);

  //OpenGL should normalize normal vectors
  //glEnable(GL_NORMALIZE);

    glEnable(GL_TEXTURE_1D);
    glBindTexture(GL_TEXTURE_1D, _texture1d->get_textureID());

  //glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  //glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);

    for(v = 0; v < nVertices; ++v)
    {
        for(i = 0; i < vertexDegree; i++)
        {
            vertice[i] = cellsOnVertex[i+v*vertexDegree];
        }

      //cout << "\nv = " << v << endl;
        _draw(vertice, nz, vertexDegree);
    }

    glDisable(GL_TEXTURE_1D);

    glPopMatrix();

    glEndList();
}

void MPAS3dTexture::display(int tl)
{
    if(0 == _display_list[tl])
        _compile_display_list(tl);

    glCallList(_display_list[tl]);
}

void MPAS3dTexture::_draw(int vertice[], int nz, int vertexDegree)
{
    int   iVertex, iTriangle, idx;
    float val[6];

    int k, m, n;

    double zlevel;
  //double factor = 0.125 / nz;
    double factor = 0.25 / nz;

    for(k = 0; k < nz - 1; k += 10)
    {
       if(k)
       {
           for(n = 0; n < vertexDegree; ++n)
           {
               val[n] = val[n + 3];
               pos[n] = pos[n+3];
           }
       }
       else
       {
           for(n = 0; n < vertexDegree; ++n)
           {
               m = nz * vertice[n];
               val[n] = _alfa + _beta * (_var[m] - _desired_level[0]);
               pos[n] = _geompas->polar2xyz(vertice[n], 1.0);
           }
       }

       for(n = 0; n < vertexDegree; ++n)
       {
           m = nz * vertice[n] + k + 1;
           val[n + 3] = _alfa + _beta * (_var[m] - _desired_level[0]);

           zlevel = 1.0 + factor * (k + 1);
           pos[n+3] = _geompas->polar2xyz(vertice[n], zlevel);
       }

       glBegin(GL_TRIANGLES);
       for(iTriangle = 0; iTriangle < 12; iTriangle += 12)
       {
           for(iVertex = 0; iVertex < 3; iVertex++)
           {
               idx = triangleIndex[iTriangle][iVertex];
               glTexCoord1d(val[idx]);
             //glTexCoord3d(val[idx], val[idx], val[idx]);
               glVertex3d(pos[idx].fX, pos[idx].fY, pos[idx].fZ);

             //cout << "\tval = " << val << ", iTriangle = " << iTriangle
             //     << ", iVertex = " << iVertex
             //     << ", idx = " << idx << endl;
             //cout << "\tpos[idx].fX = " << pos[idx].fX
             //     << ", pos[idx].fY = " << pos[idx].fY
             //     << ", pos[idx].fZ = " << pos[idx].fZ << endl;
           }
       }
       glEnd();
    }
}

