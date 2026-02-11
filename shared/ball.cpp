#include <QtOpenGL>
#include "ball.h"

//Constructor
Ball::Ball()
{
    radius = 0.03;
}

Ball::~Ball()
{
}

//Draw vertex in polar coordinates
void Ball::_ballVertex(float lon, float lat, float radius)
{
    float x = radius*Cos(lon)*Cos(lat);
    float y =          radius*Sin(lat);
    float z = radius*Sin(lon)*Cos(lat);
    glNormal3f(x,y,z);
    glVertex3f(x,y,z);
}

void Ball::draw()
{
    draw(radius);
}

void Ball::draw(float r)
{
    int i, j;

    float lon, lat0, lat1;

  //glColor4f(1.0, 0.0, 0.0, 0.75);

    for(j = -90; j < 90; j += 5)
    {
        lat0 = (float) j;
        lat1 = lat0 + 5.0;

        glBegin(GL_QUAD_STRIP);
        for(i = 0; i <= 360; i += 5)
        {
            lon = (float) i;
            _ballVertex(lon, lat0, r);
            _ballVertex(lon, lat1, r);
        }
        glEnd();
    }
}

