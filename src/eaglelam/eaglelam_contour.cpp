#include "eaglelam_contour.h"

//Constructor
EAGLELAM_Contour::EAGLELAM_Contour(ColorTable *ct)
{
    colorTable = ct;

    iwidth  = 1000;
    iheight = 1000;

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

EAGLELAM_Contour::~EAGLELAM_Contour()
{
    delete texturekeeper;

    finalize();
}

void EAGLELAM_Contour::reset(void)
{
    _opacity = 100;

    texturekeeper->reset();
}

void EAGLELAM_Contour::initialize()
{
    float *cmap = colorTable->get_cmap();

    clen[0] = colorTable->get_clen();;
    clen[1] = 3;
}

void EAGLELAM_Contour::update_colormap()
{
    float *cmap = colorTable->get_cmap();

    clen[0] = colorTable->get_clen();;
    clen[1] = 3;

    texturekeeper->reinitialize(geometry->get_nx(), geometry->get_ny(), geometry->get_nz());
}

void EAGLELAM_Contour::finalize()
{
}

void EAGLELAM_Contour::_general_setup()
{
    float opacity = 0.01 * _opacity;
}

void EAGLELAM_Contour::_viewport_setup()
{
}   

void EAGLELAM_Contour::_get_viewportXinfo()
{
}

void EAGLELAM_Contour::_viewportXsetup()
{
}  

void EAGLELAM_Contour::_get_viewportYinfo()
{
}

void EAGLELAM_Contour::_viewportYsetup()
{
}

void EAGLELAM_Contour::initialize_context()
{
}

unsigned int EAGLELAM_Contour::get_textureID()
{
    unsigned char *pixels;
    unsigned int   width;
    unsigned int   height;
    unsigned int   newTextureID;

    return newTextureID;
}

void EAGLELAM_Contour::setup(string vn, float *var)
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
    _valmin = evaluator.get_min();
    _valmax = evaluator.get_max();

    // guiGetEndpointsAndStepSize(_valmin, _valmax, cnMaxLev, 1,
    //                            &cnMinLevelValF, &cnMaxLevelValF,
    //                            &cnLevelSpacingF);

    lon = geometry->get_lon();
    lat = geometry->get_lat();

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

unsigned int EAGLELAM_Contour::get_zid(int n)
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

        float* thevar = &_value[n * _ny * _nx];
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
        dims[0] = _ny;
        dims[1] = _nx;

        _general_setup();

        _tick_setup();

        _map_setup();

        _get_viewport_info();

        delete [] pltvar;
        delete [] flon;
        delete [] flat;
    }

    zid = get_textureID();
    texturekeeper->set_zid(n, zid);

  //cout << "\tGenerate zid: " << zid << " for <" << _varname << "> at level " << n << endl;
  //cout << "\tLeave Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;

    return zid;
}

void EAGLELAM_Contour::_get_viewport_info()
{
  //cout << "\t\tFunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    _xStart = vpx;
    _yStart = vpy - vph;

    _xEnd = vpx + vpw;
    _yEnd = vpy;
  //cout << "\tLeave Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
}   

void EAGLELAM_Contour::_map_setup()
{
  //cout << "Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
  //cout << "\tThis for EAGLELAM model. map_proj = " << geometry->get_map_proj() << endl;

    gridsizes = _nx * _ny;
  //cout << "\tLeave Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
}

void EAGLELAM_Contour::_tick_setup()
{
}

