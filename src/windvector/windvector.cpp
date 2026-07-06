#include <QtOpenGL>

#include "windvector.h"

WindVector::WindVector(ColorTable *ct, NVOptions* opt)
{
    nvoptions = opt;
    set_colorTable(ct);

    arrow = new Arrow(ct);

    // _scale = 0.50;
    // _scale = 5000.0;
    // _wings = 1000.0;
    _scale = 1000.0;
    _wings = 100.0;
    _zScale = 0.50;
    // _stepsize = 5;
    _stepsize = 100;
    _local_stepsize = _stepsize;
    // _maxspeed = 100.0;
    _maxspeed = 100.0;
    // _maxspeed = 5.0;

    axx[0] = 1.0;
    axx[1] = 0.0;
    axx[2] = 0.0;

    ayy[0] = 0.0;
    ayy[1] = 1.0;
    ayy[2] = 0.0;

    azz[0] = 0.0;
    azz[1] = 0.0;
    azz[2] = 1.0;
}

WindVector::~WindVector()
{
    delete arrow;
}

void WindVector::setup(int nx, int ny, int nz,
                       float* u, float* v, float*w)
{
    _nx = nx;
    _ny = ny;
    _nz = nz;

    _u = u;
    _v = v;
    _w = w;

    _has_w = true;

    _parameter_setup();

    nvoptions->set_xsec(_nx + 1);
    nvoptions->set_ysec(_ny + 1);
    nvoptions->set_zsec(0);
}

void WindVector::setup(int nx, int ny, int nz,
                       float* u, float* v)
{
    _nx = nx;
    _ny = ny;
    _nz = nz;

    _u = u;
    _v = v;

    _has_w = false;

    _parameter_setup();

    nvoptions->set_xsec(_nx + 1);
    nvoptions->set_ysec(_ny + 1);
    nvoptions->set_zsec(0);
}

void WindVector::_parameter_setup()
{
    double hDelt, vDelt;

    if(_nx > _ny)
    {
        hDelt = 1.0 / (_nx - 1.0);

        _local_stepsize = _nx / 100;
    }
    else
    {
        hDelt = 1.0 / (_ny - 1.0);

        _local_stepsize = _ny / 100;
    }

    if(1 < _nz)
        vDelt = 1.0 / (_nz - 1.0);
    else
        vDelt = 1.0;

    _scale = 50.0 * hDelt / _maxspeed;
    _zScale = 1000.0 * vDelt / _maxspeed;

    // cout << "Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__
    //      << ", file: <" << __FILE__ << ">" << endl;
    // cout << "\t_nx = " << _nx << ", _ny = " << _ny << ", _nz = " << _nz << endl;
    // cout << "\t_scale = " << _scale << ", _zScale = " << _zScale << endl;

    if(_stepsize < _local_stepsize)
       _stepsize = _local_stepsize;
}

void WindVector::draw(int k, double z)
{
    int i, j, n;
    double zp = z + 0.025;

    if(nvoptions->get_cb(NV_VECTOR_LONGER))
    {
        arrow->longer();
        nvoptions->set_cb(NV_VECTOR_LONGER, false);
    }

    if(nvoptions->get_cb(NV_VECTOR_SHORTER))
    {
        arrow->shorter();
        nvoptions->set_cb(NV_VECTOR_SHORTER, false);
    }

    if(nvoptions->get_cb(NV_VECTOR_WIDER))
    {
        arrow->wider();
        nvoptions->set_cb(NV_VECTOR_WIDER, false);
    }

    if(nvoptions->get_cb(NV_VECTOR_NARROWER))
    {
        arrow->narrower();
        nvoptions->set_cb(NV_VECTOR_NARROWER, false);
    }

    if(nvoptions->get_cb(NV_VECTOR_MORE))
    {
        --_stepsize;
        if(1 > _stepsize)
            _stepsize = 1;
        nvoptions->set_cb(NV_VECTOR_MORE, false);
    }

    if(nvoptions->get_cb(NV_VECTOR_LESS))
    {
        ++_stepsize;
        nvoptions->set_cb(NV_VECTOR_LESS, false);
    }

    cout << "\tIn functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    cout << "\t_nx = " << _nx << ", _ny = " << _ny << ", _nz = " << _nz << endl;
    cout << "\tnvoptions->get_zsec() = " << nvoptions->get_zsec() << endl;
    cout << "\tcolorTable = " << colorTable << endl;

    _colorlen = colorTable->get_clen() - 3;
    _colormap = colorTable->get_cmap();

    if(_stepsize < _local_stepsize)
       _stepsize = _local_stepsize;

    if(has_w())
    {
      for(j = _stepsize/2; j < _ny; j += _stepsize)
      {
        n = (k * _ny + j) * _nx;

        for(i = _stepsize/2; i < _nx; i += _stepsize)
        {
            _draw_arrow(_xFlat[i], _yFlat[j], zp, _u[n+i], _v[n+i], _w[n+i]);
        }
      }
    }
    else
    {
      for(j = _stepsize/2; j < _ny; j += _stepsize)
      {
        n = (k * _ny + j) * _nx;

        for(i = _stepsize/2; i < _nx; i += _stepsize)
        {
            _draw_arrow(_xFlat[i], _yFlat[j], zp, _u[n+i], _v[n+i]);
        }
      }
    }
}

void WindVector::_set_color(double spd, float* color)
{
#if 0
    color[0]  = 1.0;
    color[1]  = 1.0;
    color[2]  = 1.0;
#else
    int idx = (int) ((spd * _colorlen) / _maxspeed);
 
    if(idx > (_colorlen - 1))
        idx = _colorlen - 1;

    color[0]  = _colormap[3*(idx+3)];
    color[1]  = _colormap[3*(idx+3) + 1];
    color[2]  = _colormap[3*(idx+3) + 2];
#endif
}

void WindVector::_draw_arrow(double x, double y, double z,
                             float u, float v, float w)
{
    float color[3];
    double tail[3];
    double head[3];
    double suvw[3];
    double dist = sqrt(u*u + v*v + w*w);

    _set_color(dist, color);
    glColor3fv(color);

    tail[0] = x;
    tail[1] = y;
    tail[2] = z;

    suvw[0] = _scale * u;
    suvw[1] = _scale * v;
    suvw[2] = _zScale * w;

    head[0] = x + suvw[0];
    head[1] = y + suvw[1];
    head[2] = z + suvw[2];

    _arrow(tail, head, suvw);
}

void WindVector::_draw_arrow(double x, double y, double z,
                             float u, float v)
{
    float color[3];
    double tail[3];
    double head[3];
    double suvw[3];
    double dist = sqrt(u*u + v*v);

    _set_color(dist, color);
    glColor3fv(color);

    tail[0] = x;
    tail[1] = y;
    tail[2] = z;

    suvw[0] = _scale * u;
    suvw[1] = _scale * v;
    suvw[2] = 0.0;

    head[0] = x + suvw[0];
    head[1] = y + suvw[1];
    head[2] = z;

    _arrow(tail, head, suvw);
}

void WindVector::_arrow(double tail[3], double head[3], double w[3])
{
    double u[3], v[3];        // arrow coordinate system

  //determine major direction:
    int axis = X_DIRECTION;
    double mag = fabs(w[0]);
    if(mag < fabs(w[1]))
    {
        axis = Y_DIRECTION;
        mag = fabs(w[1]);
    }

    if(mag < fabs(w[2]))
    {
        axis = Z_DIRECTION;
        mag = fabs(w[2]);
    }

  //set size of wings and turn w into a Unit vector:
    double d = _wings * _dist(w);

  //if(d > 0.125)
  //   d = 0.125;

  //if(d > 1.25)
  //   d = 1.25;

    if(d > 2.5)
       d = 2.5;

  //cout << "Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__
  //     << ", file: <" << __FILE__ << ">" << endl;
  //cout << "\ttail[0] = " << tail[0] << ", head[0] = " << head[0] << endl;
  //cout << "\ttail[1] = " << tail[1] << ", head[1] = " << head[1] << endl;
  //cout << "\ttail[2] = " << tail[2] << ", head[2] = " << head[2] << endl;

  //draw the shaft of the arrow:
    glBegin( GL_LINE_STRIP );
        glVertex3dv( tail );
        glVertex3dv( head );
    glEnd( );

  //draw two sets of wings in the non-major directions:
    double x, y, z;

    if(axis != X_DIRECTION)
    {
        _cross( w, axx, v );
        _cross( v, w, u  );

        x = head[0] + d * ( u[0] - w[0] );
        y = head[1] + d * ( u[1] - w[1] );
        z = head[2] + d * ( u[2] - w[2] );

        glBegin( GL_LINE_STRIP );
            glVertex3dv( head );
            glVertex3d( x, y, z );
        glEnd( );

        x = head[0] + d * ( -u[0] - w[0] );
        y = head[1] + d * ( -u[1] - w[1] );
        z = head[2] + d * ( -u[2] - w[2] );

        glBegin( GL_LINE_STRIP );
            glVertex3dv( head );
            glVertex3d( x, y, z );
        glEnd( );
    }

    if(axis != Y_DIRECTION)
    {
        _cross( w, ayy, v );
        _cross( v, w, u  );

        x = head[0] + d * ( u[0] - w[0] );
        y = head[1] + d * ( u[1] - w[1] );
        z = head[2] + d * ( u[2] - w[2] );

        glBegin( GL_LINE_STRIP );
            glVertex3dv( head );
            glVertex3d( x, y, z );
        glEnd( );

        x = head[0] + d * ( -u[0] - w[0] );
        y = head[1] + d * ( -u[1] - w[1] );
        z = head[2] + d * ( -u[2] - w[2] );

        glBegin( GL_LINE_STRIP );
            glVertex3dv( head );
            glVertex3d( x, y, z );
        glEnd( );
    }

    if(axis != Z_DIRECTION)
    {
        _cross( w, azz, v );
        _cross( v, w, u  );

        x = head[0] + d * ( u[0] - w[0] );
        y = head[1] + d * ( u[1] - w[1] );
        z = head[2] + d * ( u[2] - w[2] );

        glBegin( GL_LINE_STRIP );
            glVertex3dv( head );
            glVertex3d( x, y, z );
        glEnd( );

        x = head[0] + d * ( -u[0] - w[0] );
        y = head[1] + d * ( -u[1] - w[1] );
        z = head[2] + d * ( -u[2] - w[2] );

        glBegin( GL_LINE_STRIP );
            glVertex3dv( head );
            glVertex3d( x, y, z );
        glEnd( );
    }
}

void WindVector::_cross( double v1[3], double v2[3], double vout[3] )
{
    vout[0] = v1[1]*v2[2] - v2[1]*v1[2];
    vout[1] = v2[0]*v1[2] - v1[0]*v2[2];
    vout[2] = v1[0]*v2[1] - v2[0]*v1[1];
}

double WindVector::_dist(double v[3])
{
    double dist = v[0]*v[0] + v[1]*v[1] + v[2]*v[2];

    return dist;
}

void WindVector::setup_lonlat(double* lon, double* lat)
{
    // cout << "\nEnter " << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
    // cout << "\tlon[0] = " << lon[0] << ", lat[0] = " << lat[0] << endl;
    _lon = lon;
    _lat = lat;
    // cout << "Leave " << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
}

void WindVector::setup_xyFlat(double* xFlat, double* yFlat)
{
    // cout << "\nEnter " << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
    // cout << "\txFlat[0] = " << xFlat[0] << ", yFlat[0] = " << yFlat[0] << endl;
    _xFlat = xFlat;
    _yFlat = yFlat;
    // cout << "Leave " << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
}

void WindVector::_initWindGPUAssets()
{
    QOpenGLFunctions_3_3_Core *f = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>();
    if (!f || windVBO != 0) return;

    // 1. Definition geometry of 1 baseline unit arrow pointing right (+X)
    // Format: X, Y, Z per vertex. We use pairs to form GL_LINES
    float arrowTemplate[] = {
        // Shaft Line Strip
        0.0f, 0.0f, 0.0f,   1.0f, 0.0f, 0.0f,
        // Upper Wing Barb
        1.0f, 0.0f, 0.0f,   0.7f, 0.15f, 0.0f,
        // Lower Wing Barb
        1.0f, 0.0f, 0.0f,   0.7f, -0.15f, 0.0f
    };

    f->glGenBuffers(1, &windVBO);
    f->glBindBuffer(GL_ARRAY_BUFFER, windVBO);
    f->glBufferData(GL_ARRAY_BUFFER, sizeof(arrowTemplate), arrowTemplate, GL_STATIC_DRAW);
    f->glBindBuffer(GL_ARRAY_BUFFER, 0);

    // 2. Vertex Shader: Handles instanced rotation matrices purely on the GPU
    const char* vs = R"glsl(
        #version 330 compatibility

        layout(location = 0) in vec3 a_Position; // Local template coordinates

        uniform sampler2D u_TexU;
        uniform sampler2D u_TexV;
        uniform sampler2D u_TexW;
        uniform int u_HasW;

        uniform int u_NX;
        uniform int u_NY;
        uniform int u_Stepsize;

        uniform float u_Scale;
        uniform float u_ZScale;
        uniform float u_WingsScale;
        uniform float u_BaseZ;

        uniform float u_XMin;
        uniform float u_XMax;
        uniform float u_YMin;
        uniform float u_YMax;

        out float v_Speed;

        void main() {
            // Compute current row and column grid indexing using Instancing ID numbers
            int instancesPerRow = (u_NX + u_Stepsize - 1) / u_Stepsize;
            int gridI = (gl_InstanceID % instancesPerRow) * u_Stepsize;
            int gridJ = (gl_InstanceID / instancesPerRow) * u_Stepsize;

            if (gridJ >= u_NY) return; 

            // Normalized sampling coordinates
            vec2 uvSample = vec2(float(gridI) / float(u_NX - 1), float(gridJ) / float(u_NY - 1));

            // Extract raw vector components from the NetCDF data textures
            float uVal = texture(u_TexU, uvSample).r;
            float vVal = texture(u_TexV, uvSample).r;
            float wVal = (u_HasW == 1) ? texture(u_TexW, uvSample).r : 0.0;

            // Calculate true linear speed for fragment shader color maps
            float speed = sqrt(uVal*uVal + vVal*vVal + wVal*wVal);
            v_Speed = speed;

            // Compute dynamic anchor layouts matching flat view dimensions
            float tailX = mix(u_XMin, u_XMax, uvSample.x);
            float tailY = mix(u_YMin, u_YMax, uvSample.y);

            // --- STEP 1: Calculate the exact arrow shaft displacement vector from CPU math ---
            float su = uVal * u_Scale;
            float sv = vVal * u_Scale;
            float sw = wVal * u_ZScale;
            // vec3 dirVec = vec3(su, sv, sw);
            // --- VISUAL BOOST: Adjust this multiplier (e.g., 20.0) to change shaft size ---
            vec3 dirVec = vec3(su, sv, sw) * 20.0; 
            // ---------------------------------------------------------------------------------
            // ---------------------------------------------------------------------------------

            vec3 finalPos;
            if (a_Position.x == 0.0) {
                // Tail point anchor
                finalPos = vec3(tailX, tailY, u_BaseZ);
            } else {
                // Get the unit direction of the arrow shaft
                float len = length(dirVec);
                if (len < 0.00001) len = 1.0;
                vec3 unitDir = dirVec / len;

                if (a_Position.x == 1.0) {
                    // Arrow head vertex
                    finalPos = vec3(tailX, tailY, u_BaseZ) + dirVec;
                } else {
                    // --- BALANCED BARBS: Make wing size proportional to the visible shaft length ---
                    float d = len * (u_WingsScale * 0.005); 
                    
                    // Cap the maximum barb size so they stay clean
                    if (d > 0.15) d = 0.15; 

                    // Generate backward barbs and perpendicular offsets
                    vec3 backVec = -unitDir * d;
                    vec3 perp = normalize(vec3(-unitDir.y, unitDir.x, 0.0)) * (d * 0.4);
                    // -------------------------------------------------------------------------------

                    if (a_Position.y > 0.0) {
                        finalPos = vec3(tailX, tailY, u_BaseZ) + dirVec + backVec + perp;
                    } else {
                        finalPos = vec3(tailX, tailY, u_BaseZ) + dirVec + backVec - perp;
                    }
                }
            }

            gl_Position = gl_ModelViewProjectionMatrix * vec4(finalPos, 1.0);
        }
    )glsl";

    // 3. Fragment Shader: Colors vectors dynamically using the active 1D colormap
    const char* fs = R"glsl(
        #version 330 compatibility

        in float v_Speed;
        uniform sampler1D u_ColorMap;
        uniform float u_MaxSpeed;

        void main() {
            float fact = clamp(v_Speed / u_MaxSpeed, 0.0, 1.0);
            gl_FragColor = vec4(texture(u_ColorMap, fact).rgb, 1.0);
        }
    )glsl";

    windShader = new QOpenGLShaderProgram();
    windShader->addShaderFromSourceCode(QOpenGLShader::Vertex, vs);
    windShader->addShaderFromSourceCode(QOpenGLShader::Fragment, fs);
    windShader->link();
}

void WindVector::drawGPU(int k, double z)
{
    QOpenGLFunctions_3_3_Core *f = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>();
    if (!f) return;

    // Build variables if required
    _initWindGPUAssets();

    double zp = z + 0.025;

    // Handle user runtime interface scaling callbacks
    if (nvoptions->get_cb(NV_VECTOR_LONGER))    { _scale *= 1.2;  nvoptions->set_cb(NV_VECTOR_LONGER, false); }
    if (nvoptions->get_cb(NV_VECTOR_SHORTER))   { _scale *= 0.8;  nvoptions->set_cb(NV_VECTOR_SHORTER, false); }
    if (nvoptions->get_cb(NV_VECTOR_WIDER))     { _wings *= 1.2;  nvoptions->set_cb(NV_VECTOR_WIDER, false); }
    if (nvoptions->get_cb(NV_VECTOR_NARROWER))  { _wings *= 0.8;  nvoptions->set_cb(NV_VECTOR_NARROWER, false); }
    if (nvoptions->get_cb(NV_VECTOR_MORE))      { _stepsize = qMax(1, _stepsize - 1); nvoptions->set_cb(NV_VECTOR_MORE, false); }
    if (nvoptions->get_cb(NV_VECTOR_LESS))      { _stepsize += 1; nvoptions->set_cb(NV_VECTOR_LESS, false); }

    if (_stepsize < _local_stepsize) _stepsize = _local_stepsize;

    // Generate component texture containers dynamically on slice change updates
    size_t planeSize = static_cast<size_t>(_nx) * _ny;
    size_t offset = static_cast<size_t>(k) * planeSize;

    auto uploadTex = [&](GLuint& texID, float* dataPtr) {
        if (texID == 0) f->glGenTextures(1, &texID);
        f->glBindTexture(GL_TEXTURE_2D, texID);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        f->glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, _nx, _ny, 0, GL_RED, GL_FLOAT, dataPtr);
        f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    };

    uploadTex(texU, &_u[offset]);
    uploadTex(texV, &_v[offset]);
    if (_has_w) uploadTex(texW, &_w[offset]);

    // Calculate total layout instance counts needed to draw the view
    int stepsX = (_nx + _stepsize - 1) / _stepsize;
    int stepsY = (_ny + _stepsize - 1) / _stepsize;
    int totalInstances = stepsX * stepsY;

    if (totalInstances <= 0) return;

    // Bind texture pipelines
    f->glActiveTexture(GL_TEXTURE0); f->glBindTexture(GL_TEXTURE_2D, texU);
    f->glActiveTexture(GL_TEXTURE1); f->glBindTexture(GL_TEXTURE_2D, texV);
    f->glActiveTexture(GL_TEXTURE2); f->glBindTexture(GL_TEXTURE_2D, _has_w ? texW : 0);

    if (colorMapTexture == 0) {
        _colorLen = colorTable->get_clen();
        _colorMap = colorTable->get_cmap();

        f->glGenTextures(1, &colorMapTexture);
        f->glBindTexture(GL_TEXTURE_1D, colorMapTexture);

        vector<float> floatColorMap(_colorLen * 3);
        for (int c = 0; c < _colorLen * 3; ++c) {
             floatColorMap[c] = static_cast<float>(_colorMap[c]);
        }

        f->glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB32F, _colorLen, 0, GL_RGB, GL_FLOAT, floatColorMap.data());
        f->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        f->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        f->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    }
    // Bind colormap from legacy starviewer framework texture assets
    f->glActiveTexture(GL_TEXTURE3); f->glBindTexture(GL_TEXTURE_1D, colorMapTexture);

    // To this:
    // f->glActiveTexture(GL_TEXTURE3);
    // f->glBindTexture(GL_TEXTURE_1D, colorTable->get_textureID());

    windShader->bind();
    windShader->setUniformValue("u_TexU", 0);
    windShader->setUniformValue("u_TexV", 1);
    windShader->setUniformValue("u_TexW", 2);
    windShader->setUniformValue("u_ColorMap", 3);
    windShader->setUniformValue("u_HasW", _has_w ? 1 : 0);
    windShader->setUniformValue("u_NX", _nx);
    windShader->setUniformValue("u_NY", _ny);
    windShader->setUniformValue("u_Stepsize", _stepsize);
    windShader->setUniformValue("u_Scale", static_cast<float>(_scale));
    windShader->setUniformValue("u_ZScale", static_cast<float>(_zScale));
    // windShader->setUniformValue("u_WingsScale", static_cast<float>(_wings * 0.002)); // Scaled normalize helper
    windShader->setUniformValue("u_WingsScale", static_cast<float>(_wings));
    windShader->setUniformValue("u_BaseZ", static_cast<float>(zp));
    windShader->setUniformValue("u_MaxSpeed", static_cast<float>(_maxspeed));

    // Boundary maps matching your flat view sizes
    windShader->setUniformValue("u_XMin", -1.0f);
    windShader->setUniformValue("u_XMax", 1.0f);
    windShader->setUniformValue("u_YMin", -0.50f);
    windShader->setUniformValue("u_YMax", 0.50f);

    // Bind single arrow line template and execute instanced draw call
    f->glBindBuffer(GL_ARRAY_BUFFER, windVBO);
    f->glEnableVertexAttribArray(0);
    f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    // Draw thousands of distinct field arrows simultaneously!
    f->glDrawArraysInstanced(GL_LINES, 0, 6, totalInstances);

    f->glDisableVertexAttribArray(0);
    f->glBindBuffer(GL_ARRAY_BUFFER, 0);
    windShader->release();
}

