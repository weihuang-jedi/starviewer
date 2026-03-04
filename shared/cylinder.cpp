#include "cylinder.h"
#include <QOpenGLWidget>

using namespace std;

Cylinder::Cylinder()
{
    color[0] = 0.0;
    color[1] = 0.0;
    color[2] = 1.0;
    color[3] = 1.0;

    x1 = NULL;
    y1 = NULL;
    x2 = NULL;
    y2 = NULL;

    _initialize_sides();

    set_shape(1.0, 0.5, 1.0);
    set_scale(1.0, 1.0, 1.0);
    set_rotation(0.0, 0.0, 0.0, 0.0);
    set_translation(0.0, 0.0, 0.0);

    ++instance;
}

Cylinder::~Cylinder()
{
    if(NULL != x1)
        free(x1);
    if(NULL != y1)
        free(y1);
    if(NULL != x2)
        free(x2);
    if(NULL != y2)
        free(y2);

    x1 = NULL;
    y1 = NULL;
    x2 = NULL;
    y2 = NULL;

    --instance;
}

void Cylinder::set_rotation(double a, double x, double y, double z)
{
    angle = a;
    rx = x;
    ry = y;
    rz = z;
}

void Cylinder::set_scale(double x, double y, double z)
{
    sx = x;
    sy = y;
    sz = z;
}

void Cylinder::set_translation(double x, double y, double z)
{
    tx = x;
    ty = y;
    tz = z;
}

void Cylinder::set_shape(double h, double rd1, double rd2)
{
    int i;
    double angle, delta;

    height = h;
    r1 = rd1;
    r2 = rd2;

  //cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">";
  //cout << "\tline: " << __LINE__;
  //cout << "\tfile: <" << __FILE__ << ">" << endl;
  //cout << "\tinstance = " << instance << endl;
  //cout << "\theight = " << height << ", r1 = " << r1 << ", r2 = " << r2 << endl;

    delta = 2.0 * 3.1415926535897932 / number_of_sides;
    
  //cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">";
  //cout << "\tline: " << __LINE__;
  //cout << "\tfile: <" << __FILE__ << ">" << endl;
  //cout << "\tnumber_of_sides: <" << number_of_sides << ">" << endl;

    for(i = 0; i < number_of_sides; i++)
    {
        angle = delta * i;
        x1[i] = r1 * cos(angle);
        y1[i] = r1 * sin(angle);
        x2[i] = r2 * cos(angle);
        y2[i] = r2 * sin(angle);

      //cout << "\t\tx1[" << i << "] = " << x1[i] << ", y1[" << i << "] = " << y1[i] << endl;
      //cout << "\t\tx2[" << i << "] = " << x2[i] << ", y2[" << i << "] = " << y2[i] << endl;
    }

    x1[number_of_sides] = x1[0];
    y1[number_of_sides] = y1[0];
    x2[number_of_sides] = x2[0];
    y2[number_of_sides] = y2[0];
}

void Cylinder::set_color(float c[4])
{
    memcpy(color, c, 4*sizeof(float));
}

void Cylinder::set_sides(int n)
{
    int i;
    double angle, delta;

  //cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">";
  //cout << "\tline: " << __LINE__;
  //cout << "\tfile: <" << __FILE__ << ">" << endl;
  //cout << "\tn: <" << n << ">" << endl;
  //cout << "\theight = " << height << ", r1 = " << r1 << ", r2 = " << r2 << endl;

    if(n <= number_of_sides)
        return;

    number_of_sides = n;

    x1 = (double *)realloc(x1, (number_of_sides + 1) * sizeof(double));
    y1 = (double *)realloc(y1, (number_of_sides + 1) * sizeof(double));
    x2 = (double *)realloc(x2, (number_of_sides + 1) * sizeof(double));
    y2 = (double *)realloc(y2, (number_of_sides + 1) * sizeof(double));

    assert(x1);
    assert(y1);
    assert(x2);
    assert(y2);

    delta = 2.0 * 3.1415926535897932 / number_of_sides;
    
    for(i = 0; i < number_of_sides; i++)
    {
        angle = delta * i;
        x1[i] = r1 * cos(angle);
        y1[i] = r1 * sin(angle);
        x2[i] = r2 * cos(angle);
        y2[i] = r2 * sin(angle);
    }

    x1[number_of_sides] = x1[0];
    y1[number_of_sides] = y1[0];
    x2[number_of_sides] = x2[0];
    y2[number_of_sides] = y2[0];
}

void Cylinder::draw()
{
    int i;

  //cout << "\nEnter functions: <" << __PRETTY_FUNCTION__ << ">";
  //cout << "\tline: " << __LINE__;
  //cout << "\tfile: <" << __FILE__ << ">" << endl;
  //cout << "\tnumber_of_sides = " << number_of_sides << endl;

    if((NULL == x1) || (NULL == y1) || (NULL == x2) || (NULL == y2))
    {
#if 0
        cout << "At least one of x1, x2, y1, or y2 is NULL." << endl;
#endif
        return;
    }

    glPushMatrix();

    glTranslatef(tx, ty, tz);
    glScalef(sx, sy, sz);
    glRotatef(angle, rx, ry, rz);

    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1,1);

    glColor4fv(color);
  //cout << "\tinstance = " << instance << endl;

    glBegin(GL_QUAD_STRIP);
        for(i = 0; i <= number_of_sides; i++)
        {
          //cout << "\t\tx1[" << i << "] = " << x1[i] << ", y1[" << i << "] = " << y1[i] << endl;
          //cout << "\t\tx2[" << i << "] = " << x2[i] << ", y2[" << i << "] = " << y2[i] << endl;

            glVertex3d(x1[i],   y1[i],   0.0);
            glVertex3d(x2[i],   y2[i],   height);
        }
    glEnd();

    glDisable(GL_POLYGON_OFFSET_FILL);

    glPopMatrix();

  //cout << "\tinstance = " << instance << endl;
  //cout << "\nLeave functions: <" << __PRETTY_FUNCTION__ << ">";
  //cout << "\tline: " << __LINE__;
  //cout << "\tfile: <" << __FILE__ << ">\n" << endl;
}

void Cylinder::_initialize_sides()
{
    number_of_sides = 36;

    x1 = (double *)calloc(number_of_sides + 1, sizeof(double));
    y1 = (double *)calloc(number_of_sides + 1, sizeof(double));
    x2 = (double *)calloc(number_of_sides + 1, sizeof(double));
    y2 = (double *)calloc(number_of_sides + 1, sizeof(double));

    assert(x1);
    assert(y1);
    assert(x2);
    assert(y2);
}

int Cylinder::instance = 0;

