//$Id: radar2dviewer.cpp 14613 2013-11-04 20:48:49Z starviewer $

#include <QtOpenGL>

#include "radar2dviewer.h"

RADAR2dViewer::RADAR2dViewer(ColorTable *ct, NVOptions* opt)
{
    colorTable = ct;
    nvoptions = opt;

    locator = NULL;

    lister = new Lister();
    lister->setup(1, 1, 121);

    _zlist = NULL;

    _minval = 1.0e20;
    _maxval = -1.0e20;

    volume = NULL;

    _colorlen = colorTable->get_clen() - 3;
    _colormap = colorTable->get_cmap();
}

RADAR2dViewer::~RADAR2dViewer()
{
    locator->turnOff();
    delete lister;
}

void RADAR2dViewer::set_radar(RADARReader *r)
{
    radar = r;

    _initialize();
}

void RADAR2dViewer::setup(string vn)
{
    reset();

    _varname  = vn;

    volume = radar->get_volume(vn);

    nsweeps = volume->h.nsweeps;

    _set_minmax();
}

void RADAR2dViewer::reset()
{
    lister->reinitialize(1, 1, 121);
}

void RADAR2dViewer::_initialize()
{
    lister->reinitialize(1, 1, 121);
}

void RADAR2dViewer::get_location(double &blon, double &blat)
{
    _base_lon = radar->get_base_lon();
    _base_lat = radar->get_base_lat();
    blon = _base_lon;
    blat = _base_lat;
}

void RADAR2dViewer::draw()
{
    int lvl = nvoptions->get_zsec();
    GLuint zcl = lister->get_zid(lvl);

  //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\t_varname: <" << _varname << ">, zcl = " << zcl << endl;
  //cout << "\tlvl = " << lvl << endl;

#if 0
    _draw_grid();
#else
    if(zcl)
        glCallList(zcl);
    else
        _display();
#endif
}

void RADAR2dViewer::_draw_cross(double dist)
{
    double x, y, z;
    double lon = (double) locator->x();
    double lat = (double) locator->y();
    
    glLineWidth(2.0);
    glColor4d(1.0, 0.0, 1.0, 0.95);

    x = dist * cos(lon * ARC);
    y = dist * sin(lat * ARC);
    z = 0.0;

    glBegin(GL_LINE_STRIP);
        glVertex3f(x-0.02, y, z);
        glVertex3f(x+0.02, y, z);

        glVertex3f(x, y-0.02, z);
        glVertex3f(x, y+0.02, z);
    glEnd();
}

void RADAR2dViewer::_set_minmax()
{
    int nbins, nrays;
    int ibin, iray;

    float value;

    _minval = BADVAL;
    _maxval = -BADVAL;

    _upbound = 0.995 * _minval;
    _lowbound = 0.995 * _maxval;

    if(NULL == volume)
        return;

  //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\t_minval = " << _minval << ", _maxval = " << _maxval << endl;
  //cout << "\t_upbound = " << _upbound << ", _lowbound = " << _lowbound << endl;

    for(isweep = 0; isweep < nsweeps; ++isweep)
    {
        sweep = volume->sweep[isweep];

        nrays = sweep->h.nrays;

        for(iray = 0; iray < nrays; ++iray)
        {
            ray = sweep->ray[iray];
            nbins = ray->h.nbins;

            for(ibin = 0; ibin < nbins; ++ibin)
            {
                value = ray->h.f(ray->range[ibin]);

                if(value > _upbound)
                    continue;
                if(value < _lowbound)
                    continue;

                if(_minval > value)
                    _minval = value;
                if(_maxval < value)
                    _maxval = value;
            }
        }
    }

    cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
    cout << "\t_minval = " << _minval << ", _maxval = " << _maxval << endl;
}

void RADAR2dViewer::_draw_rect(double x[4], double y[4], double z[4])
{
    int n;

    glLineWidth(1.0);

    glBegin(GL_LINE_STRIP);
        for(n = 0; n < 4; ++n)
            glVertex3f(x[n], y[n], z[n]);

        glVertex3f(x[0], y[0], z[n]);
    glEnd();
}

void RADAR2dViewer::_draw_grid()
{
    int lvl = nvoptions->get_zsec();
    int nbins, nrays;
    int ibin, iray;

    double x[4], y[4], z[4];
  //doubel lon[4], lat[4];

    double azimuth, elev;
    double half_beamwidth;
    double gate_size;
    double range;

  //double meters2lat = 1.0/111177.0;
  //double meters2lon = meters2lat / cos(radar_lat * ARC);

    double bstart, bend;

    if(NULL == volume)
        return;

  //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;

    glPushMatrix();

  //Draw lines 
    glColor3f(0.0, 1.0, 1.0);

    sweep = volume->sweep[lvl];

    half_beamwidth = 1.05 * 0.5 * sweep->h.beam_width;
    nrays = sweep->h.nrays;

    range = 500.0 * 1000.0;

  //cout << "\tnrays = " << nrays << endl;

    for(iray = 0; iray < nrays; ++iray)
    {
        ray = sweep->ray[iray];
        elev = ray->h.elev;
        nbins = ray->h.nbins;
        gate_size = ray->h.gate_size;
      //range = (double) ray->range[0];

        azimuth = 90.0 - ray->h.azimuth;
        if(0.0 > azimuth)
            azimuth += 360.0;

      //cout << "\t\tray " << iray << ": nbins = " << nbins << ", azimuth = " << azimuth << endl;
      //cout << "\t\t\trange = " << range << ", gate_size = " << gate_size << ", half_beamwidth = " << half_beamwidth << endl;

        bstart = ray->h.range_bin1;
        for(ibin = 0; ibin < nbins; ++ibin)
        {
            bend = bstart + gate_size;

            x[0] = bstart * cos((azimuth + half_beamwidth)*ARC) / range;
            y[0] = bstart * sin((azimuth + half_beamwidth)*ARC) / range;
            z[0] = 10.0 * bstart * tan(elev*ARC) / range;

            x[1] = bstart * cos((azimuth - half_beamwidth)*ARC) / range;
            y[1] = bstart * sin((azimuth - half_beamwidth)*ARC) / range;
            z[1] = 10.0 * bstart * tan(elev*ARC) / range;

            x[2] = bend * cos((azimuth - half_beamwidth)*ARC) / range;
            y[2] = bend * sin((azimuth - half_beamwidth)*ARC) / range;
            z[2] = 10.0 * bend * tan(elev*ARC) / range;

            x[3] = bend * cos((azimuth + half_beamwidth)*ARC) / range;
            y[3] = bend * sin((azimuth + half_beamwidth)*ARC) / range;
            z[3] = 10.0 * bend * tan(elev*ARC) / range;

          //cout << "\t\t\tx[0] = " << x[0] << ", y[0] = " << y[0] << endl;
          //cout << "\t\t\tx[1] = " << x[1] << ", y[1] = " << y[1] << endl;
          //cout << "\t\t\tx[2] = " << x[2] << ", y[2] = " << y[2] << endl;
          //cout << "\t\t\tx[3] = " << x[3] << ", y[3] = " << y[3] << endl;

          //cout << "\t\t\tibin = " << ibin << ", value = " << value << endl;

            _draw_rect(x, y, z);

            bstart = bend;
        }
    }

    glPopMatrix();
}

void RADAR2dViewer::_set_color(float val)
{
    float color[4];
    int idx = 0;

    if((val > _lowbound) && ( val < _upbound))
    {
        idx = (int) (((val - _minval) * _colorlen) / (_maxval - _minval));

        color[0] = _colormap[3*(idx+3)];
        color[1] = _colormap[3*(idx+3) + 1];
        color[2] = _colormap[3*(idx+3) + 2];
      //color[3] = 0.25 + (val - _minval) / (_maxval - _minval);
        color[3] = 0.05 + (val - _minval) / (_maxval - _minval);
        if(color[3] > 1.0)
           color[3] = 1.0;
    }
    else
    {
        color[0] = 0.90;
        color[1] = 0.90;
        color[2] = 0.90;
        color[3] = 0.25;
    }

    glColor4fv(color);
}

void RADAR2dViewer::_set_color3d(float val)
{
    float color[4];
    int idx = 0;

    if((val > _lowbound) && ( val < _upbound))
    {
        idx = (int) (((val - _minval) * _colorlen) / (_maxval - _minval));

        color[0] = _colormap[3*(idx+3)];
        color[1] = _colormap[3*(idx+3) + 1];
        color[2] = _colormap[3*(idx+3) + 2];
      //color[3] = 0.25 + (val - _minval) / (_maxval - _minval);
        color[3] = 0.005 + 0.75 * (val - _minval) / (_maxval - _minval);
    }
    else
    {
        color[0] = 0.95;
        color[1] = 0.95;
        color[2] = 0.95;
        color[3] = 0.125;
    }

    glColor4fv(color);
}

void RADAR2dViewer::_fill_quad(double x[4], double y[4], double z[4], float value)
{
    int n;

    _set_color(value);

    glBegin(GL_QUADS);
        for(n = 0; n < 4; ++n)
        {
            glVertex3f(x[n], y[n], z[n]);
        }
    glEnd();
}

void RADAR2dViewer::_display()
{
    int lvl = nvoptions->get_zsec();
    int nbins, nrays;
    int ibin, iray;
    GLuint zcl = 0;

    double x[4], y[4], z[4];
  //doubel lon[4], lat[4];

    double azimuth, elev;
    double half_beamwidth;
    double gate_size;
    double range;
    float value;

  //double meters2lat = 1.0/111177.0;
  //double meters2lon = meters2lat / cos(radar_lat * ARC);

    double bstart, bend;

    if(NULL == volume)
        return;

  //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;

    if(volume->h.nsweeps <= lvl)
    {
        _display3d();
        return;
    }

    zcl = glGenLists(1);

  //cout << "\tzcl = " << zcl << endl;

    glNewList(zcl, GL_COMPILE_AND_EXECUTE);

  /*OpenGL should normalize normal vectors*/
  //glEnable(GL_NORMALIZE);

    glEnable(GL_BLEND);
  //glBlendFunc(GL_SRC_ALPHA,GL_ONE);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

  //glShadeModel(GL_SMOOTH);

  //glDisable(GL_CULL_FACE);
  //glEnable(GL_CULL_FACE);

    glPushMatrix();

    sweep = volume->sweep[lvl];

    half_beamwidth = 1.05 * 0.5 * sweep->h.beam_width;
    nrays = sweep->h.nrays;

    range = 500.0 * 1000.0;

  //cout << "\tnrays = " << nrays << endl;

    for(iray = 0; iray < nrays; ++iray)
    {
        ray = sweep->ray[iray];
        nbins = ray->h.nbins;
        elev = ray->h.elev;
        gate_size = ray->h.gate_size;

        azimuth = 90.0 - ray->h.azimuth;
        if(0.0 > azimuth)
            azimuth += 360.0;

      //cout << "\t\tray " << iray << ": nbins = " << nbins << ", azimuth = " << azimuth << endl;
      //cout << "\t\t\trange = " << range << ", gate_size = " << gate_size << ", half_beamwidth = " << half_beamwidth << endl;

        bstart = ray->h.range_bin1;
        for(ibin = 0; ibin < nbins; ++ibin)
        {
            value = ray->h.f(ray->range[ibin]);

            bend = bstart + gate_size;

            x[0] = bstart * cos((azimuth + half_beamwidth)*ARC) / range;
            y[0] = bstart * sin((azimuth + half_beamwidth)*ARC) / range;
            z[0] = 10.0 * bstart * tan(elev*ARC) / range;

            x[1] = bstart * cos((azimuth - half_beamwidth)*ARC) / range;
            y[1] = bstart * sin((azimuth - half_beamwidth)*ARC) / range;
            z[1] = 10.0 * bstart * tan(elev*ARC) / range;

            x[2] = bend * cos((azimuth - half_beamwidth)*ARC) / range;
            y[2] = bend * sin((azimuth - half_beamwidth)*ARC) / range;
            z[2] = 10.0 * bend * tan(elev*ARC) / range;

            x[3] = bend * cos((azimuth + half_beamwidth)*ARC) / range;
            y[3] = bend * sin((azimuth + half_beamwidth)*ARC) / range;
            z[3] = 10.0 * bend * tan(elev*ARC) / range;

            _fill_quad(x, y, z, value);

            bstart = bend;
        }
    }

  //glDisable(GL_CULL_FACE);

  //glNormal3f(0.0, 0.0, -1.0);

    glEndList();

    lister->set_zid(lvl, zcl);

    glPopMatrix();
}

void RADAR2dViewer::_fill_cube(double x[8], double y[8], double z[8], float value)
{
    if((value < _lowbound) || ( value > _upbound))
        return;

    _set_color3d(value);

    glBegin(GL_QUADS);
#if 1
        glVertex3f(x[1], y[1], z[1]);
        glVertex3f(x[2], y[2], z[2]);
        glVertex3f(x[7], y[7], z[7]);
        glVertex3f(x[4], y[4], z[4]);

      //glVertex3f(x[0], y[0], z[0]);
      //glVertex3f(x[2], y[2], z[2]);
      //glVertex3f(x[6], y[6], z[6]);
      //glVertex3f(x[4], y[4], z[4]);
#else
        glVertex3f(x[0], y[0], z[0]);
        glVertex3f(x[1], y[1], z[1]);
        glVertex3f(x[6], y[6], z[6]);
        glVertex3f(x[7], y[7], z[7]);

        glVertex3f(x[2], y[2], z[2]);
        glVertex3f(x[3], y[3], z[3]);
        glVertex3f(x[4], y[4], z[4]);
        glVertex3f(x[5], y[5], z[5]);

        glVertex3f(x[1], y[1], z[1]);
        glVertex3f(x[2], y[2], z[2]);
        glVertex3f(x[7], y[7], z[7]);
        glVertex3f(x[4], y[4], z[4]);

        glVertex3f(x[0], y[0], z[0]);
        glVertex3f(x[3], y[3], z[3]);
        glVertex3f(x[6], y[6], z[6]);
        glVertex3f(x[5], y[5], z[5]);
#endif
    glEnd();
}

void RADAR2dViewer::_display3d()
{
    int lvl = nvoptions->get_zsec();
    int nbins, nrays;
    int ibin,  iray;
    GLuint zcl = 0;

    double x[8], y[8], z[8];
  //doubel lon[4], lat[4];

    double azimuth, elev;
    double half_beamwidth;
    double gate_size;
    double range;
    float value;

  //double meters2lat = 1.0/111177.0;
  //double meters2lon = meters2lat / cos(radar_lat * ARC);

    double bstart, bend;

    if(NULL == volume)
        return;

  //zcl = glGenLists(1);

  //cout << "\tzcl = " << zcl << endl;

  //glNewList(zcl, GL_COMPILE_AND_EXECUTE);

  /*OpenGL should normalize normal vectors*/
  //glEnable(GL_NORMALIZE);

    glEnable(GL_BLEND);
  //glBlendFunc(GL_SRC_ALPHA,GL_ONE);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

  //glShadeModel(GL_SMOOTH);

  //glDisable(GL_CULL_FACE);
  //glEnable(GL_CULL_FACE);

    range = 500.0 * 1000.0;

    glPushMatrix();

    for(isweep = 0; isweep < nsweeps; ++isweep)
    {
        sweep = volume->sweep[isweep];

        half_beamwidth = 1.05 * 0.5 * sweep->h.beam_width;
        nrays = sweep->h.nrays;

        for(iray = 0; iray < nrays; ++iray)
        {
            ray = sweep->ray[iray];
            nbins = ray->h.nbins;
            elev = ray->h.elev;
            gate_size = ray->h.gate_size;

            azimuth = 90.0 - ray->h.azimuth;
            if(0.0 > azimuth)
                azimuth += 360.0;

            bstart = ray->h.range_bin1;
            for(ibin = 0; ibin < nbins; ++ibin)
            {
                value = ray->h.f(ray->range[ibin]);

                bend = bstart + gate_size;

                if((value > _lowbound) && ( value < _upbound))
                {
                    x[0] = bstart * cos((azimuth + half_beamwidth)*ARC) / range;
                    y[0] = bstart * sin((azimuth + half_beamwidth)*ARC) / range;
                    z[0] = 10.0 * bstart * tan((elev - half_beamwidth)*ARC) / range;

                    x[1] = bstart * cos((azimuth - half_beamwidth)*ARC) / range;
                    y[1] = bstart * sin((azimuth - half_beamwidth)*ARC) / range;
                    z[1] = 10.0 * bstart * tan((elev - half_beamwidth)*ARC) / range;

                    x[2] = bend * cos((azimuth - half_beamwidth)*ARC) / range;
                    y[2] = bend * sin((azimuth - half_beamwidth)*ARC) / range;
                    z[2] = 10.0 * bstart * tan((elev - half_beamwidth)*ARC) / range;

                    x[3] = bend * cos((azimuth + half_beamwidth)*ARC) / range;
                    y[3] = bend * sin((azimuth + half_beamwidth)*ARC) / range;
                    z[3] = 10.0 * bstart * tan((elev - half_beamwidth)*ARC) / range;

                    x[4] = bstart * cos((azimuth + half_beamwidth)*ARC) / range;
                    y[4] = bstart * sin((azimuth + half_beamwidth)*ARC) / range;
                    z[4] = 10.0 * bstart * tan((elev + half_beamwidth)*ARC) / range;

                    x[5] = bstart * cos((azimuth - half_beamwidth)*ARC) / range;
                    y[5] = bstart * sin((azimuth - half_beamwidth)*ARC) / range;
                    z[5] = 10.0 * bstart * tan((elev + half_beamwidth)*ARC) / range;

                    x[6] = bend * cos((azimuth - half_beamwidth)*ARC) / range;
                    y[6] = bend * sin((azimuth - half_beamwidth)*ARC) / range;
                    z[6] = 10.0 * bstart * tan((elev + half_beamwidth)*ARC) / range;

                    x[7] = bend * cos((azimuth + half_beamwidth)*ARC) / range;
                    y[7] = bend * sin((azimuth + half_beamwidth)*ARC) / range;
                    z[7] = 10.0 * bstart * tan((elev + half_beamwidth)*ARC) / range;

                    _fill_cube(x, y, z, value);
                }

                bstart = bend;
            }
        }
    }

  //glDisable(GL_CULL_FACE);

  //glNormal3f(0.0, 0.0, -1.0);

    glEndList();

    lister->set_zid(lvl, zcl);

    glPopMatrix();
}

