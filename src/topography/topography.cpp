//$Id: topography.cpp 5315 2015-02-06 21:24:34Z starviewer $

#include <QtOpenGL>
#include <QtGui/QImage>

#include "topography.h"

#define Cos(th) cos(3.1416*(th)/180.0)
#define Sin(th) sin(3.1416*(th)/180.0)

//
//  Constructor
//
Topography::Topography()
{
    int n;

  //_filename = "$NV_DATA/earthimages/NLCD.2km.21600x10800.png";
  //_filename = "/Users/starviewer/work/nv/data/earthimages/land_shallow_topo_350.jpg";
    _filename = "/Users/starviewer/work/nv/data/earthimages/land_shallow_topo_2048.jpg";
  //_filename = "/Users/starviewer/work/nv/data/earthimages/NLCD.2km.21600x10800.png";
  //_filename = "/Users/starviewer/work/nv/data/earthimages/NLCD.250m.21600x21600.A1.png";
  //_filename = "/Users/starviewer/ncl/src/nv/test.png";
  //_filename = "/Users/starviewer/Pictures/tornado_1.jpg";

    for(n = 0; n < 4; ++n)
    {
        _tid[n] = 0;
        _clist[n] = 0;
        _area[n] = 0.0;
    }
    _level = 1;
    _nlon = 1;
    _nlat = 1;
    _cntlon = 0.0;
    _number_tiles = 1;

    _radius = 1.0;
    _deg2arc = 3.1415926535897932 / 180.0;
    _oneover = 1.0 / 180.0;
    _cntlon = 0.0;

    _loadImage();
}

Topography::~Topography()
{
}

void Topography::_setupRegionInfo()
{
    double v;

    _minlon = nvoptions->get_minlon();
    _maxlon = nvoptions->get_maxlon();

    if(_minlon > 180.0)
        _minlon -= 360.0;

    if(_maxlon > 180.0)
    {
        _maxlon -= 360.0;
        if(_minlon == 180.0)
            _minlon = -180.0;
    }

    if((_minlon >= 0.0) && (_maxlon < 0.0))
        _cntlon = 180.0;
    else
    {
        _cntlon = 0.0;

        if(_minlon > _maxlon)
        {
           v = _minlon;
           _minlon = _maxlon;
           _maxlon = v;
        }
    }

    _minlat = nvoptions->get_minlat() - 90.0;
    _maxlat = nvoptions->get_maxlat() - 90.0;

    if(_minlat > _maxlat)
    {
       v = _minlat;
       _minlat = _maxlat;
       _maxlat = v;
    }

    _lonlat2xy(_minlon, _minlat, _xwest, _ysouth);
    _lonlat2xy(_maxlon, _maxlat, _xeast, _ynorth);

    _xcenter = 0.5 * (_xwest + _xeast);
    _ycenter = 0.5 * (_ysouth + _ynorth);

  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\t_minlon = " << _minlon << ", _maxlon = " << _maxlon << endl;
  //cout << "\t_minlat = " << _minlat << ", _maxlat = " << _maxlat << endl;
  //cout << "\t_xwest = " << _xwest << ", _xeast = " << _xeast << endl;
  //cout << "\t_ysouth = " << _ysouth << ", _ysouth = " << _ysouth << endl;
  //cout << "\t_xcenter = " << _xcenter << ", _ycenter = " << _ycenter << endl;
}

void Topography::_checkImage(QImage t)
{
    _level = 1;

    _nlon = 0;
    _nlat = 0;

    _image_width = t.width();
    _image_height = t.height();
    _width = t.width();
    _height = t.height();

    _dlon = 360.0;
    _dlat = 180.0;

    while((2000 < _width) || (2000 < _height))
    {
        _level *= 2;
        _width /= 2;
        _height /= 2;
    }

  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\t_filename: <" << _filename << ">" << endl;
  //cout << "\tt.width() = " << t.width() << ", t.height() = " << t.height() << endl;
  //cout << "\t_width = " << _width << ", _height = " << _height << endl;
  //cout << "\t_level = " << _level << endl;
}

void Topography::_loadImage()
{
    QImage b;
    QImage t;

  //_filename = "$NV_DATA/earthimages/land_shallow_topo_350.jpg";
  //_filename = "/Users/starviewer/work/nv/data/earthimages/land_shallow_topo_350.jpg";
    _filename = "/Users/starviewer/work/nv/data/earthimages/land_shallow_topo_2048.jpg";

    b.load(_filename.c_str());

    t = QGLWidget::convertToGLFormat( b );

    _checkImage(t);

    _level = 1;
    _nlon = 0;
    _nlat = 0;
    _dlon = 360.0;
    _dlat = 180.0;

    glEnable(GL_TEXTURE_2D);

    if(0 < _clist[0])
    {
        if(glIsList(_clist[0]))
            glDeleteLists(_clist[0], 1);
    }

    _clist[0] = 0;

    if(_tid[0])
        glDeleteTextures(1, &_tid[0]);

    glGenTextures(1, &_tid[0]);

  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\t_filename: <" << _filename << ">" << endl;
  //cout << "\tt.width() = " << t.width() << ", t.height() = " << t.height() << endl;
  //cout << "\t_tid[0] = " << _tid[0] << ", _level = " << _level << endl;

    _area[0] = _nlat * _dlat;
    _area[1] = _nlon * _dlon;
    _area[2] = _area[0] + _dlat;
    _area[3] = _area[1] + _dlon;

    glBindTexture(GL_TEXTURE_2D, _tid[0]);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, t.width(), t.height(),
                                0, GL_RGBA, GL_UNSIGNED_BYTE, t.bits());

    glDisable(GL_TEXTURE_2D);
}

void Topography::_Vertex(int th, int ph)
{
    double x = _radius*Cos(th)*Cos(ph);
    double y =         _radius*Sin(ph);
    double z = _radius*Sin(th)*Cos(ph);

    glTexCoord2d(0.5 + th/360.0, 0.5+ph/180.0);

    glNormal3d(x,y,z);
    glVertex3d(x,y,z);
}

void Topography::draw(double r)
{
    double sr = _radius;
    _radius = r;
    draw();
    _radius = sr;
}

void Topography::draw()
{
    int th, ph;

  //Set texture
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, _tid[0]);

    glColor4f(0.10, 0.10, 0.10, 0.10);
    for(ph = -90; ph < 90; ph += 5)
    {
       glBegin(GL_QUAD_STRIP);
       for(th = -180; th <= 180; th += 5)
       {
           _Vertex(th, ph);
           _Vertex(th, ph+5);
       }
       glEnd();
    }

    glDisable(GL_TEXTURE_2D);
}

void Topography::draw_plane(double z)
{
    int i, j;
    double s, t0, t1;
    double x, y0, y1;

    if(_clist[0])
    {
        glCallList(_clist[0]);
        return;
    }

    _loadImage();

    _clist[0] = glGenLists(1);
  //glNewList(_clist[0], GL_COMPILE);
    glNewList(_clist[0], GL_COMPILE_AND_EXECUTE);

  //Set texture
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, _tid[0]);

    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glColor4f(1.00, 1.00, 1.00, 1.00);
    glNormal3d(0.0, 0.0, 1.0);
    for(j = 0; j < 10; ++j)
    {
       t0 = 0.10 * j;
       t1 = t0 + 0.10;
       y0 = t0 - 0.5;
       y1 = t1 - 0.5;

       glBegin(GL_QUAD_STRIP);
       for(i = 0; i <= 10; ++i)
       {
           s = 0.10*i;
           x = 2.0*s - 1.0;

           glTexCoord2d(s, t0);
           glVertex3d(x, y0, z);

           glTexCoord2d(s, t1);
           glVertex3d(x, y1, z);
       }
       glEnd();
    }

    glDisable(GL_TEXTURE_2D);

    glEndList();
}

void Topography::draw_plane_grid(double z)
{
    int i, j;
    double x, y;

    glColor4f(0.90, 0.90, 0.10, 0.50);

    for(j = 0; j <= 18; ++j)
    {
       y = (double)j/18.0 - 0.5;
       glBegin(GL_LINE_STRIP);
       for(i = 0; i <= 36; ++i)
       {
           x = (double)i/18.0 - 1.0;
           glVertex3d(x, y, z);
       }
       glEnd();
    }

    for(i = 0; i <= 36; ++i)
    {
       x = (double)i/18.0 - 1.0;
       glBegin(GL_LINE_STRIP);
       for(j = 0; j <= 18; ++j)
       {
           y = (double)j/18.0 - 0.5;
           glVertex3d(x, y, z);
       }
       glEnd();
    }
}

void Topography::_lonlat2xy(double lon, double lat, double &x, double &y)
{
    x = (lon - _cntlon) * _oneover;

    if(-1.0 > x)
        x += 2.0;

    if(1.0 < x)
        x -= 2.0;

    y = lat * _oneover;
}

void Topography::drawAregion(double z)
{
    int i;

    if(nvoptions->get_cb(NV_RESET))
    {
        for(i = 0; i < _number_tiles; ++i)
        {
            if(0 < _clist[i])
            {
                if(glIsList(_clist[i]))
                    glDeleteLists(_clist[i], 1);
            }
    
            _clist[i] = 0;
        }

        nvoptions->set_cb(NV_RESET, false);
    }

    if((_minlon >= 0.0) && ( _maxlon < 0.0))
    {
        if(_clist[0] && _clist[1])
        {
            glCallList(_clist[0]);
            glCallList(_clist[1]);
        }
        else
        {
            _setupRegionInfo();

          //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
          //cout << "\t_minlon = " << _minlon << ", _maxlon = " << _maxlon << endl;
          //cout << "\t_minlat = " << _minlat << ", _maxlat = " << _maxlat << endl;

            _loadAregion(_minlon, _maxlon, _minlat, _maxlat, 2);

            glPushMatrix();
            _drawAregion(_minlon,  180.0, _minlat, _maxlat, z, 0);
            _drawAregion(-180.0, _maxlon, _minlat, _maxlat, z, 1);
            glPopMatrix();
        }
    }
    else
    {
        if(_clist[0])
        {
            glCallList(_clist[0]);
        }
        else
        {
            _setupRegionInfo();

          //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
          //cout << "\t_minlon = " << _minlon << ", _maxlon = " << _maxlon << endl;
          //cout << "\t_minlat = " << _minlat << ", _maxlat = " << _maxlat << endl;

            _loadAregion(_minlon, _maxlon, _minlat, _maxlat, 1);

            glPushMatrix();
            _drawAregion(_minlon, _maxlon, _minlat, _maxlat, z, 0);
            glPopMatrix();
        }
    }
}

void Topography::_generateTexture(double slon, double elon, double slat, double elat,
                                  int ntile, unsigned char* bits)
{
    unsigned char* newbits;
    int i, j, m, n;
    int istart, jstart;

    _dlon = elon - slon;
    _dlat = elat - slat;
    _width  = (int) ((double) _image_width  * _dlon / 360.0);
    _height = (int) ((double) _image_height * _dlat / 180.0);

    istart = (int) ((double) _image_width * (0.5 + slon / 360.0));
    jstart = (int) ((double) _image_height * (0.5 + slat / 180.0));

    glEnable(GL_TEXTURE_2D);

    if(_tid[ntile])
        glDeleteTextures(1, &_tid[ntile]);

    _tid[ntile] = 0;

    glGenTextures(1, &_tid[ntile]);

  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\tfunction: " << __PRETTY_FUNCTION__ << endl;
  //cout << "\tslon = " << slon << ", elon = " << elon << endl;
  //cout << "\tslat = " << slat << ", elat = " << elat << endl;
  //cout << "\t_image_width = " << _image_width << ", _image_height = " << _image_height << endl;
  //cout << "\t_width = " << _width << ", _height = " << _height << endl;
  //cout << "\tistart = " << istart << ", jstart = " << jstart << endl;
  //cout << "\t_tid[" << ntile << "] = " << _tid[ntile] << ", _level = " << _level << endl;

    newbits = (unsigned char*) malloc(4*_width*_height*sizeof(unsigned char));
    assert(newbits);

    for(j = 0; j < _height; ++j)
    {
        m = j * _width;
        n = (j + jstart)*_image_width + istart;

        for(i = 0; i < _width; ++i)
        {
            newbits[4*(m+i)]   = bits[4*(n+i)];
            newbits[4*(m+i)+1] = bits[4*(n+i)+1];
            newbits[4*(m+i)+2] = bits[4*(n+i)+2];
            newbits[4*(m+i)+3] = bits[4*(n+i)+3];
        }
    }

    glBindTexture(GL_TEXTURE_2D, _tid[ntile]);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, _width, _height,
                                0, GL_RGBA, GL_UNSIGNED_BYTE, newbits);

    free(newbits);

    glDisable(GL_TEXTURE_2D);
}

void Topography::_loadAregion(double slon, double elon, double slat, double elat, int n)
{
    QImage b;
    QImage t;

    unsigned char* bits;

  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\tslon = " << slon << ", elon = " << elon << endl;
  //cout << "\tslat = " << slat << ", elat = " << elat << endl;
  //cout << "\tn = " << n << endl;

  //_filename = "$NV_DATA/earthimages/land_shallow_topo_2048.jpg";
    _filename = "/Users/starviewer/work/nv/data/earthimages/land_shallow_topo_2048.jpg";

    b.load(_filename.c_str());

    t = QGLWidget::convertToGLFormat( b );

    _checkImage(t);

    bits = t.bits();

    switch(n)
    {
        case 1:
             _generateTexture(slon, elon, slat, elat, 0, bits);
             break;
        case 2:
             _generateTexture(slon,  180.0, slat, elat, 0, bits);
             _generateTexture(-180.0, elon, slat, elat, 1, bits);
             break;
        default:
             cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
             cout << "\tn = " << n << " is not a valid option for <" << __PRETTY_FUNCTION__ << ">" << endl;
    }
}

void Topography::_drawAregion(double slon, double elon,
                              double slat, double elat,
                              double z, int ntile)
{
    int i, j;
    double s, t0, t1;
    double x, y0, y1;
    double lon, lat;

    _dlon = elon - slon;
    _dlat = elat - slat;

    _clist[ntile] = glGenLists(1);
  //glNewList(_clist[ntile], GL_COMPILE);
    glNewList(_clist[ntile], GL_COMPILE_AND_EXECUTE);

  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\tfunction: " << __PRETTY_FUNCTION__ << endl;
  //cout << "\tntile = " << ntile << endl;
  //cout << "\t_clist[" << ntile << "] = " << _clist[ntile] << endl;
  //cout << "\t_tid[" << ntile << "] = " << _tid[ntile] << endl;
  //cout << "\tslon = " << slon << ", elon = " << elon << endl;
  //cout << "\tslat = " << slat << ", elat = " << elat << endl;
  //cout << "\t_dlon = " << _dlon << ", _dlat = " << _dlat << endl;

    glTranslated(-_xcenter, -_ycenter, 0.0);

  //Set texture
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, _tid[ntile]);

    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glColor4f(1.00, 1.00, 1.00, 1.00);

    glNormal3d(0.0, 0.0, 1.0);
    for(j = 0; j < 10; ++j)
    {
       t0 = 0.10 * j;
       t1 = t0 + 0.10;

       lon = slon;
       lat = slat + 0.1 * j * _dlat;
       _lonlat2xy(lon, lat, x, y0);
       lat += 0.1 * _dlat;

     //cout << "\tj = " << j << ", t0 = " << t0 << ", t1 = " << t1 << endl;

       glBegin(GL_QUAD_STRIP);
       for(i = 0; i <= 10; ++i)
       {
           s = 0.10 * i;
           lon = slon + 0.1 * i * _dlon;
           _lonlat2xy(lon, lat, x, y1);

         //if(! (i%10))
         //  //cout << "\t\ti = " << i << ", s = " << s << ", x = " << x << ", y0 = " << y0 << ", y1 = " << y1 << endl;

           glTexCoord2d(s, t0);
           glVertex3d(x, y0, z);

           glTexCoord2d(s, t1);
           glVertex3d(x, y1, z);
       }
       glEnd();
    }

    glDisable(GL_TEXTURE_2D);

    glEndList();
}

