#include <QtOpenGL>
#include <earth.h>

#define Cos(th) cos(3.1416*(th)/180.0)
#define Sin(th) sin(3.1416*(th)/180.0)

//
//  Constructor
//
Earth::Earth()
{
    strcpy(_bmpflnm, getenv("NV_DATA"));
    strcat(_bmpflnm, "/data/earth.bmp");

    _loadTexBMP();

    radius = 1.0;
    lon = NULL;
    lat = NULL;
    ter = NULL;

    deg2arc = 3.1415926535897932 / 180.0;

    read_terrain();
}

Earth::Earth(const char *flnm)
{
    strcpy(_bmpflnm, flnm);
    _loadTexBMP();
}

Earth::~Earth()
{
    if(NULL != lon)
       free(lon);
    if(NULL != lat)
       free(lat);
    if(NULL != ter)
       free(ter);
}

void Earth::_fatal(const char* format , ...)
{
   va_list args;
   va_start(args,format);
   vfprintf(stderr,format,args);
   va_end(args);
}

/*
 *  Function to print any errors encountered
 */
void Earth::_errCheck(const char *where, const char *fl, int ln)
{
   int err = glGetError();
   if(err)
      fprintf(stderr,"ERROR: [%s], in file: %s, line: %d\n",
                      where, fl, ln);
 /*
  *   fprintf(stderr,"ERROR: %s [%s], in file: %s, line: %d\n",
  *                   gluErrorString(err),
  *                   where, fl, ln);
  */
}

/*
 *  Load texture from BMP file
 */
void Earth::_loadTexBMP()
{
    QImage t;
    QImage b;
    GLuint textureID = 0;

  //int    rv = -1;
  //rv = b.load(_bmpflnm);

    b.load(_bmpflnm);

  /*
   *fprintf(stderr, "\nfile: %s, line: %d\n", __FILE__, __LINE__);
   *fprintf(stderr, "\trv = %d\n", rv);
   */

  /*
   *if(! rv)
   *{
   *    b = QImage( 16, 16, 32 );
   *    b.fill( Qt::green.rgb() );
   *}
   */

    glEnable(GL_TEXTURE_2D);
    
    t = QGLWidget::convertToGLFormat( b );

  /*
   *fprintf(stderr, "\nfile: %s, line: %d\n", __FILE__, __LINE__);
   *fprintf(stderr, "\tt.width()  = %d\n", t.width());
   *fprintf(stderr, "\tt.height() = %d\n", t.height());
   */

    glGenTextures(1, &textureID);

  /*
   *fprintf(stderr, "\nfile: %s, line: %d\n", __FILE__, __LINE__);
   *fprintf(stderr, "\ttextureID = %d\n", textureID);
   */

    // set texture name
    set_texture_id(textureID);

    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); 
    glTexImage2D(GL_TEXTURE_2D, 0, 3, t.width(), t.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, t.bits());
    glGenerateMipmap(GL_TEXTURE_2D);  //Generate mipmaps now!!!
    glDisable(GL_TEXTURE_2D);
}

/*
 *  Draw vertex in polar coordinates
 */
void Earth::_Vertex(int th, int ph)
{
   double x = radius*Cos(th)*Cos(ph);
   double y =         radius*Sin(ph);
   double z = radius*Sin(th)*Cos(ph);
   glNormal3d(x,y,z);
   glTexCoord2d(0.5*(1.0 - th/180.0), 0.5+ph/180.0);
   glVertex3d(x,y,z);
}

/*
 *  Draw earth
 */
void Earth::draw(float r)
{
    float sr = radius;
    radius = r;
    draw();
    radius = sr;
}

/*
 *  Draw earth
 */
void Earth::draw()
{
    int th,ph;

  /*
   *  Draw surface of the planet
   */
    //  Set texture
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, get_texture_id());
    //  Latitude bands
    glColor3f(1,1,1);
    for(ph = -90; ph < 90; ph += 5)
    {
       glBegin(GL_QUAD_STRIP);
           for(th = -180; th <= 180; th += 5)
           {
               _Vertex(th,ph);
               _Vertex(th,ph+5);
           }
       glEnd();
    }

    glDisable(GL_TEXTURE_2D);
}

/*
 *  Draw earth
 */
void Earth::draw_plane(float z)
{
    int i, j;
    float x, y0, y1;

  //Set texture
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, get_texture_id());

  //Latitude bands
    glColor3f(1,1,1);
    for(j = 0; j < nlat - 1; ++j)
    {
       y0 = lat[j] / 180.0;
       y1 = lat[j + 1] / 180.0;

       glBegin(GL_QUAD_STRIP);
       for(i = 0; i < nlon/2; ++i)
       {
           x = lon[i] / 180.0;
           glNormal3d(0.0, 0.0, 1.0);
           glTexCoord2d(0.5*x + 1.0, y0);
           glVertex3d(x, y0, z);

           glNormal3d(0.0, 0.0, 1.0);
           glTexCoord2d(0.5*x + 1.0, y1);
           glVertex3d(x, y1, z);
       }

       x = 0.0;
       glNormal3d(0.0, 0.0, 1.0);
       glTexCoord2d(0.5*x + 1.0, y0);
       glVertex3d(x, y0, z);

       glNormal3d(0.0, 0.0, 1.0);
       glTexCoord2d(0.5*x + 1.0, y1);
       glVertex3d(x, y1, z);

       glEnd();

       glBegin(GL_QUAD_STRIP);
       x = 0.0;
       glNormal3d(0.0, 0.0, 1.0);
       glTexCoord2d(0.5*x, y0);
       glVertex3d(x, y0, z);

       glNormal3d(0.0, 0.0, 1.0);
       glTexCoord2d(0.5*x, y1);
       glVertex3d(x, y1, z);

       for(i = nlon/2; i < nlon; ++i)
       {
           x = lon[i] / 180.0;
           glNormal3d(0.0, 0.0, 1.0);
           glTexCoord2d(0.5*x, y0);
           glVertex3d(x, y0, z);

           glNormal3d(0.0, 0.0, 1.0);
           glTexCoord2d(0.5*x, y1);
           glVertex3d(x, y1, z);
       }
       glEnd();
    }

    glDisable(GL_TEXTURE_2D);
}

void Earth::bump_plane(float z)
{
    int i, j;
    float x, y0, y1;

    float hgt = 0.0;
    float scl = 0.1 / maxhgt;

  //Set texture
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, get_texture_id());

  //OpenGL should normalize normal vectors
  //glEnable(GL_NORMALIZE);

#if 1
    for(j = 0; j < nlat - 1; ++j)
    {
       y0 = lat[j] / 180.0;
       y1 = lat[j + 1] / 180.0;

       glBegin(GL_QUAD_STRIP);
           hgt = ter[j*nlon];
           glNormal3d(0.0, 0.0, 1.0);
           glTexCoord2d(0.0, y0 + 0.5);
           if(hgt > 0.0)
               glVertex3d(-1.0, y0, z + scl*hgt);
           else
               glVertex3d(-1.0, y0, z);

           glNormal3d(0.0, 0.0, 1.0);
           glTexCoord2d(0.0, y1 + 0.5);
           if(hgt > 0.0)
               glVertex3d(-1.0, y1, z + scl*hgt);
           else
               glVertex3d(-1.0, y1, z);

           for(i = 0; i < nlon; ++i)
           {
               x = lon[i] / 180.0;
               glNormal3d(0.0, 0.0, 1.0);
               glTexCoord2d(0.5*(x + 1.0), y0 + 0.5);
               hgt = ter[j*nlon + i];
               if(hgt > 0.0)
                   glVertex3d(x, y0, z + scl*hgt);
               else
                   glVertex3d(x, y0, z);

               glNormal3d(0.0, 0.0, 1.0);
               glTexCoord2d(0.5*(x + 1.0), y1 + 0.5);
               hgt = ter[(j+1)*nlon + i]; 
               if(hgt > 0.0)
                   glVertex3d(x, y1, z + scl*hgt);
               else
                   glVertex3d(x, y1, z);
           }

           glNormal3d(0.0, 0.0, 1.0);
           glTexCoord2d(1.0 , y0 + 0.5);
           if(hgt > 0.0)
               glVertex3d(1.0, y0, z + scl*hgt);
           else
               glVertex3d(1.0, y0, z);

           glNormal3d(0.0, 0.0, 1.0);
           glTexCoord2d(1.0 , y1 + 0.5);
           if(hgt > 0.0)
               glVertex3d(1.0, y1, z + scl*hgt);
           else
               glVertex3d(1.0, y1, z);
       glEnd();
    }
#else
    for(j = 0; j < nlat - 1; ++j)
    {
       y0 = lat[j] / 180.0;
       y1 = lat[j + 1] / 180.0;

       glBegin(GL_QUAD_STRIP);
           hgt = ter[j*nlon];
           glNormal3d(0.0, 0.0, 1.0);
           glTexCoord2d(0.5, y0 + 0.5);
           if(hgt > 0.0)
               glVertex3d(0.0, y0, z + scl*hgt);
           else
               glVertex3d(0.0, y0, z);

           glNormal3d(0.0, 0.0, 1.0);
           glTexCoord2d(0.5, y1 + 0.5);
           if(hgt > 0.0)
               glVertex3d(0.0, y1, z + scl*hgt);
           else
               glVertex3d(0.0, y1, z);

       for(i = 0; i < nlon/2; ++i)
       {
           x = 2.0 + lon[i] / 180.0;
           glNormal3d(0.0, 0.0, 1.0);
           glTexCoord2d(0.5*x - 0.5, y0 + 0.5);
           hgt = ter[j*nlon + i];
           if(hgt > 0.0)
               glVertex3d(x - 1.0, y0, z + scl*hgt);
           else
               glVertex3d(x - 1.0, y0, z);

           glNormal3d(0.0, 0.0, 1.0);
           glTexCoord2d(0.5*x - 0.5, y1 + 0.5);
           hgt = ter[(j+1)*nlon + i]; 
           if(hgt > 0.0)
               glVertex3d(x - 1.0, y1, z + scl*hgt);
           else
               glVertex3d(x - 1.0, y1, z);

         //if(0 == j)
         //   fprintf(stderr, "\tlon[%d] = %f, x = %f\n", i, lon[i], x);
       }
       glEnd();

       glBegin(GL_QUAD_STRIP);
       for(i = nlon/2; i < nlon; ++i)
       {
           x = lon[i] / 180.0;
           glNormal3d(0.0, 0.0, 1.0);
           glTexCoord2d(0.5*x + 0.5 , y0 + 0.5);
           hgt = ter[j*nlon + i]; 
           if(hgt > 0.0)
               glVertex3d(x - 1.0, y0, z + scl*hgt);
           else
               glVertex3d(x - 1.0, y0, z);

           glNormal3d(0.0, 0.0, 1.0);
           glTexCoord2d(0.5*x + 0.5 , y1 + 0.5);
           hgt = ter[(j+1)*nlon + i];
           if(hgt > 0.0)
               glVertex3d(x - 1.0, y1, z + scl*hgt);
           else
               glVertex3d(x - 1.0, y1, z);

         //if(0 == j)
         //   fprintf(stderr, "\tlon[%d] = %f, x = %f\n", i, lon[i], x);
       }

           glNormal3d(0.0, 0.0, 1.0);
           glTexCoord2d(1.0 , y0 + 0.5);
           if(hgt > 0.0)
               glVertex3d(0.0, y0, z + scl*hgt);
           else
               glVertex3d(0.0, y0, z);

           glNormal3d(0.0, 0.0, 1.0);
           glTexCoord2d(1.0 , y1 + 0.5);
           if(hgt > 0.0)
               glVertex3d(0.0, y1, z + scl*hgt);
           else
               glVertex3d(0.0, y1, z);
       glEnd();
    }
#endif

    glDisable(GL_TEXTURE_2D);
}

void Earth::read_terrain()
{
    FILE* infl;

    float hgt = 0.0;

    int i = 0;
    int j = 0;
    int n = 0;
    int nter = 0;

    strcpy(_topoflnm, getenv("NV_DATA"));
    strcat(_topoflnm, "/data/topo.bin");

  //cout << "File: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "_topoflnm: " << _topoflnm << endl;

    infl = fopen(_topoflnm, "r");
    if(NULL == infl)
    {
        fprintf(stderr, "\nfile: %s, line: %d\n", __FILE__, __LINE__);
        fprintf(stderr, "\nCan not open <%s> for reading\n", _topoflnm);
        return;
    }

    fread(&nlon, sizeof(int), 1, infl);
    fread(&nlat, sizeof(int), 1, infl);
    fread(&nter, sizeof(int), 1, infl);

  //fprintf(stderr, "\nfile: %s, line: %d\n", __FILE__, __LINE__);
  //fprintf(stderr, "\tnlon = %d, nlat = %d, nter = %d\n", nlon, nlat, nter);

    lon = (float *) calloc(nlon, sizeof(float));
    lat = (float *) calloc(nlat, sizeof(float));
    ter = (float *) calloc(nter, sizeof(float));

    fread(lon, sizeof(float), nlon, infl);
    fread(lat, sizeof(float), nlat, infl);
    fread(ter, sizeof(float), nter, infl);

    fclose(infl);

    maxhgt = 0.0;
    minhgt = 0.0;

    n = 0;
    for(j = 0; j < nlat; ++j)
    {
      //fprintf(stderr, "\tlat[%d] = %f\n", j, lat[j]);
        for(i = 0; i < nlon; ++i)
        {
          //if(0 == j)
          //   fprintf(stderr, "\tlon[%d] = %f\n", i, lon[i]);

            hgt = ter[n];
            if(maxhgt < hgt)
               maxhgt = hgt;
            if(minhgt > hgt)
               minhgt = hgt;

            ++n;
        }
    }

  //cout << "\tmaxhgt = " << maxhgt << endl;
  //cout << "\tminhgt = " << minhgt << endl;

  //Maximum mountain height: 6244.
  //Maximum ocean depth: 10651.
}

void Earth::bump(float r)
{
    float sr = radius;
    radius = r;
  //bump();
    radius = sr;
}

void Earth::bump()
{
    int i, j, n;

    float hgt = 0.0;
    float scl = 0.1 / maxhgt;

  //Set texture
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, get_texture_id());

  //Latitude bands
    for(j = 0; j < nlat - 1; ++j)
    {
       glBegin(GL_QUAD_STRIP);
           n = nlon/2;
           hgt = ter[j*nlon + n];
           if(hgt > 0.0)
               hgt *= scl;
           else
               hgt = 0.0;
           _bumpVertex(-180.0, lat[j], radius + hgt);

           hgt = ter[(j+1)*nlon + n];
           if(hgt > 0.0)
               hgt *= scl;
           else
               hgt = 0.0;
           _bumpVertex(-180.0, lat[j+1], radius + hgt);

           for(i = 0; i < nlon; ++i)
           {
               if(i < nlon/2)
                  n = nlon/2 + i;
               else
                  n = nlon - i;
               hgt = ter[j*nlon + n];
               if(hgt > 0.0)
                   hgt *= scl;
               else
                   hgt = 0.0;
               _bumpVertex(lon[i], lat[j], radius + hgt);

               hgt = ter[(j+1)*nlon + n];
               if(hgt > 0.0)
                   hgt *= scl;
               else
                   hgt = 0.0;
               _bumpVertex(lon[i], lat[j+1], radius + hgt);
           }

           n = nlon/2;
           hgt = ter[j*nlon + n];
           if(hgt > 0.0)
               hgt *= scl;
           else
               hgt = 0.0;
           _bumpVertex(180.0, lat[j], radius + hgt);

           hgt = ter[(j+1)*nlon + n];
           if(hgt > 0.0)
               hgt *= scl;
           else
               hgt = 0.0;
           _bumpVertex(180.0, lat[j+1], radius + hgt);
       glEnd();
    }

    glDisable(GL_TEXTURE_2D);
}

void Earth::_bumpVertex(float th, float ph, double radius)
{
    double r = radius*cos(ph * deg2arc);
    double x = r*cos((90.0+th) * deg2arc);
    double y = radius*sin(ph * deg2arc);
    double z = r*sin((90.0+th) * deg2arc);

    glNormal3d(x,y,z);
    glTexCoord2d((1.0 - (180.0 + th)/360.0), 0.5+ph/180.0);
    glVertex3d(x,y,z);
}

