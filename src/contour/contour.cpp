#include "contour.h"

#ifdef debug
#undef debug
#endif

#include <ncarg/hlu/MapPlot.h>
#include "nclInterface.h"


//Constructor
Contour::Contour(ColorTable *ct)
{
    evaluator = new Evaluator();

    dims[0] = 1;
    dims[1] = 1;

    pltvar = NULL;

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

    _opacity = 100;

    set_cnMaxLev(20);
    cnLevelSelectionMode = "ManualLevels";
    cnMinLevelValF = 0.0;
    cnMaxLevelValF = 20.0;
    cnLevelSpacingF = 1.0;

    hasGeoInfo = false;
}

Contour::~Contour()
{
    if(_onx)
        free(_x_id);
    if(_ony)
        free(_y_id);
    if(_onz)
        free(_z_id);

    delete evaluator;

    finalize();
}

void Contour::reset(void)
{
    _opacity = 100;
}

void Contour::initialize()
{
    float *cmap = colorTable->get_cmap();

    clen[0] = colorTable->get_clen();;
    clen[1] = 3;

#ifdef USENCL
  //Initialize the high level utility library
    guiNhlInitialize();

    surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, (float)iwidth, (float)iheight);

    setCairoQtSurface(surface);
    setCairoQtWinSize(iwidth, iheight);

  //Create an application context. Set the app dir to the current directory
  //so the application looks for a resource file in the working directory.
  //In this example the resource file supplies the plot title only.
    srlist = guiNhlRLCreate(NhlSETRL);
    guiNhlRLClear(srlist);
    guiNhlRLSetString(srlist,(char *)NhlNappUsrDir,"./");
    guiNhlCreate(&appid,"contour",NhlappClass,NhlDEFAULT_APP,srlist);

    guiNhlRLClear(srlist);
    guiNhlRLSetString(srlist,(char *)NhlNwkFormat,"qt");
    guiNhlCreate(&wksid,"workstation",NhlcairoQtWorkstationClass,appid,srlist);

    guiNhlRLClear(srlist);
    guiNhlRLSetMDFloatArray(srlist,NhlNwkColorMap,cmap,2,clen);
    guiNhlSetValues(wksid,srlist);

    guiNhlSetColor(wksid, 0, 1.0, 1.0, 1.0);
    guiNhlSetColor(wksid, 1, 0.0, 0.0, 1.0);
#endif
}

void Contour::update_colormap()
{
    float *cmap = colorTable->get_cmap();

    clen[0] = colorTable->get_clen();;
    clen[1] = 3;

#ifdef USENCL
    guiNhlRLClear(srlist);
    guiNhlRLSetMDFloatArray(srlist,NhlNwkColorMap,cmap,2,clen);
    guiNhlSetValues(wksid,srlist);

    reinitialize();
#endif
}

void Contour::finalize()
{
#ifdef USENCL
  //Destroy the objects created, close the HLU library and exit.
    guiNhlDestroy(wksid);
    guiNhlDestroy(appid);

  //Destroy the surface
    cairo_surface_destroy(surface);

    guiNhlClose();
#endif
}

void Contour::reinitialize()
{
    int n;

    hasGeoInfo = false;

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
}

void Contour::_general_setup()
{
    float opacity = 0.01 * _opacity;

    guiNhlRLClear(srlist);
    guiNhlRLSetFloat(srlist,NhlNcnLineThicknessF,2.0);
    guiNhlRLSetFloat(srlist,NhlNcnFillOpacityF,opacity);
  //guiNhlRLSetString(srlist,NhlNtxFontColor,"Blue");
  //guiNhlRLSetString(srlist,NhlNtmXBLabelFontColor,"Blue");
  //guiNhlRLSetString(srlist,NhlNtmXTLabelFontColor,"Blue");
  //guiNhlRLSetString(srlist,NhlNtmYLLabelFontColor,"Blue");
  //guiNhlRLSetString(srlist,NhlNtmYRLabelFontColor,"Blue");
    guiNhlRLSetString(srlist,NhlNcnMonoLineDashPattern,"false");
    guiNhlRLSetString(srlist,NhlNcnMonoLineColor,"false");
    guiNhlSetValues(cnid,srlist);

    guiNhlRLClear(srlist);
    guiNhlRLSetFloat(srlist,NhlNcnLineThicknessF,1.0);
    guiNhlRLSetString(srlist,NhlNcnMonoLineDashPattern,"true");
    guiNhlRLSetString(srlist,NhlNcnMonoLineColor,"true");
    guiNhlRLSetString(srlist,NhlNcnFillOn,"true");
    guiNhlRLSetString(srlist,NhlNcnMonoFillColor,"true");
    guiNhlRLSetString(srlist,NhlNcnMonoFillPattern,"false");

  //Turn off the contour lines and labels.
    guiNhlRLSetString(srlist,NhlNcnLinesOn,"False");
    guiNhlRLSetString(srlist,NhlNcnLineLabelsOn,"False");
    guiNhlRLSetString(srlist,NhlNcnHighLabelsOn,"False");
    guiNhlRLSetString(srlist,NhlNcnLowLabelsOn,"False");
    guiNhlRLSetString(srlist,NhlNcnInfoLabelOn,"False");

  //guiNhlRLSetString(srlist,NhlNtmYLMode,"EXPLICIT");
  //guiNhlRLSetFloat(srlist,NhlNtmXBLabelFontHeightF,.016);
  //guiNhlRLSetFloat(srlist,NhlNtmYLLabelFontHeightF,.016);
  //guiNhlRLSetString(srlist,NhlNtmXBLabelFont,"times-roman");
  //guiNhlRLSetString(srlist,NhlNtmYLLabelFont,"times-roman");
  //guiNhlRLSetString(srlist,NhlNtmXBMinorOn,"False");
  //guiNhlRLSetString(srlist,NhlNtmYLMinorOn,"False");
    guiNhlRLSetString(srlist,NhlNpmLabelBarDisplayMode,"ALWAYS");
    guiNhlRLSetFloat(srlist,NhlNpmLabelBarHeightF,.15);
    guiNhlRLSetFloat(srlist,NhlNpmLabelBarWidthF,vpw);
    guiNhlRLSetString(srlist,NhlNpmLabelBarSide,"bottom");
    guiNhlRLSetString(srlist,NhlNlbOrientation,"horizontal");
    guiNhlRLSetInteger(srlist,NhlNlbBoxLinesOn,0);
    guiNhlRLSetString(srlist,NhlNlbLabelsOn,"True");
    guiNhlRLSetString(srlist,NhlNlbPerimOn,"False");
    guiNhlRLSetString(srlist,NhlNlbAutoManage,"False");
    guiNhlRLSetFloat(srlist,NhlNlbLabelFontHeightF,0.015);
    guiNhlRLSetString(srlist,NhlNlbLabelFont,"times-roman");

    guiNhlSetValues(cnid,srlist);

    guiNhlRLClear(srlist);
  //guiNhlRLSetInteger(srlist,NhlNcnLineColor,NhlBACKGROUND);
    guiNhlRLSetString(srlist,NhlNcnMonoFillColor,"false");
    guiNhlRLSetString(srlist,NhlNcnMonoFillPattern,"true");
    guiNhlSetValues(cnid,srlist);


    guiNhlRLClear(srlist);
  //guiNhlRLSetString(srlist,NhlNcnLevelSelectionMode,cnLevelSelectionMode.c_str());
    guiNhlRLSetString(srlist,NhlNcnLevelSelectionMode,"ManualLevels");
    guiNhlRLSetFloat(srlist,NhlNcnMinLevelValF,cnMinLevelValF);
    guiNhlRLSetFloat(srlist,NhlNcnMaxLevelValF,cnMaxLevelValF);
    guiNhlRLSetFloat(srlist,NhlNcnLevelSpacingF,cnLevelSpacingF);
    guiNhlSetValues(cnid,srlist);
}

void Contour::_viewport_setup()
{
    guiNhlRLSetFloat(srlist,NhlNvpXF,vpx);
    guiNhlRLSetFloat(srlist,NhlNvpYF,vpy);
    guiNhlRLSetFloat(srlist,NhlNvpWidthF,vpw);
    guiNhlRLSetFloat(srlist,NhlNvpHeightF,vph);
    guiNhlSetValues(cnid,srlist);
}   

void Contour::_get_viewportXinfo()
{
    grlist = guiNhlRLCreate(NhlGETRL);
    guiNhlRLClear(grlist);
    guiNhlRLGetFloat(grlist,NhlNvpXF,&vpxX);
    guiNhlRLGetFloat(grlist,NhlNvpYF,&vpyX);
    guiNhlRLGetFloat(grlist,NhlNvpWidthF,&vpwX);
    guiNhlRLGetFloat(grlist,NhlNvpHeightF,&vphX);
    guiNhlGetValues(cnid,grlist);

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

void Contour::_viewportXsetup()
{
    guiNhlRLSetFloat(srlist,NhlNvpXF,vpxX);
    guiNhlRLSetFloat(srlist,NhlNvpYF,vpyX);
    guiNhlRLSetFloat(srlist,NhlNvpWidthF,vpwX);
    guiNhlRLSetFloat(srlist,NhlNvpHeightF,vphX);
    guiNhlSetValues(cnid,srlist);
}  

void Contour::_get_viewportYinfo()
{
    grlist = guiNhlRLCreate(NhlGETRL);
    guiNhlRLClear(grlist);
    guiNhlRLGetFloat(grlist,NhlNvpXF,&vpxY);
    guiNhlRLGetFloat(grlist,NhlNvpYF,&vpyY);
    guiNhlRLGetFloat(grlist,NhlNvpWidthF,&vpwY);
    guiNhlRLGetFloat(grlist,NhlNvpHeightF,&vphY);
    guiNhlGetValues(cnid,grlist);

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

void Contour::_viewportYsetup()
{
    guiNhlRLSetFloat(srlist,NhlNvpXF,vpxY);
    guiNhlRLSetFloat(srlist,NhlNvpYF,vpyY);
    guiNhlRLSetFloat(srlist,NhlNvpWidthF,vpwY);
    guiNhlRLSetFloat(srlist,NhlNvpHeightF,vphY);
    guiNhlSetValues(cnid,srlist);
}

void Contour::initialize_context()
{
#ifdef USENCL
  //Set surface to translucent color (r, g, b, a) without disturbing graphics state.
    context = getCairoQtContext();
    cairo_set_source_rgba(context, 1.0, 1.0, 1.0, 1.0);
  //cairo_fill(context);
    cairo_save(context);
    cairo_set_operator(context, CAIRO_OPERATOR_SOURCE);
  //cairo_set_operator(context, CAIRO_OPERATOR_CLEAR);
    cairo_paint(context);
    cairo_restore(context);
#endif
}

unsigned int Contour::get_textureID(unsigned int textureID)
{
#ifdef USENCL
    unsigned char *pixels;
    unsigned int   width;
    unsigned int   height;
    unsigned int   newTextureID;

    pixels = static_cast<unsigned char *>(cairo_image_surface_get_data(surface));
    width  = cairo_image_surface_get_width(surface);
    height = cairo_image_surface_get_height(surface);

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
#else
    return 0;
#endif
}

void Contour::setup(string vn, float *var)
{
    size_t size;

    _varname  = vn;
    _value = var;

    reinitialize();

    set_nx(geometry->get_nx());
    set_ny(geometry->get_ny());
    set_nz(geometry->get_nz());

    if(geometry->get_has1dLon() && geometry->get_has1dLat())
    {
        hasGeoInfo = true;
    }

  //_opacity = 100;

    size = geometry->get_nx() * geometry->get_ny() * geometry->get_nz();

    evaluator->set_value(size, _value);

    _minval = evaluator->get_min();
    _maxval = evaluator->get_max();

    guiGetEndpointsAndStepSize((float)_minval, (float)_maxval, cnMaxLev, 1,
                               &cnMinLevelValF, &cnMaxLevelValF,
                               &cnLevelSpacingF);

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

void Contour::set_nx(int nx)
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

void Contour::set_ny(int ny)
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

void Contour::set_nz(int nz)
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


unsigned int Contour::get_xid(int n)
{
    int j, k, m;
    float* flat;
    float* flev;

    if(0 < _x_id[n])
        return _x_id[n];

#ifdef USENCL
    initialize_context();

    if(n < _nx)
    {
        flat = new float[_ny];
        flev = new float[_nz];
        if(hasGeoInfo)
        {
            lat = geometry->get_lat();
            lev = geometry->get_lev();

            for(j = 0; j < _ny; ++j)
            {
                flat[j] = (float)lat[j];
            }

            lev = geometry->get_lev();

            for(k = 0; k < _nz; ++k)
            {
                flev[k] = (float)lev[k];
            }
        }

        pltvar = new float[_nz * _ny];

        for(k = 0; k < _nz; ++k)
        {
            for(j = 0; j < _ny; ++j)
            {
                m = n + (j + k * _ny) * _nx;
                pltvar[j + k * _ny] = (float) _value[m];
            }
        }

      //cout << "\tpltvar[0] = " << pltvar[0] << endl;

      //Create a ScalarField data object using the data set defined above.
        guiNhlRLClear(srlist);
        dims[0] = _nz;
        dims[1] = _ny;
        guiNhlRLSetMDFloatArray(srlist,(char *)NhlNsfDataArray,pltvar,2,dims);

        if(hasGeoInfo)
        {
          //cout << "\tlev[0] = " << lev[0] << endl;
          //cout << "\tlev[_nz-1] = " << lev[_nz-1] << endl;

            guiNhlRLSetFloat(srlist,NhlNtmYLDataTopF,flev[0]);
            guiNhlRLSetFloat(srlist,NhlNtmYLDataBottomF,flev[_nz-1]);
            guiNhlRLSetFloat(srlist,NhlNtmXBDataRightF,flat[_ny-1]);
            guiNhlRLSetFloat(srlist,NhlNtmXBDataLeftF,flat[0]);
            guiNhlRLSetInteger(srlist,NhlNtmYLStyle,NhlIRREGULAR);
            guiNhlRLSetInteger(srlist,NhlNtmXBMode,NhlEXPLICIT);
            guiNhlRLSetInteger(srlist,NhlNtmXBMinorOn,False);
            guiNhlRLSetFloatArray(srlist,NhlNtmXBValues,geometry->get_ylvalues(),geometry->get_nlabels());
        }

        guiNhlCreate(&dataid,"x_plane",NhlscalarFieldClass,appid,srlist);

      //Create a ContourPlot object, supplying the ScalarField object as data
        guiNhlRLClear(srlist);
        guiNhlRLSetInteger(srlist,(char *)NhlNcnScalarFieldData,dataid);
        guiNhlCreate(&cnid, "ContourPlot", NhlcontourPlotClass, wksid, srlist);

        _general_setup();

        guiNhlSetValues(cnid,srlist);

      //Draw the contour
        guiNhlDraw(cnid);

        _get_viewportXinfo();

        guiNhlFrame(wksid);

        delete [] pltvar;
        delete [] flat;
        delete [] flev;
    }

    _x_id[n] = get_textureID(_x_id[n]);

  //Destroy the objects created, close the HLU library and exit.
    guiNhlDestroy(dataid);
    guiNhlDestroy(cnid);
#else
    _y_id[n] = 0;
#endif

    return _x_id[n];
}

unsigned int Contour::get_yid(int n)
{
    int i, k, m;
    float* flon;
    float* flev;

    if(0 < _y_id[n])
        return _y_id[n];

#ifdef USENCL
    initialize_context();

    if(n < _ny)
    {
        flon = new float[_nx];
        flev = new float[_nz];
        if(hasGeoInfo)
        {
            lon = geometry->get_lon();

            for(i = 0; i < _nx; ++i)
            {
                flon[i] = (float)lon[i];
            }

            lev = geometry->get_lev();

            for(k = 0; k < _nz; ++k)
            {
                flev[k] = (float)lev[k];
            }
        }
        pltvar = new float[_nz * _nx];

        for(k = 0; k < _nz; ++k)
        {
            for(i = 0; i < _nx; ++i)
            {
                m = i + (n + k * _ny) * _nx;
                pltvar[i + k * _nx] = (float) _value[m];
            }
        }

      //cout << "\tpltvar[0] = " << pltvar[0] << endl;

      //Create a ScalarField data object using the data set defined above.
        guiNhlRLClear(srlist);
        dims[0] = _nz;
        dims[1] = _nx;
        guiNhlRLSetMDFloatArray(srlist,(char *)NhlNsfDataArray,pltvar,2,dims);

        if(hasGeoInfo)
        {
          //cout << "\tlev[0] = " << lev[0] << endl;
          //cout << "\tlev[_nz-1] = " << lev[_nz-1] << endl;

            guiNhlRLSetFloat(srlist,NhlNtmYLDataTopF,flev[0]);
            guiNhlRLSetFloat(srlist,NhlNtmYLDataBottomF,flev[_nz-1]);
            guiNhlRLSetFloat(srlist,NhlNtmXBDataRightF,flon[0]);
            guiNhlRLSetFloat(srlist,NhlNtmXBDataLeftF,flon[_nx - 1]);
            guiNhlRLSetInteger(srlist,NhlNtmYLStyle,NhlIRREGULAR);
            guiNhlRLSetInteger(srlist,NhlNtmXBMode,NhlEXPLICIT);
            guiNhlRLSetInteger(srlist,NhlNtmXBMinorOn,False);
            guiNhlRLSetFloatArray(srlist,NhlNtmXBValues,geometry->get_xbvalues(),geometry->get_nlabels());
        }

        guiNhlCreate(&dataid,"x_plane",NhlscalarFieldClass,appid,srlist);

      //Create a ContourPlot object, supplying the ScalarField object as data
        guiNhlRLClear(srlist);
        guiNhlRLSetInteger(srlist,(char *)NhlNcnScalarFieldData,dataid);
        guiNhlCreate(&cnid, "ContourPlot", NhlcontourPlotClass, wksid, srlist);

        _general_setup();

        guiNhlSetValues(cnid,srlist);

      //Draw the contour
        guiNhlDraw(cnid);

        _get_viewportYinfo();

        guiNhlFrame(wksid);

        delete [] pltvar;
        delete [] flon;
        delete [] flev;
    }

    _y_id[n] = get_textureID(_y_id[n]);

  //Destroy the objects created, close the HLU library and exit.
    guiNhlDestroy(dataid);
    guiNhlDestroy(cnid);
#else
    _y_id[n] = 0;
#endif

    return _y_id[n];
}

unsigned int Contour::get_zid(int n)
{
    int i, j, m;
    float* flon;
    float* flat;

    if(0 < _z_id[n])
        return _z_id[n];

#ifdef USENCL
  //cout << "\tAt Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__
  //     << ", file: <" << __FILE__ << ">" << endl << endl;
  //cout << "\tn = " << n << endl;
  //cout << "\t_nx = " << _nx << endl;
  //cout << "\t_ny = " << _ny << endl;
  //cout << "\t_nz = " << _nz << endl;

    if(n >= _nz)
        return 0;

    initialize_context();

    flon = new float[_nx];
    flat = new float[_ny];
    if(hasGeoInfo)
    {
        lon = geometry->get_lon();
        lat = geometry->get_lat();

        for(i = 0; i < _nx; ++i)
        {
            flon[i] = (float)lon[i];
        }

        lat = geometry->get_lat();

        for(j = 0; j < _ny; ++j)
        {
            flat[j] = (float)lat[j];
        }
    }

    pltvar = new float[_ny * _nx];

    for(j = 0; j < _ny; ++j)
    {
        m = j * _nx;
        for(i = 0; i < _nx; ++i)
        {
            pltvar[m + i] = (float) _value[n * _ny * _nx + m + i];
        }
    }
  //cout << "\tAt Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__
  //     << ", file: <" << __FILE__ << ">" << endl << endl;
  //cout << "\tpltvar[0] = " << pltvar[0] << endl;
  //cout << "\tpltvar[" << _ny * _nx - 1 << "] = " << pltvar[_ny * _nx - 1] << endl;

  //Create a ScalarField data object using the data set defined above.
    guiNhlRLClear(srlist);
    dims[0] = _ny;
    dims[1] = _nx;
    guiNhlRLSetMDFloatArray(srlist,(char *)NhlNsfDataArray,pltvar,2,dims);

    if(hasGeoInfo)
    {
        guiNhlRLSetFloat(srlist,NhlNsfXCStartV,flon[0]);
        guiNhlRLSetFloat(srlist,NhlNsfXCEndV,flon[_nx-1]);
        guiNhlRLSetFloat(srlist,NhlNsfYCStartV,flat[0]);
        guiNhlRLSetFloat(srlist,NhlNsfYCEndV,flat[_ny-1]);
    }

    guiNhlCreate(&dataid,"z_plane",NhlscalarFieldClass,appid,srlist);

  //Create a ContourPlot object, supplying the ScalarField object as data
    guiNhlRLClear(srlist);
    guiNhlRLSetInteger(srlist,(char *)NhlNcnScalarFieldData,dataid);
    guiNhlCreate(&cnid, "ContourPlot", NhlcontourPlotClass, wksid, srlist);

    _general_setup();
    _tick_setup();

    guiNhlSetValues(cnid,srlist);

    if(hasGeoInfo)
    {
        _map_setup();

      //Draw the contour & map
        guiNhlAddOverlay(mapid, cnid, -1);

        _get_viewport_info();

        guiNhlDraw(mapid);
    }
    else
    {
        _get_viewport_info();

        guiNhlDraw(cnid);
    }

    guiNhlFrame(wksid);

    delete [] pltvar;
    delete [] flon;
    delete [] flat;

    _z_id[n] = get_textureID(_z_id[n]);

  //Destroy the objects created, close the HLU library and exit.
    guiNhlDestroy(dataid);
    guiNhlDestroy(cnid);
    if(0 < mapid)
        guiNhlDestroy(mapid);
#else
    _z_id[n] = 0;
#endif

    return _z_id[n];
}

void Contour::_tick_setup()
{
    if(hasGeoInfo)
        guiNhlRLSetString(srlist,NhlNtiXAxisString,"Longitude");
    else
        guiNhlRLSetString(srlist,NhlNtiXAxisString,"X-Direction");

    guiNhlRLSetString(srlist,NhlNtiXAxisFont,"helvetica");
    guiNhlRLSetFloat(srlist,NhlNtiXAxisFontHeightF,.02);

    if(hasGeoInfo)
        guiNhlRLSetString(srlist,NhlNtiYAxisString,"Latitude");
    else
        guiNhlRLSetString(srlist,NhlNtiYAxisString,"Y-Direction");

    guiNhlRLSetString(srlist,NhlNtiYAxisFont,"helvetica");
    guiNhlRLSetFloat(srlist,NhlNtiYAxisFontHeightF,.02);

  //Set resources of the inherited Title objects.
    guiNhlRLSetString(srlist,NhlNtiMainString,_varname.c_str());
  //guiNhlRLSetString(srlist,NhlNtiMainString,title);
    guiNhlRLSetString(srlist,NhlNtiMainFont,"helvetica");
    guiNhlRLSetFloat(srlist,NhlNtiMainFontHeightF,.03);

    if(hasGeoInfo)
    {
      //Set resources of the inherited TickMark object.
        guiNhlRLSetString(srlist,NhlNtmXTMode,"AUTOMATIC");
        guiNhlRLSetString(srlist,NhlNtmXBMode,"EXPLICIT");
        guiNhlRLSetFloatArray(srlist,NhlNtmXBValues,geometry->get_xbvalues(),geometry->get_nlabels());
        guiNhlRLSetStringArray(srlist,NhlNtmXBLabels,geometry->get_xblabels(),geometry->get_nlabels());
        guiNhlRLSetString(srlist,NhlNtmYRMode,"Automatic");
        guiNhlRLSetString(srlist,NhlNtmYLMode,"EXPLICIT");
        guiNhlRLSetFloatArray(srlist,NhlNtmYLValues,geometry->get_ylvalues(),geometry->get_nlabels());
        guiNhlRLSetStringArray(srlist,NhlNtmYLLabels,geometry->get_yllabels(),geometry->get_nlabels());
    }
}

void Contour::_map_setup()
{
    mapid = -1;

    guiNhlRLClear(srlist);

    guiNhlRLSetFloat(srlist,NhlNvpXF,vpx);
    guiNhlRLSetFloat(srlist,NhlNvpXF,vpx);
    guiNhlRLSetFloat(srlist,NhlNvpYF,vpy);
    guiNhlRLSetFloat(srlist,NhlNvpWidthF,vpw);
    guiNhlRLSetFloat(srlist,NhlNvpHeightF,vph);

    guiNhlRLSetString(srlist,NhlNmpOutlineOn, "True");
    guiNhlRLSetString(srlist,NhlNmpOutlineBoundarySets, "USStates");
    guiNhlRLSetString(srlist,NhlNmpUSStateLineColor, "Black");

    guiNhlRLSetString(srlist,NhlNmpFillOn,"True");
    guiNhlRLSetString(srlist,NhlNmpLandFillColor,"Black");
    guiNhlRLSetString(srlist,NhlNmpOceanFillColor,"Transparent");
    guiNhlRLSetString(srlist,NhlNmpInlandWaterFillColor,"Black");
    guiNhlRLSetString(srlist,NhlNmpGridLineColor,"LightGray");

    guiNhlRLSetString(srlist,NhlNmpGeophysicalLineColor,"Black");
    guiNhlRLSetFloat(srlist,NhlNmpGeophysicalLineThicknessF,1.);

  //guiNhlRLSetFloat(srlist,NhlNmpCenterLonF,180.0);

    guiNhlRLSetString(srlist,NhlNmpDataBaseVersion, "MediumRes");
  //guiNhlRLSetString(srlist,NhlNmpDataResolution, "MediumRes");

    guiNhlCreate(&mapid,"map",NhlmapPlotClass,wksid,srlist);
}

void Contour::_get_viewport_info()
{
    guiNhlRLClear(grlist);
    grlist = guiNhlRLCreate(NhlGETRL);
    guiNhlRLGetFloat(grlist,NhlNvpXF,&vpw);
    guiNhlRLGetFloat(grlist,NhlNvpYF,&vpy);
    guiNhlRLGetFloat(grlist,NhlNvpWidthF,&vpw);
    guiNhlRLGetFloat(grlist,NhlNvpHeightF,&vph);
    guiNhlGetValues(mapid,grlist);

    _xStart = vpx;
    _yStart = vpy - vph;

    _xEnd = vpx + vpw;
    _yEnd = vpy;
}   

