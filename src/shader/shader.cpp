#include "shader.h"

//Constructor
Shader::Shader()
{
    _initialize();
}

Shader::~Shader()
{
}

void Shader::_initialize()
{
   id[0] = 0;
   id[1] = 0;

   _x = 0.0;
   _y = 0.0;
   _z = 0.0;
   _w = 0.0;

   _r = 1.0;
   _g = 1.0;
   _b = 1.0;
   _a = 1.0;

   id[1] = createShaderProg("ndc2rgb.vert","ndc2rgb.frag");
}

//Reset view angle
void Shader::reset(void)
{
   _x = 0.0;
   _y = 0.0;
   _z = 0.0;
   _w = 0.0;

   _r = 1.0;
   _g = 1.0;
   _b = 1.0;
   _a = 1.0;

   updateGL();   //  Request redisplay
}

//shader on/off
void Shader::selectShader(int f)
{
   if(f)
   {
       _shaderOn = true;
   }
   else
   {
       _shaderOn = false;
   }

   updateGL();
}

void Shader::errCheck(const char* where)
{
    int err = glGetError();
    if(err)
        cout << "ERROR: <" << gluErrorString(err) << "> in [" << where << "]" << endl;
}

#if 0
//Read text file
void Shader::readText(const char *srcFile)
{
    int n;

  //Open file
    FILE* f = fopen(srcFile,"rt");
    if(!f)
        fatal("Cannot open text file %s\n",srcFile);

  //Seek to end to determine size, then rewind
    fseek(f,0,SEEK_END);
    n = ftell(f);
    rewind(f);

  //Allocate memory for the whole file
    _source = (char*)malloc(n+1);
    if(!_source)
        fatal("Cannot allocate %d bytes for text file %s\n",n+1,srcFile);

  //Snarf the file
    if(fread(_source,n,1,f)!=1)
        fatal("Cannot read %d bytes for text file %s\n",n+1,srcFile);
    _source[n] = 0;

  //Close and return
    fclose(f);
}

//print Shader Log
void Shader::printShaderLog(int obj,const char *srcFile)
{
    int len=0;
    glGetShaderiv(obj,GL_INFO_LOG_LENGTH,&len);

    if(len>1)
    {
        int n=0;
        char *buffer = (char *)malloc(len);

        if(!buffer)
        {
            cout << "Cannot allocate " << len << " bytes of text for shader file: "
                  << srcFile << endl;
            exit (-1);
        }
        glGetShaderInfoLog(obj,len,&n,buffer);
        cout << "Shader log of: " << srcFile << endl;
        cout << buffer << endl << endl;
        free(buffer);
    }

    glGetShaderiv(obj,GL_COMPILE_STATUS,&len);

    if(!len)
    {
        cout << "Error compiling: " << srcFile << endl;
        exit (-1);
    }
}
#endif

//create Shader
void Shader::createShader(int prog,const GLenum type,const char *srcFile)
{
  //create the shader
    int shaderID = glCreateShader(type);

  //Load source code from file
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

    cout << "Functions: <" << __PRETTY_FUNCTION__
         << ">, line: " << __LINE__
         << ", file: <" << __FILE__ << ">" << endl;
    cout << "source of <" << srcFile << ">:" << endl;
    cout << source << endl;

  //Close and return
    fclose(f);

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
}

//create Shader Program
int Shader::createShaderProg(const char* VertFile,const char* FragFile)
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
void Shader::printProgramLog(int obj)
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

#if 0
void Shader::fatal(const char* format , ...)
{
    va_list args;
    va_start(args,format);
    vfprintf(stderr,format,args);
    va_end(args);
    exit(-1);
}
#endif

void Shader::draw()
{
    glPushMatrix();

  //cout << "Functions: <" << __PRETTY_FUNCTION__
  //     << ">, line: " << __LINE__
  //     << ", file: <" << __FILE__ << ">" << endl;
  //cout << "\tx, y, z = " << _x << ", " << _y << ", " << _z << endl;

    glScalef(0.25, 0.25, 0.25);
    glTranslatef(_x, _y, _z);

    _a = 1.0 - sqrt(_x*_x + _y*_y + _z*_z);
    if(_a < 0.1)
        _a = 0.1;

    glColor4f(_x + 0.5, _y + 0.5, _z + 0.5, _a);

    _draw_ball(0.1);

    glPopMatrix();
}

//Draw vertex in polar coordinates
void Shader::_ballVertex(float lon, float lat, float radius)
{
    float x = radius*Cos(lon)*Cos(lat);
    float y =          radius*Sin(lat);
    float z = radius*Sin(lon)*Cos(lat);
    glNormal3f(x,y,z);
    glVertex3f(x,y,z);
}

void Shader::_draw_ball(float r)
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

