#include "selecting.h"

Selecting::Selecting()
{
    bufsize = BUFSIZE;
    presize = bufsize;

    buffer = new GLuint[bufsize];

    mode = GL_SELECT;

    nhits = 0;
}

Selecting::~Selecting()
{
    delete [] buffer;
}

void Selecting::processHits()
{
   GLuint i, j, ii, jj;
   GLuint names, *ptr;

   cout << "\nfile: " << __FILE__ << ", line: " << __LINE__
        << ", function: " << __PRETTY_FUNCTION__ << endl;
   cout << "\tnhits = " << nhits << endl;

   ptr = (GLuint *) buffer;

   for(i = 0; i < nhits; ++i)
   {
      names = *ptr;
      printf(" number of names for this hit = %d\n", names);
      ptr++;
      printf(" z1 is %g;", (float) *ptr/0x7fffffff); ptr++;
      printf(" z2 is %g\n", (float) *ptr/0x7fffffff); ptr++;
      printf(" names are: ");
      for(j = 0; j < names; ++j)
      {
         printf ("%d ", *ptr);
         if(j == 0)
            ii = *ptr;
         else if (j == 1)
            jj = *ptr;
         ptr++;

         printf("\tii = %ud, jj = %ud\n", ii, jj);
      }
      printf("\n");
   }
}

void Selecting::start(int button, int state, GLuint x, GLuint y)
{
    GLint viewport[4];

    glGetIntegerv(GL_VIEWPORT, viewport);

    glSelectBuffer(bufsize, buffer);
    (void)glRenderMode(GL_SELECT);

    glInitNames();
    glPushName(0);

  //create 5x5 pixel picking region near cursor location*/
    gluPickMatrix(x, y, 5.0, 5.0, viewport);

    nhits = glRenderMode(GL_RENDER);
    processHits();
} 

