//$Id: vtk3dviewer.cpp 4865 2014-01-01 15:49:23Z starviewer $

#include <QtOpenGL>

#include "vtk3dviewer.h"

VTK3dViewer::VTK3dViewer(ColorTable *ct, NVOptions* opt)
             : VTK2dViewer(ct, opt)
{
    int n;

    mTetrahedron = new MarchTetrahedron();
    
    colormap = (float**) calloc(5, sizeof(float*));
    assert(colormap);
    for(n = 0; n < 5; ++n)
    {
        colormap[n] = (float*) calloc(4, sizeof(float));
        assert(colormap[n]);
    }
 
    colormap[0][0] = 0.0;
    colormap[0][1] = 0.0;
    colormap[0][2] = 1.0;
    colormap[0][3] = 0.1;
    
    colormap[1][0] = 0.0;
    colormap[1][1] = 1.0;
    colormap[1][2] = 0.0;
    colormap[1][3] = 0.2;
    
    colormap[2][0] = 1.0;
    colormap[2][1] = 1.0;
    colormap[2][2] = 0.0;
    colormap[2][3] = 0.3;
    
    colormap[3][0] = 1.0;
    colormap[3][1] = 0.0;
    colormap[3][2] = 0.0;
    colormap[3][3] = 0.5;
    
    colormap[4][0] = 1.0;
    colormap[4][1] = 0.0;
    colormap[4][2] = 1.0;
    colormap[4][3] = 0.7;

    mTetrahedron->set_colormap(colormap, 5);
}

VTK3dViewer::~VTK3dViewer()
{
    int n;
    
    for(n = 0; n < 5; ++n)
        free(colormap[n]);

    free(colormap);
}

void VTK3dViewer::draw_isosurface()
{
  //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\t_minval =" << _minval << ", _maxval = " << _maxval << endl;

    isovalue[0] = 0.05 * (_maxval - _minval) + _minval;
    isovalue[1] = 0.10 * (_maxval - _minval) + _minval;
    isovalue[2] = 0.20 * (_maxval - _minval) + _minval;
    isovalue[3] = 0.40 * (_maxval - _minval) + _minval;
    isovalue[4] = 0.80 * (_maxval - _minval) + _minval;

    mTetrahedron->set_isolevel(isovalue, 5);

    _isosurface();
}

void VTK3dViewer::_isosurface()
{
    int lvl = nvoptions->get_zsec();
    GLuint zcl = lister->get_zid(lvl);;

    if(zcl)
    {
        glCallList(zcl);
        return;
    }

    zcl = glGenLists(1);

    cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
    cout << "\tzcl = " << zcl << endl;

    glNewList(zcl, GL_COMPILE_AND_EXECUTE);

  //OpenGL should normalize normal vectors
    glEnable(GL_NORMALIZE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

    glShadeModel(GL_SMOOTH);

  //glDisable(GL_CULL_FACE);
  //glEnable(GL_CULL_FACE);

    glPushMatrix();


  //glDisable(GL_CULL_FACE);

  //glNormal3f(0.0, 0.0, -1.0);

    glEndList();

    lister->set_zid(lvl, zcl);

    glPopMatrix();
}

