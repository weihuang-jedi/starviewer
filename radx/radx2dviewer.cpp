//$Id: radx2dviewer.cpp 5321 2015-02-20 03:20:52Z starviewer $

#include <QtOpenGL>

#include "radx2dviewer.h"

#define MAKEMARK        { cerr << endl << "file: " << __FILE__ << "\tline: " << __LINE__ << endl << endl; }

Radx2dViewer::Radx2dViewer(ColorTable *ct, NVOptions* opt)
{
    colorTable = ct;
    nvoptions = opt;

    locator = NULL;

    lister = new Lister();
    lister->setup(1, 1, 121);

    _zlist = NULL;

    _minval = 1.0e20;
    _maxval = -1.0e20;

    volume = NULL;

    _colorlen = colorTable->get_clen() - 3;
    _colormap = colorTable->get_cmap();
}

Radx2dViewer::~Radx2dViewer()
{
    if(NULL != locator)
        locator->turnOff();
    delete lister;
}

void Radx2dViewer::set_radx(RadxReader *r)
{
    radx = r;

    _initialize();
}

void Radx2dViewer::setup(string vn)
{
  //MAKEMARK;
    reset();

    _varname  = vn;

    volume = radx->getRadxVol(vn);

    nsweeps = radx->get_nsweeps();
}

void Radx2dViewer::reset()
{
    lister->reinitialize(1, 1, 121);
}

void Radx2dViewer::_initialize()
{
    lister->reinitialize(1, 1, 121);
}

void Radx2dViewer::get_location(double &blon, double &blat, double &balt)
{
  //MAKEMARK
    _base_lon = radx->get_baseLon();
    _base_lat = radx->get_baseLat();
    _base_alt = radx->get_baseAlt();
    blon = _base_lon;
    blat = _base_lat;
    balt = _base_lat;
}

void Radx2dViewer::draw()
{
    int lvl = nvoptions->get_zsec();
    GLuint zcl = lister->get_zid(lvl);

  //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\t_varname: <" << _varname << ">, zcl = " << zcl << endl;
  //cout << "\tlvl = " << lvl << endl;

#if 0
    _draw_grid();
#else
    if(zcl)
        glCallList(zcl);
    else
        _display();
#endif
}

void Radx2dViewer::_draw_cross(double dist)
{
    double x, y, z;
    double lon = (double) locator->x();
    double lat = (double) locator->y();
    
    glLineWidth(2.0);
    glColor4d(1.0, 0.0, 1.0, 0.95);

    x = dist * cos(lon * ARC);
    y = dist * sin(lat * ARC);
    z = 0.0;

    glBegin(GL_LINE_STRIP);
        glVertex3f(x-0.02, y, z);
        glVertex3f(x+0.02, y, z);

        glVertex3f(x, y-0.02, z);
        glVertex3f(x, y+0.02, z);
    glEnd();
}

void Radx2dViewer::_draw_rect(double x[4], double y[4], double z[4])
{
    int n;

    glLineWidth(1.0);

    glBegin(GL_LINE_STRIP);
        for(n = 0; n < 4; ++n)
            glVertex3f(x[n], y[n], z[n]);

        glVertex3f(x[0], y[0], z[n]);
    glEnd();
}

void Radx2dViewer::_draw_grid()
{
  //MAKEMARK
#if 0
            _draw_rect(x, y, z);
#endif
}

void Radx2dViewer::_set_color(float val)
{
    float color[4];
    int idx = 0;

    if((val > _lowbound) && ( val < _upbound))
    {
        idx = (int) (((val - _minval) * _colorlen) / (_maxval - _minval));

        color[0] = _colormap[3*(idx+3)];
        color[1] = _colormap[3*(idx+3) + 1];
        color[2] = _colormap[3*(idx+3) + 2];
      //color[3] = 0.25 + (val - _minval) / (_maxval - _minval);
        color[3] = 0.05 + (val - _minval) / (_maxval - _minval);
        if(color[3] > 1.0)
           color[3] = 1.0;
    }
    else
    {
        color[0] = 0.90;
        color[1] = 0.90;
        color[2] = 0.90;
        color[3] = 0.25;
    }

    glColor4fv(color);
}

void Radx2dViewer::_set_color3d(float val)
{
    float color[4];
    int idx = 0;

    if((val > _lowbound) && ( val < _upbound))
    {
        idx = (int) (((val - _minval) * _colorlen) / (_maxval - _minval));

        color[0] = _colormap[3*(idx+3)];
        color[1] = _colormap[3*(idx+3) + 1];
        color[2] = _colormap[3*(idx+3) + 2];
      //color[3] = 0.25 + (val - _minval) / (_maxval - _minval);
        color[3] = 0.005 + 0.75 * (val - _minval) / (_maxval - _minval);
    }
    else
    {
        color[0] = 0.95;
        color[1] = 0.95;
        color[2] = 0.95;
        color[3] = 0.125;
    }

    glColor4fv(color);
}

void Radx2dViewer::_fill_quad(double x[4], double y[4], double z[4], float value)
{
    int n;

    _set_color(value);

    glBegin(GL_QUADS);
        for(n = 0; n < 4; ++n)
        {
            glVertex3f(x[n], y[n], z[n]);
        }
    glEnd();
}

void Radx2dViewer::_display()
{
    int lvl = nvoptions->get_zsec();
    size_t ngates, prev_ngates = 0;
    size_t igate, iray, ifield;
    GLuint zcl = 0;

    double x[4], y[4], z[4];

    double azimuth, elev;
    double half_beamwidth;
    double gate_size;
    double range;
    double scale;
    double offset;
    float* value;

    double bstart, bend;

    RadxSweep* sweep;
    RadxRay* ray;
    vector<RadxField *> fields;
    RadxField* field;

    int fieldIndex = -1;
    size_t sweepStartRayIndex = -1;
    size_t sweepEndRayIndex = -1;

  //int sweepNumber = -1;
  //size_t sweepNRays = -1;
  //double sweepFixedAngle = -1;

    if(NULL == volume)
        return;

    sweeps = volume->getSweeps();
    rays = volume->getRays();

    sweep = sweeps[lvl];

  //sweep->print(cout);

  //range = 500.0 * 1000.0;
    range = 500.0;

    sweepStartRayIndex = sweep->getStartRayIndex();
    sweepEndRayIndex = sweep->getEndRayIndex();

  //sweepNumber = sweep->getSweepNumber();
  //sweepNRays = sweep->getNRays();
  //sweepFixedAngle = sweep->getFixedAngleDeg();

  //cout << "\tlvl = " << lvl << "\tsweepNumber = " << sweepNumber << endl;
  //cout << "\tgateSpace = " << radx->get_gateSpace() << endl;
  //cout << "\t_varname: " << _varname << endl;

    fields = rays[0]->getFields();
    ngates = rays[0]->getNGates();
    prev_ngates = ngates;
    for(ifield = 0; ifield < fields.size(); ++ifield)
    {
        field = fields[ifield];
        field->convertToFl32();
        if(0 == _varname.compare(field->getName()))
        {
            field->convertToFl32();
            _longName = field->getLongName();
            _standardName = field->getStandardName();
            _units = field->getUnits();

            scale = field->getScale();
            offset = field->getOffset();
            _maxval = field->getMaxValue();
            _minval = field->getMinValue();
            _missing = field->getMissingFl32();

            fieldIndex = ifield;
            break;
        }
    }

    if(0 > fieldIndex)
        return;

  //value = new float[prev_ngates];

    _set_minmax();

  //MAKEMARK

  //cout << "\t_minval = " << _minval << ", _maxval = " << _maxval << ", _missing = " << _missing << endl;
  //cout << "\tscale = " << scale << ", offset = " << offset << endl;

  //if(lvl >= nsweeps)
  //{
  //    _display3d();
  //    return;
  //}

    zcl = glGenLists(1);

  //cout << "\tzcl = " << zcl << endl;

    glNewList(zcl, GL_COMPILE_AND_EXECUTE);

  /*OpenGL should normalize normal vectors*/
  //glEnable(GL_NORMALIZE);

    glEnable(GL_BLEND);
  //glBlendFunc(GL_SRC_ALPHA,GL_ONE);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

  //glShadeModel(GL_SMOOTH);

  //glDisable(GL_CULL_FACE);
  //glEnable(GL_CULL_FACE);

    glPushMatrix();

    ray = rays[0];
  //cout << "\nRay: 0" << endl;
  //cout << "\tgetFixedAngleDeg() = " << ray->getFixedAngleDeg() << endl;
  //cout << "\tgetTrueScanRateDegPerSec() = " << ray->getTrueScanRateDegPerSec() << endl;
  //cout << "\tgetTargetScanRateDegPerSec() = " << ray->getTargetScanRateDegPerSec() <<endl;
  //cout << "\tgetAngleResDeg() = " << ray->getAngleResDeg() <<endl;

    for(iray = sweepStartRayIndex; iray <= sweepEndRayIndex; ++iray)
    {
        ray = rays[iray];

      //cout << "\nRay: " << iray << endl;
      //double tm ray->getTimeDouble();
      //double fa ray->getFixedAngleDeg();

        azimuth = 90.0 - ray->getAzimuthDeg();
        if(0.0 > azimuth)
            azimuth += 360.0;
        elev = ray->getElevationDeg();

      //nbins = ray->getNSamples();
      //gate_size = ray->getUnambigRangeKm();

        ngates = ray->getNGates();

        if(ngates > prev_ngates)
        {
            prev_ngates = ngates;
          //delete [] value;
          //value = new float[ngates];
        }

      //cout << "\t\tray " << iray << ": elev = " << elev << ", azimuth = " << azimuth << endl;
      //cout << "\t\t\tngates = " << ray->getNGates() << endl;
      //cout << "\t\t\tgateRange = " << ray->getGateRangeKm(0) << endl;

      //ray->printWithFieldData(cout);

        fields = ray->getFields();
        field = fields[fieldIndex];
        field->convertToFl32();
      //field->setDataFl32(ngates, value, true);
        value = (float*) field->getDataFl32();

        half_beamwidth = 0.525*ray->getFixedAngleDeg();
      //half_beamwidth = ray->getAngleResDeg();
        gate_size = field->getGateSpacingKm();
        bstart = field->getStartRangeKm();
        for(igate = 0; igate < ngates; ++igate)
        {
          //bend = field->getGateRangeKm(igate);
            bend = bstart + gate_size;

          //cout << "\t\tgate: " << igate << ", bstart = " << bstart << ", bend = " << bend << ", value = " << value[igate] << endl;

            x[0] = bstart * cos((azimuth + half_beamwidth)*ARC) / range;
            y[0] = bstart * sin((azimuth + half_beamwidth)*ARC) / range;
            z[0] = 10.0 * bstart * tan(elev*ARC) / range;

            x[1] = bstart * cos((azimuth - half_beamwidth)*ARC) / range;
            y[1] = bstart * sin((azimuth - half_beamwidth)*ARC) / range;
            z[1] = 10.0 * bstart * tan(elev*ARC) / range;

            x[2] = bend * cos((azimuth - half_beamwidth)*ARC) / range;
            y[2] = bend * sin((azimuth - half_beamwidth)*ARC) / range;
            z[2] = 10.0 * bend * tan(elev*ARC) / range;

            x[3] = bend * cos((azimuth + half_beamwidth)*ARC) / range;
            y[3] = bend * sin((azimuth + half_beamwidth)*ARC) / range;
            z[3] = 10.0 * bend * tan(elev*ARC) / range;

            _fill_quad(x, y, z, value[igate]);

            bstart = bend;
        }
    }

  //delete [] value;

  //glDisable(GL_CULL_FACE);

  //glNormal3f(0.0, 0.0, -1.0);

    glEndList();

    lister->set_zid(lvl, zcl);

    glPopMatrix();
}

void Radx2dViewer::_fill_cube(double x[8], double y[8], double z[8], float value)
{
    if((value < _lowbound) || ( value > _upbound))
        return;

    _set_color3d(value);

    glBegin(GL_QUADS);
#if 1
        glVertex3f(x[1], y[1], z[1]);
        glVertex3f(x[2], y[2], z[2]);
        glVertex3f(x[7], y[7], z[7]);
        glVertex3f(x[4], y[4], z[4]);

      //glVertex3f(x[0], y[0], z[0]);
      //glVertex3f(x[2], y[2], z[2]);
      //glVertex3f(x[6], y[6], z[6]);
      //glVertex3f(x[4], y[4], z[4]);
#else
        glVertex3f(x[0], y[0], z[0]);
        glVertex3f(x[1], y[1], z[1]);
        glVertex3f(x[6], y[6], z[6]);
        glVertex3f(x[7], y[7], z[7]);

        glVertex3f(x[2], y[2], z[2]);
        glVertex3f(x[3], y[3], z[3]);
        glVertex3f(x[4], y[4], z[4]);
        glVertex3f(x[5], y[5], z[5]);

        glVertex3f(x[1], y[1], z[1]);
        glVertex3f(x[2], y[2], z[2]);
        glVertex3f(x[7], y[7], z[7]);
        glVertex3f(x[4], y[4], z[4]);

        glVertex3f(x[0], y[0], z[0]);
        glVertex3f(x[3], y[3], z[3]);
        glVertex3f(x[6], y[6], z[6]);
        glVertex3f(x[5], y[5], z[5]);
#endif
    glEnd();
}

void Radx2dViewer::_display3d()
{
  //MAKEMARK
#if 0
    int lvl = nvoptions->get_zsec();
    int nbins, nrays;
    int ibin,  iray;
    GLuint zcl = 0;

    double x[8], y[8], z[8];
  //doubel lon[4], lat[4];

    double azimuth, elev;
    double half_beamwidth;
    double gate_size;
    double range;
    double value;

  //double meters2lat = 1.0/111177.0;
  //double meters2lon = meters2lat / cos(radx_lat * ARC);

    double bstart, bend;

    if(NULL == volume)
        return;

  //zcl = glGenLists(1);

  //cout << "\tzcl = " << zcl << endl;

  //glNewList(zcl, GL_COMPILE_AND_EXECUTE);

  /*OpenGL should normalize normal vectors*/
  //glEnable(GL_NORMALIZE);

    glEnable(GL_BLEND);
  //glBlendFunc(GL_SRC_ALPHA,GL_ONE);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

  //glShadeModel(GL_SMOOTH);

  //glDisable(GL_CULL_FACE);
  //glEnable(GL_CULL_FACE);

    range = 500.0 * 1000.0;

    glPushMatrix();

    for(isweep = 0; isweep < nsweeps; ++isweep)
    {
        sweep = volume->sweep[isweep];

        half_beamwidth = 1.05 * 0.5 * sweep->h.beam_width;
        nrays = sweep->h.nrays;

        for(iray = 0; iray < nrays; ++iray)
        {
            ray = sweep->ray[iray];
            nbins = ray->h.nbins;
            elev = ray->h.elev;
            gate_size = ray->h.gate_size;

            azimuth = 90.0 - ray->h.azimuth;
            if(0.0 > azimuth)
                azimuth += 360.0;

            bstart = ray->h.range_bin1;
            for(ibin = 0; ibin < nbins; ++ibin)
            {
                value = ray->h.f(ray->range[ibin]);

                bend = bstart + gate_size;

                if((value > _lowbound) && ( value < _upbound))
                {
                    x[0] = bstart * cos((azimuth + half_beamwidth)*ARC) / range;
                    y[0] = bstart * sin((azimuth + half_beamwidth)*ARC) / range;
                    z[0] = 10.0 * bstart * tan((elev - half_beamwidth)*ARC) / range;

                    x[1] = bstart * cos((azimuth - half_beamwidth)*ARC) / range;
                    y[1] = bstart * sin((azimuth - half_beamwidth)*ARC) / range;
                    z[1] = 10.0 * bstart * tan((elev - half_beamwidth)*ARC) / range;

                    x[2] = bend * cos((azimuth - half_beamwidth)*ARC) / range;
                    y[2] = bend * sin((azimuth - half_beamwidth)*ARC) / range;
                    z[2] = 10.0 * bstart * tan((elev - half_beamwidth)*ARC) / range;

                    x[3] = bend * cos((azimuth + half_beamwidth)*ARC) / range;
                    y[3] = bend * sin((azimuth + half_beamwidth)*ARC) / range;
                    z[3] = 10.0 * bstart * tan((elev - half_beamwidth)*ARC) / range;

                    x[4] = bstart * cos((azimuth + half_beamwidth)*ARC) / range;
                    y[4] = bstart * sin((azimuth + half_beamwidth)*ARC) / range;
                    z[4] = 10.0 * bstart * tan((elev + half_beamwidth)*ARC) / range;

                    x[5] = bstart * cos((azimuth - half_beamwidth)*ARC) / range;
                    y[5] = bstart * sin((azimuth - half_beamwidth)*ARC) / range;
                    z[5] = 10.0 * bstart * tan((elev + half_beamwidth)*ARC) / range;

                    x[6] = bend * cos((azimuth - half_beamwidth)*ARC) / range;
                    y[6] = bend * sin((azimuth - half_beamwidth)*ARC) / range;
                    z[6] = 10.0 * bstart * tan((elev + half_beamwidth)*ARC) / range;

                    x[7] = bend * cos((azimuth + half_beamwidth)*ARC) / range;
                    y[7] = bend * sin((azimuth + half_beamwidth)*ARC) / range;
                    z[7] = 10.0 * bstart * tan((elev + half_beamwidth)*ARC) / range;

                    _fill_cube(x, y, z, value);
                }

                bstart = bend;
            }
        }
    }

  //glDisable(GL_CULL_FACE);

  //glNormal3f(0.0, 0.0, -1.0);

    glEndList();

    lister->set_zid(lvl, zcl);

    glPopMatrix();
#endif
}

void Radx2dViewer::_set_minmax()
{
    int lvl = nvoptions->get_zsec();
    size_t ngates, prev_ngates = 0;
    size_t igate, iray, ifield;

    double scale;
    double offset;
    float* value;

    RadxSweep* sweep;
    RadxRay* ray;
    vector<RadxField *> fields;
    RadxField* field;

    int fieldIndex = -1;
  //int sweepNumber = -1;
    size_t sweepStartRayIndex = -1;
    size_t sweepEndRayIndex = -1;

  //MAKEMARK

    if(NULL == volume)
        return;

    sweeps = volume->getSweeps();
    rays = volume->getRays();

    sweep = sweeps[lvl];

  //sweepNumber = sweep->getSweepNumber();
    sweepStartRayIndex = sweep->getStartRayIndex();
    sweepEndRayIndex = sweep->getEndRayIndex();

    fields = rays[0]->getFields();
    ngates = rays[0]->getNGates();
    prev_ngates = ngates;
    for(ifield = 0; ifield < fields.size(); ++ifield)
    {
        field = fields[ifield];
      //field->printWithData(cout);
        field->convertToFl32();
        if(0 == _varname.compare(field->getName()))
        {
            field->convertToFl32();
            _longName = field->getLongName();
            _standardName = field->getStandardName();
            _units = field->getUnits();

            scale = field->getScale();
            offset = field->getOffset();
            _maxval = field->getMaxValue();
            _minval = field->getMinValue();
            _missing = field->getMissingFl32();

            fieldIndex = ifield;
            break;
        }
    }

    if(0 > fieldIndex)
        return;

    _minval =  1.0e20;
    _maxval = -1.0e20;
    _lowbound = 0.5 * _missing;
    _upbound = 1.0e10;
  //value = new float[prev_ngates];

  //cout << "\t_minval = " << _minval << ", _maxval = " << _maxval << ", _missing = " << _missing << endl;
  //cout << "\tscale = " << scale << ", offset = " << offset << endl;

    for(iray = sweepStartRayIndex; iray <= sweepEndRayIndex; ++iray)
    {
        ray = rays[iray];

        ngates = ray->getNGates();

        if(ngates > prev_ngates)
        {
            prev_ngates = ngates;
          //delete [] value;
          //value = new float[ngates];
        }

        fields = ray->getFields();
        field = fields[fieldIndex];
        field->convertToFl32();
      //field->setDataFl32(ngates, value, true);
        value = (float*) field->getDataFl32();

        for(igate = 0; igate < ngates; ++igate)
        {
            if(value[igate] < _lowbound)
               continue;
            if(value[igate] > _upbound)
               continue;

            if(value[igate] < _minval)
               _minval = value[igate];

            if(value[igate] > _maxval)
               _maxval = value[igate];
        }
    }

  //delete [] value;

  //MAKEMARK

    if(_minval > 0.0)
        _lowbound = 0.95 * _minval;
    else
        _lowbound = 1.05 * _minval;

    if(_maxval > 0.0)
        _upbound = 1.05 * _maxval;
    else
        _upbound = 0.95 * _maxval;

  //MAKEMARK

  //cout << "\t_minval = " << _minval << ", _maxval = " << _maxval << ", _missing = " << _missing << endl;
  //cout << "\t_lowbound = " << _lowbound << ", _upbound = " << _upbound << endl;
  //cout << "\tscale = " << scale << ", offset = " << offset << endl;
}

