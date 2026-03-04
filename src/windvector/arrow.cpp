//$Id: arrow.cpp 4913 2014-01-09 21:20:34Z starviewer $

#include <QtOpenGL>

#include "arrow.h"

Arrow::Arrow(ColorTable *ct)
{
    colorTable = ct;

    _noc = NUMBEROFCYLINDERS;
    cone = new Cone();
    cylinder = new Cylinder[_noc];

  //_sh = 0.1;
  //_sv = 0.1;
  //_sh = 0.001;
  //_sv = 0.01;
    _sh = 0.005;
    _sv = 0.02;
    _sw = 0.01;
    _maxspeed = 50.0;

    _colorlen = colorTable->get_clen() - 3;
    _colormap = colorTable->get_cmap();

    if(2 > _noc)
        _noc = 2;
}

Arrow::~Arrow()
{
    delete cone;
    delete [] cylinder;
}

void Arrow::setup(float x, float y, float z,
                  float u, float v, float w)
{
    _u = u;
    _v = v;
    _w = w;
    _speed = sqrt(u*u + v*v + w*w);

    _dx = _sh * u;
    _dy = _sh * v;
    _dz = _sv * w;

    _length = sqrt(_dx*_dx + _dy*_dy + _dz*_dz);
    _width = _sw * _length;

#if 0
    _x1 = x;
    _y1 = y;
    _z1 = z;

    _x2 = x + _dx;
    _y2 = y + _dy;
    _z2 = z + _dz;
#else
    _x2 = x;
    _y2 = y;
    _z2 = z;

    _x1 = x - _dx;
    _y1 = y - _dy;
    _z1 = z - _dz;
#endif

  //cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">";
  //cout << "\tline: " << __LINE__;
  //cout << "\tfile: <" << __FILE__ << ">" << endl;
  //cout << "\tu = " << u << ", v = " << v << ", w = " << w << endl;
  //cout << "\tx = " << x << ", y = " << y << ", z = " << z << endl;
  //cout << "\tdx = " << _dx << ", dy = " << _dy << ", _dz = " << _dz << endl;
  //cout << "\tspeed = " << _speed << ", _length = " << _length << ", _width = " << _width << endl;
}

void Arrow::_set_color(double spd)
{
    int idx = (int) (((float)(spd * _colorlen)) / _maxspeed);
 
    if(idx > (_colorlen - 1))
       idx = _colorlen - 1;

    color[0]  = _colormap[3*(idx+3)];
    color[1]  = _colormap[3*(idx+3) + 1];
    color[2]  = _colormap[3*(idx+3) + 2];
    color[3]  = 1.0;
}

void Arrow::draw()
{
    int n;

  //cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">";
  //cout << "\tline: " << __LINE__;
  //cout << "\tfile: <" << __FILE__ << ">" << endl;

    glPushMatrix();

    _set_color(_speed);
  //glColor4fv(color);

    if(0.01 > _speed)
    {
      //draw a dot
        glBegin(GL_POINTS);
            glVertex3f(_x1, _y1, _z1);
        glEnd();
    }
    else if(0.1 > _speed)
    {
      //draw a line
        glBegin(GL_LINE_STRIP);
            glVertex3f(_x1, _y1, _z1);
            glVertex3f(_x2, _y2, _z2);
        glEnd();
    }
    else
    {
        double hs = 1.0;

      //Normalize vector.
      //nv = get_normalized_vector(vector_struct &vec);
      //Our case is simple:
      //_dx = _u / _speed;
      //_dy = _v / _speed;
      //_dz = _w / _speed;

      //Get the dot product:
      //double dotproduct(vector_struct v1, vector_struct v2)
      //Our case is simple, as v1 = (0, 0, 1), v2 = (u, v, w):

        _angle = acos(_w / _speed) * DEG;

      //Get the cross vector, or the normal vector:
      //nv = cross(vector_struct v1, vector_struct v2);
      //Our case is simple, as v1 = (0, 0, 1), v2 = (_u, _v, _w):
        hs = sqrt(_u*_u + _v*_v);
        if(0.01 > hs)
        {
           _rx = 0.0;
           _ry = 1.0;
           _rz = 0.0;

           if(0.0 < _w)
              _angle = 0.0;
           else
              _angle = 180.0;
        }
        else
        {
           _rx = -_v / hs;
           _ry = _u / hs;
           _rz = 0.0;
        }

        cone->set_color(color);

        if(0.5 > _speed)
        {
          //draw a cone
            cone->set_shape(_length, 2.0 * _width);
            cone->set_sides(36);
            cone->set_translation(_x1, _y1, _z1);
            cone->set_rotation(_angle, _rx, _ry, _rz);
            cone->draw();
        }
        else
        {
            double f = 1.0 / (2.0 + _noc);
            double h = f * _length;
            double w = (2.0 + _noc) * _width;
            double r1 = 0.0;
            double r2 = r1 + _width;
            double s = -f;

          //draw a big cone, and _noc of cylinders.
            for(n = 0; n < _noc; ++n)
            {
                r1 = r2;
                r2 += (0.5 * _width);
                cylinder[n].set_color(color);
                cylinder[n].set_shape(h, r1, r2);
                cylinder[n].set_sides(36);
                s += f;
                cylinder[n].set_translation(_x1 + s*_dx,
                                            _y1 + s*_dy,
                                            _z1 + s*_dz);
                cylinder[n].set_rotation(_angle, _rx, _ry, _rz);
                cylinder[n].draw();
            }

            s += f;
            cone->set_shape(2.0*h, w);
            cone->set_sides(36);
            cone->set_translation(_x1 + s*_dx,
                                  _y1 + s*_dy,
                                  _z1 + s*_dz);
            cone->set_rotation(_angle, _rx, _ry, _rz);
            cone->draw();
        }
    }

    glPopMatrix();
}

vector_struct get_normalized_vector(vector_struct &vec)
{
    vector_struct nv;
    nv.length = sqrt(vec.x*vec.x + vec.y*vec.y + vec.z*vec.z);
    vec.length = nv.length;

    if(1.0e-10 > nv.length)
    {
        nv.x = 0.0;
        nv.y = 0.0;
        nv.z = 1.0;
        nv.length = 1.0e-10;
    }
    else
    {
        nv.x = vec.x / nv.length;
        nv.y = vec.y / nv.length;
        nv.z = vec.z / nv.length;
    }

    return nv;
}

vector_struct cross(vector_struct v1, vector_struct v2)
{
    vector_struct nv;

    nv.length = 1.0;
    nv.x = v1.y * v2.z - v2.y * v1.z;
    nv.y = v1.z * v2.x - v2.z * v1.x;
    nv.z = v1.x * v2.y - v2.x * v1.y;

    return nv;
}

double dotproduct(vector_struct v1, vector_struct v2)
{
    return (v1.x*v2.x + v1.y*v2.y + v1.z*v2.z);
}

