#include "picking.h"

Picking::Picking()
{
    int i;

    screenWidth = 400;
    screenHeight = 400;

    selected = 0;

    cursize = 0;
    maxsize = 1024;
    presize = maxsize;
    buffer = new GLuint[4*maxsize*maxsize];

    hitlist = (GLuint **)malloc(MaxHits*sizeof(GLuint*));
    for(i = 0; i < MaxHits; ++i)
        hitlist[i] = (GLuint *)malloc(2*sizeof(GLuint));
}

Picking::~Picking()
{
    int i;

    delete [] buffer;

    for(i = 0; i < MaxHits; ++i)
        free(hitlist[i]);

    free(hitlist);
}

void Picking::set_screensize(int w, int h)
{
    screenWidth = w;
    screenHeight = h;
}

void Picking::initialize(int x, int y)
{
    GLdouble gdx = (GLdouble) x;
    GLdouble gdy = (GLdouble) y;
  //This choose the buffer where store the values for the selection data
    glSelectBuffer(4*maxsize*maxsize, buffer);

  //This retrieve info about the viewport
    glGetIntegerv(GL_VIEWPORT, viewport);

    gdy = (GLdouble) viewport[3] - y;

  //Switching in selecton mode
    mode = GL_SELECT;
    glRenderMode(mode);

  //Clearing the name's stack
  //This stack contains all the info about the objects
    glInitNames();

  //Now fill the stack with one element (or glLoadName will generate an error)
    glPushName(-1);

    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
    cout << "\tfunction: " << __PRETTY_FUNCTION__ << endl;
    cout << "\tviewport: (" << viewport[0] << "," << viewport[1] << ","
                            << viewport[2] << "," << viewport[3] << ")" << endl;
    cout << "\t(x, y) = (" << x << ", " << y << ") -> (" << gdx << ", " << gdy << ")" << endl;

  //gluPickMatrix(gdx, gdy, 1.0, 1.0, viewport);
  //gluPerspective(60, 1.0, 0.0001, 1000.0);
}

void Picking::processHits()
{
  //get number of objects drawed in that area and return to render mode
    nhits = glRenderMode(GL_RENDER);

  //Print a list of the objects
    printHits();

    _gen_hitlist();
}

void Picking::loadName(int n)
{
  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\tfunction: " << __PRETTY_FUNCTION__ << endl;
  //cout << "\tglLoadName(" << n << ")" << endl;

    glLoadName(n);
}

void Picking::pushName(int n)
{
  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\tfunction: " << __PRETTY_FUNCTION__ << endl;
  //cout << "\t\tglPushName(" << n << ")" << endl;

    glPushName(n);
}

void Picking::printHits()
{
    int i;
    unsigned int j;
    GLuint names, *ptr;

    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
    cout << "\tfunction: " << __PRETTY_FUNCTION__ << endl;

    printf("\nhits = %d\n", nhits);
    ptr = (GLuint *) buffer;

    for(i = 0; i < nhits; ++i)
    {
        names = *ptr;

        printf("\tHit %d has %u names\n", i, names);
        ptr++;

        printf("\tdepth z1 = %g, ", (float) *ptr/0x7fffffff);
        ptr++;

        printf("\tz2 = %g\n", (float) *ptr/0x7fffffff);
        ptr++;

        if(1 < names)
            printf("\tthe names are: ");
        else if(0 < names)
            printf("\tthe name is: ");

        for(j = 0; j < names; ++j)
        {
            printf ("%d ", *ptr);
            ptr++;
        }
        printf ("\n");
    }
}

void Picking::_gen_hitlist()
{
    int i;
    unsigned int j;
    GLuint names, *ptr;

    GLint mhits = nhits;

    if(mhits > MaxHits)
        mhits = MaxHits;

    ptr = (GLuint *) buffer;

    for(i = 0; i < mhits; ++i)
    {
        names = *ptr;
        ptr++;

      //depth z1 = (float) *ptr/0x7fffffff;
        ptr++;

      //depth z2 = (float) *ptr/0x7fffffff;
        ptr++;

        hitlist[i][0] = -1;
        hitlist[i][1] = -1;

        for(j = 0; j < names; ++j)
        {
            if(2 > j)
                hitlist[i][j] = *ptr;
            ptr++;
        }
    }
}

