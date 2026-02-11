#ifndef _Radx3DVIEWER_H
#define _Radx3DVIEWER_H

//$Id: radx3dviewer.h 4939 2014-01-27 22:28:57Z starviewer $

#include "marchTetrahedron.h"
#include "radx2dviewer.h"

class Radx3dViewer : public Radx2dViewer
{
    public:
        Radx3dViewer(ColorTable* ct, NVOptions* opt);
       ~Radx3dViewer();

        void draw_isosurface();

    protected:
        MarchTetrahedron *mTetrahedron;

        RadxSweep* low_sweep;
        RadxSweep* up_sweep;

        RadxRay** working_ray;
        RadxRay* low_ray;
        RadxRay* up_ray;

        int current_bin_index;

        int num_up_rays;
        int num_low_rays;

        int i_up_ray;
        int i_low_ray;

        int ibin[3];
        int nbins[3];

        int processed_up_rays;
        int processed_low_rays;

        int* up_sweep_ray_ids;
        int* low_sweep_ray_ids;

        float* up_sweep_ray_azimuth;
        float* low_sweep_ray_azimuth;

        double range;
        double scale;
        double up_azimuth;
        double low_azimuth;

        double azimuth[4];
        double elev[4];

        double binpos[3];
        double gate_size[3];

        double x[4];
        double y[4];
        double z[4];
        double r[4];
        float  v[4];

        int nisolevel;
        float** colormap;
        float* isovalue;

        pt3d point[4];

        bool has_more_point;

        void _advance_low_ray();
        void _advance_up_ray();
        void _set_sweep_ray_ids();
        void _replace_lower_sweep();
        void _set_ray_param(int n);
        void _set_new_point();
        void _advance_bin();
        void _isosurface();
};
#endif

