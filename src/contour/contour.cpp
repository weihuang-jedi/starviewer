#include "contour.h"

#ifdef debug
#undef debug
#endif

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
}

void Contour::update_colormap()
{
    float *cmap = colorTable->get_cmap();

    clen[0] = colorTable->get_clen();;
    clen[1] = 3;
}

void Contour::finalize()
{
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

   /*
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
   */
}

void Contour::_viewport_setup()
{
   /*
    guiNhlRLSetFloat(srlist,NhlNvpXF,vpx);
    guiNhlRLSetFloat(srlist,NhlNvpYF,vpy);
    guiNhlRLSetFloat(srlist,NhlNvpWidthF,vpw);
    guiNhlRLSetFloat(srlist,NhlNvpHeightF,vph);
    guiNhlSetValues(cnid,srlist);
   */
}   

void Contour::_get_viewportXinfo()
{
   /*
    grlist = guiNhlRLCreate(NhlGETRL);
    guiNhlRLClear(grlist);
    guiNhlRLGetFloat(grlist,NhlNvpXF,&vpxX);
    guiNhlRLGetFloat(grlist,NhlNvpYF,&vpyX);
    guiNhlRLGetFloat(grlist,NhlNvpWidthF,&vpwX);
    guiNhlRLGetFloat(grlist,NhlNvpHeightF,&vphX);
    guiNhlGetValues(cnid,grlist);
   */

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
   /*
    guiNhlRLSetFloat(srlist,NhlNvpXF,vpxX);
    guiNhlRLSetFloat(srlist,NhlNvpYF,vpyX);
    guiNhlRLSetFloat(srlist,NhlNvpWidthF,vpwX);
    guiNhlRLSetFloat(srlist,NhlNvpHeightF,vphX);
    guiNhlSetValues(cnid,srlist);
   */
}  

void Contour::_get_viewportYinfo()
{
   /*
    grlist = guiNhlRLCreate(NhlGETRL);
    guiNhlRLClear(grlist);
    guiNhlRLGetFloat(grlist,NhlNvpXF,&vpxY);
    guiNhlRLGetFloat(grlist,NhlNvpYF,&vpyY);
    guiNhlRLGetFloat(grlist,NhlNvpWidthF,&vpwY);
    guiNhlRLGetFloat(grlist,NhlNvpHeightF,&vphY);
    guiNhlGetValues(cnid,grlist);
   */

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
   /*
    guiNhlRLSetFloat(srlist,NhlNvpXF,vpxY);
    guiNhlRLSetFloat(srlist,NhlNvpYF,vpyY);
    guiNhlRLSetFloat(srlist,NhlNvpWidthF,vpwY);
    guiNhlRLSetFloat(srlist,NhlNvpHeightF,vphY);
    guiNhlSetValues(cnid,srlist);
   */
}

void Contour::initialize_context()
{
}

unsigned int Contour::get_textureID(unsigned int textureID)
{
    return 0;
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

  /*
    guiGetEndpointsAndStepSize((float)_minval, (float)_maxval, cnMaxLev, 1,
                               &cnMinLevelValF, &cnMaxLevelValF,
                               &cnLevelSpacingF);
   */
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

    return _x_id[n];
}

unsigned int Contour::get_yid(int n)
{
    int i, k, m;
    float* flon;
    float* flev;

    if(0 < _y_id[n])
        return _y_id[n];

    return _y_id[n];
}

unsigned int Contour::get_zid(int n)
{
    int i, j, m;
    float* flon;
    float* flat;

    if(0 < _z_id[n])
        return _z_id[n];

    return _z_id[n];
}

void Contour::_tick_setup()
{
   /*
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
    */
}

void Contour::_map_setup()
{
    mapid = -1;
    /*
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
    */
}

void Contour::_get_viewport_info()
{
   /*
    guiNhlRLClear(grlist);
    grlist = guiNhlRLCreate(NhlGETRL);
    guiNhlRLGetFloat(grlist,NhlNvpXF,&vpw);
    guiNhlRLGetFloat(grlist,NhlNvpYF,&vpy);
    guiNhlRLGetFloat(grlist,NhlNvpWidthF,&vpw);
    guiNhlRLGetFloat(grlist,NhlNvpHeightF,&vph);
    guiNhlGetValues(mapid,grlist);
    */
    _xStart = vpx;
    _yStart = vpy - vph;

    _xEnd = vpx + vpw;
    _yEnd = vpy;
}   

