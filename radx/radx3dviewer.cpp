//$Id: radx3dviewer.cpp 5321 2015-02-20 03:20:52Z starviewer $

#include <QtOpenGL>

#include "radx3dviewer.h"

#define MAKEMARK        { cerr << endl << "file: " << __FILE__ << "\tline: " << __LINE__ << endl << endl; }

Radx3dViewer::Radx3dViewer(ColorTable *ct, NVOptions* opt)
             : Radx2dViewer(ct, opt)
{
    int n;

    up_sweep_ray_ids = NULL;
    low_sweep_ray_ids = NULL;

    up_sweep_ray_azimuth = NULL;
    low_sweep_ray_azimuth = NULL;

    mTetrahedron = new MarchTetrahedron();
    
    nisolevel = 8;
    isovalue = (float*) calloc(nisolevel, sizeof(float));
    assert(isovalue);
    colormap = (float**) calloc(nisolevel, sizeof(float*));
    assert(colormap);
    for(n = 0; n < nisolevel; ++n)
    {
        colormap[n] = (float*) calloc(4, sizeof(float));
        assert(colormap[n]);
    }
 
    isovalue[0] = -5.0;
    isovalue[1] = -2.5;
    isovalue[2] = -1.25;
    isovalue[3] =  2.5;
    isovalue[4] =  5.0;
    isovalue[5] = 10.0;
    isovalue[6] = 15.0;
    isovalue[7] = 20.0;

    colormap[0][0] = 0.0;
    colormap[0][1] = 0.0;
    colormap[0][2] = 1.0;
    colormap[0][3] = 0.4;
    
    colormap[1][0] = 0.0;
    colormap[1][1] = 1.0;
    colormap[1][2] = 1.0;
    colormap[1][3] = 0.2;
    
    colormap[2][0] = 0.0;
    colormap[2][1] = 1.0;
    colormap[2][2] = 0.0;
    colormap[2][3] = 0.1;
    
    colormap[3][0] = 1.0;
    colormap[3][1] = 1.0;
    colormap[3][2] = 0.0;
    colormap[3][3] = 0.1;
    
    colormap[4][0] = 1.0;
    colormap[4][1] = 0.5;
    colormap[4][2] = 0.0;
    colormap[4][3] = 0.2;
    
    colormap[5][0] = 1.0;
    colormap[5][1] = 0.0;
    colormap[5][2] = 0.0;
    colormap[5][3] = 0.4;
    
    colormap[6][0] = 1.0;
    colormap[6][1] = 0.0;
    colormap[6][2] = 0.5;
    colormap[6][3] = 0.6;
    
    colormap[7][0] = 1.0;
    colormap[7][1] = 0.0;
    colormap[7][2] = 1.0;
    colormap[7][3] = 0.8;

    mTetrahedron->set_colormap(colormap, 5);
}

Radx3dViewer::~Radx3dViewer()
{
    int n;
    
    for(n = 0; n < 5; ++n)
        free(colormap[n]);

    free(colormap);

  //delete working_ray;
    delete mTetrahedron;
}

void Radx3dViewer::draw_isosurface()
{
  //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\t_minval =" << _minval << ", _maxval = " << _maxval << endl;

  //isovalue[0] = 0.05 * (_maxval - _minval) + _minval;
  //isovalue[1] = 0.10 * (_maxval - _minval) + _minval;
  //isovalue[2] = 0.20 * (_maxval - _minval) + _minval;
  //isovalue[3] = 0.40 * (_maxval - _minval) + _minval;
  //isovalue[4] = 0.80 * (_maxval - _minval) + _minval;

    mTetrahedron->set_isolevel(isovalue, nisolevel);

    _isosurface();
}

void Radx3dViewer::_advance_low_ray()
{
    MAKEMARK
#if 0
    low_ray = low_sweep->ray[i_low_ray];
    low_azimuth = 90.0 - low_ray->h.azimuth;
    if(0.0 > low_azimuth)
        low_azimuth += 360.0;
#endif
}

void Radx3dViewer::_advance_up_ray()
{
    MAKEMARK
#if 0
    up_ray = up_sweep->ray[i_up_ray];
    up_azimuth = 90.0 - up_ray->h.azimuth;
    if(0.0 > up_azimuth)
        up_azimuth += 360.0;
#endif
}

void Radx3dViewer::_replace_lower_sweep()
{
    MAKEMARK
#if 0
    if(NULL != low_sweep_ray_ids)
        free(low_sweep_ray_ids);
    if(NULL != low_sweep_ray_azimuth)
        free(low_sweep_ray_azimuth);

    low_sweep = up_sweep;
    low_sweep_ray_ids = up_sweep_ray_ids;
    low_sweep_ray_azimuth = up_sweep_ray_azimuth;
    low_sweep = up_sweep;

    num_low_rays = num_up_rays;
    processed_low_rays = 0;
    i_low_ray = 0;
#endif
}

void Radx3dViewer::_set_sweep_ray_ids()
{
    MAKEMARK
#if 0
    int n;
    double pre_up_azimuth = -999.9;

    up_sweep = volume->sweep[isweep];

    num_up_rays = up_sweep->h.nrays;

  //cout << "\t\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\t\tnum_up_rays =" << num_up_rays << endl;

    up_sweep_ray_ids = (int*)calloc(num_up_rays + 5, sizeof(int));
    assert(up_sweep_ray_ids);
    up_sweep_ray_azimuth = (float*)calloc(num_up_rays + 5, sizeof(float));
    assert(up_sweep_ray_azimuth);
    processed_up_rays = 0;

    i_up_ray = num_up_rays - 1;
    _advance_up_ray();
    pre_up_azimuth = up_azimuth;
    i_up_ray = 0;
    _advance_up_ray();

    while(processed_up_rays < num_up_rays)
    {
      //cout << "\t\t\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
      //cout << "\t\t\tisweep =" << isweep << ", i_up_ray =" << i_up_ray << endl;
      //cout << "\t\t\tup_azimuth = " << up_azimuth << ", pre_up_azimuth = " << pre_up_azimuth << endl;

        if((pre_up_azimuth > 300.0) && (up_azimuth < 100.0))
        {

            for(n = 0; n < num_up_rays; ++n)
            {
                up_sweep_ray_ids[n] = i_up_ray;
                up_sweep_ray_azimuth[n] = up_azimuth;
              //cout << "\t\t\tup_sweep_ray_azimuth[" << n << "] = " << up_sweep_ray_azimuth[n];
              //cout << "\t\t\tup_sweep_ray_ids[" << n << "] = " << up_sweep_ray_ids[n] << endl;
                ++i_up_ray;
                if(i_up_ray >= num_up_rays)
                    i_up_ray = 0;
                _advance_up_ray();
            }

            for(n = 0; n < 5; ++n)
            {
                up_sweep_ray_ids[num_up_rays+n] = up_sweep_ray_ids[n];
                up_sweep_ray_azimuth[num_up_rays+n] = up_sweep_ray_azimuth[n] + 360.0;
            }

            break;
        }

        if((pre_up_azimuth < 100.0) && (up_azimuth > 300.0))
        {
            --i_up_ray;
            if(i_up_ray < 0)
                i_up_ray = num_up_rays - 1;
            _advance_up_ray();

            for(n = 0; n < num_up_rays; ++n)
            {
                up_sweep_ray_ids[n] = i_up_ray;
                up_sweep_ray_azimuth[n] = up_azimuth;
              //cout << "\t\t\tup_sweep_ray_azimuth[" << n << "] = " << up_sweep_ray_azimuth[n];
              //cout << "\t\t\tup_sweep_ray_ids[" << n << "] = " << up_sweep_ray_ids[n] << endl;
                --i_up_ray;
                if(i_up_ray < 0)
                    i_up_ray = num_up_rays - 1;
                _advance_up_ray();
            }

            for(n = 0; n < 5; ++n)
            {
                up_sweep_ray_ids[num_up_rays+n] = up_sweep_ray_ids[n];
                up_sweep_ray_azimuth[num_up_rays+n] = up_sweep_ray_azimuth[n] + 360.0;
            }

            break;
        }

        if(processed_up_rays >= num_up_rays)
        {
            cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
            cout << "\tisweep = " << isweep << ", nsweeps =" << nsweeps << endl;
            cout << "\tprocessed_up_rays = " << processed_up_rays << ", num_up_rays =" << num_up_rays << endl;
            cout << "\tThis should not happen." << endl;
            exit -1;
            break;
        }

        pre_up_azimuth = up_azimuth;
        ++i_up_ray;
        if(i_up_ray >= num_up_rays)
            i_up_ray = 0;
        _advance_up_ray();
        ++processed_up_rays;
    }

    processed_up_rays = 0;
#endif
}

void Radx3dViewer::_set_ray_param(int n)
{
    MAKEMARK
#if 0
    nbins[n] = working_ray[n]->h.nbins;
    elev[n] = working_ray[n]->h.elev;
    gate_size[n] = working_ray[n]->h.gate_size;

    azimuth[n] = 90.0 - working_ray[n]->h.azimuth;
    if(0.0 > azimuth[n])
        azimuth[n] += 360.0;
#endif
}

void Radx3dViewer::_advance_bin()
{
    x[current_bin_index] = x[3];
    y[current_bin_index] = y[3];
    z[current_bin_index] = z[3];
    r[current_bin_index] = r[3];
    v[current_bin_index] = v[3];

    azimuth[current_bin_index] = azimuth[3];
    elev[current_bin_index] = elev[3];

    point[current_bin_index].fX = point[3].fX;
    point[current_bin_index].fY = point[3].fY;
    point[current_bin_index].fZ = point[3].fZ;
}

void Radx3dViewer::_set_new_point()
{
    MAKEMARK
#if 0
    int idx = -1;

    if((r[0] < r[1]) && (r[0] < r[2]))
        idx = 0;
    else if((r[1] < r[0]) && (r[1] < r[2]))
        idx = 1;
    else
        idx = 2;

    ++ibin[idx];

    if(ibin[idx] >= nbins[idx])
    {
        has_more_point = false;
        return;
    }

    binpos[idx] += gate_size[idx];
    x[3] = binpos[idx] * cos(azimuth[idx]*ARC) * scale;
    y[3] = binpos[idx] * sin(azimuth[idx]*ARC) * scale;
    z[3] = 10.0 * binpos[idx] * tan(elev[idx]*ARC) * scale;
    v[3] = working_ray[idx]->h.f(working_ray[idx]->range[ibin[idx]]);

    r[3] = x[3]*x[3] + y[3]*y[3] + z[3]*z[3];

    point[3].fX = x[3];
    point[3].fY = y[3];
    point[3].fZ = z[3];

    azimuth[3] = azimuth[idx];
    elev[3] = elev[idx];

    current_bin_index = idx;
#endif
}

void Radx3dViewer::_isosurface()
{
#if 0
    MAKEMARK
    int n;
    bool drawit = true;
    bool forwardup = false;
    int lvl = nvoptions->get_zsec();
    GLuint zcl = lister->get_zid(lvl);;

    if(zcl)
    {
        glCallList(zcl);
        return;
    }

    if(NULL == volume)
        return;

    working_ray = (Ray**) calloc(4, sizeof(Ray*));
    assert(working_ray);

    cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
    cout << "\tnsweeps =" << nsweeps << endl;
    zcl = glGenLists(1);

    cout << "\tzcl = " << zcl << endl;

    glNewList(zcl, GL_COMPILE_AND_EXECUTE);

  //OpenGL should normalize normal vectors
    glEnable(GL_NORMALIZE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

    glShadeModel(GL_SMOOTH);

  //glDisable(GL_CULL_FACE);
  //glEnable(GL_CULL_FACE);

    range = 500.0 * 1000.0;
    scale = 1.0 / range;

    glPushMatrix();

    nsweeps = volume->h.nsweeps;

    isweep = 0;
    _set_sweep_ray_ids();
    _replace_lower_sweep();

    for(isweep = 1; isweep < nsweeps; ++isweep)
    {
        processed_up_rays = 0;
        processed_low_rays = 0;

        _set_sweep_ray_ids();

      //cout << "\t\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
      //cout << "\t\tisweep =" << isweep << ", num_up_rays =" << num_up_rays;

        i_low_ray = low_sweep_ray_ids[processed_low_rays];
        i_up_ray = up_sweep_ray_ids[processed_up_rays];

        _advance_low_ray();
        _advance_up_ray();

        up_azimuth = up_sweep_ray_azimuth[processed_up_rays];
        low_azimuth = low_sweep_ray_azimuth[processed_low_rays];

        working_ray[0] = low_sweep->ray[i_low_ray];
        working_ray[1] =  up_sweep->ray[i_up_ray];
        _set_ray_param(0);
        _set_ray_param(1);

        while(1)
        {
          //cout << "\t\t\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
          //cout << "\t\t\tisweep =" << isweep << ", i_up_ray =" << i_up_ray << ", i_low_ray =" << i_low_ray << endl;
          //cout << "\t\t\tisweep =" << isweep << ", processed_up_rays =" << processed_up_rays;
          //cout << ", processed_low_rays =" << processed_low_rays << endl;
          //cout << "\t\t\tup_azimuth = " << up_azimuth << ", low_azimuth = " << low_azimuth << endl;
          //cout << "\t\t\tup_sweep_ray_azimuth[" << processed_up_rays << "] = " << up_sweep_ray_azimuth[processed_up_rays];
          //cout << "\t\t\tup_sweep_ray_ids[" << processed_up_rays << "] = " << up_sweep_ray_ids[processed_up_rays] << endl;

            if(up_azimuth < low_azimuth)
            {
                ++processed_up_rays;
                i_up_ray = up_sweep_ray_ids[processed_up_rays];
                _advance_up_ray();
                up_azimuth = up_sweep_ray_azimuth[processed_up_rays];
                working_ray[2] = up_sweep->ray[i_up_ray];
                forwardup = true;
            }
            else
            {
                ++processed_low_rays;
                i_low_ray = low_sweep_ray_ids[processed_low_rays];
                _advance_low_ray();
                low_azimuth = low_sweep_ray_azimuth[processed_low_rays];
                working_ray[2] = low_sweep->ray[i_low_ray];
                forwardup = false;
            }

            _set_ray_param(2);

          //cout << "\t\t\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
          //cout << "\t\t\tisweep =" << isweep << ", i_up_ray =" << i_up_ray << ", i_low_ray =" << i_low_ray << endl;
          //cout << "\t\t\tup_azimuth = " << up_azimuth << ", low_azimuth = " << low_azimuth << endl;
          //cout << "\t\t\tnbins[0] = " << nbins[0];
          //cout << "\t\t\tnbins[1] = " << nbins[1];
          //cout << "\t\t\tnbins[2] = " << nbins[2] << endl;

            drawit = true;
            for(n = 0; n < 3; ++n)
            {
                ibin[n] = 0;
                binpos[n] = working_ray[n]->h.range_bin1;

              //cout << "\t\t\t\tbinpos[" << n << "] = " << binpos[n];
              //cout << ", azimuth[" << n << "] = " << azimuth[n];
              //cout << ", elev[" << n << "] = " << elev[n] << endl;

                x[n] = binpos[n] * cos(azimuth[n]*ARC) * scale;
                y[n] = binpos[n] * sin(azimuth[n]*ARC) * scale;
                z[n] = 10.0 * binpos[n] * tan(elev[n]*ARC) * scale;
                v[n] = working_ray[n]->h.f(working_ray[n]->range[ibin[n]]);

                r[n] = x[n]*x[n] + y[n]*y[n] + z[n]*z[n];

                point[n].fX = x[n];
                point[n].fY = y[n];
                point[n].fZ = z[n];
            }

            has_more_point = true;
            while(has_more_point)
            {
                _set_new_point();

                drawit = true;
                for(n = 0; n < 4; ++n)
                {
                    if(v[n] < _lowbound)
                    {
                        drawit = false;
                        break;
                    }
                    else if(v[n] > _upbound)
                    {
                        drawit = false;
                        break;
                    }
                }

              //if(drawit && (i_up_ray == 678))
                if(drawit)
                {
                    mTetrahedron->vTetrahedron(point, v);

                  //cout << "\t\t\t\tibin[0] = " << ibin[0];
                  //cout << ", ibin[1] = " << ibin[1];
                  //cout << ", ibin[2] = " << ibin[2] << endl;

                  //for(n = 0; n < 4; ++n)
                  //{
                  //    cout << "\t\t\tv[" << n << "] = " << v[n];
                  //    cout << ", x[" << n << "] = " << x[n] * range;
                  //    cout << ", y[" << n << "] = " << y[n] * range;
                  //    cout << ", z[" << n << "] = " << z[n] * range << endl;
                  //}

                  //cout << "\t\t\t\tibin[" << current_bin_index << "] = " << ibin[current_bin_index] << endl;
                  //cout << "\t\t\tv[3] = " << v[3];
                  //cout << ", x[3] = " << x[3] * range;
                  //cout << ", y[3] = " << y[3] * range;
                  //cout << ", z[3] = " << z[3] * range << endl;
                }

                _advance_bin();
            }

            if((processed_up_rays > num_up_rays) && (processed_low_rays > num_low_rays))
                break;

            if(forwardup)
            {
                working_ray[1] = up_sweep->ray[i_up_ray];
                _set_ray_param(1);
            }
            else
            {
                working_ray[0] = low_sweep->ray[i_low_ray];
                _set_ray_param(0);
            }
        }

        _replace_lower_sweep();
    }

  //glDisable(GL_CULL_FACE);

  //glNormal3f(0.0, 0.0, -1.0);

    glEndList();

    lister->set_zid(lvl, zcl);

    glPopMatrix();
#endif
}

