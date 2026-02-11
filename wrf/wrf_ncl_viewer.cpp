//$Id: wrf_ncl_viewer.cpp 4927 2014-01-23 21:49:37Z starviewer $

#include <QtOpenGL>

#include "wrf_ncl_viewer.h"

#define ARC     (3.14159256359/180.0)
#define Cos(th) cos(ARC*(th))
#define Sin(th) sin(ARC*(th))

WRF_NCL_Viewer::WRF_NCL_Viewer(ColorTable *ct, NVOptions* opt)
{
    colorTable = ct;
    nvoptions = opt;

    _var = NULL;

  //earth = new Earth();
    contour = new WRF_Contour(colorTable);

    _hasOP = false;

    first_time = true;
}

WRF_NCL_Viewer::~WRF_NCL_Viewer()
{
  //cout << "\tEnter function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
  //delete earth;
    delete contour;
  //cout << "\tLeave function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
}

void WRF_NCL_Viewer::draw()
{
    if(nvoptions->get_cb(NV_RESET))
    {
        nvoptions->set_cb(NV_RESET, false);
        contour->reset();
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

void WRF_NCL_Viewer::_display_all()
{
    glScalef(1.0, 1.0, 0.4);
    if(nz > 1)
    {
        glTranslatef(-0.5, -0.5, -0.5);
    }
    else
    {
        glTranslatef(-0.5, -0.5, 0.0);
    }

    if((nx > nvoptions->get_xsec()) && (nz > 1))
    {
        _display_Xplane(nvoptions->get_xsec());
    }

    if(nvoptions->get_cb(NV_HASX2) && (nx > nvoptions->get_xsec2()) && (nz > 1))
    {
        _display_Xplane(nvoptions->get_xsec2());
    }

    if((ny > nvoptions->get_ysec()) && (nz > 1))
    {
        _display_Yplane(nvoptions->get_ysec());
    }

    if(nvoptions->get_cb(NV_HASY2) && (ny > nvoptions->get_ysec2()) && (nz > 1))
    {
        _display_Yplane(nvoptions->get_ysec2());
    }

    if(nz > nvoptions->get_zsec())
    {
        _display_Zplane(nvoptions->get_zsec());
    }

    if(nvoptions->get_cb(NV_HASZ2) && (nz > nvoptions->get_zsec2()) && (nz > 1))
    {
        _display_Zplane(nvoptions->get_zsec2());
    }
}

void WRF_NCL_Viewer::_display_Xplane(int xs)
{
    float s1, t1, s2, t2;
    float x1, y1, z1, y2, z2;

    float scl = 1.0;

    unsigned int tid;

    tid = contour->get_xid(xs);

    scl = (contour->get_xEnd() - contour->get_xStart()) / nx;
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

  //glNormal3f(0.0, 0.0, -1.0);

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

void WRF_NCL_Viewer::_display_Yplane(int ys)
{
    float s1, t1, s2, t2;
    float x1, y1, z1, x2, z2;

    float scl = 1.0;

    unsigned int tid;

    tid = contour->get_yid(ys);

    scl = (contour->get_yEnd() - contour->get_yStart()) / ny;
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

  //glNormal3f(0.0, 0.0, -1.0);

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

void WRF_NCL_Viewer::_display_Zplane(int zs)
{
    float s1, t1, s2, t2;
    float x1, y1, z1, x2, y2;

    float scl = 1.0;

    unsigned int tid;

    tid = contour->get_zid(zs);

  //cout << "\tEnter function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
  //cout << "\ttid = " << tid << endl;
  //cout << "\tcontour->get_yStart() = " << contour->get_yStart() << endl;
  //cout << "\tcontour->get_yEnd()   = " << contour->get_yEnd() << endl;
  //cout << "\tcontour->get_yStartX()= " << contour->get_yStartX() << endl;
  //cout << "\tcontour->get_zEndX()  = " << contour->get_zEndX() << endl;

    scl = (contour->get_zEndX() - contour->get_zStartX()) / nz;
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
        x1 = contour->get_xEnd();
        y1 = contour->get_yStart();
        x2 = 1.0;
        y2 = contour->get_yEnd();
        s1 = contour->get_xEnd();
        t1 = contour->get_yStart();
        s2 = 1.0;
        t2 = contour->get_yEnd();
        _zQuad(x1, y1, x2, y2, z1, s1, t1, s2, t2);

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
  //cout << "\tLeave function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
}

void WRF_NCL_Viewer::_display_with_bump()
{
    glScalef(1.0, 1.0, 0.5);

    if(nz > 1)
    {
        glTranslatef(-0.5, -0.5, -0.5);
    }
    else
    {
        glTranslatef(-0.5, -0.5, 0.0);
    }

    if((nx > nvoptions->get_xsec()) && (nz > 1))
    {
        _display_Xplane_with_bump(nvoptions->get_xsec());
    }

    if((ny > nvoptions->get_ysec()) && (nz > 1))
    {
        _display_Yplane_with_bump(nvoptions->get_ysec());
    }

    if(nz > nvoptions->get_zsec())
    {
        _display_Zplane_with_bump(nvoptions->get_zsec());
    }
}

void WRF_NCL_Viewer::_display_Xplane_with_bump(int xs)
{
    int   j, k, m;
    float bs, bf;
    float delty, deltz;
    float delty2;
    float sx, sy, sz;

    float *pltvar = NULL;

    float s1, t1, s2, t2;
    float x1, y1, z1, x2, y2, z2;

    unsigned int tid;

    bs = 1.0/(_maxval - _minval);
    bf = -0.5;

    pltvar = new float[nz * ny];

    for(k = 0; k < nz; ++k)
    {
        for(j = 0; j < ny; ++j)
        {
            m = xs + (j + k * nyp) * nxp;
            pltvar[j + k * ny] = _var[m];
        }
    }

    sy = 1.0/(ny - 1.0);
    sz = 1.25/(nz - 1.0);

    delty = sy * (contour->get_yEnd()  - contour->get_yStart());
    delty2= sy * (contour->get_yEndX() - contour->get_yStartX());
    deltz = sz * (contour->get_zEndX() - contour->get_zStartX());

    sx = (contour->get_xEnd() - contour->get_xStart()) / (nx - 1);
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

    for(k = 0; k < nz - 1; ++k)
    {
        z1 = contour->get_zStartX() + k * deltz;
        z2 = z1 + deltz;

        glBegin(GL_QUAD_STRIP);
            for(j = 0; j < ny; ++j)
            {
                y1 = contour->get_yStart() + j * delty;
                x2 = x1 + sx * (bs * (pltvar[j + k * ny] - _minval) + bf);
                s1 = contour->get_yStartX() + j * delty2;
                glTexCoord2d(s1, 1.0 - z1);
                glVertex3f(x2,y1,z1);

                x2 = x1 + sx * (bs * (pltvar[j + (k + 1) * ny] - _minval) + bf);
                glTexCoord2d(s1, 1.0 - z2);
                glVertex3f(x2,y1,z2);
            }
        glEnd();
    }

    glDisable(GL_TEXTURE_2D);

    delete [] pltvar;
}

void WRF_NCL_Viewer::_display_Yplane_with_bump(int ys)
{
    int   i, k, m;
    float bs, bf;
    float deltx, deltz;
    float deltx2;
    float sx, sy, sz;

    float *pltvar = NULL;

    float s1, t1, s2, t2;
    float x1, y1, z1, x2, y2, z2;

    unsigned int tid;

    bs = 1.0/(_maxval - _minval);
    bf = -0.5;

    pltvar = new float[nz * nx];

    for(k = 0; k < nz; ++k)
    {
        for(i = 0; i < nx; ++i)
        {
            m = i + (ys + k * nyp) * nxp;
            pltvar[i + k * nxp] = _var[m];
        }
    }

    sx = 1.0/(nx - 1.0);
    sz = 1.25/(nz - 1.0);

    deltx = sx * (contour->get_xEnd()  - contour->get_xStart());
    deltx2= sx * (contour->get_xEndY() - contour->get_xStartY());
    deltz = sz * (contour->get_zEndY() - contour->get_zStartY());

    sy = (contour->get_yEnd() - contour->get_yStart()) / (ny - 1);
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

    for(k = 0; k < nz - 1; ++k)
    {
        z1 = contour->get_zStartX() + k * deltz;
        z2 = z1 + deltz;

        glBegin(GL_QUAD_STRIP);
            for(i = 0; i < nx; ++i)
            {
                x1 = contour->get_xStart() + i * deltx;
                y2 = y1 + sy * (bs * (pltvar[i + k * nx] - _minval) + bf);
                s1 = contour->get_xStartY() + i * deltx2;
                glTexCoord2d(s1, 1.0 - z1);
                glVertex3f(x1,y2,z1);

                y2 = y1 + sy * (bs * (pltvar[i + (k + 1) * nx] - _minval) + bf);
                glTexCoord2d(s1, 1.0 - z2);
                glVertex3f(x1,y2,z2);
            }
        glEnd();
    }

    glDisable(GL_TEXTURE_2D);

    delete [] pltvar;
}

void WRF_NCL_Viewer::_display_Zplane_with_bump(int zs)
{
    int   i, j, n;
    float bs, bf;
    float deltx, delty;
    float sx, sy, sz;

    double* pltvar = NULL;

    float s1, t1, s2, t2;
    float x1, y1, z1, x2, y2, z2;

    unsigned int tid;

    bs = 1.0/(_maxval - _minval);

    if(nz > 1)
        bf = -0.5;
    else
        bf = 0.0;

    n = zs * nxp * nyp;

    pltvar = &_var[n];

    sx = 1.0/(nx - 1.0);
    sy = 1.0/(ny - 1.0);

    deltx = sx * (contour->get_xEnd() - contour->get_xStart());
    delty = sy * (contour->get_yEnd() - contour->get_yStart());

    sz = 1.25 * (contour->get_zEndX() - contour->get_zStartX()) / nz;
    z1 = contour->get_zStartX() + zs * sz;

    sz = 0.25;

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
        x1 = contour->get_xEnd();
        y1 = contour->get_yStart();
        x2 = 1.0;
        y2 = contour->get_yEnd();
        s1 = contour->get_xEnd();
        t1 = contour->get_yStart();
        s2 = 1.0;
        t2 = contour->get_yEnd();
        _zQuad(x1, y1, x2, y2, z1, s1, t1, s2, t2);

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

    for(j = 0; j < ny - 1; ++j)
    {
        y1 = contour->get_yStart() + j * delty;
        y2 = y1 + delty;

        glBegin(GL_QUAD_STRIP);
            for(i = 0; i < nx; ++i)
            {
                x1 = contour->get_xStart() + i * deltx;
                z2 = z1 + sz * (bs * (pltvar[i + j * nx] - _minval) + bf);
                glTexCoord2f(x1, 1.0 - y1);
                glVertex3f(x1,y1,z2);

                z2 = z1 + sz * (bs * (pltvar[i + (j + 1) * nx] - _minval) + bf);
                glTexCoord2f(x1, 1.0 - y2);
                glVertex3f(x1,y2,z2);
            }
        glEnd();
    }

    glDisable(GL_TEXTURE_2D);
}

void WRF_NCL_Viewer::_display_on_height_surface()
{
    glScalef(1.0, 1.0, 0.4);

    if(nz > 1)
    {
        glTranslatef(-0.5, -0.5, -0.5);
    }
    else
    {
        glTranslatef(-0.5, -0.5, 0.0);
    }

    if(nz > 1)
    {
        if(nx > nvoptions->get_xsec())
        {
            _display_Xplane_on_height_surface(nvoptions->get_xsec());
        }

        if(nx > nvoptions->get_xsec2())
        {
            _display_Xplane_on_height_surface(nvoptions->get_xsec2());
        }

        if(ny > nvoptions->get_ysec())
        {
            _display_Yplane_on_height_surface(nvoptions->get_ysec());
        }

        if(ny > nvoptions->get_ysec2())
        {
            _display_Yplane_on_height_surface(nvoptions->get_ysec2());
        }
    }

    if(nz > nvoptions->get_zsec())
    {
        _display_Zplane_on_height_surface(nvoptions->get_zsec());
    }

    if(nz > nvoptions->get_zsec2())
    {
        _display_Zplane_on_height_surface(nvoptions->get_zsec2());
    }
}

void WRF_NCL_Viewer::_display_Xplane_on_height_surface(int xs)
{
    int j, k, m;
    float* height = NULL;
    double* values = NULL;

    float deltx, delty, deltz, delty2;
    float sx, sy, sz;

    float s1, t1, s2, t2;
    float x1, y1, z1, y2, z2;

    unsigned int tid;

    values = geometry->get_hgt();

    height = new float[nz * ny];

    for(k = 0; k < nz; ++k)
    {
      for(j = 0; j < ny; ++j)
      {
          m = xs + (j + k * ny) * nx;
          height[j + k * ny] = values[m];
      }
    }

    sx = 1.0/(nx - 1.0);
    sy = 1.0/(ny - 1.0);
    sz = 1.25/(nz - 1.0);

    deltx = sx * (contour->get_xEnd()  - contour->get_xStart());
    delty = sy * (contour->get_yEnd()  - contour->get_yStart());
    delty2= sy * (contour->get_yEndX() - contour->get_yStartX());
    deltz = sz * (contour->get_zEndX() - contour->get_zStartX());

    x1 = contour->get_xStart() + xs * deltx;
    sz = 1.25 * (contour->get_zEndX() - contour->get_zStartX()) / (_hmax + 0.01);

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
                glVertex3f(x1,y1,z1);

                glTexCoord2d(s1, 1.0 - t2);

                z2 = contour->get_zStartX() + sz * height[j + (k + 1) * ny];
                glVertex3f(x1,y1,z2);
            }
        glEnd();
    }

    glDisable(GL_TEXTURE_2D);

    delete [] height;
}

void WRF_NCL_Viewer::_display_Yplane_on_height_surface(int ys)
{
    int i, k, m;
    float *height = NULL;
    double* values = NULL;

    float deltx,  delty, deltz;
    float deltx2;
    float sx, sy, sz;

    float s1, t1, s2, t2;
    float x1, y1, z1, x2, z2;

    unsigned int tid;

    values = geometry->get_hgt();

    height = new float[nz * nx];

    for(k = 0; k < nz; ++k)
    {
        for(i = 0; i < nx; ++i)
        {
            m = i + (ys + k * ny) * nx;
            height[i + k * nx] = values[m];
        }
    }

    sx = 1.0/(nx - 1.0);
    sy = 1.0/(ny - 1.0);
    sz = 1.25/(nz - 1.0);

    deltx = sx * (contour->get_xEnd()  - contour->get_xStart());
    deltx2= sx * (contour->get_xEndY() - contour->get_xStartY());
    delty = sy * (contour->get_yEnd()  - contour->get_yStart());
    deltz = sz * (contour->get_zEndY() - contour->get_zStartY());

    y1 = contour->get_yStart() + ys * delty;
    sz = 1.25 * (contour->get_zEndY() - contour->get_zStartY()) / (_hmax + 0.01);

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
                glVertex3f(x1,y1,z1);

                glTexCoord2d(s1, 1.0 - t2);
                z2 = contour->get_zStartY() + sz * height[i + (k + 1) * nx];
                glVertex3f(x1,y1,z2);
            }
        glEnd();
    }

    glDisable(GL_TEXTURE_2D);

    delete [] height;
}

void WRF_NCL_Viewer::_display_Zplane_on_height_surface(int zs)
{
    int i, j, n;
    double* height = NULL;
    double* values = NULL;

    float deltx, delty;
    float sx, sy, sz;

    float s1, t1, s2, t2;
    float x1, y1, z1, x2, y2, z2;

    unsigned int tid;

    tid = contour->get_zid(zs);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tid);

    glNormal3d(0.0,0.0,1.0);

    sx = 1.0/(nx - 1.0);
    sy = 1.0/(ny - 1.0);

    deltx = sx * (contour->get_xEnd() - contour->get_xStart());
    delty = sy * (contour->get_yEnd() - contour->get_yStart());

    glNormal3d(0.0, 0.0, 1.0);

    values = geometry->get_hgt();

    n = zs * nx * ny;

    height = &values[n];

    sz = 1.25 * (contour->get_zEndX() - contour->get_zStartX()) / (_hmax + 0.01);
    z1 = contour->get_zStartX() + sz * height[0];

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
        x1 = contour->get_xEnd();
        y1 = contour->get_yStart();
        x2 = 1.0;
        y2 = contour->get_yEnd();
        s1 = contour->get_xEnd();
        t1 = contour->get_yStart();
        s2 = 1.0;
        t2 = contour->get_yEnd();
        _zQuad(x1, y1, x2, y2, z1, s1, t1, s2, t2);
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
                z1 = contour->get_zStartX() + sz * height[i + j * nx];
                glTexCoord2f(x1, 1.0 - t1);
                glVertex3f(x1,t1,z1);

                z2 = contour->get_zStartX() + sz * height[i + (j + 1) * nx];
                glTexCoord2f(x1, 1.0 - t2);
                glVertex3f(x1,t2,z2);
            }
        glEnd();
    }
    glDisable(GL_TEXTURE_2D);
}

void WRF_NCL_Viewer::_display_on_sphere()
{
    if(nz > 1)
    {
        if(nx > nvoptions->get_xsec())
        {
            _display_Xplane_on_sphere(nvoptions->get_xsec());
        }

      //if(nvoptions->get_cb(NV_HASX2) && (nx > nvoptions->get_xsec2()))
      //{
      //    _display_Xplane_on_sphere(nvoptions->get_xsec2());
      //}

        if(ny > nvoptions->get_ysec())
        {
            _display_Yplane_on_sphere(nvoptions->get_ysec());
        }

      //if(nvoptions->get_cb(NV_HASY2) && (ny > nvoptions->get_ysec2()))
      //{
      //    _display_Yplane_on_sphere(nvoptions->get_ysec2());
      //}
    }

    if(nz > nvoptions->get_zsec())
    {
          //cout << "Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__
      //     << ", file: <" << __FILE__ << ">" << endl;
      //cout << "\tVar name: <" << _varname << ">, nvoptions->get_zsec() = " << nvoptions->get_zsec() << endl;

        _display_Zplane_on_sphere(nvoptions->get_zsec());
    }

  //if(nvoptions->get_cb(NV_HASZ2) && (nz > nvoptions->get_zsec2()))
  //{
  //    _display_Zplane_on_sphere(nvoptions->get_zsec2());
  //}

  //earth->draw();
}

void WRF_NCL_Viewer::_display_Xplane_on_sphere(int xs)
{
    int j, k, m, n;
    float* height = NULL;
    double* values = NULL;

    float delty, deltz;
    float sy, sz;

    float s1, t1;
    float x1, y1, z1;

    unsigned int tid;

    double* lon = NULL;
    double* lat = NULL;

    if(geometry->isXstaggered())
    {
        lon = geometry->get_ulon();
        lat = geometry->get_ulat();
    }
    else if(geometry->isYstaggered())
    {
        lon = geometry->get_vlon();
        lat = geometry->get_vlat();
    }
    else
    {
        lon = geometry->get_lon();
        lat = geometry->get_lat();
    }

    if(xs >= nx)
        return;

    values = geometry->get_hgt();

    height = new float[ny * nz];

    for(k = 0; k < nz; ++k)
    {
        for(j = 0; j < ny; ++j)
        {
            m = xs + (j + k * ny) * nx;
            height[j + k * ny] = values[m];
        }
    }

    sy = 1.0/(ny - 1.0);
    sz = 1.25/(nz - 1.0);

    delty = sy * (contour->get_yEndX() - contour->get_yStartX());
    deltz = sz * (contour->get_zEndX() - contour->get_zStartX());

    sz = 0.125 / (_hmax + 0.01);

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
                x1 = lon[n + j * nxp];
                y1 = lat[n + j * nxp];
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

void WRF_NCL_Viewer::_display_Yplane_on_sphere(int ys)
{
    int i, k, m, n;
    float* height = NULL;
    double* values = NULL;

    float deltx, deltz;
    float sx, sz;

    float s1, t1;
    float x1, y1, z2;

    unsigned int tid;

    double* lon = NULL;
    double* lat = NULL;

    if(geometry->isXstaggered())
    {
        lon = geometry->get_ulon();
        lat = geometry->get_ulat();
    }
    else if(geometry->isYstaggered())
    {
        lon = geometry->get_vlon();
        lat = geometry->get_vlat();
    }
    else
    { 
        lon = geometry->get_lon();
        lat = geometry->get_lat();
    }

    if(ys >= ny)
        return;

    values = geometry->get_hgt();
    height = new float[nx * nz];

    for(k = 0; k < nz; ++k)
    {
        for(i = 0; i < nx; ++i)
        {
            m = i + (ys + k * nyp) * nx;
            height[i + k * nx] = values[m];
        }
    }

    sx = 1.0/(nx - 1.0);
    sz = 1.25/(nz - 1.0);

    deltx = sx * (contour->get_xEndY() - contour->get_xStartY());
    deltz = sz * (contour->get_zEndX() - contour->get_zStartX());

    sz = 0.125 / (_hmax + 0.01);

    tid = contour->get_yid(ys);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tid);

  //glNormal3d(x,y,z);

    for(k = 0; k < nz - 1; ++k)
    {
        n = nxp;
        glBegin(GL_QUAD_STRIP);
            for(i = 0; i < nx; ++i)
            {
                --n;
                x1 = lon[n + ys * nxp];
                y1 = lat[n + ys * nxp];
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

void WRF_NCL_Viewer::_display_Zplane_on_sphere(int zs)
{
    int i, j, n;
    double* height = NULL;
    double* values = NULL;

    float deltx, delty;
    float sx, sy, sz;

    float s1, t1;
    float x1, y1, z1;

    unsigned int tid;

    double* lon = NULL;
    double* lat = NULL;

    if(geometry->isXstaggered())
    {
        lon = geometry->get_ulon();
        lat = geometry->get_ulat();
    }
    else if(geometry->isYstaggered())
    {
        lon = geometry->get_vlon();
        lat = geometry->get_vlat();
    }
    else
    {
        lon = geometry->get_lon();
        lat = geometry->get_lat();
    }

    if(zs >= nz)
        return;

  //cout << "Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__
  //     << ", file: <" << __FILE__ << ">" << endl;
  //cout << "\tVar name: <" << _varname << ">, level = " << zs << endl;

    sx = 1.0/(nx - 1.0);
    sy = 1.0/(ny - 1.0);
    sz = 1.25/(nz - 1.0);

    deltx = sx * (contour->get_xEnd() - contour->get_xStart());
    delty = sy * (contour->get_yEnd() - contour->get_yStart());

  //glNormal3d(x,y,z);

    values = geometry->get_hgt();
    n = zs * nx * ny;
    height = &values[n];

    sz = 0.125 / (_hmax + 0.01);

    tid = contour->get_zid(zs);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tid);

    for(j = 0; j < ny - 1; ++j)
    {
        n = nxp;
        glBegin(GL_QUAD_STRIP);
        for(i = 0; i < nx; ++i)
        {
            --n;
            x1 = lon[n + j * nxp];
            y1 = lat[n + j * nxp];
            z1 = 0.501 + sz * height[i + j * nx];
            s1 = contour->get_xStart() + i * deltx;
            t1 = contour->get_yStart() + (j + 1) * delty;
            _Vertex(x1, y1, z1, s1, 1.0 - t1);

            x1 = lon[n + (j + 1) * nxp];
            y1 = lat[n + (j + 1) * nxp];
            z1 = 0.501 + sz * height[i + (j + 1) * nx];
            s1 = contour->get_xStart() + i * deltx;
            t1 = contour->get_yStart() + (j + 1) * delty;
            _Vertex(x1, y1, z1, s1, 1.0 - t1);
        }
        glEnd();
    }
    glDisable(GL_TEXTURE_2D);
}

void WRF_NCL_Viewer::_display_Global_on_sphere()
{
    int i, j;

    float deltx, delty;

    float s1, t1, t2;
    float x1, y1, y2;

    unsigned int tid;

    deltx = contour->get_xEnd() - contour->get_xStart();
    delty = contour->get_yEnd() - contour->get_yStart();

  //glNormal3d(x,y,z);

    tid = contour->get_zid(nvoptions->get_zsec());

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tid);

    for(j = -90; j < 90; j += 5)
    {
        y1 = (float) j;
        y2 = y1 + 5.0;
        t1 = contour->get_yStart() + delty * (0.5 + y1/180.0);
        t2 = contour->get_yStart() + delty * (0.5 + y2/180.0);

        glBegin(GL_QUAD_STRIP);
        for(i = 0; i <= 360; i += 5)
        {
            x1 = (float) i;
            s1 = contour->get_xStart() + deltx*x1/360.0;
            _Vertex(x1, y1, 0.505, 1.0 - s1, 1.0 - t1);
            _Vertex(x1, y2, 0.505, 1.0 - s1, 1.0 - t2);
        }
        glEnd();
    }
    glDisable(GL_TEXTURE_2D);
}

void WRF_NCL_Viewer::_marchingCubeSetup()
{
  //Need to do nothing here.
}

void WRF_NCL_Viewer::set_geometry(WRF_Geometry *gm)
{
  //cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">, line: "
  //     << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
  //gm->print();

    geometry = gm;

    contour->set_geometry(gm);

    contour->set_nx(geometry->get_nx());
    contour->set_ny(geometry->get_ny());
    contour->set_nz(geometry->get_nz());
}

void WRF_NCL_Viewer::setup(string vn, double *var)
{
    size_t sz;
    Evaluator evaluator;
    double* hgt = geometry->get_hgt();

  //cout << "\tEnter function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
  //cout << "\tplot var name: " << vn << endl;

    _varname  = vn;
    _var = var;

    nxp = geometry->get_nx();
    nyp = geometry->get_ny();
    nzp = geometry->get_nz();

    nx = nxp;
    if(geometry->isXstaggered())
        --nx;

    ny = nyp;
    if(geometry->isYstaggered())
        --ny;

    nz = nzp;
    if(geometry->isZstaggered())
        --nz;

    sz = nxp * nyp * nzp;
    evaluator.set_value(sz, _var);

    _minval = evaluator.get_min();
    _maxval = evaluator.get_max();

    sz = nx * ny * nz;
    evaluator.set_value(sz, hgt);
    _hmin = evaluator.get_min();
    _hmax = evaluator.get_max();

    if(first_time)
    {
        nvoptions->set_xsec(nxp + 1);
        nvoptions->set_xsec2(nxp + 1);
        nvoptions->set_ysec(nyp + 1);
        nvoptions->set_ysec2(nyp + 1);
        nvoptions->set_zsec(0);
        nvoptions->set_zsec2(nzp + 1);

        first_time = false;
    }

  //cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">, line: "
  //     << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
  //cout << "\tvariable name: <" << _varname << ">" << endl;
  //cout << "\tvariable value: <" << _var[0] << ">" << endl;

    contour->setup(vn, _var);

    _marchingCubeSetup();

  //draw();
  //cout << "\tLeave function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
}

void WRF_NCL_Viewer::set_OP(bool s)
{
    _hasOP = s;
  //contour->reset();
}

//Draw vertex in polar coordinates
void WRF_NCL_Viewer::_Vertex(float lon, float lat, float r, float s, float t)
{
    float x = r*Cos(lon)*Cos(lat);
    float y =          r*Sin(lat);
    float z = r*Sin(lon)*Cos(lat);
  //glNormal3f(x,y,z);
    glTexCoord2f(s, t);
    glVertex3f(x,y,z);
}

void WRF_NCL_Viewer::_point(float x, float y, float z, float s, float t)
{
    glTexCoord2d(s, 1.0 - t);
    glVertex3f(x,y,z);
}

void WRF_NCL_Viewer::_xQuad(float x1, float y1, float z1, float y2, float z2,
                    float s1, float t1, float s2, float t2)
{
    _point(x1, y1, z1, s1, t1);
    _point(x1, y2, z1, s2, t1);
    _point(x1, y2, z2, s2, t2);
    _point(x1, y1, z2, s1, t2);
}

void WRF_NCL_Viewer::_yQuad(float x1, float y1, float z1, float x2, float z2,
                    float s1, float t1, float s2, float t2)
{
    _point(x1, y1, z1, s1, t1);
    _point(x2, y1, z1, s2, t1);
    _point(x2, y1, z2, s2, t2);
    _point(x1, y1, z2, s1, t2);
}

void WRF_NCL_Viewer::_zQuad(float x1, float y1, float x2, float y2, float z2,
                    float s1, float t1, float s2, float t2)
{
    _point(x1, y1, z2, s1, t1);
    _point(x2, y1, z2, s2, t1);
    _point(x2, y2, z2, s2, t2);
    _point(x1, y2, z2, s1, t2);
}

void WRF_NCL_Viewer::set_opacity(int o)
{
  //cout << "Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__
  //     << ", file: <" << __FILE__ << ">" << endl;
  //cout << "\tOpacity = " << o << endl;

    _opacity = o;
    contour->set_opacity(o);
    contour->reset();
    draw();
}

