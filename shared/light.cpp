#include "light.h"

Light::Light()
{
    longitude = 0.0;  // Light longitude
    latitude  = 0.0;  // Light latitude
    altitude  = 1.0;  // Light altitude

  //Light position and colors
    
    set_emission(0.1,0.1,0.1,1.0);
    set_ambient(0.3,0.3,0.3,1.0);
    set_diffuse(0.8,0.8,0.8,1.0);
    set_specular(1.0,1.0,1.0,1.0);
    set_shinyness(16.0);

    set_longitude(longitude);
    set_latitude(latitude);
    set_altitude(altitude);

    size = 0.10;
    ball = new Ball();
    ball->set_radius(size);

    lightOn = false;
    drawBall = false;
}

Light::~Light()
{
    delete ball;
}

void Light::set_ballsize(float s)
{
    size = s;
    ball->set_radius(size);
}

void Light::set_emission(float r, float g, float b, float a)
{
    emission[0] = r;
    emission[1] = g;
    emission[2] = b;
    emission[3] = a;
}

void Light::set_ambient(float r, float g, float b, float a)
{
    ambient[0] = r;
    ambient[1] = g;
    ambient[2] = b;
    ambient[3] = a;
}

void Light::set_diffuse(float r, float g, float b, float a)
{
    diffuse[0] = r;
    diffuse[1] = g;
    diffuse[2] = b;
    diffuse[3] = a;
}

void Light::set_specular(float r, float g, float b, float a)
{
    specular[0] = r;
    specular[1] = g;
    specular[2] = b;
    specular[3] = a;
}

void Light::set_altitude(float alt)
{
    altitude = alt;
    set_position();
}

void Light::set_latitude(float lat)
{
    latitude = lat;
    set_position();
}

void Light::set_longitude(float lon)
{
    longitude = lon;
    set_position();
}

void Light::set_position()
{
    float delt = altitude * Cos(latitude);

    position[0] = delt * Sin(longitude);
    position[1] = altitude * Sin(latitude);
    position[2] = delt * Cos(longitude);
    position[3] = 1.0;
}

void Light::set_shinyness(float s)
{
    shinyness[0] = s;
}

void Light::draw()
{
  //cout << "Functions: <" << __PRETTY_FUNCTION__
  //     << ">, line: " << __LINE__
  //     << ", file: <" << __FILE__ << ">" << endl;
  //cout << "\tlongitude = " << longitude
  //     << ", latitude = " << latitude
  //     << ", altitude = " << altitude << endl;

  //OpenGL should normalize normal vectors
    glEnable(GL_NORMALIZE);

    if(drawBall)
    {
      //Draw light position as sphere (still no light here)
        glColor3f(1,1,1);
        glPushMatrix();
        glTranslatef(position[0],position[1],position[2]);

        ball->draw(size);

        glPopMatrix();
    }

  //Enable lighting
    glEnable(GL_LIGHTING);

  //glColor sets ambient and diffuse color materials
    glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);

  //Enable light 0
    glEnable(GL_LIGHT0);

  //Set ambient, diffuse, specular components and position of light 0
    glLightfv(GL_LIGHT0, GL_AMBIENT,  ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
    glLightfv(GL_LIGHT0, GL_POSITION, position);

  //Set materials
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shinyness);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION,  emission);
}

