#include "noise.h"

Noise::Noise()
{
    start = false;
    B = 8;
    BM = B - 1;
    theNoise = NULL;
    nx = 0;
    ny = 0;
    nz = 0;
}

Noise::~Noise()
{
    if(NULL != theNoise)
        delete [] theNoise;
}

//Normalize 3D vector
void Noise::normalize3(double v[3])
{
   double s;

   s = sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
   v[0] /=  s;
   v[1] /=  s;
   v[2] /=  s;
}

//Initialize Noise
void Noise::InitNoise()
{
   int i,j,k;

   srand(30757);
   for (i=0;i<B;i++)
   {
      p[i] = i;
      for (j=0;j<3;j++)
         g3[i][j] = (double)((rand() % (B + B)) - B) / B;
      normalize3(g3[i]);
   }

   while (--i)
   {
      k = p[i];
      p[i] = p[j = rand() % B];
      p[j] = k;
   }

   for (i=0;i<B+2;i++)
   {
      p[B+i] = p[i];
      for (j=0;j<3;j++)
         g3[B+i][j] = g3[i][j];
   }
}

//Set noise frequency
void Noise::SetNoiseFrequency(int frequency)
{
   start = true;
   B = frequency;
   BM = B-1;
}

double Noise::s_curve(double t)
{
    return ( t * t * (3. - 2. * t) );
}

double Noise::lerp(double t, double a, double b)
{
    return ( a + t * (b - a) );
}

double Noise::at3(double rx, double ry, double rz, double q[3])
{
    return ( rx * q[0] + ry * q[1] + rz * q[2] );
}

void Noise::set_val(int i, double &t, int &b0, int &b1,
                    double &r0, double &r1, double vec[3])
{
    t  = vec[i] + NNNN;
    b0 = ((int)t) & BM;
    b1 = (b0+1) & BM;
    r0 = t - (int)t;
    r1 = r0 - 1.;
}

//3D noise
double Noise::noise3(double vec[3])
{
   int i, j;
   int bx0, bx1, by0, by1, bz0, bz1, b00, b10, b01, b11;
   double rx0, rx1, ry0, ry1, rz0, rz1, sy, sz, a, b, c, d, t, u, v;

   if(start)
   {
      start = false;
      InitNoise();
   }

   set_val(0, t, bx0, bx1, rx0, rx1, vec);
   set_val(1, t, by0, by1, ry0, ry1, vec);
   set_val(2, t, bz0, bz1, rz0, rz1, vec);

   i = p[bx0];
   j = p[bx1];

   b00 = p[i + by0];
   b10 = p[j + by0];
   b01 = p[i + by1];
   b11 = p[j + by1];

   t  = s_curve(rx0);
   sy = s_curve(ry0);
   sz = s_curve(rz0);

   u = at3(rx0, ry0, rz0, g3[b00 + bz0]);
   v = at3(rx1, ry0, rz0, g3[b10 + bz0]);
   a = lerp(t,u,v);

   u = at3(rx0, ry1, rz0, g3[b01 + bz0]);
   v = at3(rx1, ry1, rz0, g3[b11 + bz0]);
   b = lerp(t, u, v);

   c = lerp(sy,a,b);

   u = at3(rx0, ry0, rz1, g3[b00 + bz1]);
   v = at3(rx1, ry0, rz1, g3[b10 + bz1]);
   a = lerp(t, u, v);

   u = at3(rx0, ry1, rz1, g3[b01 + bz1]);
   v = at3(rx1, ry1, rz1, g3[b11 + bz1]);
   b = lerp(t, u, v);

   d = lerp(sy,a,b);

   return lerp(sz,c,d);
}

void Noise::setup(int ix, int iy, int iz)
{
    nx = ix;
    ny = iy;
    nz = iz;
    theNoise = (double *) malloc(nx * ny * nz * sizeof(double));
    assert(theNoise);

  //cout << "\nEnter functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
  //cout << "\tnx = " << nx << ", ny = " << ny << ", nz = " << nz << endl;

    createNoise3D();

  //cout << "Leave functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
}

//
//  Create 3D noise texture
//
void Noise::createNoise3D()
{
 //cout << "\nEnter functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;

   int f,i,j,k,inc;
   int numOctaves = 4;
   double ni[3];
   double inci,incj,inck;
   int frequency = 4;
   double amp = 0.5;
   char* ptr;

 //cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;

   int grid_size = nx * ny * nz;
   int char_size = 4 * nx * ny * nz;
   int n = 0;

 //cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
 //cout << "\tgrid_size = " << grid_size << endl;
 //cout << "\tchar_size = " << char_size << endl;

 //char pix[grid_size * 4];
 //char pix[char_size];
   char *pix = (char *)calloc(char_size, 1);

 //cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;

   for (f=0,inc=0; f<numOctaves; f++,frequency*=2,inc++,amp*=0.5)
   {
      SetNoiseFrequency(frequency);
      ptr = pix;
      ni[0] = ni[1] = ni[2] = 0;

      if(inc%2)
         frequency /= 2;

      inck = 1.0 / (nz / frequency);
      for(k = 0; k < nz; ++k, ni[2] += inck)
      {
         incj = 1.0 / (ny / frequency);
         for(j = 0; j < ny; ++j, ni[1] += incj)
         {
            inci = 1.0 / (nx / frequency);
            for(i = 0; i < nx; ++i, ni[0] += inci, ptr += 4)
               *(ptr + inc) = (char) (((noise3(ni) + 1.0) * amp) * 128.0);
         }
      }
   }

   for(n = 0; n < grid_size; ++n)
       theNoise[n] = (double) pix[n];
     //theNoise[n] = (double) pix[4*n];

   free(pix);

 //cout << "Leave functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
}

