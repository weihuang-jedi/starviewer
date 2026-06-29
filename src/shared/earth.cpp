#include <earth.h>

#define Cos(th) cos(3.141592659*(th)/180.0)
#define Sin(th) sin(3.141592659*(th)/180.0)

//
//  Constructor
//
Earth::Earth()
{
    char _bmpflnm[1024];
    const char* path = getenv("STARVIEWERHOME");
    if (path == nullptr) {
        cout << "ERROR: STARVIEWERHOME not set!" << endl;
        throw(errno);
    }
    strcpy(_bmpflnm, path);

    strcat(_bmpflnm, "/data/earth.bmp");

    _earthflnm = _bmpflnm;

    _loadTexBMP();

    radius = 1.0;

    deg2arc = 3.1415926535897932 / 180.0;

    // read_terrain();
}

Earth::Earth(string flnm)
{
    // cout << "\nEnter functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    // cout << "\tflnm: <" << flnm << ">" << endl;
    _earthflnm = flnm;
    // cout << "\t_earthflnm: <" << _earthflnm << ">" << endl;
    _loadTexBMP();

    radius = 1.0;

    deg2arc = 3.1415926535897932 / 180.0;
    // cout << "Leave functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
}

Earth::~Earth()
{
   lon.clear();
   lon.shrink_to_fit();
   lat.clear();
   lat.shrink_to_fit();
}

void Earth::initializeGL() {
    initializeOpenGLFunctions(); // This "activates" the function pointers
    // ...
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
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
    GLuint textureID = 0;

    // cout << "enter functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    // cout << "\t_earthflnm: <" << _earthflnm << ">" << endl;

    initializeGL();

    // Load the image
    QImage b(_earthflnm.c_str());

    // cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;

    if (b.isNull()) {
        qDebug() << "Failed to load image";
    }
  //else {
  //    qDebug() << "Image loaded successfully" << b.size();
  //}

    // cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    // Guard check: ensures an OpenGL context is bound to this thread
    if (QOpenGLContext::currentContext() == nullptr) {
        cerr << "ERROR: Attempted to run OpenGL commands without an active context bound!" << endl;
        return; // Safely back out instead of segfaulting
    }

    // cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    glEnable(GL_TEXTURE_2D);

    // cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    
    QImage t = QGLWidget::convertToGLFormat( b );

    // cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;

    glGenTextures(1, &textureID);

    // set texture name
    set_texture_id(textureID);

    // cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    // cout << "\tt.width(): " << t.width() << ", t.height(): " << t.height() << endl;

    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); 
    glTexImage2D(GL_TEXTURE_2D, 0, 3, t.width(), t.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, t.bits());
    glGenerateMipmap(GL_TEXTURE_2D);  //Generate mipmaps now!!!
    glDisable(GL_TEXTURE_2D);
    // cout << "Leave functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
}

// Draw vertex in polar coordinates
void Earth::_Vertex(int th, int ph)
{
   double x = radius*Sin((double)th)*Cos((double)ph);
   double y = radius*Sin((double)ph);
   double z = radius*Cos((double)th)*Cos((double)ph);
   glNormal3d(x,y,z);
   if(th >= 0)
       glTexCoord2d(0.5+(double)th/360.0, 0.5+(double)ph/180.0);
   else
       glTexCoord2d(0.5+(double)th/360.0, 0.5+(double)ph/180.0);
   glVertex3d(x,y,z);
}

// Draw earth
void Earth::draw(float r)
{
    float sr = radius;
    radius = r;
    draw();
    radius = sr;
}

// Draw earth
void Earth::draw()
{
    int i,j,intv;

    // cout << "\nEnter functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    intv = 2;

    //  Draw surface of the planet
    //  Set texture
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, get_texture_id());
    // cout << "functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    //  Latitude bands
    glColor3f(1,1,1);
    for(j = 90; j > -90; j -= intv)
    {
       glBegin(GL_QUAD_STRIP);
       for(i = -180; i <= 180; i += intv)
       {
           _Vertex(i,j);
           _Vertex(i,j-intv);
       }
       glEnd();
    }

    glDisable(GL_TEXTURE_2D);
    // cout << "Leave functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
}

// Draw earth
void Earth::draw_plane(float z)
{
    int i, j;
    float x, y0, y1;

  //Set texture
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, get_texture_id());

  //Latitude bands
    glColor3f(1,1,1);
    for(j = 180; j > 0; --j)
    {
       y0 = (float)j / 180.0;
       y1 = (float)(j - 1) / 180.0;

       glBegin(GL_QUAD_STRIP);
       for(i = 0; i < 361; ++i)
       {
           x = (float)i / 360.0;
           glNormal3d(0.0, 0.0, 1.0);
           glTexCoord2d(x, y0);
           glVertex3d(2.0*(x-0.5), y0-0.5, z);

           glNormal3d(0.0, 0.0, 1.0);
           glTexCoord2d(x, y1);
           glVertex3d(2.0*(x-0.5), y1-0.5, z);
       }

       glEnd();
    }

    glDisable(GL_TEXTURE_2D);
}

void Earth::bump_plane(float z)
{
    int i, j;
    float x, y0, y1;

  //Set texture
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, get_texture_id());

  //OpenGL should normalize normal vectors
  //glEnable(GL_NORMALIZE);

    for(j = 0; j < nlat - 1; ++j)
    {
       y0 = lat[j] / 180.0;
       y1 = lat[j + 1] / 180.0;

       glBegin(GL_QUAD_STRIP);
           glNormal3d(0.0, 0.0, 1.0);
           glTexCoord2d(0.0, y0 + 0.5);
           glVertex3d(0.0, y0, z);

           glNormal3d(0.0, 0.0, 1.0);
           glTexCoord2d(0.0, y1 + 0.5);
           glVertex3d(0.0, y1, z);

           for(i = 0; i < nlon; ++i)
           {
               x = lon[i] / 180.0;
	       if(x > 1.0)
		  x -= 2.0;
               glNormal3d(0.0, 0.0, 1.0);
               glTexCoord2d(x, y0 + 0.5);
               glVertex3d(x, y0, z);

               glNormal3d(0.0, 0.0, 1.0);
               glTexCoord2d(x, y1 + 0.5);
               glVertex3d(x, y1, z);
           }

           glNormal3d(0.0, 0.0, 1.0);
           glTexCoord2d(1.0 , y0 + 0.5);
           glVertex3d(0.0, y0, z);

           glNormal3d(0.0, 0.0, 1.0);
           glTexCoord2d(1.0 , y1 + 0.5);
           glVertex3d(0.0, y1, z);
       glEnd();
    }

    glDisable(GL_TEXTURE_2D);
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

#if 0
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
#endif
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

