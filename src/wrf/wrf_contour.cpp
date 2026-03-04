// 1. Include NCL/NCARG headers first
#include <ncarg/hlu/MapPlot.h>

// 2. Kill the 'debug' macro defined by NCL's headers
#ifdef debug
#undef debug
#endif

// 3. NOW include your project headers and Qt
#include "nclInterface.h"
#include "wrf_contour.h" // This likely includes QtOpenGL/Qt

//Constructor
WRF_Contour::WRF_Contour(ColorTable *ct)
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

    initialize();

    texturekeeper = new TextureKeeper();

    _opacity = 100;

    set_cnMaxLev(20);
    cnLevelSelectionMode = "ManualLevels";
    cnMinLevelValF = 0.0;
    cnMaxLevelValF = 20.0;
    cnLevelSpacingF = 1.0;

    reset();
}

WRF_Contour::~WRF_Contour()
{
    delete texturekeeper;

    finalize();
}

void WRF_Contour::reset(void)
{
    _opacity = 100;

    texturekeeper->reset();
}

void WRF_Contour::initialize()
{
    float *cmap = colorTable->get_cmap();

    clen[0] = colorTable->get_clen();;
    clen[1] = 3;

#ifdef USENCL
  //cout << "\tEnter Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
  //Initialize the high level utility library
    guiNhlInitialize();

  //cout << "\t\tFunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, (double)iwidth, (double)iheight);

    setCairoQtSurface(surface);
    setCairoQtWinSize(iwidth, iheight);

  //Create an application context. Set the app dir to the current directory
  //so the application looks for a resource file in the working directory.
  //In this example the resource file supplies the plot title only.
    if(srlist >= 0)
        guiNhlRLClear(srlist);
    srlist = guiNhlRLCreate(NhlSETRL);
    guiNhlRLSetString(srlist,(char *)NhlNappUsrDir,"./");
    guiNhlCreate(&appid,"contour",NhlappClass,NhlDEFAULT_APP,srlist);

    guiNhlRLClear(srlist);
    srlist = guiNhlRLCreate(NhlSETRL);
    guiNhlRLSetString(srlist,(char *)NhlNwkFormat,"qt");
    guiNhlCreate(&wksid,"workstation",NhlcairoQtWorkstationClass,appid,srlist);

    guiNhlRLClear(srlist);
    srlist = guiNhlRLCreate(NhlSETRL);
    guiNhlRLSetMDFloatArray(srlist,NhlNwkColorMap,cmap,2,clen);
    guiNhlSetValues(wksid,srlist);

    guiNhlSetColor(wksid, 0, 1.0, 1.0, 1.0);
    guiNhlSetColor(wksid, 1, 0.0, 0.0, 1.0);
  //cout << "\tLeave Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
#endif
}

void WRF_Contour::update_colormap()
{
    float *cmap = colorTable->get_cmap();

    clen[0] = colorTable->get_clen();;
    clen[1] = 3;

#ifdef USENCL
  //cout << "\tEnter Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    guiNhlRLClear(srlist);
    guiNhlRLSetMDFloatArray(srlist,NhlNwkColorMap,cmap,2,clen);
    guiNhlSetValues(wksid,srlist);
  //cout << "\tLeave Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
#endif

    texturekeeper->reinitialize(geometry->get_nx(), geometry->get_ny(), geometry->get_nz());
}

void WRF_Contour::finalize()
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

void WRF_Contour::_general_setup()
{
    float opacity = 0.01 * _opacity;

#ifdef USENCL
  //cout << "\tEnter Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
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
  //cout << "\tLeave Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
#endif
}

void WRF_Contour::_viewport_setup()
{
#ifdef USENCL
    guiNhlRLSetFloat(srlist,NhlNvpXF,vpx);
    guiNhlRLSetFloat(srlist,NhlNvpYF,vpy);
    guiNhlRLSetFloat(srlist,NhlNvpWidthF,vpw);
    guiNhlRLSetFloat(srlist,NhlNvpHeightF,vph);
    guiNhlSetValues(cnid,srlist);
#endif
}   

void WRF_Contour::_get_viewportXinfo()
{
#ifdef USENCL
 //cout << "\tEnter Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
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

  //cout << "\tvpxX = " << vpxX << ", vpyX = " << vpyX;
  //cout << ", vpwX = " << vpwX << ", vpwX = " << vpwX << endl;
  //cout << "\t_yStartX = " << _yStartX << ", _zStartX = " << _zStartX;
  //cout << ", _zEndX   = " << _yEndX   << ", _zEnd    = " << _zEndX << endl;
  //cout << "\tLeave Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
#endif
}

void WRF_Contour::_viewportXsetup()
{
#ifdef USENCL
    guiNhlRLSetFloat(srlist,NhlNvpXF,vpxX);
    guiNhlRLSetFloat(srlist,NhlNvpYF,vpyX);
    guiNhlRLSetFloat(srlist,NhlNvpWidthF,vpwX);
    guiNhlRLSetFloat(srlist,NhlNvpHeightF,vphX);
    guiNhlSetValues(cnid,srlist);
#endif
}  

void WRF_Contour::_get_viewportYinfo()
{
#ifdef USENCL
  //cout << "\tEnter Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
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

  //cout << "\tvpxY = " << vpxY << ", vpyY = " << vpyY;
  //cout << ", vpwY = " << vpwY << ", vpwY = " << vpwY << endl;
  //cout << "\t_xStartY = " << _xStartY << ", _zStartY = " << _zStartY;
  //cout << ", _xEndY   = " << _xEndY   << ", _zEndY   = " << _zEndY << endl;
  //cout << "\tLeave Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
#endif
}

void WRF_Contour::_viewportYsetup()
{
#ifdef USENCL
    guiNhlRLSetFloat(srlist,NhlNvpXF,vpxY);
    guiNhlRLSetFloat(srlist,NhlNvpYF,vpyY);
    guiNhlRLSetFloat(srlist,NhlNvpWidthF,vpwY);
    guiNhlRLSetFloat(srlist,NhlNvpHeightF,vphY);
    guiNhlSetValues(cnid,srlist);
#endif
}

void WRF_Contour::initialize_context()
{
#ifdef USENCL
  //cout << "\tEnter Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
  //Set surface to translucent color (r, g, b, a) without disturbing graphics state.
    context = getCairoQtContext();
    cairo_set_source_rgba(context, 1.0, 1.0, 1.0, 1.0);
    cairo_save(context);
    cairo_set_operator(context, CAIRO_OPERATOR_SOURCE);
    cairo_paint(context);
    cairo_restore(context);
  //cout << "\tLeave Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
#endif
}

unsigned int WRF_Contour::get_textureID()
{
    unsigned char *pixels;
    unsigned int   width;
    unsigned int   height;
    unsigned int   newTextureID;

#ifdef USENCL
  //cout << "\tEnter Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;

    pixels = static_cast<unsigned char *>(cairo_image_surface_get_data(surface));
    width  = cairo_image_surface_get_width(surface);
    height = cairo_image_surface_get_height(surface);

  //cout << "\t\twidth = " << width << ", height = " << height << endl;

    glEnable(GL_TEXTURE_2D);

  //Set pixel storage mode 
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        
  //Generate a texture name
    glGenTextures(1, &newTextureID);

  //cout << "\tFunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
  //cout << "\tnewTextureID  = " << newTextureID << endl;

    glBindTexture(GL_TEXTURE_2D, newTextureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, pixels);

    glDisable(GL_TEXTURE_2D);

  //cout << "\tLeave Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
#endif
    return newTextureID;
}

void WRF_Contour::setup(string vn, double *var)
{
    size_t sz = 0;
    Evaluator evaluator;

    _varname  = vn;
    _value = var;

  //cout << "\tEnter Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
  //cout << "\t_varname: <" << _varname << ">" << endl;
  //cout << "\t_value[0] = " << _value[0] << endl;

    texturekeeper->reinitialize(geometry->get_nx(), geometry->get_ny(), geometry->get_nz());

    set_nx(geometry->get_nx());
    set_ny(geometry->get_ny());
    set_nz(geometry->get_nz());

  //_opacity = 100;

    sz = geometry->get_nx() * geometry->get_ny() * geometry->get_nz();
    evaluator.set_value(sz, var);
    _minval = evaluator.get_min();
    _maxval = evaluator.get_max();

    guiGetEndpointsAndStepSize(_minval, _maxval, cnMaxLev, 1,
                               &cnMinLevelValF, &cnMaxLevelValF,
                               &cnLevelSpacingF);

    if(geometry->isXstaggered())
    {
        lon = geometry->get_ulon();
        lat = geometry->get_ulat();
    }
    else if(geometry->isYstaggered())
    {
        lon = geometry->get_vlon();
        lat = geometry->get_vlat();
    }
    else
    {
        lon = geometry->get_lon();
        lat = geometry->get_lat();
    }

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

  //cout << "\tLeave Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
}

unsigned int WRF_Contour::get_xid(int n)
{
    int j, k, m;
    unsigned int xid = texturekeeper->get_xid(n);

  //cout << "\tEnter Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;

    if(0 < xid)
        return xid;

    initialize_context();

    if(n < _nx)
    {
        pltvar = new float[_nz * _ny];

        for(k = 0; k < _nz; ++k)
        {
            for(j = 0; j < _ny; ++j)
            {
                m = n + (j + k * _ny) * _nx;
                pltvar[j + k * _ny] = _value[m];
            }
        }

      //cout << "Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__
      //     << ", file: <" << __FILE__ << ">" << endl;
      //cout << "\tpltvar[0] = " << pltvar[0] << endl;

      //Create a ScalarField data object using the data set defined above.
        guiNhlRLClear(srlist);
        dims[0] = _nz;
        dims[1] = _ny;
        guiNhlRLSetMDFloatArray(srlist,(char *)NhlNsfDataArray,pltvar,2,dims);

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

        pltvar = NULL;
    }

    xid = get_textureID();
    texturekeeper->set_xid(n, xid);

  //Destroy the objects created, close the HLU library and exit.
    guiNhlDestroy(dataid);
    guiNhlDestroy(cnid);

  //cout << "\tLeave Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;

    return xid;
}

unsigned int WRF_Contour::get_yid(int n)
{
    int i, k, m;

    unsigned int yid = texturekeeper->get_yid(n);

  //cout << "\tEnter Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    if(0 < yid)
        return yid;

    initialize_context();

    if(n < _ny)
    {
        pltvar = new float[_nz * _nx];

        for(k = 0; k < _nz; ++k)
        {
            for(i = 0; i < _nx; ++i)
            {
                m = i + (n + k * _ny) * _nx;
                pltvar[i + k * _nx] = _value[m];
            }
        }

      //cout << "\tpltvar[0] = " << pltvar[0] << endl;

      //Create a ScalarField data object using the data set defined above.
        guiNhlRLClear(srlist);
        dims[0] = _nz;
        dims[1] = _nx;
        guiNhlRLSetMDFloatArray(srlist,(char *)NhlNsfDataArray,pltvar,2,dims);

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

        pltvar = NULL;
    }

    yid = get_textureID();
    texturekeeper->set_yid(n, yid);

  //Destroy the objects created, close the HLU library and exit.
    guiNhlDestroy(dataid);
    guiNhlDestroy(cnid);
  //cout << "\tLeave Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;

    return yid;
}

unsigned int WRF_Contour::get_zid(int n)
{
    int i, j, k;
    unsigned int zid = texturekeeper->get_zid(n);
    float* flon;
    float* flat;

  //cout << "\tEnter Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    if(0 < zid)
    {
      //cout << "Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__
      //     << ", file: <" << __FILE__ << ">" << endl;
      //cout << "\tReuse zid: " << zid << " for var <" << _varname << "> at level " << n << endl;
        return zid;
    }

    initialize_context();

    if(n < _nz)
    {
        pltvar = new float[_ny * _nx];
        flon = new float[_ny * _nx];
        flat = new float[_ny * _nx];

        double* thevar = &_value[n * _ny * _nx];
        for(j = 0; j < _ny; ++j)
        {
            k = j * _nx;
            for(i = 0; i < _nx; ++i)
            {
                pltvar[k+i] = thevar[k+i];
                flon[k+i] = lon[k+i];
                flat[k+i] = lat[k+i];
            }
        }

      //cout << "Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__
      //     << ", file: <" << __FILE__ << ">" << endl;
      //cout << "\tnx = " << _nx << ", ny = " << _ny << endl;
      //cout << "\t_varname: <" << _varname << ">" << endl;
      //cout << "\tpltvar[0] = " << pltvar[0] << endl;

      //Create a ScalarField data object using the data set defined above.
        guiNhlRLClear(srlist);
        dims[0] = _ny;
        dims[1] = _nx;
        guiNhlRLSetMDFloatArray(srlist,(char *)NhlNsfDataArray,pltvar,2,dims);

        guiNhlRLSetMDFloatArray(srlist,(char *)NhlNsfXArray,flon,2,dims);
        guiNhlRLSetMDFloatArray(srlist,(char *)NhlNsfYArray,flat,2,dims);

        guiNhlCreate(&dataid,"z_plane",NhlscalarFieldClass,appid,srlist);

      //Create a ContourPlot object, supplying the ScalarField object as data
        guiNhlRLClear(srlist);
        guiNhlRLSetInteger(srlist,(char *)NhlNcnScalarFieldData,dataid);
        guiNhlCreate(&cnid, "WRF_Contour", NhlcontourPlotClass, wksid, srlist);

        _general_setup();

        _tick_setup();

        guiNhlSetValues(cnid,srlist);

        _map_setup();

      //Draw the contour & map
        guiNhlAddOverlay(mapid, cnid, -1);

        _get_viewport_info();

        guiNhlDraw(mapid);

        guiNhlFrame(wksid);

        delete [] pltvar;
        delete [] flon;
        delete [] flat;
    }

    zid = get_textureID();
    texturekeeper->set_zid(n, zid);

  //Destroy the objects created, close the HLU library and exit.
    guiNhlDestroy(dataid);
    guiNhlDestroy(cnid);

    if(0 < mapid)
        guiNhlDestroy(mapid);

  //cout << "\tGenerate zid: " << zid << " for <" << _varname << "> at level " << n << endl;
  //cout << "\tLeave Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;

    return zid;
}

void WRF_Contour::_get_viewport_info()
{
  //cout << "\tEnter Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    if(grlist >= 0)
        guiNhlRLClear(grlist);
  //cout << "\t\tFunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    grlist = guiNhlRLCreate(NhlGETRL);
    guiNhlRLGetFloat(grlist,NhlNvpXF,&vpw);
    guiNhlRLGetFloat(grlist,NhlNvpYF,&vpy);
    guiNhlRLGetFloat(grlist,NhlNvpWidthF,&vpw);
    guiNhlRLGetFloat(grlist,NhlNvpHeightF,&vph);
    guiNhlGetValues(cnid,grlist);

  //cout << "\t\tFunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    _xStart = vpx;
    _yStart = vpy - vph;

    _xEnd = vpx + vpw;
    _yEnd = vpy;
  //cout << "\tLeave Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
}   

void WRF_Contour::_map_setup()
{
    int left;
    int rite;

  //cout << "\tEnter Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    mapid = 0;

    guiNhlRLClear(srlist);
    guiNhlRLSetString(srlist,NhlNvpKeepAspect,"False");
    guiNhlRLSetString(srlist,NhlNvpOn,"True");
    guiNhlRLSetFloat(srlist,NhlNvpXF,vpx);
    guiNhlRLSetFloat(srlist,NhlNvpYF,vpy);
    guiNhlRLSetFloat(srlist,NhlNvpWidthF,vpw);
    guiNhlRLSetFloat(srlist,NhlNvpHeightF,vph);

    guiNhlRLSetString(srlist,NhlNmpFillOn,"False");
    guiNhlRLSetString(srlist,NhlNmpGeophysicalLineColor,"Black");
    guiNhlRLSetString(srlist,NhlNmpNationalLineColor,"Black");
    guiNhlRLSetString(srlist,NhlNmpUSStateLineColor,"Black");

  //guiNhlRLSetInteger(srlist,NhlNamZone,3);
  //guiNhlRLSetString(srlist,NhlNamJust,"BottomLeft");
  //guiNhlRLSetString(srlist,NhlNamSide,"Top");
  //guiNhlRLSetFloat(srlist,NhlNamParallelPosF,0.005);
  //guiNhlRLSetFloat(srlist,NhlNamOrthogonalPosF,0.03);
  //guiNhlRLSetString(srlist,NhlNamResizeNotify,"False");

    guiNhlRLSetString(srlist,NhlNmpOutlineOn, "True");
    guiNhlRLSetString(srlist,NhlNmpOutlineBoundarySets, "USStates");
    guiNhlRLSetString(srlist,NhlNmpUSStateLineColor, "Black");

    guiNhlRLSetString(srlist,NhlNmpGridAndLimbOn, "False");
    guiNhlRLSetFloat(srlist,NhlNmpGridSpacingF,  360.0);

    if(0 == geometry->get_map_proj())
    {
        guiNhlRLSetString(srlist,NhlNmpProjection,"CylindricalEquidistant");
        guiNhlRLSetFloat(srlist,NhlNmpGridSpacingF,30.0);
        guiNhlRLSetFloat(srlist,NhlNmpCenterLatF,0.0);

        if(-500.0 < geometry->get_stand_lon())
            guiNhlRLSetFloat(srlist,NhlNmpCenterLonF,geometry->get_stand_lon());
        else
            guiNhlRLSetFloat(srlist,NhlNmpCenterLonF,geometry->get_cen_lon());
    }
    else if(1 == geometry->get_map_proj())
    {
        guiNhlRLSetString(srlist,NhlNmpProjection,"LambertConformal");
        guiNhlRLSetFloat(srlist,NhlNmpLambertParallel1F,geometry->get_truelat1());
        guiNhlRLSetFloat(srlist,NhlNmpLambertParallel2F,geometry->get_truelat2());

      //guiNhlRLSetFloat(srlist,NhlNmpCenterLatF,geometry->get_cen_lat());
        if(-500.0 < geometry->get_stand_lon())
            guiNhlRLSetFloat(srlist,NhlNmpLambertMeridianF,geometry->get_stand_lon());
        else
            guiNhlRLSetFloat(srlist,NhlNmpLambertMeridianF,geometry->get_cen_lon());
    }
    else if(2 == geometry->get_map_proj())
    {
        guiNhlRLSetString(srlist,NhlNmpProjection,"Stereographic");

        guiNhlRLSetFloat(srlist,NhlNmpCenterLatF,geometry->get_cen_lat());
        if(-500.0 < geometry->get_stand_lon())
            guiNhlRLSetFloat(srlist,NhlNmpCenterLonF,geometry->get_stand_lon());
        else
            guiNhlRLSetFloat(srlist,NhlNmpCenterLonF,geometry->get_cen_lon());
    }
    else if(3 == geometry->get_map_proj())
    {
        guiNhlRLSetString(srlist,NhlNmpProjection,"Mercator");
        guiNhlRLSetFloat(srlist,NhlNmpCenterLatF,0.0);

        if(-500.0 < geometry->get_stand_lon())
            guiNhlRLSetFloat(srlist,NhlNmpCenterLonF,geometry->get_stand_lon());
        else
            guiNhlRLSetFloat(srlist,NhlNmpCenterLonF,geometry->get_cen_lon());
    }
    else if(6 == geometry->get_map_proj())
    {
        if(-500.0 < geometry->get_pole_lat())
            guiNhlRLSetFloat(srlist,NhlNmpCenterRotF,90.0-geometry->get_pole_lat());
        guiNhlRLSetString(srlist,NhlNmpProjection,"Lat/Lon");

        guiNhlRLSetFloat(srlist,NhlNmpCenterLatF,geometry->get_cen_lat());
        if(-500.0 < geometry->get_stand_lon())
            guiNhlRLSetFloat(srlist,NhlNmpCenterLonF,geometry->get_stand_lon());
        else
            guiNhlRLSetFloat(srlist,NhlNmpCenterLonF,geometry->get_cen_lon());
    }

  //cout << "Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
  //cout << "\tThis for WRF model. map_proj = " << geometry->get_map_proj() << endl;

    gridsizes = _nx * _ny;
    left = 0;
    rite = gridsizes - 1;
    guiNhlRLSetString(srlist,NhlNmpLimitMode,"Corners");
    guiNhlRLSetFloat(srlist,NhlNmpLeftCornerLonF,lon[left]);
    guiNhlRLSetFloat(srlist,NhlNmpLeftCornerLatF,lat[left]);
    guiNhlRLSetFloat(srlist,NhlNmpRightCornerLonF,lon[rite]);
    guiNhlRLSetFloat(srlist,NhlNmpRightCornerLatF,lat[rite]);

  //guiNhlRLSetString(srlist,NhlNmpLandFillColor,"Black");
  //guiNhlRLSetString(srlist,NhlNmpOceanFillColor,"Transparent");
  //guiNhlRLSetString(srlist,NhlNmpInlandWaterFillColor,"Black");
  //guiNhlRLSetString(srlist,NhlNmpGridLineColor,"LightGray");
  //guiNhlRLSetString(srlist,NhlNmpGeophysicalLineColor,"Black");
  //guiNhlRLSetFloat(srlist,NhlNmpGeophysicalLineThicknessF,1.);

    guiNhlRLSetString(srlist,NhlNpmTickMarkDisplayMode,"Always");

    guiNhlCreate(&mapid,"map",NhlmapPlotClass,wksid,srlist);
  //cout << "\tLeave Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
}

void WRF_Contour::_tick_setup()
{
  //Set resources of the inherited Title objects.
    guiNhlRLSetString(srlist,NhlNtiMainString,_varname.c_str());
  //guiNhlRLSetString(srlist,NhlNtiMainString,title);
    guiNhlRLSetString(srlist,NhlNtiMainFont,"helvetica");
    guiNhlRLSetFloat(srlist,NhlNtiMainFontHeightF,.03);
}

