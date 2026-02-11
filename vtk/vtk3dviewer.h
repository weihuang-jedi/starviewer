#ifndef _VTK3DVIEWER_H_
#define _VTK3DVIEWER_H_

//$Id: vtk3dviewer.h 4835 2013-11-08 15:52:57Z starviewer $

#include "marchTetrahedron.h"
#include "vtk2dviewer.h"

class VTK3dViewer : public VTK2dViewer
{
    public:
        VTK3dViewer(ColorTable* ct, NVOptions* opt);
       ~VTK3dViewer();

        void draw_isosurface();

    protected:
        MarchTetrahedron *mTetrahedron;

        float** colormap;
        float isovalue[5];

        pt3d point[4];

        void _isosurface();
};
#endif

