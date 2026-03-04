#include <QtOpenGL>

#include <mpas3dviewer.h>

MPAS3dViewer::MPAS3dViewer(ColorTable* ct)
{
    colorTable = ct;

    texture1d = new Texture1d();
    texture1d->set_colors(ct->get_clen(), ct->get_cmap());
    texture1d->set_name(ct->get_name());

    _processed = NULL;
    _var = NULL;

    _tlevel = 0;

    marchTetrahedron = new MarchTetrahedron();
}

MPAS3dViewer::~MPAS3dViewer()
{
    free(_processed);

    delete texture1d;
}

void MPAS3dViewer::set_geometry(Geometry *gm)
{
    geometry = gm;

    marchTetrahedron->set_colorTable(colorTable);
    marchTetrahedron->set_geometry(geometry);

    _initialize();
}

void MPAS3dViewer::setup(string vn, double *var)
{
    char varstr[128];
    strcpy(varstr, vn.c_str());

    _varname = vn;
    _var = var;

    marchTetrahedron->reset();

    reset();
}

void MPAS3dViewer::reset()
{
    int n;

    _tlevel = 0;

    for(n = 0; n < geometry->get_nt(); n++)
    {
        _processed[n] = false;
    }
}

void MPAS3dViewer::_initialize()
{
    _processed = (bool *) malloc(geometry->get_nt() * sizeof(bool));
    assert(_processed);

    reset();
}

void MPAS3dViewer::draw(int tl)
{
    double *var;

    _tlevel = tl;
    if(tl >= geometry->get_nt())
        _tlevel = 0;
    else if(tl < 0)
        _tlevel = geometry->get_nt() - 1;

  //cout << "\ndraw, file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\t_tlevel = " << _tlevel << endl;

    var = _evaluate();

    marchTetrahedron->set_var(var);
    marchTetrahedron->display(tl);
}

double *MPAS3dViewer::_evaluate()
{
    double *var;
    int m;
    int n;

    m = _tlevel * geometry->get_nz() * geometry->get_nCells();
    var =  &(_var[m]);

    _processed[_tlevel] = true;

    _valmax = var[0];
    _valmin = var[0];
    m = geometry->get_nz() * geometry->get_nCells();

    for(n = 0; n < m; ++n)
    {
        if(_valmax < var[n])
           _valmax = var[n];
        if(_valmin > var[n])
           _valmin = var[n];
    }

  //cout << "\nin _evaluate, file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\t_valmin: " << _valmin << ", _valmax: " << _valmax << endl;

    if(1.0e-10 > (_valmax - _valmin))
       _valmax += 1.0e-10;

  //colorTable->set_nl(5);

    return var;
}

