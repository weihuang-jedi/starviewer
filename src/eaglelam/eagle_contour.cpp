#include "eagle_contour.h"

//Constructor
EAGLE_Contour::EAGLE_Contour(ColorTable *ct)
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

EAGLE_Contour::~EAGLE_Contour()
{
    delete texturekeeper;

    finalize();
}

void EAGLE_Contour::reset(void)
{
    _opacity = 100;

    texturekeeper->reset();
}

void EAGLE_Contour::initialize()
{
    float *cmap = colorTable->get_cmap();

    clen[0] = colorTable->get_clen();;
    clen[1] = 3;
}

void EAGLE_Contour::update_colormap()
{
    float *cmap = colorTable->get_cmap();

    clen[0] = colorTable->get_clen();;
    clen[1] = 3;

    texturekeeper->reinitialize(geometry->get_nx(), geometry->get_ny(), geometry->get_nz());
}

void EAGLE_Contour::finalize()
{
}

void EAGLE_Contour::_general_setup()
{
    float opacity = 0.01 * _opacity;
}

void EAGLE_Contour::_viewport_setup()
{
}   

void EAGLE_Contour::_get_viewportXinfo()
{
}

void EAGLE_Contour::_viewportXsetup()
{
}  

void EAGLE_Contour::_get_viewportYinfo()
{
}

void EAGLE_Contour::_viewportYsetup()
{
}

void EAGLE_Contour::initialize_context()
{
}

unsigned int EAGLE_Contour::get_textureID()
{
    unsigned char *pixels;
    unsigned int   width;
    unsigned int   height;
    unsigned int   newTextureID;

    return newTextureID;
}

void EAGLE_Contour::setup(string vn, double *var)
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

unsigned int EAGLE_Contour::get_xid(int n)
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

unsigned int EAGLE_Contour::get_yid(int n)
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

unsigned int EAGLE_Contour::get_zid(int n)
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
        guiNhlCreate(&cnid, "EAGLE_Contour", NhlcontourPlotClass, wksid, srlist);

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

void EAGLE_Contour::_get_viewport_info()
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

void EAGLE_Contour::_map_setup()
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
  //cout << "\tThis for EAGLE model. map_proj = " << geometry->get_map_proj() << endl;

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

void EAGLE_Contour::_tick_setup()
{
  //Set resources of the inherited Title objects.
    guiNhlRLSetString(srlist,NhlNtiMainString,_varname.c_str());
  //guiNhlRLSetString(srlist,NhlNtiMainString,title);
    guiNhlRLSetString(srlist,NhlNtiMainFont,"helvetica");
    guiNhlRLSetFloat(srlist,NhlNtiMainFontHeightF,.03);
}

