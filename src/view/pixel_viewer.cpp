//$Id: pixel_viewer.cpp 5103 2014-04-09 22:27:17Z starviewer $

#include <QtOpenGL>
#include "pixel_viewer.h"

#ifndef GL_PI
#define GL_PI   3.1415926535897932
#endif

//Constructor
Pixel_Viewer::Pixel_Viewer(NVOptions* opt)
{
    cout << "Enter " << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
    nvoptions = opt;

    evaluator = new Evaluator();
    colorMap = NULL;

    _initialize();

    shaderID = 0;

    textureID = 0;

    numberShaderPasses = 1;

    initializeGL();

  //Image texture
    glGenTextures(1,&img);
    glBindTexture(GL_TEXTURE_2D,img);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
    cout << "Leave " << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
}

Pixel_Viewer::~Pixel_Viewer()
{
    if(NULL != colorMap)
        delete [] colorMap;

    delete evaluator;
}

void Pixel_Viewer::initializeGL() {
    // This connects the glCreateShader, glLinkProgram, etc. symbols
    initializeOpenGLFunctions();

    // Now your existing code will work
    // createShaderProg(...);
}

void Pixel_Viewer::_setup_shader(NV_FRAG_Type type, const char* name)
{
    char fragname[1024];

    cout << "Enter " << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
    strcpy(fragname, getenv("NV_HOME"));
    strcat(fragname, "/pixel/");
    strcat(fragname, name);

    cout << "\t fragname: " << fragname << endl;
    shader[type]  = createShaderProg(NULL, fragname);
    cout << "Leave " << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
}

void Pixel_Viewer::_initialize()
{
 //Create Shader Programs
   shader[NV_FRAG_NONE]       = 0;
   _setup_shader(NV_FRAG_COPY,       "copy.frag");
   _setup_shader(NV_FRAG_SMOOTH,     "smooth.frag");
   _setup_shader(NV_FRAG_SHARPEN,    "sharpen.frag");
   _setup_shader(NV_FRAG_BLUR,       "blur.frag");
   _setup_shader(NV_FRAG_EROSION,    "erosion.frag");
   _setup_shader(NV_FRAG_DILATION,   "dilation.frag");
   _setup_shader(NV_FRAG_LAPLACIAN,  "laplacian.frag");
   _setup_shader(NV_FRAG_PREWITT,    "prewitt.frag");
   _setup_shader(NV_FRAG_SOBEL,      "sobel.frag");
   _setup_shader(NV_FRAG_BLACKWHITE, "bNw.frag");

   _screenWidth = 900.0;
   _screenHeight = 900.0;

   dX = 1.0/_screenWidth;
   dY = 1.0/_screenHeight;
}

void Pixel_Viewer::setup(string vn, float *var)
{
    size_t gridsize = 0;

    name = vn;
    _var = var;

    nx = geometry->get_nx();
    ny = geometry->get_ny();
    nz = geometry->get_nz();

    gridsize = nx * ny * nz;

    evaluator->set_FillValue(fillValue, hasFillValue);
    evaluator->set_value(gridsize, _var);

    _parameter_setup();

    reset();

    _createTexture(0);
}

void Pixel_Viewer::errCheck(const char* where)
{
#if 0
    int err = glGetError();
    if(err)
        cout << "ERROR: <" << gluErrorString(err) << "> in [" << where << "]" << endl;
#endif
}

//create Shader
void Pixel_Viewer::createShader(int prog,const GLenum type,const char *srcFile)
{
  //create the shader
    int shaderID = glCreateShader(type);

  //load source code from file
  //readText(srcFile);

    int n;
    char *source = NULL;

  //Open file
    FILE* f = fopen(srcFile,"rt");
    if(!f)
    {
        cout << "Cannot open text file: " << srcFile << endl;
        exit (-1);
    }

    cout << "Enter " << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;

  //Seek to end to determine size, then rewind
    fseek(f,0,SEEK_END);
    n = ftell(f);
    rewind(f);

  //Allocate memory for the whole file
    source = (char*)malloc(n+1);
    if(!source)
    if(!f)
    {
        cout << "Cannot allocate " << n+1 << " bytes for text file: " << srcFile << endl;
        exit (-1);
    }

  //Snarf the file
    if(fread(source,n,1,f)!=1)
    {
        cout << "Cannot read " << n+1 << " bytes for text file: " << srcFile << endl;
        exit (-1);
    }
    source[n] = 0;

  //cout << "Functions: <" << __PRETTY_FUNCTION__
  //     << ">, line: " << __LINE__
  //     << ", file: <" << __FILE__ << ">" << endl;
  //cout << "source of <" << srcFile << ">:" << endl;
  //cout << source << endl;

  //Close and return
    fclose(f);

    cout << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
    glShaderSource(shaderID,1,(const char**)&source,NULL);
    free(source);

  //Compile the shader
    glCompileShader(shaderID);

  //Check for errors
  //printShaderLog(shaderID,srcFile);
    int len=0;
    glGetShaderiv(shaderID,GL_INFO_LOG_LENGTH,&len);

    if(len>1)
    {
        char *buffer = (char *)malloc(len);

        if(!buffer)
        {
            cout << "Cannot allocate " << len << " bytes of text for shader file: "
                  << srcFile << endl;
            exit (-1);
        }
        glGetShaderInfoLog(shaderID,len,&n,buffer);
        cout << "Shader log of: " << srcFile << endl;
        cout << buffer << endl << endl;
        free(buffer);
    }

    glGetShaderiv(shaderID,GL_COMPILE_STATUS,&len);

    if(!len)
    {
        cout << "Error compiling: " << srcFile << endl;
        exit (-1);
    }

  //Attach to shader program
    glAttachShader(prog,shaderID);

    cout << "Leave " << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
}

//create Shader Program
int Pixel_Viewer::createShaderProg(const char* VertFile,const char* FragFile)
{
  //create program
    int prog = glCreateProgram();

  //create and compile vertex shader
    if(VertFile)
    {
        createShader(prog,GL_VERTEX_SHADER,VertFile);
    }

  //create and compile fragment shader
    if(FragFile)
    {
        createShader(prog,GL_FRAGMENT_SHADER,FragFile);
    }

  //Link program
    glLinkProgram(prog);

  //Check for errors
    printProgramLog(prog);

  //Return name
    return prog;
}

//print Program Log
void Pixel_Viewer::printProgramLog(int obj)
{
    int len=0;
    glGetProgramiv(obj,GL_INFO_LOG_LENGTH,&len);

    if(len>1)
    {
        int n=0;
        char *buffer = (char *)malloc(len);
        if(!buffer)
        {
            cout << "Cannot allocate " << len << " bytes of text for program log" << endl;
            exit (-1);
        }
        glGetProgramInfoLog(obj,len,&n,buffer);
        cout << buffer << endl;
    }

    glGetProgramiv(obj,GL_LINK_STATUS,&len);
    if(!len)
    {
        cout << "Error linking programs" << endl;
        exit (-1);
    }
}

void Pixel_Viewer::draw()
{
    int k, sid;
    QString shadernm;

  //Clear screen and Z-buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  //Enable Z-buffering in OpenGL
    glEnable(GL_DEPTH_TEST);

    if(NULL != nvoptions)
    {
       _screenWidth = (double) nvoptions->get_screenWidth();
       _screenHeight = (double) nvoptions->get_screenHeight();

       dX = 1.0/_screenWidth;
       dY = 1.0/_screenHeight;
    }

  //Select shader
    if(nvoptions->get_cb(NV_BLACKWHITEON))
    {
        shaderID = shader[NV_FRAG_BLACKWHITE];
        shadernm = QString("BlackWhite");
    }
    else if(nvoptions->get_cb(NV_SOBELON))
    {
        shaderID = shader[NV_FRAG_SOBEL];
        shadernm = QString("sobel");
    }
    else if(nvoptions->get_cb(NV_PREWITTON))
    {
        shaderID = shader[NV_FRAG_PREWITT];
        shadernm = QString("prewitt");
    }
    else if(nvoptions->get_cb(NV_LAPLACIANON))
    {
        shaderID = shader[NV_FRAG_LAPLACIAN];
        shadernm = QString("laplacian");
    }
    else if(nvoptions->get_cb(NV_DILATIONON))
    {
        shaderID = shader[NV_FRAG_DILATION];
        shadernm = QString("dilation");
    }
    else if(nvoptions->get_cb(NV_EROSIONON))
    {
        shaderID = shader[NV_FRAG_EROSION];
        shadernm = QString("erosion");
    }
    else if(nvoptions->get_cb(NV_BLURON))
    {
        shaderID = shader[NV_FRAG_BLUR];
        shadernm = QString("blur");
    }
    else if(nvoptions->get_cb(NV_SHARPENON))
    {
        shaderID = shader[NV_FRAG_SHARPEN];
        shadernm = QString("sharpen");
    }
    else if(nvoptions->get_cb(NV_SMOOTHON))
    {
        shaderID = shader[NV_FRAG_SMOOTH];
        shadernm = QString("smooth");
    }
    else if(nvoptions->get_cb(NV_COPYON))
    {
        shaderID = shader[NV_FRAG_COPY];
        shadernm = QString("copy");
    }
    else
    {
        shaderID = shader[NV_FRAG_NONE];
        shadernm = QString("none");
    }

  //Set shader
    glUseProgram(shaderID);

    if(shaderID)
    {
      //Set offsets
        sid = glGetUniformLocation(shaderID,"dX");
        if(0 <= sid)
            glUniform1f(sid,dX);

        sid = glGetUniformLocation(shaderID,"dY");
        if(0 <= sid)
            glUniform1f(sid,dY);
    }

    glColor4f(1,1,1,0);

  //draw_cube();

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,textureID);

    glBegin(GL_QUADS);
        glTexCoord2f(0,0);
        glVertex3f(-1,-1,0.0);
        glTexCoord2f(1,0);
        glVertex3f(+1,-1,0.0);
        glTexCoord2f(1,1);
        glVertex3f(+1,+1,0.0);
        glTexCoord2f(0,1);
        glVertex3f(-1,+1,0.0);
    glEnd();

    glDisable(GL_TEXTURE_2D);

  //Disable depth
    glDisable(GL_DEPTH_TEST);

#if 1
  //Indentity projections
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

  //Shader passes
    for(k = 0; k < numberShaderPasses; ++k)
    {
        glBindTexture(GL_TEXTURE_2D, img);

      //Copy original scene to texture
        glCopyTexImage2D(GL_TEXTURE_2D,0,GL_RGBA8,0,0,_screenWidth,_screenHeight,0);

      //Redraw the texture
        glClear(GL_COLOR_BUFFER_BIT);
        glEnable(GL_TEXTURE_2D);

        glBegin(GL_QUADS);
            glTexCoord2f(0,0);
            glVertex2f(-1,-1);
            glTexCoord2f(0,1);
            glVertex2f(-1,+1);
            glTexCoord2f(1,1);
            glVertex2f(+1,+1);
            glTexCoord2f(1,0);
            glVertex2f(+1,-1);
        glEnd();

        glDisable(GL_TEXTURE_2D);
    }
#endif

  //No shader for what follows
    glUseProgram(shader[0]);

  //Render the scene and make it visible
    errCheck(__PRETTY_FUNCTION__);
}

void Pixel_Viewer::reset()
{
    if(glIsTexture(textureID))
        glDeleteTextures(1, &textureID);
}

void Pixel_Viewer::set_colorTable(ColorTable* ct)
{
    int i, n = 0;
    float* cmap;

    colorTable = ct;

    numberofcolors = colorTable->get_clen() - 3;
    cmap = colorTable->get_cmap();

    if(NULL != colorMap)
        delete colorMap;

    colorMap = new float[4 * numberofcolors];
    
    for(n = 0; n < numberofcolors; ++n)
    {
        for(i = 0; i < 3; ++i)
        {
            colorMap[4*n + i] = cmap[3*(n+3) + i];
        }

        colorMap[4*n + 3] = 1.0;
    }
}

void Pixel_Viewer::_parameter_setup()
{
    hasFillValue = geometry->get_hasFillValue();
    if(hasFillValue)
    {
        fillValue = geometry->get_fillValue();
    }

    vMinimum = evaluator->get_min();
    vMaximum = evaluator->get_max();
    scale  = 1.0/(vMaximum - vMinimum + 1.0e-20);
    zScale = 1.0/(nz + 0.01);
}

void Pixel_Viewer::_createTexture(int zs)
{
    int i, j, n;
    float f;

    size_t grid_size;

    unsigned char* imgbits = NULL;

    n = zs * nx * ny;

  //Generate 2D texture
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    grid_size = 4 * nx * ny;

    imgbits = (unsigned char*) malloc(grid_size);
    assert(imgbits);

    pltvar = &_var[n];

    for(j = 0; j < ny - 1; ++j)
    {
        for(i = 0; i < nx; ++i)
        {
            n = j * nx + i;
            f = scale * (pltvar[n] - vMinimum);
            _setColor(f, &imgbits[4*n]);
        }
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, nx, ny,
                                0, GL_RGBA, GL_UNSIGNED_BYTE, imgbits);

    if(glGetError())
    {
        cout << "Functions: <" << __PRETTY_FUNCTION__
             << ">, line: " << __LINE__
             << ", file: <" << __FILE__ << ">" << endl;
        cout << "\tError in glTexImage2D." << endl;
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 

    free(imgbits);
}

void Pixel_Viewer::_setColor(float f, unsigned char* color4f)
{
    int i, n, n1;
    float d;

    d = f * (float) numberofcolors;
    n = (int) d;
    d -= (float) n;
    n1 = n + 1;

    for(i = 0; i < 4; ++i)
        color4f[i] = (unsigned char) (255.999 * ((1.0-d) * colorMap[4*n + i]
                                                     + d * colorMap[4*n1 + i]));
}

