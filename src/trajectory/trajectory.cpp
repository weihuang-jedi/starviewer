//$Id: trajectory.cpp 4977 2014-02-14 16:11:38Z starviewer $

#include <QtOpenGL>

#include "trajectory.h"

#define PUTAMARK	{ cerr << "\nfunctions: <" << __PRETTY_FUNCTION__ << ">"; \
                          cerr << "\tline: " << __LINE__; \
                          cerr << "\tfile: <" << __FILE__ << ">" << endl; }

int Trajectory::instance = 0;

Trajectory::Trajectory(ColorTable *ct, NVOptions* opt)
{
    colorTable = ct;
    nvoptions = opt;

    _number_of_points = 0;

    _width = 0.005;

  //topography = new Topography();
    topography.set_nvoptions(opt);

  //cone = new Cone();
    cone.set_sides(36);

  //cylinder = new EqualCylinder();

    interpolater = new Interp();

    _nx = 0;
    _ny = 0;
    _nz = 0;

    _dt = 1.0;
    _dx = 1.0;
    _tspan = 1.0;
    dtoverdx = 1.0;

    trajlist = 0;

    _maxval = 15000.0;

    ++instance;

  //cerr << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">";
  //cerr << "\tline: " << __LINE__;
  //cerr << "\tfile: <" << __FILE__ << ">" << endl;
}

Trajectory::~Trajectory()
{
    --instance;

  //if(NULL != cone)
  //    delete cone;
  //if(NULL != cylinder)
  //    delete cylinder;
  //if(NULL != topography)
  //    delete topography;
    if(NULL != interpolater)
        delete interpolater;

    if(_number_of_points)
        delete [] position;

  //cone = NULL;
  //cylinder = NULL;
  //topography = NULL;
    interpolater = NULL;
    _number_of_points = 0;
}

void Trajectory::setup(int nx, int ny, int nz,
                       float* u, float* v, float* w, float* h)
{
    _nx = nx;
    _ny = ny;
    _nz = nz;

    _u1 = u;
    _v1 = v;
    _w1 = w;
    _h1 = h;

    _u2 = u;
    _v2 = v;
    _w2 = w;
    _h2 = h;

    west = 0.0;
    east = _nx - 1.0;
    south = 0.0;
    north = _ny - 1.0;

    _parameter_setup();

    nvoptions->set_xsec(_nx + 1);
    nvoptions->set_ysec(_ny + 1);
    nvoptions->set_zsec(0);

    cerr << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">";
    cerr << "\tline: " << __LINE__;
    cerr << "\tfile: <" << __FILE__ << ">" << endl;
    cerr << "\tnx = " << nx << ", ny = " << ny << ", nz = " << nz << endl;
}

void Trajectory::_parameter_setup()
{
    if(_nx > _ny)
    {
        _xyDelt = 1.0 / (_nx - 1.0);
        _yStart = 0.5 * (1.0 - (_ny - 1.0) * _xyDelt);
        _xStart = 0.0;
    }
    else
    {
        _xyDelt = 1.0 / (_ny - 1.0);
        _xStart = 0.5 * (1.0 - (_nx - 1.0) * _xyDelt);
        _yStart = 0.0;
    }

    if(1 < _nz)
        _zDelt = 1.0 / (_nz - 1.0);
    else
        _zDelt = 1.0;

    h_stepsize = 25;
    v_stepsize = 5;
    _number_of_points = (_nx/h_stepsize) * (_ny/h_stepsize) * (_nz/v_stepsize);
    position = new Position[_number_of_points];
    initialize();
}

void Trajectory::initialize()
{
    double x, y, z;
    int i, j, k;
    long n = 0;

    _tfac = 0.0;
    for(k = v_stepsize/2; k < _nz; k += v_stepsize)
    {
        if(n >= _number_of_points)
            break;

        z = 0.5 + (double)k;

        for(j = h_stepsize/2; j < _nx; j += h_stepsize)
        {
            y = 0.5 + (double)j;

            for(i = h_stepsize/2; i < _nx; i += h_stepsize)
            {
                if(n >= _number_of_points)
                   break;

                x = 0.5 + (double)i;

                position[n].init(x, y, z);
                position[n].set_outbound(_outofbound(position[n]));
                ++n;
            }
        }
    }

  //cerr << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">";
  //cerr << "\tline: " << __LINE__;
  //cerr << "\tfile: <" << __FILE__ << ">" << endl;
  //cerr << "\t_nx = " << _nx << ", _ny = " << _ny << ", _nz = " << _nz << endl;
  //cerr << "\t_number_of_points = " << _number_of_points << endl;
}

void Trajectory::_set_color(double val)
{
    int idx = (int) ((val * _colorlen) / _maxval);
 
    if(idx > (_colorlen - 1))
        idx = _colorlen - 1;

    color[0]  = _colormap[3*(idx+3)];
    color[1]  = _colormap[3*(idx+3) + 1];
    color[2]  = _colormap[3*(idx+3) + 2];
}

void Trajectory::set_dtNdx(float dt, float dx)
{
    _dt = dt;
    _dx = dx;
    dtoverdx = dt / dx;

  //cerr << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">";
  //cerr << "\tline: " << __LINE__;
  //cerr << "\tfile: <" << __FILE__ << ">" << endl;
  //cerr << "\tdt = " << dt << ", dx = " << dx << endl;
}

void Trajectory::set_tspan(float t1, float t2)
{
  //_tspan = 60.0*(t2 - t1);
    _tspan = 10.0*(t2 - t1);
  //_tspan = t2 - t1;

    cerr << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">";
    cerr << "\tline: " << __LINE__;
    cerr << "\tfile: <" << __FILE__ << ">" << endl;
    cerr << "\tt1 = " << t1 << ", t2 = " << t2 << endl;
}

void Trajectory::draw()
{
  //cerr << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">";
  //cerr << "\tline: " << __LINE__;
  //cerr << "\tfile: <" << __FILE__ << ">" << endl;

  //if(glIsList(trajlist))
        //glDeleteLists(trajlist, 1);

    if(trajlist)
    {
        glCallList(trajlist);
        return;
    }

    trajlist = glGenLists(1);
    glNewList(trajlist, GL_COMPILE_AND_EXECUTE);

    glPushMatrix();

    glEnable(GL_BLEND);
  //glBlendFunc(GL_SRC_ALPHA,GL_ONE);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

  //glDisable(GL_DEPTH_TEST);

    _display();

  //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  //glEnable(GL_DEPTH_TEST);

    glDisable(GL_BLEND);

    glPopMatrix();

    glEndList();
}

void Trajectory::setup_position(double* lon, double* lat)
{
    _lon = lon;
    _lat = lat;
}

void Trajectory::draw_earth_image(float z)
{
    int i, j, n;
    float x, y0, y1;
    float s0, s1, t0, t1;

  //cerr << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">";
  //cerr << "\tline: " << __LINE__;
  //cerr << "\tfile: <" << __FILE__ << ">" << endl;

  //Set texture
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, topography.get_tid(0));

  //Latitude bands
    glColor3f(1.0, 1.0, 1.0);
    glNormal3d(0.0, 0.0, 1.0);
    for(j = 0; j < _ny - 1; ++j)
    {
       n = j * _nx;
       y1 = _yStart + _xyDelt * (j + 1);
       y0 = _yStart + _xyDelt * j;

       glBegin(GL_QUAD_STRIP);
       for(i = 0; i < _nx; ++i)
       {
           x = _xStart + _xyDelt * i;

           s0 = (float) _lon[n + i] / 360.0;
           t0 = (float) _lat[n + i] / 180.0;
           glTexCoord2d(0.5 + s0, 0.5 + t0);
           glVertex3d(x, y0, z);

           s1 = (float) _lon[n + _nx + i] / 360.0;
           t1 = (float) _lat[n + _nx + i] / 180.0;
           glTexCoord2d(0.5 + s1, 0.5 + t1);
           glVertex3d(x, y1, z);
       }
       glEnd();
    }

    glDisable(GL_TEXTURE_2D);
}

void Trajectory::_display()
{

    cerr << "\nEnter functions: <" << __PRETTY_FUNCTION__ << ">";
    cerr << "\tline: " << __LINE__;
    cerr << "\tfile: <" << __FILE__ << ">" << endl;
  //cerr << "\t_nx = " << _nx << ", _ny = " << _ny << ", _nz = " << _nz << endl;

    _colorlen = colorTable->get_clen() - 3;
    _colormap = colorTable->get_cmap();

    glLineWidth(1.5);

    glScalef(1.0, 1.0, 0.5);
    glTranslatef(-0.5, -0.5, -0.5);

  //if(nvoptions->get_cb(NV_TRAJECTORY_ONLY))
  //    draw_earth_image(-0.001);

    initialize();

    _tfac = 0.0;

    while(_tfac < _tspan)
    {
      //cerr << "\t_tfac = " << _tfac << endl;

        _advance();

        _tfac += _dt;
    }

    cerr << "Leave functions: <" << __PRETTY_FUNCTION__ << ">";
    cerr << "\tline: " << __LINE__;
    cerr << "\tfile: <" << __FILE__ << ">" << endl << endl;
}

void Trajectory::_advance()
{
    long n;
    int nk;

    double hws, hwn, hes, hen;
    double dx, dy, dz;
    double hn;

    Position adv_pos;

    for(n = 0; n < _number_of_points; ++n)
    {
        if(position[n].is_outbound())
            continue;

        ip = (int)position[n].x;
        jp = (int)position[n].y;
        kp = (int)position[n].z;

        z_fac = position[n].z - (double) kp;

        nwsb = ip + _nx*(jp + kp*_ny);
        nwnb = nwsb + _nx;

        nwst = nwsb + _nx*_ny;
        nwnt = nwnb + _nx*_ny;

        hws = interpolater->interpt(_tfac, _tspan, _h1[nwsb], _h2[nwsb]);
        hwn = interpolater->interpt(_tfac, _tspan, _h1[nwnb], _h2[nwnb]);
        hes = interpolater->interpt(_tfac, _tspan, _h1[nwsb+1], _h2[nwsb+1]);
        hen = interpolater->interpt(_tfac, _tspan, _h1[nwnb+1], _h2[nwnb+1]);
        h_bot = interpolater->interp2d(position[n].x, position[n].y, hws, hes, hwn, hen);

        hws = interpolater->interpt(_tfac, _tspan, _h1[nwst], _h2[nwst]);
        hwn = interpolater->interpt(_tfac, _tspan, _h1[nwnt], _h2[nwnt]);
        hes = interpolater->interpt(_tfac, _tspan, _h1[nwst+1], _h2[nwst+1]);
        hen = interpolater->interpt(_tfac, _tspan, _h1[nwnt+1], _h2[nwnt+1]);
        h_top = interpolater->interp2d(position[n].x, position[n].y, hws, hes, hwn, hen);

        hc = (1.0 - z_fac) * h_bot + z_fac * h_top;

        dx = dtoverdx * _get_value(n, _u1, _u2);
        dy = dtoverdx * _get_value(n, _v1, _v2);
        dz = _dt * _get_value(n, _w1, _w2);

        adv_pos.x = position[n].x + dx;
        adv_pos.y = position[n].y + dy;
        adv_pos.set_outbound(_outofbound(adv_pos));

        if(adv_pos.is_outbound())
        {
            position[n] = adv_pos;
            continue;
        }

        hn = hc + dz;

        if(hn >= h_top)
        {
            nk = kp + 1;
            if(nk < (_nz - 1))
            {
                nwsb += _nx*_ny;
                nwnb += _nx*_ny;

                hws = interpolater->interpt(_tfac + _dt, _tspan, _h1[nwsb], _h2[nwsb]);
                hwn = interpolater->interpt(_tfac + _dt, _tspan, _h1[nwnb], _h2[nwnb]);
                hes = interpolater->interpt(_tfac + _dt, _tspan, _h1[nwsb+1], _h2[nwsb+1]);
                hen = interpolater->interpt(_tfac + _dt, _tspan, _h1[nwnb+1], _h2[nwnb+1]);
                h_bot = interpolater->interp2d(adv_pos.x, adv_pos.y, hws, hes, hwn, hen);

                nwst += _nx*_ny;
                nwnt += _nx*_ny;

                hws = interpolater->interpt(_tfac + _dt, _tspan, _h1[nwst], _h2[nwst]);
                hwn = interpolater->interpt(_tfac + _dt, _tspan, _h1[nwnt], _h2[nwnt]);
                hes = interpolater->interpt(_tfac + _dt, _tspan, _h1[nwst+1], _h2[nwst+1]);
                hen = interpolater->interpt(_tfac + _dt, _tspan, _h1[nwnt+1], _h2[nwnt+1]);
                h_top = interpolater->interp2d(adv_pos.x, adv_pos.y, hws, hes, hwn, hen);
 
                z_fac = (hn - h_bot) / (h_top - h_bot);
                dz = z_fac + nk - position[n].z;
                adv_pos.z = z_fac + nk;
            }
            else
            {
                adv_pos.set_outbound(true);
            }
        }
        else if(hn < h_bot)
        {
            nk = kp - 1;
            if(0 > nk)
            {
                adv_pos.set_outbound(true);
            }
            else
            {
                nwsb -= _nx*_ny;
                nwnb -= _nx*_ny;

                hws = interpolater->interpt(_tfac + _dt, _tspan, _h1[nwsb], _h2[nwsb]);
                hwn = interpolater->interpt(_tfac + _dt, _tspan, _h1[nwnb], _h2[nwnb]);
                hes = interpolater->interpt(_tfac + _dt, _tspan, _h1[nwsb+1], _h2[nwsb+1]);
                hen = interpolater->interpt(_tfac + _dt, _tspan, _h1[nwnb+1], _h2[nwnb+1]);
                h_bot = interpolater->interp2d(adv_pos.x, adv_pos.y, hws, hes, hwn, hen);
 
                nwst -= _nx*_ny;
                nwnt -= _nx*_ny;

                hws = interpolater->interpt(_tfac + _dt, _tspan, _h1[nwst], _h2[nwst]);
                hwn = interpolater->interpt(_tfac + _dt, _tspan, _h1[nwnt], _h2[nwnt]);
                hes = interpolater->interpt(_tfac + _dt, _tspan, _h1[nwst+1], _h2[nwst+1]);
                hen = interpolater->interpt(_tfac + _dt, _tspan, _h1[nwnt+1], _h2[nwnt+1]);
                h_top = interpolater->interp2d(adv_pos.x, adv_pos.y, hws, hes, hwn, hen);

                z_fac = (hn - h_bot) / (h_top - h_bot);
                dz = z_fac + nk - position[n].z;
                adv_pos.z = z_fac + nk;
            }
        }
        else
        {
            hws = interpolater->interpt(_tfac + _dt, _tspan, _h1[nwsb], _h2[nwsb]);
            hwn = interpolater->interpt(_tfac + _dt, _tspan, _h1[nwnb], _h2[nwnb]);
            hes = interpolater->interpt(_tfac + _dt, _tspan, _h1[nwsb+1], _h2[nwsb+1]);
            hen = interpolater->interpt(_tfac + _dt, _tspan, _h1[nwnb+1], _h2[nwnb+1]);
            h_bot = interpolater->interp2d(adv_pos.x, adv_pos.y, hws, hes, hwn, hen);

            hws = interpolater->interpt(_tfac + _dt, _tspan, _h1[nwst], _h2[nwst]);
            hwn = interpolater->interpt(_tfac + _dt, _tspan, _h1[nwnt], _h2[nwnt]);
            hes = interpolater->interpt(_tfac + _dt, _tspan, _h1[nwst+1], _h2[nwst+1]);
            hen = interpolater->interpt(_tfac + _dt, _tspan, _h1[nwnt+1], _h2[nwnt+1]);
            h_top = interpolater->interp2d(adv_pos.x, adv_pos.y, hws, hes, hwn, hen);

            z_fac = (hn - h_bot) / (h_top - h_bot);
            dz = z_fac + kp - position[n].z;
            adv_pos.z = z_fac + kp;
        }

      //Draw the trajectory.

        if(adv_pos.is_outbound())
        {
            position[n] = adv_pos;
            continue;
        }

      //if((0.5*_dt > _tfac) || (1.5*_dt > (_tspan - _tfac)))
      //    _draw_increment(position[n], adv_pos, true);
      //else
            _draw_increment(position[n], adv_pos, false);

        position[n] = adv_pos;
    }
}

double Trajectory::_get_value(long n, float* var1, float* var2)
{
    double vws, vwn, ves, ven;
    double v_bot, v_top;

    vws = interpolater->interpt(_tfac, _tspan, var1[nwsb], var2[nwsb]);
    vwn = interpolater->interpt(_tfac, _tspan, var1[nwnb], var2[nwnb]);
    ves = interpolater->interpt(_tfac, _tspan, var1[nwsb+1], var2[nwsb+1]);
    ven = interpolater->interpt(_tfac, _tspan, var1[nwnb+1], var2[nwnb+1]);
    v_bot = interpolater->interp2d(position[n].x, position[n].y, vws, ves, vwn, ven);

    vws = interpolater->interpt(_tfac, _tspan, var1[nwst], var2[nwst]);
    vwn = interpolater->interpt(_tfac, _tspan, var1[nwnt], var2[nwnt]);
    ves = interpolater->interpt(_tfac, _tspan, var1[nwst+1], var2[nwst+1]);
    ven = interpolater->interpt(_tfac, _tspan, var1[nwnt+1], var2[nwnt+1]);
    v_top = interpolater->interp2d(position[n].x, position[n].y, vws, ves, vwn, ven);

    return ((1.0 - z_fac) * v_bot + z_fac * v_top);
}

bool Trajectory::_outofbound(Position pos)
{
    if(pos.x < west)
        return true;
    if(pos.x > east)
        return true;

    if(pos.y < south)
        return true;
    if(pos.y > north)
        return true;

    return false;
}

void Trajectory::_draw_increment(Position origin, Position adv_pos, bool draw_cone)
{
    Position increment;
    Position unit_increment;
    Position rotation;
    double angle, length;
    double x, y, z;

  //cerr << "\nEnter functions: <" << __PRETTY_FUNCTION__ << ">";
  //cerr << "\tline: " << __LINE__;
  //cerr << "\tfile: <" << __FILE__ << ">" << endl;

    increment = adv_pos - origin;
    unit_increment = increment.unit();

    glPushMatrix();

  //length = sqrt(increment.x*increment.x + increment.y*increment.y + increment.z*increment.z);
    length = sqrt((_xyDelt * (adv_pos.x - origin.x)) * (_xyDelt * (adv_pos.x - origin.x))
                + (_xyDelt * (adv_pos.y - origin.y)) * (_xyDelt * (adv_pos.y - origin.y))
                + ( _zDelt * (adv_pos.z - origin.z)) * ( _zDelt * (adv_pos.z - origin.z)));

    _set_color(h_bot);
    glColor4fv(color);

    angle = acos(unit_increment.z) * DEG;
  //length *= _xyDelt;
    rotation.init(origin.y, -origin.x, 0.0);
  //rotation = rotation.unit();

    x = _xStart + _xyDelt * origin.x;
    y = _yStart + _xyDelt * origin.y;
    z = _zDelt * origin.z;

  //cerr << "\tlength = " << length << ", angle = " << angle << ", width = " << _width << endl;
  //cerr << "\tx = " << x << ", y = " << y << ", z = " << z << endl;
  //cerr << "\tcylinder.get_instances() = " << cylinder.get_instances() << endl;

    glLineWidth(4.0);

    color[0] = 1.0;
    color[1] = 0.0;
    color[2] = 1.0;
    color[3] = 1.0;

    if(MINIMUM_CYLINDER_HEIGHT >= increment.get_length())
    {
        glBegin(GL_LINES);
            glVertex3f(x, y, z);
            glVertex3f(_xStart + _xyDelt * adv_pos.x,
                       _yStart + _xyDelt * adv_pos.y,
                       _zDelt * adv_pos.z);
        glEnd();
    }
    else
    {
        cylinder.set_color(color);
      //cylinder.set_shape(1.0, 1.0);
        cylinder.set_scale(length, _width);
        cylinder.set_translation(x, y, z);
        cylinder.set_rotation(angle, rotation.x, rotation.y, rotation.z);
        cylinder.draw();

        if(draw_cone)
        {
            color[0] = 1.0;
            color[1] = 1.0;
            color[2] = 0.0;
            color[3] = 1.0;
            cone.set_color(color);
            if(length < _width)
                cone.set_shape(2.0*_width, 3.0 * _width);
            else
                cone.set_shape(length, 3.0 * _width);
            cone.set_scale(_width, _width, length);
            cone.set_translation(x, y, z);
            cone.set_rotation(angle, rotation.x, rotation.y, rotation.z);
            cone.draw();
        }
    }

    glPopMatrix();

  //cerr << "Leave functions: <" << __PRETTY_FUNCTION__ << ">";
  //cerr << "\tline: " << __LINE__;
  //cerr << "\tfile: <" << __FILE__ << ">" << endl << endl;
}

