//$Id: windvector.cpp 5321 2015-02-20 03:20:52Z starviewer $

#include <QtOpenGL>

#include "windvector.h"

WindVector::WindVector(ColorTable *ct, NVOptions* opt)
{
    nvoptions = opt;
    set_colorTable(ct);

    arrow = new Arrow(ct);

    // _scale = 0.50;
    // _scale = 5000.0;
    // _wings = 1000.0;
    _scale = 1000.0;
    _wings = 100.0;
    _zScale = 0.50;
    _stepsize = 5;
    _local_stepsize = _stepsize;
    _maxspeed = 100.0;
    // _maxspeed = 5.0;

    axx[0] = 1.0;
    axx[1] = 0.0;
    axx[2] = 0.0;

    ayy[0] = 0.0;
    ayy[1] = 1.0;
    ayy[2] = 0.0;

    azz[0] = 0.0;
    azz[1] = 0.0;
    azz[2] = 1.0;
}

WindVector::~WindVector()
{
    delete arrow;
}

void WindVector::setup(int nx, int ny, int nz,
                       float* u, float* v, float*w)
{
    _nx = nx;
    _ny = ny;
    _nz = nz;

    _u = u;
    _v = v;
    _w = w;

    _has_w = true;

    _parameter_setup();

    nvoptions->set_xsec(_nx + 1);
    nvoptions->set_ysec(_ny + 1);
    nvoptions->set_zsec(0);
}

void WindVector::setup(int nx, int ny, int nz,
                       float* u, float* v)
{
    _nx = nx;
    _ny = ny;
    _nz = nz;

    _u = u;
    _v = v;

    _has_w = false;

    _parameter_setup();

    nvoptions->set_xsec(_nx + 1);
    nvoptions->set_ysec(_ny + 1);
    nvoptions->set_zsec(0);
}

void WindVector::_parameter_setup()
{
    double hDelt, vDelt;

    if(_nx > _ny)
    {
        hDelt = 1.0 / (_nx - 1.0);

        _local_stepsize = _nx / 100;
    }
    else
    {
        hDelt = 1.0 / (_ny - 1.0);

        _local_stepsize = _ny / 100;
    }

    if(1 < _nz)
        vDelt = 1.0 / (_nz - 1.0);
    else
        vDelt = 1.0;

    _scale = 50.0 * hDelt / _maxspeed;
    _zScale = 1000.0 * vDelt / _maxspeed;

    // cout << "Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__
    //      << ", file: <" << __FILE__ << ">" << endl;
    // cout << "\t_nx = " << _nx << ", _ny = " << _ny << ", _nz = " << _nz << endl;
    // cout << "\t_scale = " << _scale << ", _zScale = " << _zScale << endl;

    if(_stepsize < _local_stepsize)
       _stepsize = _local_stepsize;
}

void WindVector::draw(int k, double z)
{
    int i, j, n;
    double zp = z + 0.025;

    if(nvoptions->get_cb(NV_VECTOR_LONGER))
    {
        arrow->longer();
        nvoptions->set_cb(NV_VECTOR_LONGER, false);
    }

    if(nvoptions->get_cb(NV_VECTOR_SHORTER))
    {
        arrow->shorter();
        nvoptions->set_cb(NV_VECTOR_SHORTER, false);
    }

    if(nvoptions->get_cb(NV_VECTOR_WIDER))
    {
        arrow->wider();
        nvoptions->set_cb(NV_VECTOR_WIDER, false);
    }

    if(nvoptions->get_cb(NV_VECTOR_NARROWER))
    {
        arrow->narrower();
        nvoptions->set_cb(NV_VECTOR_NARROWER, false);
    }

    if(nvoptions->get_cb(NV_VECTOR_MORE))
    {
        --_stepsize;
        if(1 > _stepsize)
            _stepsize = 1;
        nvoptions->set_cb(NV_VECTOR_MORE, false);
    }

    if(nvoptions->get_cb(NV_VECTOR_LESS))
    {
        ++_stepsize;
        nvoptions->set_cb(NV_VECTOR_LESS, false);
    }

    // cout << "\tIn functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    // cout << "\t_nx = " << _nx << ", _ny = " << _ny << ", _nz = " << _nz << endl;
    // cout << "\tnvoptions->get_zsec() = " << nvoptions->get_zsec() << endl;
    // cout << "\tcolorTable = " << colorTable << endl;

    _colorlen = colorTable->get_clen() - 3;
    _colormap = colorTable->get_cmap();

    if(_stepsize < _local_stepsize)
       _stepsize = _local_stepsize;

    if(has_w())
    {
      for(j = _stepsize/2; j < _ny; j += _stepsize)
      {
        n = (k * _ny + j) * _nx;

        for(i = _stepsize/2; i < _nx; i += _stepsize)
        {
            _draw_arrow(_xFlat[i], _yFlat[j], zp, _u[n+i], _v[n+i], _w[n+i]);
        }
      }
    }
    else
    {
      for(j = _stepsize/2; j < _ny; j += _stepsize)
      {
        n = (k * _ny + j) * _nx;

        for(i = _stepsize/2; i < _nx; i += _stepsize)
        {
            _draw_arrow(_xFlat[i], _yFlat[j], zp, _u[n+i], _v[n+i]);
        }
      }
    }
}

void WindVector::_set_color(double spd, float* color)
{
#if 0
    color[0]  = 1.0;
    color[1]  = 1.0;
    color[2]  = 1.0;
#else
    int idx = (int) ((spd * _colorlen) / _maxspeed);
 
    if(idx > (_colorlen - 1))
        idx = _colorlen - 1;

    color[0]  = _colormap[3*(idx+3)];
    color[1]  = _colormap[3*(idx+3) + 1];
    color[2]  = _colormap[3*(idx+3) + 2];
#endif
}

void WindVector::_draw_arrow(double x, double y, double z,
                             float u, float v, float w)
{
    float color[3];
    double tail[3];
    double head[3];
    double suvw[3];
    double dist = sqrt(u*u + v*v + w*w);

    _set_color(dist, color);
    glColor3fv(color);

    tail[0] = x;
    tail[1] = y;
    tail[2] = z;

    suvw[0] = _scale * u;
    suvw[1] = _scale * v;
    suvw[2] = _zScale * w;

    head[0] = x + suvw[0];
    head[1] = y + suvw[1];
    head[2] = z + suvw[2];

    _arrow(tail, head, suvw);
}

void WindVector::_draw_arrow(double x, double y, double z,
                             float u, float v)
{
    float color[3];
    double tail[3];
    double head[3];
    double suvw[3];
    double dist = sqrt(u*u + v*v);

    _set_color(dist, color);
    glColor3fv(color);

    tail[0] = x;
    tail[1] = y;
    tail[2] = z;

    suvw[0] = _scale * u;
    suvw[1] = _scale * v;
    suvw[2] = 0.0;

    head[0] = x + suvw[0];
    head[1] = y + suvw[1];
    head[2] = z;

    _arrow(tail, head, suvw);
}

void WindVector::_arrow(double tail[3], double head[3], double w[3])
{
    double u[3], v[3];        // arrow coordinate system

  //determine major direction:
    int axis = X_DIRECTION;
    double mag = fabs(w[0]);
    if(mag < fabs(w[1]))
    {
        axis = Y_DIRECTION;
        mag = fabs(w[1]);
    }

    if(mag < fabs(w[2]))
    {
        axis = Z_DIRECTION;
        mag = fabs(w[2]);
    }

  //set size of wings and turn w into a Unit vector:
    double d = _wings * _dist(w);

  //if(d > 0.125)
  //   d = 0.125;

  //if(d > 1.25)
  //   d = 1.25;

    if(d > 2.5)
       d = 2.5;

  //cout << "Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__
  //     << ", file: <" << __FILE__ << ">" << endl;
  //cout << "\ttail[0] = " << tail[0] << ", head[0] = " << head[0] << endl;
  //cout << "\ttail[1] = " << tail[1] << ", head[1] = " << head[1] << endl;
  //cout << "\ttail[2] = " << tail[2] << ", head[2] = " << head[2] << endl;

  //draw the shaft of the arrow:
    glBegin( GL_LINE_STRIP );
        glVertex3dv( tail );
        glVertex3dv( head );
    glEnd( );

  //draw two sets of wings in the non-major directions:
    double x, y, z;

    if(axis != X_DIRECTION)
    {
        _cross( w, axx, v );
        _cross( v, w, u  );

        x = head[0] + d * ( u[0] - w[0] );
        y = head[1] + d * ( u[1] - w[1] );
        z = head[2] + d * ( u[2] - w[2] );

        glBegin( GL_LINE_STRIP );
            glVertex3dv( head );
            glVertex3d( x, y, z );
        glEnd( );

        x = head[0] + d * ( -u[0] - w[0] );
        y = head[1] + d * ( -u[1] - w[1] );
        z = head[2] + d * ( -u[2] - w[2] );

        glBegin( GL_LINE_STRIP );
            glVertex3dv( head );
            glVertex3d( x, y, z );
        glEnd( );
    }

    if(axis != Y_DIRECTION)
    {
        _cross( w, ayy, v );
        _cross( v, w, u  );

        x = head[0] + d * ( u[0] - w[0] );
        y = head[1] + d * ( u[1] - w[1] );
        z = head[2] + d * ( u[2] - w[2] );

        glBegin( GL_LINE_STRIP );
            glVertex3dv( head );
            glVertex3d( x, y, z );
        glEnd( );

        x = head[0] + d * ( -u[0] - w[0] );
        y = head[1] + d * ( -u[1] - w[1] );
        z = head[2] + d * ( -u[2] - w[2] );

        glBegin( GL_LINE_STRIP );
            glVertex3dv( head );
            glVertex3d( x, y, z );
        glEnd( );
    }

    if(axis != Z_DIRECTION)
    {
        _cross( w, azz, v );
        _cross( v, w, u  );

        x = head[0] + d * ( u[0] - w[0] );
        y = head[1] + d * ( u[1] - w[1] );
        z = head[2] + d * ( u[2] - w[2] );

        glBegin( GL_LINE_STRIP );
            glVertex3dv( head );
            glVertex3d( x, y, z );
        glEnd( );

        x = head[0] + d * ( -u[0] - w[0] );
        y = head[1] + d * ( -u[1] - w[1] );
        z = head[2] + d * ( -u[2] - w[2] );

        glBegin( GL_LINE_STRIP );
            glVertex3dv( head );
            glVertex3d( x, y, z );
        glEnd( );
    }
}

void WindVector::_cross( double v1[3], double v2[3], double vout[3] )
{
    vout[0] = v1[1]*v2[2] - v2[1]*v1[2];
    vout[1] = v2[0]*v1[2] - v1[0]*v2[2];
    vout[2] = v1[0]*v2[1] - v2[0]*v1[1];
}

double WindVector::_dist(double v[3])
{
    double dist = v[0]*v[0] + v[1]*v[1] + v[2]*v[2];

    return dist;
}

void WindVector::setup_lonlat(double* lon, double* lat)
{
    // cout << "\nEnter " << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
    // cout << "\tlon[0] = " << lon[0] << ", lat[0] = " << lat[0] << endl;
    _lon = lon;
    _lat = lat;
    // cout << "Leave " << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
}

void WindVector::setup_xyFlat(double* xFlat, double* yFlat)
{
    // cout << "\nEnter " << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
    // cout << "\txFlat[0] = " << xFlat[0] << ", yFlat[0] = " << yFlat[0] << endl;
    _xFlat = xFlat;
    _yFlat = yFlat;
    // cout << "Leave " << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
}

