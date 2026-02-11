#include "lic.h"

#include <QtOpenGL>

#include <iostream>

using namespace std;

LineIntegralConvolution::LineIntegralConvolution(ColorTable* ct, NVOptions* opt)
{
    colorTable = ct;
    nvoptions = opt;

  //cout << "\nEnter functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;

    _nx = 1;
    _ny = 1;
    _nz = 1;

    maxvecmag = 0.0;

    _wings = 10.0;
    _stepsize = 5;
  //_maxvalue = 100.0;
  //_maxvalue = 50.0;
    _maxvalue = 255.0;

    lister = new Lister();
  //earth = new Earth();

    noise = new Noise();

    grid = NULL;

    first_time = true;

  //cout << "Leave functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
}

LineIntegralConvolution::~LineIntegralConvolution()
{
    delete lister;
    delete noise;

    if(NULL != grid)
        delete grid;
}

void LineIntegralConvolution::draw()
{
  //cout << "\nEnter functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
  //cout << "\t_nx = " << _nx << ", _ny = " << _ny << ", _nz = " << _nz << endl;
  //cout << "\tnvoptions->get_xsec() = " << nvoptions->get_xsec() << endl;
  //cout << "\tnvoptions->get_ysec() = " << nvoptions->get_ysec() << endl;
  //cout << "\tnvoptions->get_zsec() = " << nvoptions->get_zsec() << endl;

    glPushMatrix();

    _colorlen = colorTable->get_clen() - 3;
    _colormap = colorTable->get_cmap();

    //caleNtranslate();

    glLineWidth(1.5);

    if((_nx > nvoptions->get_xsec()) && (_nz > 1))
    {
        xcl = lister->get_xid(nvoptions->get_xsec());

        if(xcl)
            glCallList(xcl);
        else
            _display_Xplane();
    }

    if((_ny > nvoptions->get_ysec()) && (_nz > 1))
    {
        ycl = lister->get_yid(nvoptions->get_ysec());

        if(ycl)
            glCallList(ycl);
        else
            _display_Yplane();
    }

    if(_nz > nvoptions->get_zsec())
    {
        zcl = lister->get_zid(nvoptions->get_zsec());

        if(zcl)
            glCallList(zcl);
        else
            _display_Zplane();
    }

    glPopMatrix();

  //cout << "Leave functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
}

void LineIntegralConvolution::setup(int ix, int iy, int iz,
                                    float* u, float* v, float*w)
{
    int nxyz = ix*iy*iz;
    int i;

    _nx = ix;
    _ny = iy;
    _nz = iz;

    _u = u;
    _v = v;
#if 1
    for(i = 0; i < nxyz; ++i)
        w[i] = 0.00001*w[i];
#endif
    _w = w;

  //cout << "\nEnter functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;

    lister->setup(_nx, _ny, _nz);

  //cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;

    _parameter_setup();

  //cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;

    if(first_time)
    {
        nvoptions->set_xsec(_nx + 1);
        nvoptions->set_ysec(_ny + 1);
        nvoptions->set_zsec(0);

        first_time = false;
    }

    grid = new RegGrid(colorTable);
    grid->setup(_nx, _ny);

  //cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;

    noise->setup(_nx, _ny, _nz);
  //cout << "Leave functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
}

void LineIntegralConvolution::_generate_lic()
{
    double I0;
    double I;
    int i, j, m;
    StreamLine s;
    int nsum=0, tmpsum=0;
    int ic = (_nx + 1)/2;
    int jc = (_ny + 1)/2;

    Point* fwd;
    Point* bwd;

  //cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">, line: "
  //     << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
  //

    _setup_zPlane();

    s.set_nx(_nx);
    s.set_ny(_ny);
    s.set_vecdata(grid->vecdata);

    for(j = _stepsize/2; j < _ny; j += _stepsize)
    {
        for(i = _stepsize/2; i < _nx; i += _stepsize)
        {
            if(grid->hitdata[j][i] < minNumHits)
            {
	        s.GenStreamLine(i-ic,j-jc);
	      //s.GenStreamLine(i,j);

	        I = I0 = grid->ComputeI(s,nsum);
	        tmpsum = nsum;
	        for(m = 1; m < grid->get_maxsteps(); ++m) 
                    grid->ComputeIFwd(s,I,m,tmpsum);

	        I = I0;
	        tmpsum = nsum;

    	        for(m = 1; m < grid->get_maxsteps(); ++m)
    	            grid->ComputeIBwd(s,I,-m,tmpsum);

                fwd = s.get_fwd_points();
                _set_color(grid->Idata[j][i]);
    	        _draw_line(fwd, grid->get_maxsteps());

                bwd = s.get_bwd_points();
                _set_color(grid->Idata[j][i]);
    	        _draw_line(bwd, grid->get_maxsteps());
            }
        }
    }
}

void LineIntegralConvolution::_parameter_setup()
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
    {
        _zDelt = 1.0 / (_nz - 1.0);
        _zStart = 0.0;
    }
    else
    {
        _zDelt = 1.0;
        _zStart = 0.0;
    }

    _zDelt  = 1000.0;
    _xyDelt *= 2.0;
    _xStart -= 0.5;
    _yStart -= 0.5;
    _zStart  = 0.0125;

  //cout << "Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__
  //     << ", file: <" << __FILE__ << ">" << endl;
  //cout << "\t_nx = " << _nx << ", _ny = " << _ny << ", _nz = " << _nz << endl;
}

void LineIntegralConvolution::_display_Xplane()
{
#if 0
    int j, k, n;
    float x, y, z;

    x = _xStart + nvoptions->get_xsec() * _xyDelt;

    for(k = 0; k < _nz; ++k)
    {
        z = k * _zDelt;

        for(j = _stepsize/2; j < _ny; j += _stepsize)
        {
            y = _yStart + j * _xyDelt;
            n = nvoptions->get_xsec() + (j + k * _ny) * _nx;
          //_draw_arrow(x, y, z, _u[n], _v[n], _w[n]);
        }
    }
#endif
}

void LineIntegralConvolution::_display_Yplane()
{
#if 0
    int i, k, n;
    float x, y, z;

    y = _yStart + nvoptions->get_ysec() * _xyDelt;

    for(k = 0; k < _nz; ++k)
    {
        z = k * _zDelt;

        for(i = _stepsize/2; i < _nx; i += _stepsize)
        {
            x = _xStart + i * _xyDelt;
            n = i + (nvoptions->get_ysec() + k * _ny) * _nx;
          //_draw_arrow(x, y, z, _u[n], _v[n], _w[n]);
        }
    }
#endif
}

void LineIntegralConvolution::_display_Zplane()
{
    zcl = glGenLists(1);
  //glNewList(zcl, GL_COMPILE);
  //glNewList(zcl, GL_COMPILE_AND_EXECUTE);
  //lister->set_zid(nvoptions->get_zsec(), zcl);

  //cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">, line: "
  //     << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
  //cout << "\tzcl[" << nvoptions->get_zsec() << "] = " << zcl << endl;

    glPopMatrix();

  //scaleNtranslate();

    _generate_lic();

    glPopMatrix();

    glEndList();
}

void LineIntegralConvolution::_set_color(Point p)
{
    float color[3];
    int idx = (int) ((p.size() * _colorlen) / _maxvalue);
 
    if(idx > (_colorlen - 1))
        idx = _colorlen - 1;

    color[0]  = _colormap[3*(idx+3)];
    color[1]  = _colormap[3*(idx+3) + 1];
    color[2]  = _colormap[3*(idx+3) + 2];

    glColor3fv(color);
}

void LineIntegralConvolution::_set_color(double spd)
{
    float color[3];
    int idx = (int) ((spd * _colorlen) / _maxvalue);

    if(idx > (_colorlen - 1))
        idx = _colorlen - 1;

    color[0]  = _colormap[3*(idx+3)];
    color[1]  = _colormap[3*(idx+3) + 1];
    color[2]  = _colormap[3*(idx+3) + 2];

    glColor3fv(color);
}

void LineIntegralConvolution::_point2xyz(Point p, double &x, double &y, double &z)
{
    x = p.coord[0] * _xyDelt + _xStart;
    y = p.coord[1] * _xyDelt + _yStart;
    z = p.coord[2] * _zDelt  + _zStart;
}

void LineIntegralConvolution::_draw_line(Point p1, Point p2)
{
    double x1, y1, z1, x2, y2, z2;

    _point2xyz(p1, x1, y1, z1);
    _point2xyz(p1, x2, y2, z2);

    glBegin( GL_LINE_STRIP );
        _set_color(p1);
        glVertex3f(x1, y1, z1);
        _set_color(p2);
        glVertex3f(x2, y2, z2);
    glEnd( );
}

void LineIntegralConvolution::_draw_line(Point* p, int n)
{
    int i, j, m;
  //double u, v, w, spd;
    double x, y, z;

  //cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">, line: "
  //     << __LINE__ << ", file: <" << __FILE__ << ">" << endl;

    glBegin( GL_LINE_STRIP );
        for(m = 0; m < n; ++m)
        {
          //_set_color(p[i]);
          //cout << "\tp: " << p[m].coord[0]/nx - 0.5 << ", "
          //                << p[m].coord[1]/ny - 0.5 << ", "
          //                << p[m].coord[2] << endl;

            if(grid->getPixel(p[m], i, j))
            {
              //u = grid->vecdata[j][i].coord[0];
              //v = grid->vecdata[j][i].coord[1];
              //w = grid->vecdata[j][i].coord[2];
              //spd = sqrt(u * u + v * v + w * w);
              //_set_color(spd);
              //glColor3d((double) i / _nx, (double) j / _ny, (double) nvoptions->get_zsec() / _nz);
                _set_color(grid->Idata[j][i]);
                _point2xyz(p[m], x, y, z);
                glVertex3d(x, y, z);
            }
        }
    glEnd( );
}

void LineIntegralConvolution::_setup_zPlane()
{
    int i, j, n;
    double* noisePtr = noise->get_noise();

    _maxvalue = noisePtr[0];
    
    for(j = 0; j < _ny; ++j)
    {
        n = (nvoptions->get_zsec()*_ny + j)*_nx;
        for(i = 0; i < _nx; ++i)
        {
            grid->vecdata[j][i].coord[0] = _u[n+i];
            grid->vecdata[j][i].coord[1] = _v[n+i];
            grid->vecdata[j][i].coord[2] = _w[n+i];

            grid->texdata[j][i] = 0;
            grid->hitdata[j][i] = 0;
            grid->Idata[j][i]   = noisePtr[n+i];

            if(_maxvalue < noisePtr[n+i])
	        _maxvalue = noisePtr[n+i];
        }

    }

    grid->set_maxvecmag(50.0);
}

void LineIntegralConvolution::scaleNtranslate()
{
    glScalef(1.0, 1.0, 0.4);

    if(_nz > 1)
    {
        glTranslatef(-0.5, -0.5, -0.5);
    }
    else
    {
        glTranslatef(-0.5, -0.5, 0.0);
    }
}

