#include <QOpenGLWidget>

#include "colorBar.h"

//
//  Constructor
//
ColorBar::ColorBar(ColorTable* ct)
{
    colorTable = ct;

    texture1d = new Texture1d();
    texture1d->set_colors(ct->get_clen(), ct->get_cmap());
    texture1d->set_name(ct->get_name());

    _id = -1;
 
    _isPerspective = true;

    sWidth = 900;
    sHeight = 600;
}

ColorBar::~ColorBar()
{
    delete texture1d;
}

void ColorBar::reset()
{
    if(0 <= _id)
        glDeleteLists(_id, 1);

    _id = -1;
}

void ColorBar::update_colortable(ColorTable* ct)
{
  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__
  //     << ", function: " << __PRETTY_FUNCTION__ << endl;

    colorTable = ct;
    delete texture1d;

    texture1d = new Texture1d();
    texture1d->set_colors(ct->get_clen(), ct->get_cmap());
    texture1d->set_name(ct->get_name());

    reset();
}

void ColorBar::generate_colorBar()
{
    int n;
    double f, s, a;
    double x, y;

    int maxLev = 10;

  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__
  //     << ", function: " << __PRETTY_FUNCTION__ << endl;

  //a = 2.0 * sWidth / 1000.0;
    a = 2.0;

    _id = glGenLists(1);
    glNewList(_id, GL_COMPILE_AND_EXECUTE);

  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__
  //     << ", function: " << __PRETTY_FUNCTION__ << endl;
  //cout << "\t_id = " << _id << endl;

    glPushMatrix();

    glDisable(GL_LIGHTING);

    glEnable(GL_TEXTURE_1D);
    glBindTexture(GL_TEXTURE_1D, texture1d->get_textureID());

  //glColor4f(1.0, 1.0, 1.0, 1.0);
    glColor4f(0.0, 0.0, 0.0, 0.0);

    s = 1.0 / (double) maxLev;
    glBegin(GL_QUAD_STRIP);
    for(n = 0; n <= maxLev; ++n)
    {
        f = s * (double) n;
        x = a * (f - 0.5);
        y = -0.600;
        glNormal3d(0.0, 0.0, -1.0);
        glTexCoord1d(f);
        glVertex2d(x, y);

        y = -0.575;
        glNormal3d(0.0, 0.0, -1.0);
        glTexCoord1d(f);
        glVertex2d(x, y);
    }
    glEnd();

    glDisable(GL_TEXTURE_1D);

    glPopMatrix();

    glEndList();
}

void ColorBar::set_widthNheight(int w, int h)
{
  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__
  //     << ", function: " << __PRETTY_FUNCTION__ << endl;

    sWidth = w;
    sHeight = h;

    reset();
}

void ColorBar::draw()
{
  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__
  //     << ", function: " << __PRETTY_FUNCTION__ << endl;

    if(0 > _id)
        generate_colorBar();
    else
        glCallList(_id);
}

void ColorBar::draw(int w, int h)
{
    set_widthNheight(w, h);
    draw();
}

