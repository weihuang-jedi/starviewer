//$Id: ncl_viewer.cpp 4927 2014-01-23 21:49:37Z starviewer $

#include <QtOpenGL>

#include "ncl_viewer.h"

NCL_Viewer::NCL_Viewer(ColorTable *ct, NVOptions* opt)
{
    colorTable = ct;
    nvoptions = opt;

    _var = NULL;

    contour = NULL;
  //earth = new Earth();

    _time = 0;

    _first_time = true;
}

NCL_Viewer::~NCL_Viewer()
{
}

void NCL_Viewer::draw()
{
    if(nvoptions->get_cb(NV_RESET))
    {
        nvoptions->set_cb(NV_RESET, false);
        contour->reinitialize();
    }

    glPushMatrix();

    if(nvoptions->get_cb(NV_SPHEREON))
    {
        _display_on_sphere();
    }
    else if(nvoptions->get_cb(NV_BUMPON))
    {
        _display_with_bump();
    }
    else if(nvoptions->get_cb(NV_ONHEIGHTSURFACE))
    {
        _display_on_height_surface();
    }
    else
    {
        _display_all();
    }

    glPopMatrix();
}

void NCL_Viewer::_display_all()
{
    glScalef(1.0, 1.0, 0.4);
    if(geometry->get_nz() > 1)
    {
        glTranslatef(-0.5, -0.5, -0.5);
    }
    else
    {
        glTranslatef(-0.5, -0.5, 0.0);
    }

    if((geometry->get_nx() > nvoptions->get_xsec()) && (geometry->get_nz() > 1))
    {
        _display_Xplane(nvoptions->get_xsec());
    }

    if(nvoptions->get_cb(NV_HASX2) && (geometry->get_nx() > nvoptions->get_xsec2()) && (geometry->get_nz() > 1))
    {
        _display_Xplane(nvoptions->get_xsec2());
    }

    if((geometry->get_ny() > nvoptions->get_ysec()) && (geometry->get_nz() > 1))
    {
        _display_Yplane(nvoptions->get_ysec());
    }

    if(nvoptions->get_cb(NV_HASY2) && (geometry->get_ny() > nvoptions->get_ysec2()) && (geometry->get_nz() > 1))
    {
        _display_Yplane(nvoptions->get_ysec2());
    }

  //cout << "\tAt Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__
  //     << ", file: <" << __FILE__ << ">" << endl << endl;
  //cout << "\tgeometry->get_nz() = " << geometry->get_nz();
  //cout << "\tnvoptions->get_zsec() = " << nvoptions->get_zsec() << endl;

    if(geometry->get_nz() > nvoptions->get_zsec())
    {
        _display_Zplane(nvoptions->get_zsec());
    }

    if(nvoptions->get_cb(NV_HASZ2) && (geometry->get_nz() > nvoptions->get_zsec2()) && (geometry->get_nz() > 1))
    {
        _display_Zplane(nvoptions->get_zsec2());
    }
}

void NCL_Viewer::_display_Xplane(int xs)
{
    double s1, t1, s2, t2;
    double x1, y1, z1, y2, z2;

    double scl = 1.0;
  //int nxs = xs + geometry->get_nx();
    unsigned int tid;

  //if(nxs >= geometry->get_nx())
  //    nxs -= geometry->get_nx();

  //tid = contour->get_xid(nxs);
    tid = contour->get_xid(xs);

    scl = (contour->get_xEnd() - contour->get_xStart()) / geometry->get_nx();
    x1 = contour->get_xStart() + xs * scl;

  //cout << "Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__
  //     << ", file: <" << __FILE__ << ">" << endl;
  //cout << "\txStart() = " << contour->get_xStart()
  //     << ", xEnd() = " << contour->get_xEnd() << endl;
  //cout << "\tyStart() = " << contour->get_yStart()
  //     << ", yEnd() = " << contour->get_yEnd() << endl;
  //cout << "\tyStartX() = " << contour->get_yStartX()
  //     << ", yEndX() = " << contour->get_yEndX() << endl;
  //cout << "\tzStartX() = " << contour->get_zStartX()
  //     << ", zEndX() = " << contour->get_zEndX() << endl;

  //glNormal3d(0.0, 0.0, -1.0);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tid);

    glBegin(GL_QUADS);
      //South side
        y1 = 0.0;
        z1 = contour->get_zStartX();
        y2 = contour->get_yStart();
        z2 = contour->get_zEndX();
        s1 = 0.0;
        t1 = contour->get_zStartX();
        s2 = contour->get_yStartX();
        t2 = contour->get_zEndX();
        _xQuad(x1, y1, z1, y2, z2, s1, t1, s2, t2);

      //North side (No need to display for now).
      //y1 = contour->get_yEnd();
      //z1 = 0.0;
      //y2 = 1.0;
      //z2 = 1.0;
      //s1 = contour->get_yEndX();
      //t1 = 0.0;
      //s2 = 1.0;
      //t2 = 1.0;
      //_xQuad(x1, y1, z1, y2, z2, s1, t1, s2, t2);

      //Bottom
        y1 = 0.0;
        z1 = 0.0;
        y2 = 1.0;
        z2 = contour->get_zStartX();
        s1 = 0.0;
        t1 = 0.0;
        s2 = 1.0;
        t2 = contour->get_zStartX();
        _xQuad(x1, y1, z1, y2, z2, s1, t1, s2, t2);

      //Top (No need for now)
      //y1 = contour->get_yStart();
      //z1 = contour->get_yEnd();
      //y2 = 1.0;
      //z2 = 1.0;
      //s1 = contour->get_yStartX();
      //t1 = contour->get_zStartX();
      //s2 = contour->get_yEndX();
      //t2 = 1.0;
        _xQuad(x1, y1, z1, y2, z2, s1, t1, s2, t2);

      //Center
        y1 = contour->get_yStart();
        z1 = contour->get_zStartX();
        y2 = contour->get_yEnd();
        z2 = contour->get_zEndX();
        s1 = contour->get_yStartX();
        t1 = contour->get_zStartX();
        s2 = contour->get_yEndX();
        t2 = contour->get_zEndX();
        _xQuad(x1, y1, z1, y2, z2, s1, t1, s2, t2);
    glEnd();

    glDisable(GL_TEXTURE_2D);
}

void NCL_Viewer::_display_Yplane(int ys)
{
    double s1, t1, s2, t2;
    double x1, y1, z1, x2, z2;

    double scl = 1.0;

    unsigned int tid;

    tid = contour->get_yid(ys);

    scl = (contour->get_yEnd() - contour->get_yStart()) / geometry->get_ny();
    y1 = contour->get_yStart() + ys * scl;

  //cout << "Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__
  //     << ", file: <" << __FILE__ << ">" << endl;
  //cout << "\txStart() = " << contour->get_xStart() 
  //     << ", xEnd() = " << contour->get_xEnd() << endl;
  //cout << "\tyStart() = " << contour->get_yStart()
  //     << ", yEnd() = " << contour->get_yEnd() << endl;
  //cout << "\txStartY() = " << contour->get_xStartY()
  //     << ", xEndY() = " << contour->get_xEndY() << endl;
  //cout << "\tzStaYtY() = " << contour->get_zStartY()
  //     << ", zEndY() = " << contour->get_zEndY() << endl;

  //glNormal3d(0.0, 0.0, -1.0);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tid);

    glBegin(GL_QUADS);
      //West side
        x1 = 0.0;
        z1 = contour->get_zStartY();
        x2 = contour->get_xStart();
        z2 = contour->get_zEndY();
        s1 = 0.0;
        t1 = contour->get_zStartY();
        s2 = contour->get_xStartY();
        t2 = contour->get_zEndY();
        _yQuad(x1, y1, z1, x2, z2, s1, t1, s2, t2);

      //East side (No need to display for now).
      //x1 = contour->get_xEnd();
      //z1 = 0.0;
      //x2 = 1.0;
      //z2 = 1.0;
      //s1 = contour->get_xEndY();
      //t1 = 0.0;
      //s2 = 1.0;
      //t2 = 1.0;
      //_yQuad(x1, y1, z1, x2, z2, s1, t1, s2, t2);

      //Bottom
        x1 = 0.0;
        z1 = 0.0;
        x2 = 1.0;
        z2 = contour->get_zStartY();
        s1 = 0.0;
        t1 = 0.0;
        s2 = 1.0;
        t2 = contour->get_zStartY();
        _yQuad(x1, y1, z1, x2, z2, s1, t1, s2, t2);

      //Top (No need for now)
      //x1 = contour->get_xStart();
      //z1 = contour->get_zEndY();
      //x2 = 1.0;
      //z2 = 1.0;
      //s1 = contour->get_xStartY();
      //t1 = contour->get_zEndY();
      //s2 = 1.0;
      //t2 = 1.0;
      //_yQuad(x1, y1, z1, x2, z2, s1, t1, s2, t2);

      //Center
        x1 = contour->get_xStart();
        z1 = contour->get_zStartY();
        x2 = contour->get_xEnd();
        z2 = contour->get_zEndY();
        s1 = contour->get_xStartY();
        t1 = contour->get_zStartY();
        s2 = contour->get_xEndY();
        t2 = contour->get_zEndY();
        _yQuad(x1, y1, z1, x2, z2, s1, t1, s2, t2);
    glEnd();

    glDisable(GL_TEXTURE_2D);
}

void NCL_Viewer::_display_Zplane(int zs)
{
    double s1, t1, s2, t2;
    double x1, y1, z1, x2, y2;

    double scl = 1.0;

    unsigned int tid;

    tid = contour->get_zid(zs);

    scl = (contour->get_zEndX() - contour->get_zStartX()) / geometry->get_nz();
    z1 = contour->get_zStartX() + zs * scl;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tid);

  //glNormal3d(x,y,z);

    glBegin(GL_QUADS);
      //South side
        x1 = 0.0;
        y1 = 0.0;
        x2 = 1.0;
        y2 = contour->get_yStart();
        s1 = 0.0;
        t1 = 0.0;
        s2 = 1.0;
        t2 = contour->get_yStart();
        _zQuad(x1, y1, x2, y2, z1, s1, t1, s2, t2);

      //North side
        x1 = 0.0;
        y1 = contour->get_yEnd();
        x2 = 1.0;
        y2 = 1.0;
        s1 = 0.0;
        t1 = contour->get_yEnd();
        s2 = 1.0;
        t2 = 1.0;
        _zQuad(x1, y1, x2, y2, z1, s1, t1, s2, t2);

      //West side
        x1 = 0.0;
        y1 = contour->get_yStart();
        x2 = contour->get_xStart();
        y2 = contour->get_yEnd();
        s1 = 0.0;
        t1 = contour->get_yStart();
        s2 = contour->get_xStart();
        t2 = contour->get_yEnd();
        _zQuad(x1, y1, x2, y2, z1, s1, t1, s2, t2);

      //East side (No need to display for now).
      //x1 = contour->get_xEnd();
      //y1 = contour->get_yStart();
      //x2 = 1.0;
      //y2 = contour->get_yEnd();
      //s1 = contour->get_xEnd();
      //t1 = contour->get_yStart();
      //s2 = 1.0;
      //t2 = contour->get_yEnd();
      //_zQuad(x1, y1, x2, y2, z1, s1, t1, s2, t2);

      //Center
        x1 = contour->get_xStart();
        y1 = contour->get_yStart();
        x2 = contour->get_xEnd();
        y2 = contour->get_yEnd();
        s1 = contour->get_xStart();
        t1 = contour->get_yStart();
        s2 = contour->get_xEnd();
        t2 = contour->get_yEnd();
        _zQuad(x1, y1, x2, y2, z1, s1, t1, s2, t2);
    glEnd();

    glDisable(GL_TEXTURE_2D);
}

void NCL_Viewer::_display_with_bump()
{
    glScalef(1.0, 1.0, 0.5);

    if(geometry->get_nz() > 1)
    {
        glTranslatef(-0.5, -0.5, -0.5);
    }
    else
    {
        glTranslatef(-0.5, -0.5, 0.0);
    }

    if((geometry->get_nx() > nvoptions->get_xsec()) && (geometry->get_nz() > 1))
    {
        _display_Xplane_with_bump(nvoptions->get_xsec());
    }

    if((geometry->get_ny() > nvoptions->get_ysec()) && (geometry->get_nz() > 1))
    {
        _display_Yplane_with_bump(nvoptions->get_ysec());
    }

    if(geometry->get_nz() > nvoptions->get_zsec())
    {
        _display_Zplane_with_bump(nvoptions->get_zsec());
    }
}

void NCL_Viewer::_display_Xplane_with_bump(int xs)
{
    int   j, k, m;
    double bs, bf;
    double delty, deltz;
    double delty2;
    double sx, sy, sz;

    float minval;
    float *pltvar = NULL;

    double s1, t1, s2, t2;
    double x1, y1, z1, x2, y2, z2;

    int _nx = geometry->get_nx();
    int _ny = geometry->get_ny();
    int _nz = geometry->get_nz();

    unsigned int tid;

    minval = contour->get_min() - 1.0e-20;
    bs = 1.0/(contour->get_max() - minval);
    bf = -0.5;

    pltvar = new float[_nz * _ny];

    for(k = 0; k < _nz; ++k)
    {
        for(j = 0; j < _ny; ++j)
        {
            m = xs + (j + k * _ny) * _nx;
            pltvar[j + k * _ny] = _var[m];
        }
    }

    sy = 1.0/(geometry->get_ny() - 1.0);
    sz = 1.0/(geometry->get_nz() - 1.0);

    delty = sy * (contour->get_yEnd()  - contour->get_yStart());
    delty2= sy * (contour->get_yEndX() - contour->get_yStartX());
    deltz = sz * (contour->get_zEndX() - contour->get_zStartX());

    sx = (contour->get_xEnd() - contour->get_xStart()) / (geometry->get_nx() - 1);
    x1 = contour->get_xStart() + xs * sx;

    sx = 0.2;

    tid = contour->get_xid(xs);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tid);

  //glNormal3d(x,y,z);

    glBegin(GL_QUADS);
      //South side
        y1 = 0.0;
        z1 = 0.0;
        y2 = contour->get_yStart();
        z2 = contour->get_yEnd();
        s1 = 0.0;
        t1 = 0.0;
        s2 = contour->get_yStartX();
        t2 = contour->get_zEndX();
        _xQuad(x1, y1, z1, y2, z2, s1, t1, s2, t2);

      //North side (No need to display for now).
      //y1 = contour->get_yEnd();
      //z1 = 0.0;
      //y2 = 1.0;
      //z2 = 1.0;
      //s1 = contour->get_yEndX();
      //t1 = 0.0;
      //s2 = 1.0;
      //t2 = 1.0;
      //_xQuad(x1, y1, z1, y2, z2, s1, t1, s2, t2);

      //Bottom
        y1 = 0.0;
        z1 = 0.0;
        y2 = 1.0;
        z2 = contour->get_zStartX();
        s1 = 0.0;
        t1 = 0.0;
        s2 = 1.0;
        t2 = contour->get_zStartX();
        _xQuad(x1, y1, z1, y2, z2, s1, t1, s2, t2);

      //Top (No need for now)
      //y1 = contour->get_yStart();
      //z1 = contour->get_yEnd();
      //y2 = 1.0;
      //z2 = 1.0;
      //s1 = contour->get_yStartX();
      //t1 = contour->get_zStartX();
      //s2 = contour->get_yEndX();
      //t2 = 1.0;
        _xQuad(x1, y1, z1, y2, z2, s1, t1, s2, t2);

      //Center
      //y1 = contour->get_yStart();
      //z1 = contour->get_zStartX();
      //y2 = contour->get_yEnd();
      //z2 = contour->get_zEndX();
      //s1 = contour->get_yStartX();
      //t1 = contour->get_zStartX();
      //s2 = contour->get_yEndX();
      //t2 = contour->get_zEndX();
      //_xQuad(x1, y1, z1, y2, z2, s1, t1, s2, t2);
    glEnd();

    for(k = 0; k < geometry->get_nz() - 1; ++k)
    {
        z1 = contour->get_zStartX() + k * deltz;
        z2 = z1 + deltz;

        glBegin(GL_QUAD_STRIP);
            for(j = 0; j < geometry->get_ny(); ++j)
            {
                y1 = contour->get_yStart() + j * delty;
                x2 = x1 + sx * (bs * (pltvar[j + k * _ny] - minval) + bf);
                s1 = contour->get_yStartX() + j * delty2;
                glTexCoord2d(s1, 1.0 - z1);
                glVertex3d(x2,y1,z1);

                x2 = x1 + sx * (bs * (pltvar[j + (k + 1) * _ny] - minval) + bf);
                glTexCoord2d(s1, 1.0 - z2);
                glVertex3d(x2,y1,z2);
            }
        glEnd();
    }

    glDisable(GL_TEXTURE_2D);

    delete [] pltvar;
}

void NCL_Viewer::_display_Yplane_with_bump(int ys)
{
    int   i, k, m;
    double bs, bf;
    double deltx, deltz;
    double deltx2;
    double sx, sy, sz;

    float minval;
    float *pltvar = NULL;

    double s1, t1, s2, t2;
    double x1, y1, z1, x2, y2, z2;

    int _nx = geometry->get_nx();
    int _ny = geometry->get_ny();
    int _nz = geometry->get_nz();

    unsigned int tid;

    minval = contour->get_min() - 1.0e-20;
    bs = 1.0/(contour->get_max() - minval);
    bf = -0.5;

    pltvar = new float[geometry->get_nz() * geometry->get_nx()];

    for(k = 0; k < _nz; ++k)
    {
        for(i = 0; i < _nx; ++i)
        {
            m = i + (ys + k * _ny) * _nx;
            pltvar[i + k * _nx] = _var[m];
        }
    }

    sx = 1.0/(geometry->get_nx() - 1.0);
    sz = 1.0/(geometry->get_nz() - 1.0);

    deltx = sx * (contour->get_xEnd()  - contour->get_xStart());
    deltx2= sx * (contour->get_xEndY() - contour->get_xStartY());
    deltz = sz * (contour->get_zEndY() - contour->get_zStartY());

    sy = (contour->get_yEnd() - contour->get_yStart()) / (geometry->get_ny() - 1);
    y1 = contour->get_yStart() + ys * sy;

    sy = 0.2;

    tid = contour->get_yid(ys);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tid);

  //glNormal3d(x,y,z);

    glBegin(GL_QUADS);
      //South side
      //x1 = 0.0;
      //z1 = 0.0;
      //x2 = contour->get_xStart();
      //z2 = 1.0;
      //s1 = 0.0;
      //t1 = 0.0;
      //s2 = contour->get_xStartY();
      //t2 = 1.0;
      //_yQuad(x1, y1, z1, x2, z2, s1, t1, s2, t2);

      //North side (No need to display for now).
      //x1 = contour->get_xEnd();
      //z1 = 0.0;
      //x2 = 1.0;
      //z2 = 1.0;
      //s1 = contour->get_xEndY();
      //t1 = 0.0;
      //s2 = 1.0;
      //t2 = 1.0;
      //_yQuad(x1, y1, z1, x2, z2, s1, t1, s2, t2);

      //Bottom
        x1 = 0.0;
        z1 = 0.0;
        x2 = 1.0;
        z2 = contour->get_zStartY();
        s1 = 0.0;
        t1 = 0.0;
        s2 = 1.0;
        t2 = contour->get_zStartY();
        _yQuad(x1, y1, z1, x2, z2, s1, t1, s2, t2);

      //Top (No need for now)
      //x1 = contour->get_xStart();
      //z1 = contour->get_zEndY();
      //x2 = 1.0;
      //z2 = 1.0;
      //s1 = contour->get_xStartY();
      //t1 = contour->get_zEndY();
      //s2 = 1.0;
      //t2 = 1.0;
      //_yQuad(x1, y1, z1, x2, z2, s1, t1, s2, t2);

      //Center
      //x1 = contour->get_xStart();
      //z1 = contour->get_zStartY();
      //x2 = contour->get_xEnd();
      //z2 = contour->get_zEndY();
      //s1 = contour->get_xStartY();
      //t1 = contour->get_zStartY();
      //s2 = contour->get_xEndY();
      //t2 = contour->get_zEndY();
      //_yQuad(x1, y1, z1, x2, z2, s1, t1, s2, t2);
    glEnd();

    for(k = 0; k < geometry->get_nz() - 1; ++k)
    {
        z1 = contour->get_zStartX() + k * deltz;
        z2 = z1 + deltz;

        glBegin(GL_QUAD_STRIP);
            for(i = 0; i < geometry->get_nx(); ++i)
            {
                x1 = contour->get_xStart() + i * deltx;
                y2 = y1 + sy * (bs * (pltvar[i + k * _nx] - minval) + bf);
                s1 = contour->get_xStartY() + i * deltx2;
                glTexCoord2d(s1, 1.0 - z1);
                glVertex3d(x1,y2,z1);

                y2 = y1 + sy * (bs * (pltvar[i + (k + 1) * _nx] - minval) + bf);
                glTexCoord2d(s1, 1.0 - z2);
                glVertex3d(x1,y2,z2);
            }
        glEnd();
    }

    glDisable(GL_TEXTURE_2D);

    delete [] pltvar;
}

void NCL_Viewer::_display_Zplane_with_bump(int zs)
{
    int   i, j, n;
    double bs, bf;
    double deltx, delty;
    double sx, sy, sz;

    float minval;
    float *pltvar = NULL;

    double s1, t1, s2, t2;
    double x1, y1, z1, x2, y2, z2;

    unsigned int tid;

    minval = contour->get_min() - 1.0e-20;
    bs = 1.0/(contour->get_max() - minval);

  //if(geometry->get_nz() > 1)
        bf = -0.5;
  //else
  //    bf = 0.0;

    n = (zs + _time * geometry->get_nz())
      * geometry->get_nx() * geometry->get_ny();

    pltvar = &_var[n];

    sx = 1.0/(geometry->get_nx() - 1.0);
    sy = 1.0/(geometry->get_ny() - 1.0);

    deltx = sx * (contour->get_xEnd() - contour->get_xStart());
    delty = sy * (contour->get_yEnd() - contour->get_yStart());

    sz = (contour->get_zEndX() - contour->get_zStartX()) / geometry->get_nz();
    z1 = contour->get_zStartX() + zs * sz;

    sz = 0.2;

    tid = contour->get_zid(zs);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tid);

  //glNormal3d(x,y,z);

    glBegin(GL_QUADS);
      //South side
        x1 = 0.0;
        y1 = 0.0;
        x2 = 1.0;
        y2 = contour->get_yStart();
        s1 = 0.0;
        t1 = 0.0;
        s2 = 1.0;
        t2 = contour->get_yStart();
        _zQuad(x1, y1, x2, y2, z1, s1, t1, s2, t2);

      //North side
        x1 = 0.0;
        y1 = contour->get_yEnd();
        x2 = 1.0;
        y2 = 1.0;
        s1 = 0.0;
        t1 = contour->get_yEnd();
        s2 = 1.0;
        t2 = 1.0;
        _zQuad(x1, y1, x2, y2, z1, s1, t1, s2, t2);

      //West side
        x1 = 0.0;
        y1 = contour->get_yStart();
        x2 = contour->get_xStart();
        y2 = contour->get_yEnd();
        s1 = 0.0;
        t1 = contour->get_yStart();
        s2 = contour->get_xStart();
        t2 = contour->get_yEnd();
        _zQuad(x1, y1, x2, y2, z1, s1, t1, s2, t2);

      //East side (No need to display for now).
      //x1 = contour->get_xEnd();
      //y1 = contour->get_yStart();
      //x2 = 1.0;
      //y2 = contour->get_yEnd();
      //s1 = contour->get_xEnd();
      //t1 = contour->get_yStart();
      //s2 = 1.0;
      //t2 = contour->get_yEnd();
      //_zQuad(x1, y1, x2, y2, z1, s1, t1, s2, t2);

      //Center
      //x1 = contour->get_xStart();
      //y1 = contour->get_yStart();
      //x2 = contour->get_xEnd();
      //y2 = contour->get_yEnd();
      //s1 = contour->get_xStart();
      //t1 = contour->get_yStart();
      //s2 = contour->get_xEnd();
      //t2 = contour->get_yEnd();
      //_zQuad(x1, y1, x2, y2, z1, s1, t1, s2, t2);
    glEnd();

    for(j = 0; j < geometry->get_ny() - 1; ++j)
    {
        y1 = contour->get_yStart() + j * delty;
        y2 = y1 + delty;

        glBegin(GL_QUAD_STRIP);
            for(i = 0; i < geometry->get_nx(); ++i)
            {
                x1 = contour->get_xStart() + i * deltx;
                z2 = z1 + sz * (bs * (pltvar[i + j * geometry->get_nx()] - minval) + bf);
                glTexCoord2d(x1, 1.0 - y1);
                glVertex3d(x1,y1,z2);

                z2 = z1 + sz * (bs * (pltvar[i + (j + 1) * geometry->get_nx()] - minval) + bf);
                glTexCoord2d(x1, 1.0 - y2);
                glVertex3d(x1,y2,z2);
            }
        glEnd();
    }

    glDisable(GL_TEXTURE_2D);
}

void NCL_Viewer::_display_on_height_surface()
{
    glScalef(1.0, 1.0, 0.4);

    if(geometry->get_nz() > 1)
    {
        glTranslatef(-0.5, -0.5, -0.5);
    }
    else
    {
        glTranslatef(-0.5, -0.5, 0.0);
    }

    if(geometry->get_nz() > 1)
    {
        if(geometry->get_nx() > nvoptions->get_xsec())
        {
            _display_Xplane_on_height_surface(nvoptions->get_xsec());
        }

        if(geometry->get_nx() > nvoptions->get_xsec2())
        {
            _display_Xplane_on_height_surface(nvoptions->get_xsec2());
        }

        if(geometry->get_ny() > nvoptions->get_ysec())
        {
            _display_Yplane_on_height_surface(nvoptions->get_ysec());
        }

        if(geometry->get_ny() > nvoptions->get_ysec2())
        {
            _display_Yplane_on_height_surface(nvoptions->get_ysec2());
        }
    }

    if(geometry->get_nz() > nvoptions->get_zsec())
    {
        _display_Zplane_on_height_surface(nvoptions->get_zsec());
    }

    if(geometry->get_nz() > nvoptions->get_zsec2())
    {
        _display_Zplane_on_height_surface(nvoptions->get_zsec2());
    }
}

void NCL_Viewer::_display_Xplane_on_height_surface(int xs)
{
    int j, k;
  //int m;
    double *height = NULL;
  //float *values = NULL;

    double deltx, delty, deltz, delty2;
    double sx, sy, sz;

    double s1, t1, s2, t2;
    double x1, y1, z1, y2, z2;

    int nx = geometry->get_nx();
    int ny = geometry->get_ny();
    int nz = geometry->get_nz();

    unsigned int tid;

  //values = geometry->get_lev();

    height = new double[nz * ny];

    for(k = 0; k < nz; ++k)
    {
      for(j = 0; j < ny; ++j)
      {
        //m = xs + (j + k * ny) * nx;
        //height[j + k * ny] = values[m];
          height[j + k * ny] = (double) k / nz - 0.5;
      }
    }

    sx = 1.0/(nx - 1.0);
    sy = 1.0/(ny - 1.0);
    sz = 1.0/(nz - 1.0);

    deltx = sx * (contour->get_xEnd()  - contour->get_xStart());
    delty = sy * (contour->get_yEnd()  - contour->get_yStart());
    delty2= sy * (contour->get_yEndX() - contour->get_yStartX());
    deltz = sz * (contour->get_zEndX() - contour->get_zStartX());

    x1 = contour->get_xStart() + xs * deltx;
    if(1 < nz)
        sz *= (contour->get_zEndX() - contour->get_zStartX());
    else
        sz = (contour->get_zEndX() - contour->get_zStartX());

  //cout << "Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__
  //     << ", file: <" << __FILE__ << ">" << endl;
  //cout << "\tcontour->get_yStart() = " << contour->get_yStart() << endl;
  //cout << "\tcontour->get_yEnd()   = " << contour->get_yEnd() << endl;
  //cout << "\tcontour->get_yStartX()= " << contour->get_yStartX() << endl;
  //cout << "\tcontour->get_zEndX()  = " << contour->get_zEndX() << endl;
  //cout << "\theight[ny * nz -1] = " << height[ny * nz -1] << endl;

    tid = contour->get_xid(xs);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tid);

    glNormal3d(1.0, 0.0, 0.0);

    glBegin(GL_QUADS);
      //South side
        y1 = 0.0;
        z1 = contour->get_zStartX();
        y2 = contour->get_yStart();
        z2 = contour->get_zEndX();
        s1 = 0.0;
        t1 = contour->get_zStartX();
        s2 = contour->get_yStartX();
        t2 = contour->get_zEndX();
        _xQuad(x1, y1, z1, y2, z2, s1, t1, s2, t2);

      //North side (No need to display for now).
      //y1 = contour->get_yEnd();
      //z1 = contour->get_zStartX();
      //y2 = 1.0;
      //z2 = contour->get_zEndX();
      //s1 = contour->get_yEndX();
      //t1 = 0.0;
      //s2 = 1.0;
      //t2 = 1.0;
      //_xQuad(x1, y1, z1, y2, z2, s1, t1, s2, t2);

      //Bottom
        y1 = 0.0;
        z1 = 0.0;
        y2 = 1.0;
        z2 = contour->get_zStartX();
        s1 = 0.0;
        t1 = 0.0;
        s2 = 1.0;
        t2 = contour->get_zStartX();
        _xQuad(x1, y1, z1, y2, z2, s1, t1, s2, t2);

      //Top (No need for now)
      //y1 = contour->get_yStart();
      //z1 = contour->get_yEnd();
      //y2 = 1.0;
      //z2 = 1.0;
      //s1 = contour->get_yStartX();
      //t1 = contour->get_zStartX();
      //s2 = contour->get_yEndX();
      //t2 = 1.0;
      //_xQuad(x1, y1, z1, y2, z2, s1, t1, s2, t2);
    glEnd();

    for(k = 0; k < nz - 1; ++k)
    {
        t1 = contour->get_zStartX() + k * deltz;
        t2 = contour->get_zStartX() + (k + 1) * deltz;

        glBegin(GL_QUAD_STRIP);
            for(j = 0; j < ny; ++j)
            {
                s1 = contour->get_yStartX() + j * delty2;
                glTexCoord2d(s1, 1.0 - t1);

                y1 = contour->get_yStart() + j * delty;
                z1 = contour->get_zStartX() + sz * height[j + k * ny];
                glVertex3d(x1,y1,z1);

                glTexCoord2d(s1, 1.0 - t2);

                z2 = contour->get_zStartX() + sz * height[j + (k + 1) * ny];
                glVertex3d(x1,y1,z2);
            }
        glEnd();
    }

    glDisable(GL_TEXTURE_2D);

    delete [] height;
}

void NCL_Viewer::_display_Yplane_on_height_surface(int ys)
{
    int i, k;
  //int m;
    double *height = NULL;
  //float *values = NULL;

    double deltx,  delty, deltz;
    double deltx2;
    double sx, sy, sz;

    double s1, t1, s2, t2;
    double x1, y1, z1, x2, z2;

    int nx = geometry->get_nx();
    int ny = geometry->get_ny();
    int nz = geometry->get_nz();

    unsigned int tid;

  //values = geometry->get_lev();

    height = new double[nz * nx];

    for(k = 0; k < nz; ++k)
    {
        for(i = 0; i < nx; ++i)
        {
          //m = i + (ys + k * ny) * nx;
          //height[i + k * nx] = values[m];
            height[i + k * nx] = (double) k / nz - 0.5;
        }
    }

    sx = 1.0/(nx - 1.0);
    sy = 1.0/(ny - 1.0);
    sz = 1.0/(nz - 1.0);

    deltx = sx * (contour->get_xEnd()  - contour->get_xStart());
    deltx2= sx * (contour->get_xEndY() - contour->get_xStartY());
    delty = sy * (contour->get_yEnd()  - contour->get_yStart());
    deltz = sz * (contour->get_zEndY() - contour->get_zStartY());

    y1 = contour->get_yStart() + ys * delty;
    if(1 < nz)
        sz *= (contour->get_zEndY() - contour->get_zStartY());
    else
        sz = (contour->get_zEndY() - contour->get_zStartY());

    tid = contour->get_yid(ys);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tid);

    glNormal3d(0.0, 1.0, 0.0);

    glBegin(GL_QUADS);
      //South side
        x1 = 0.0;
        z1 = 0.0;
        x2 = contour->get_xStart();
        z2 = 1.0;
        s1 = 0.0;
        t1 = 0.0;
        s2 = contour->get_xStartY();
        t2 = 1.0;
        _yQuad(x1, y1, z1, x2, z2, s1, t1, s2, t2);

      //cout << "Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__
      //     << ", file: <" << __FILE__ << ">" << endl;
      //cout << "\tcontour->get_xStart() = " << contour->get_xStart() << endl;
      //cout << "\tcontour->get_xStartY()= " << contour->get_xStartY()<< endl;

      //North side (No need to display for now).
      //x1 = contour->get_xEnd();
      //z1 = 0.0;
      //x2 = 1.0;
      //z2 = 1.0;
      //s1 = contour->get_xEndY();
      //t1 = 0.0;
      //s2 = 1.0;
      //t2 = 1.0;
      //_yQuad(x1, y1, z1, x2, z2, s1, t1, s2, t2);

      //Bottom
        x1 = 0.0;
        z1 = 0.0;
        x2 = 1.0;
        z2 = contour->get_zStartY();
        s1 = 0.0;
        t1 = 0.0;
        s2 = 1.0;
        t2 = contour->get_zStartY();
        _yQuad(x1, y1, z1, x2, z2, s1, t1, s2, t2);

      //Top (No need for now)
      //x1 = contour->get_xStart();
      //z1 = contour->get_zEndY();
      //x2 = 1.0;
      //z2 = 1.0;
      //s1 = contour->get_xStartY();
      //t1 = contour->get_zEndY();
      //s2 = 1.0;
      //t2 = 1.0;
      //_yQuad(x1, y1, z1, x2, z2, s1, t1, s2, t2);
    glEnd();

    for(k = 0; k < nz - 1; ++k)
    {
        t1 = contour->get_zStartY() + k * deltz;
        t2 = contour->get_zStartY() + (k + 1) * deltz;

        glBegin(GL_QUAD_STRIP);
            for(i = 0; i < nx; ++i)
            {
                s1 = contour->get_xStartY() + i * deltx2;
                glTexCoord2d(s1, 1.0 - t1);

                x1 = contour->get_xStart() + i * deltx;
                z1 = contour->get_zStartY() + sz * height[i + k * nx];
                glVertex3d(x1,y1,z1);

                glTexCoord2d(s1, 1.0 - t2);
                z2 = contour->get_zStartY() + sz * height[i + (k + 1) * nx];
                glVertex3d(x1,y1,z2);
            }
        glEnd();
    }

    glDisable(GL_TEXTURE_2D);

    delete [] height;
}

void NCL_Viewer::_display_Zplane_on_height_surface(int zs)
{
    int i, j;
  //int n;
  //float *values = NULL;
    double height = 0.0;

    double deltx, delty;
    double sx, sy;
  //double sz;

    double s1, t1, s2, t2;
    double x1, y1, z1, x2, y2, z2;

    int nx = geometry->get_nx();
    int ny = geometry->get_ny();
    int nz = geometry->get_nz();

    unsigned int tid;

    tid = contour->get_zid(zs);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tid);

    glNormal3d(0.0,0.0,1.0);

    sx = 1.0/(geometry->get_nx() - 1.0);
    sy = 1.0/(geometry->get_ny() - 1.0);

    deltx = sx * (contour->get_xEnd() - contour->get_xStart());
    delty = sy * (contour->get_yEnd() - contour->get_yStart());

    glNormal3d(0.0, 0.0, 1.0);

  //values = geometry->get_lev();

  //n = zs * nx * ny;

  //height = &values[n];
    height = (double) zs / nz - 0.5;

  //sz = (contour->get_zEndX() - contour->get_zStartX()) / nz;
    z1 = contour->get_zStartX() + height;

    glBegin(GL_QUADS);
      //South side
        x1 = 0.0;
        y1 = 0.0;
        x2 = 1.0;
        y2 = contour->get_yStart();
        s1 = 0.0;
        t1 = 0.0;
        s2 = 1.0;
        t2 = contour->get_yStart();
        _zQuad(x1, y1, x2, y2, z1, s1, t1, s2, t2);

      //cout << "Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__
      //     << ", file: <" << __FILE__ << ">" << endl;
      //cout << "\tcontour->get_yStart() = " << contour->get_yStart() << endl;

      //North side
        x1 = 0.0;
        y1 = contour->get_yEnd();
        x2 = 1.0;
        y2 = 1.0;
        s1 = 0.0;
        t1 = contour->get_yEnd();
        s2 = 1.0;
        t2 = 1.0;
        _zQuad(x1, y1, x2, y2, z1, s1, t1, s2, t2);

      //West side
        x1 = 0.0;
        y1 = contour->get_yStart();
        x2 = contour->get_xStart();
        y2 = contour->get_yEnd();
        s1 = 0.0;
        t1 = contour->get_yStart();
        s2 = contour->get_xStart();
        t2 = contour->get_yEnd();
        _zQuad(x1, y1, x2, y2, z1, s1, t1, s2, t2);

      //East side (No need to display for now).
      //x1 = contour->get_xEnd();
      //y1 = contour->get_yStart();
      //x2 = 1.0;
      //y2 = contour->get_yEnd();
      //s1 = contour->get_xEnd();
      //t1 = contour->get_yStart();
      //s2 = 1.0;
      //t2 = contour->get_yEnd();
      //_zQuad(x1, y1, x2, y2, z1, s1, t1, s2, t2);
    glEnd();

    for(j = 0; j < ny - 1; ++j)
    {
        t1 = contour->get_yStart() + j * delty;
        t2 = t1 + delty;

      //y1 = t1;
      //y2 = t2;

        glBegin(GL_QUAD_STRIP);
            for(i = 0; i < nx; ++i)
            {
                x1 = contour->get_xStart() + i * deltx;
                z1 = contour->get_zStartX() + height;
                glTexCoord2d(x1, 1.0 - t1);
                glVertex3d(x1,t1,z1);

                z2 = contour->get_zStartX() + height;
                glTexCoord2d(x1, 1.0 - t2);
                glVertex3d(x1,t2,z2);
            }
        glEnd();
    }
    glDisable(GL_TEXTURE_2D);
}

void NCL_Viewer::_display_on_sphere()
{
    if(geometry->get_nz() > 1)
    {
        if(geometry->get_nx() > nvoptions->get_xsec())
        {
            _display_Xplane_on_sphere(nvoptions->get_xsec());
        }

      //if(nvoptions->get_cb(NV_HASX2) && (geometry->get_nx() > nvoptions->get_xsec2()))
      //{
      //    _display_Xplane_on_sphere(nvoptions->get_xsec2());
      //}

        if(geometry->get_ny() > nvoptions->get_ysec())
        {
            _display_Yplane_on_sphere(nvoptions->get_ysec());
        }

      //if(nvoptions->get_cb(NV_HASY2) && (geometry->get_ny() > nvoptions->get_ysec2()))
      //{
      //    _display_Yplane_on_sphere(nvoptions->get_ysec2());
      //}
    }

    if(geometry->get_nz() > nvoptions->get_zsec())
    {
      //cout << "Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__
      //     << ", file: <" << __FILE__ << ">" << endl;
      //cout << "\tVar name: <" << _varname << ">, nvoptions->get_zsec() = " << nvoptions->get_zsec() << endl;

        _display_Zplane_on_sphere(nvoptions->get_zsec());
    }

  //if(nvoptions->get_cb(NV_HASZ2) && (geometry->get_nz() > nvoptions->get_zsec2()))
  //{
  //    _display_Zplane_on_sphere(nvoptions->get_zsec2());
  //}

  //earth->draw();
}

void NCL_Viewer::_display_Xplane_on_sphere(int xs)
{
    int j, k, n;
  //int m;
    double *height = NULL;
  //float *values = NULL;

    double delty, deltz;
    double sy, sz;

    double s1, t1;
    double x1, y1, z1;

    unsigned int tid;

    int nx = geometry->get_nx();
    int ny = geometry->get_ny();
    int nz = geometry->get_nz();

    double *lon = NULL;
    double *lat = NULL;

    if(xs >= nx)
        return;

    lon = geometry->get_lon();
    lat = geometry->get_lat();

  //values = geometry->get_lev();

    height = new double[ny * nz];

    for(k = 0; k < nz; ++k)
    {
        for(j = 0; j < ny; ++j)
        {
          //m = xs + (j + k * ny) * nx;
          //height[j + k * ny] = values[m];
            height[j + k * ny] = (double) k;
        }
    }

    sy = 1.0/(ny - 1.0);
    sz = 1.0/(nz - 1.0);

    delty = sy * (contour->get_yEndX() - contour->get_yStartX());
    deltz = sz * (contour->get_zEndX() - contour->get_zStartX());

    sz = 0.1 / (nz + 0.01);

    tid = contour->get_xid(xs);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tid);

  //glNormal3d(x,y,z);

    n = nx - 1 - xs;
    for(k = 0; k < nz - 1; ++k)
    {
        glBegin(GL_QUAD_STRIP);
            for(j = 0; j < ny; ++j)
            {
                x1 = lon[n + j * nx];
                y1 = lat[n + j * nx];
                z1 = 0.501 + sz * height[j + k * ny];
                s1 = contour->get_yStartX() + j * delty;
                t1 = contour->get_zStartX() + k * deltz;
                _Vertex(x1, y1, z1, s1, 1.0 - t1);

                z1 = 0.501 + sz * height[j + (k + 1) * ny];
                t1 = contour->get_zStartX() + (k + 1) * deltz;
                _Vertex(x1, y1, z1, s1, 1.0 - t1);
            }
        glEnd();
    }

    glDisable(GL_TEXTURE_2D);

    delete [] height;
}

void NCL_Viewer::_display_Yplane_on_sphere(int ys)
{
    int i, k, n;
  //int i, k, n;
    double *height = NULL;
  //float *values = NULL;

    double deltx, deltz;
    double sx, sz;

    double s1, t1;
    double x1, y1, z2;

    unsigned int tid;

    int nx = geometry->get_nx();
    int ny = geometry->get_ny();
    int nz = geometry->get_nz();

    double *lon = NULL;
    double *lat = NULL;

    if(ys >= ny)
        return;

    lon = geometry->get_lon();
    lat = geometry->get_lat();

  //values = geometry->get_lev();
    height = new double[nx * nz];

    for(k = 0; k < nz; ++k)
    {
        for(i = 0; i < nx; ++i)
        {
          //m = i + (ys + k * ny) * nx;
          //height[i + k * nx] = values[m];
            height[i + k * nx] = (double) k;
        }
    }

    sx = 1.0/(nx - 1.0);
    sz = 1.0/(nz - 1.0);

    deltx = sx * (contour->get_xEndY() - contour->get_xStartY());
    deltz = sz * (contour->get_zEndX() - contour->get_zStartX());

    sz = 0.1 / (nz + 0.01);

    tid = contour->get_yid(ys);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tid);

  //glNormal3d(x,y,z);

    for(k = 0; k < nz - 1; ++k)
    {
        n = nx;
        glBegin(GL_QUAD_STRIP);
            for(i = 0; i < nx; ++i)
            {
                --n;
                x1 = lon[n + ys * nx];
                y1 = lat[n + ys * nx];
                z2 = 0.501 + sz * height[i + k * nx];
                s1 = contour->get_xStartY() + i * deltx;
                t1 = contour->get_zStartY() + k * deltz;
                _Vertex(x1, y1, z2, s1, 1.0 - t1);

                z2 = 0.501 + sz * height[i + (k + 1) * nx];
                t1 = contour->get_zStartY() + (k + 1) * deltz;
                _Vertex(x1, y1, z2, s1, 1.0 - t1);
            }
        glEnd();
    }

    glDisable(GL_TEXTURE_2D);
    delete [] height;
}

void NCL_Viewer::_display_Zplane_on_sphere(int zs)
{
    int i, j, n;
    double height;
  //float *values = NULL;

    double deltx, delty;
    double sx, sy, sz;

    double s1, t1;
    double x1, y1, z1;

    unsigned int tid;

    int nx = geometry->get_nx();
    int ny = geometry->get_ny();
    int nz = geometry->get_nz();

    double *lon = NULL;
    double *lat = NULL;

    if(zs >= nz)
        return;

    lon = geometry->get_lon();
    lat = geometry->get_lat();

  //cout << "Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__
  //     << ", file: <" << __FILE__ << ">" << endl;
  //cout << "\tVar name: <" << _varname << ">, level = " << zs << endl;

    sx = 1.0/(nx - 1.0);
    sy = 1.0/(ny - 1.0);
    sz = 1.0/(nz - 1.0);

    deltx = sx * (contour->get_xEnd() - contour->get_xStart());
    delty = sy * (contour->get_yEnd() - contour->get_yStart());

  //glNormal3d(x,y,z);

  //values = geometry->get_lev();
  //n = (zs + _time * nz) * nx * ny;
  //height = &values[n];

    sz = 0.1 / (nz + 0.01);

    height = 0.5 + zs * sz;

    tid = contour->get_zid(zs);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tid);

    for(j = 0; j < ny - 1; ++j)
    {
        n = nx;
        glBegin(GL_QUAD_STRIP);
        for(i = 0; i < nx; ++i)
        {
            --n;
            x1 = lon[n + j * nx];
            y1 = lat[n + j * nx];
          //z1 = 0.501 + sz * height[i + j * nx];
            z1 = height;
            s1 = contour->get_xStart() + i * deltx;
            t1 = contour->get_yStart() + (j + 1) * delty;
            _Vertex(x1, y1, z1, s1, 1.0 - t1);

            x1 = lon[n + (j + 1) * nx];
            y1 = lat[n + (j + 1) * nx];
          //z1 = 0.501 + sz * height[i + (j + 1) * nx];
            z1 = height;
            s1 = contour->get_xStart() + i * deltx;
            t1 = contour->get_yStart() + (j + 1) * delty;
            _Vertex(x1, y1, z1, s1, 1.0 - t1);
        }
        glEnd();
    }
    glDisable(GL_TEXTURE_2D);
}

void NCL_Viewer::_display_Global_on_sphere()
{
    int i, j;

    double deltx, delty;

    double s1, t1, t2;
    double x1, y1, y2;

    unsigned int tid;

    deltx = contour->get_xEnd() - contour->get_xStart();
    delty = contour->get_yEnd() - contour->get_yStart();

  //glNormal3d(x,y,z);

    tid = contour->get_zid(nvoptions->get_zsec());

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tid);

    for(j = -90; j < 90; j += 5)
    {
        y1 = (double) j;
        y2 = y1 + 5.0;
        t1 = contour->get_yStart() + delty * (0.5 + y1/180.0);
        t2 = contour->get_yStart() + delty * (0.5 + y2/180.0);

        glBegin(GL_QUAD_STRIP);
        for(i = 0; i <= 360; i += 5)
        {
            x1 = (double) i;
            s1 = contour->get_xStart() + deltx*x1/360.0;
            _Vertex(x1, y1, 0.505, 1.0 - s1, 1.0 - t1);
            _Vertex(x1, y2, 0.505, 1.0 - s1, 1.0 - t2);
        }
        glEnd();
    }
    glDisable(GL_TEXTURE_2D);
}

void NCL_Viewer::set_geometry(Geometry *gm)
{
    geometry = gm;

    contour = new Contour(colorTable);

    contour->set_geometry(gm);

    contour->set_nx(geometry->get_nx());
    contour->set_ny(geometry->get_ny());
    contour->set_nz(geometry->get_nz());
}

void NCL_Viewer::setup(string vn, float *var)
{
    _varname  = vn;
    _var = var;

    if(_first_time)
    {
        nvoptions->set_xsec(geometry->get_nx() + 1);
        nvoptions->set_xsec2(geometry->get_nx() + 1);
        nvoptions->set_ysec(geometry->get_ny() + 1);
        nvoptions->set_ysec2(geometry->get_ny() + 1);
        nvoptions->set_zsec(0);
        nvoptions->set_zsec2(geometry->get_nz() + 1);
        _first_time = true;
    }
    _time   = 0;

    contour->setup(vn, var);
}

void NCL_Viewer::set_OP(bool s)
{
    _hasOP = s;
  //contour->reinitialize();
}

//Draw vertex in polar coordinates
void NCL_Viewer::_Vertex(double lon, double lat, double r, double s, double t)
{
    double x = r*Cos(lon)*Cos(lat);
    double y =          r*Sin(lat);
    double z = r*Sin(lon)*Cos(lat);
  //glNormal3d(x,y,z);
    glTexCoord2d(s, t);
    glVertex3d(x,y,z);
}

void NCL_Viewer::_point(double x, double y, double z, double s, double t)
{
    glTexCoord2d(s, 1.0 - t);
    glVertex3d(x,y,z);
}

void NCL_Viewer::_xQuad(double x1, double y1, double z1, double y2, double z2,
                        double s1, double t1, double s2, double t2)
{
    _point(x1, y1, z1, s1, t1);
    _point(x1, y2, z1, s2, t1);
    _point(x1, y2, z2, s2, t2);
    _point(x1, y1, z2, s1, t2);
}

void NCL_Viewer::_yQuad(double x1, double y1, double z1, double x2, double z2,
                        double s1, double t1, double s2, double t2)
{
    _point(x1, y1, z1, s1, t1);
    _point(x2, y1, z1, s2, t1);
    _point(x2, y1, z2, s2, t2);
    _point(x1, y1, z2, s1, t2);
}

void NCL_Viewer::_zQuad(double x1, double y1, double x2, double y2, double z2,
                        double s1, double t1, double s2, double t2)
{
    _point(x1, y1, z2, s1, t1);
    _point(x2, y1, z2, s2, t1);
    _point(x2, y2, z2, s2, t2);
    _point(x1, y2, z2, s1, t2);
}

void NCL_Viewer::set_opacity(int o)
{
  //cout << "Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__
  //     << ", file: <" << __FILE__ << ">" << endl;
  //cout << "\tOpacity = " << o << endl;

    _opacity = o;
    contour->set_opacity(o);
    contour->reinitialize();
    draw();
}

