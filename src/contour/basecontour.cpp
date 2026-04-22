#include "basecontour.h"

//Constructor
BaseContour::BaseContour(ColorTable *ct)
{
    colorTable = ct;

    iwidth  = 1000;
    iheight = 1000;

    grlist = -1;
    srlist = -1;

    appid = -1;
    mapid = -1;
    wksid = -1;
    dataid = -1;
    cnid = -1;

    _z_id = NULL;
    _y_id = NULL;
    _x_id = NULL;

    initialize();

    reset();

    _onx = 0;
    _ony = 0;
    _onz = 0;
    _ont = 0;
    _onf = 0;

    _opacity = 100;

    set_cnMaxLev(20);
    cnLevelSelectionMode = "ManualLevels";
    cnMinLevelValF = 0.0;
    cnMaxLevelValF = 20.0;
    cnLevelSpacingF = 1.0;
}

BaseContour::~BaseContour()
{
    if(_onx)
        free(_x_id);
    if(_ony)
        free(_y_id);
    if(_onz)
        free(_z_id);

    finalize();
}

void BaseContour::reset(void)
{
    _time = 0;
    _opacity = 100;
}

void BaseContour::initialize()
{
    float *cmap = colorTable->get_cmap();

    clen[0] = colorTable->get_clen();;
    clen[1] = 3;

  //surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, (float)iwidth, (float)iheight);

  //setCairoQtSurface(surface);
  //setCairoQtWinSize(iwidth, iheight);
}

void BaseContour::update_colormap()
{
    float *cmap = colorTable->get_cmap();

    clen[0] = colorTable->get_clen();;
    clen[1] = 3;

    reinitialize();
}

void BaseContour::finalize()
{
  //Destroy the surface
  //cairo_surface_destroy(surface);
}

void BaseContour::reinitialize()
{
    int n;

    if(_onx)
    {
        for(n = 0; n < _onx; ++n)
        {
            if(0 < _x_id[n])
            {
                if(glIsTexture(_x_id[n]))
                    glDeleteTextures(1, &_x_id[n]);
            }

            _x_id[n] = 0;
        }
    }

    if(_ony)
    {
        for(n = 0; n < _ony; ++n)
        {
            if(0 < _y_id[n])
            {
                if(glIsTexture(_y_id[n]))
                    glDeleteTextures(1, &_y_id[n]);
            }

            _y_id[n] = 0;
        }
    }

    if(_onz)
    {
        for(n = 0; n < _onz; ++n)
        {
            if(0 < _z_id[n])
            {
                if(glIsTexture(_z_id[n]))
                    glDeleteTextures(1, &_z_id[n]);
            }

            _z_id[n] = 0;
        }
    }

    _onx = 0;
    _ony = 0;
    _onz = 0;
    _ont = 0;
}

void BaseContour::_general_setup()
{
    float opacity = 0.01 * _opacity;
}

void BaseContour::_viewport_setup()
{
}   

void BaseContour::_get_viewportXinfo()
{
    _yStartX = vpxX;
    _zStartX = vpyX - vphX;

    _yEndX = vpxX + vpwX;
    _zEndX = vpyX;

  //cout << "Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__
  //     << ", file: <" << __FILE__ << ">" << endl;
  //cout << "\tvpxX = " << vpxX << ", vpyX = " << vpyX;
  //cout << ", vpwX = " << vpwX << ", vpwX = " << vpwX << endl;
  //cout << "\t_yStartX = " << _yStartX << ", _zStartX = " << _zStartX;
  //cout << ", _zEndX   = " << _yEndX   << ", _zEnd    = " << _zEndX << endl;
}

void BaseContour::_viewportXsetup()
{
}  

void BaseContour::_get_viewportYinfo()
{
    _xStartY = vpxY;
    _zStartY = vpyY - vphY;

    _xEndY = vpxY + vpwY;
    _zEndY = vpyY;

  //cout << "Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__
  //     << ", file: <" << __FILE__ << ">" << endl;
  //cout << "\tvpxY = " << vpxY << ", vpyY = " << vpyY;
  //cout << ", vpwY = " << vpwY << ", vpwY = " << vpwY << endl;
  //cout << "\t_xStartY = " << _xStartY << ", _zStartY = " << _zStartY;
  //cout << ", _xEndY   = " << _xEndY   << ", _zEndY   = " << _zEndY << endl;
}

void BaseContour::_viewportYsetup()
{
}

void BaseContour::initialize_context()
{
  //Set surface to translucent color (r, g, b, a) without disturbing graphics state.
  //context = getCairoQtContext();
  //cairo_set_source_rgba(context, 1.0, 1.0, 1.0, 1.0);
  //cairo_save(context);
  //cairo_set_operator(context, CAIRO_OPERATOR_SOURCE);
  //cairo_paint(context);
  //cairo_restore(context);
}

unsigned int BaseContour::get_textureID(unsigned int textureID)
{
    unsigned char *pixels;
    unsigned int   width;
    unsigned int   height;
    unsigned int   newTextureID;

  //pixels = static_cast<unsigned char *>(cairo_image_surface_get_data(surface));
  //width  = cairo_image_surface_get_width(surface);
  //height = cairo_image_surface_get_height(surface);

    if(textureID)
    {
        if(glIsTexture(textureID))
            glDeleteTextures(1, &textureID);
    }

    glEnable(GL_TEXTURE_2D);

  //Set pixel storage mode 
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        
  //Generate a texture name
    glGenTextures(1, &newTextureID);

  //cout << "Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__
  //     << ", file: <" << __FILE__ << ">" << endl;
  //cout << "\tnewTextureID  = " << newTextureID << endl;

    glBindTexture(GL_TEXTURE_2D, newTextureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, pixels);

    glDisable(GL_TEXTURE_2D);

    return newTextureID;
}

void BaseContour::setup(string vn, float *var)
{
     _varname  = vn;
     _value = var;

    reinitialize();

    set_nx(geometry->get_nx());
    set_ny(geometry->get_ny());
    set_nz(geometry->get_nz());
    set_nt(geometry->get_nt());
    set_nf(geometry->get_nf());

    _time = 0;
  //_opacity = 100;

    vpx = 0.2;
    vpy = 0.8;
    vpw = 0.6;
    vph = 0.6;

    vpxX = vpx;
    vpyX = vpy;
    vpwX = vpw;
    vphX = vph;

    vpxY = vpx;
    vpyY = vpy;
    vpwY = vpw;
    vphY = vph;

    _xStart = vpx;
    _yStart = vpy - vph;

    _xEnd = vpx + vpw;
    _yEnd = vpy;

    _yStartX = vpxX;
    _zStartX = vpyX - vphX;

    _yEndX = vpxX + vpwX;
    _zEndX = vpyX;

    _xStartY = vpxY;
    _zStartY = vpyY - vphY;

    _xEndY = vpxY + vpwY;
    _zEndY = vpyY;
}

void BaseContour::set_nx(int nx)
{
    int n;

    _nx = nx;
    if(_onx)
    {
        if(nx > _onx)
        {
            _x_id = (unsigned int *)realloc(_x_id, nx * sizeof(unsigned int));
            assert(_x_id);
        }
    }
    else
    {
        _x_id = (unsigned int *)calloc(nx, sizeof(unsigned int));
        assert(_x_id);
    }

    for(n = 0; n < _onx; ++n)
    {
        _x_id[n] = 0;
    }

    _onx = nx;
}

void BaseContour::set_ny(int ny)
{
    int n;

    _ny = ny;
    if(_ony)
    {
        if(ny > _ony)
        {
            _y_id = (unsigned int *)realloc(_y_id, ny * sizeof(unsigned int));
            assert(_y_id);
        }
    }
    else
    {
        _y_id = (unsigned int *)calloc(ny, sizeof(unsigned int));
        assert(_y_id);
    }

    for(n = 0; n < ny; ++n)
    {
        _y_id[n] = 0;
    }

    _ony = ny;
}

void BaseContour::set_nz(int nz)
{
    int n;

    _nz = nz;
    if(_onz)
    {
        if(nz > _onz)
        {
            _z_id = (unsigned int *)realloc(_z_id, nz * sizeof(unsigned int));
            assert(_z_id);
        }
    }
    else
    {
        _z_id = (unsigned int *)calloc(nz, sizeof(unsigned int));
        assert(_z_id);
    }

    for(n = 0; n < nz; ++n)
    {
        _z_id[n] = 0;
    }

    _onz = nz;
}

