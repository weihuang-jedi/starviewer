//$Id: radartranslator.cpp 14610 2013-10-25 21:50:16Z starviewer $
#include <QtOpenGL>

#include "radartranslator.h"
#include "colorTable.h"
#include "util.h"

template<typename T>
string number2string(T n)
{
    string value;

    stringstream stream;
    stream << n;
    if(stream.fail())
    {
        cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
        cout << "\tFailed convert " << n << " to string." << endl;
        exit (-1);
    }

    value = stream.str();
    return value;
}

//
//  Constructor
//
RADARTranslator::RADARTranslator(ColorTable* ct, NVOptions* opt,
                                 string flnm, bool isList, string rid, QWidget* parent)
               : BaseTranslator(ct, opt, parent)
{
    _filename = flnm;
    _hasFileList = isList;
    radarid = rid;

    _saveJpg = false;
    _jpgNotSaved = true;
    _startSave = false;

    coastline2 = new CoastLine2(rid);
    coastline2->set_nvoptions(opt);
}

RADARTranslator::~RADARTranslator()
{
  //delete radarcontroller;
}

void RADARTranslator::setup()
{
    double range = 500.0 * 1000.0;
    double scale = 111177.0 / range;

    nvoptions->set_xsec(-1);
    nvoptions->set_ysec(-1);
    nvoptions->set_zsec(0);

    radarcontroller = new RADARController(colorTable, nvoptions,
                                          _filename, _hasFileList, radarid);

    _title = radarcontroller->get_title();
  //_timestr = radarcontroller->get_timestring();
 
    _varname = radarcontroller->get_varname();

  //updateSliders();

    dim = 0.5;

    radarcontroller->get_location(_base_lon, _base_lat);
    coastline2->set_base_lon(_base_lon);
    coastline2->set_base_lat(_base_lat);
    coastline2->set_scale(scale);

    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
    cout << "\tfunction: " << __PRETTY_FUNCTION__ << endl;
    cout << "\t_base_lon = " << _base_lon << ", _base_lat= " << _base_lat << endl;
}

//******************************************************************
//*************************  OpenGL Events  ************************
//******************************************************************
void RADARTranslator::paintGL()
{
  //Clear screen and Z-buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    glShadeModel(GL_SMOOTH);

    setBackgroundColor();

  //Reset transformations
    glLoadIdentity();

    setViewOptions();
    drawAxis();

    if(light->on())
    {
        glEnable(GL_LIGHTING);
        light->draw();
    }
    else
        glDisable(GL_LIGHTING);

    coastline2->drawLimitedArea(0.0);
    radarcontroller->draw();

  //Draw color bar
    drawColorBar();

    if(light->on())
        glDisable(GL_LIGHTING);

    setLabelColor();

    make_timeNpositionString();
    writeHeader();

    writeVarInfo();

    writeFrameInfo();

    if(locator->on())
        writeLocatorMsg();

  //Done
    glFlush();
}

void RADARTranslator::writeFrameInfo()
{
    if(nvoptions->get_cb(NV_ANIMATIONON))
    {
        _frameinfo = "Frame No. "+QString::number(_glbTime);
        emit frame(_frameinfo);
    }
}

void RADARTranslator::createVarInfo()
{
  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;

    _varinfo = "Dim";

  //_varinfo = "Var min=" + QString::number(radarcontroller->get_minval())
  //         + ", max=" + QString::number( radarcontroller->get_maxval());
}

void RADARTranslator::writeVarInfo()
{
    createVarInfo();
    emit info(_varinfo);
}

void RADARTranslator::make_timeNpositionString()
{
    int framenumb = 0;

  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;

  //framenumb = radarcontroller->get_curTime();
  //if((0 <= framenumb) && (NULL != _timestr))
  //{
      //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
      //cout << "\tframenumb = " << framenumb << endl;
      //cout << "\t_timestr[" << framenumb << "] = <" << _timestr[framenumb] << ">" << endl;
      //_timeinfo = "          " + _timestr[framenumb];
  //    _timeinfo = _timestr[framenumb];
  //}
  //else
    {
        string tstr = number2string<int>(_glbTime);
        _timeinfo = "Time: ";
        _timeinfo += tstr;
    }

    _position = "Position: ";
    string xstr = number2string<int>(nvoptions->get_xsec());
    string ystr = number2string<int>(nvoptions->get_ysec());
    string zstr = number2string<int>(nvoptions->get_zsec());

    if(-1 < nvoptions->get_xsec())
        _position += "Ray: " + xstr;
    if(-1 < nvoptions->get_ysec())
        _position += ", Bin: " + ystr;
    if(-1 < nvoptions->get_zsec())
        _position += ", Sweep: " + zstr;
}

void RADARTranslator::select1dVar(const QString& str)
{
    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
    cout << "1d var: <" << str.toStdString() << "> is selected." << endl;
}

void RADARTranslator::select2dVar(const QString& str)
{
    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
    cout << "1d var: <" << str.toStdString() << "> is selected." << endl;
}

void RADARTranslator::updateSliders()
{
    int intvl = 0;
    int stepsize = 1;

    sliderNspinX->set_max(radarcontroller->get_nrays());
    sliderNspinX->setValue(0);

    intvl = (radarcontroller->get_nrays()+9)/10;
    sliderNspinX->set_tickinterval(intvl);
    stepsize = (radarcontroller->get_nrays()+99)/100;
    sliderNspinX->set_step(stepsize);

    sliderNspinY->set_max(radarcontroller->get_nbins());
    sliderNspinY->setValue(0);

    intvl = (radarcontroller->get_nbins()+9)/10;
    sliderNspinY->set_tickinterval(intvl);
    stepsize = (radarcontroller->get_nbins()+99)/100;
    sliderNspinY->set_step(stepsize);

    sliderNspinZ->set_max(radarcontroller->get_nsweeps());
    sliderNspinZ->setValue(0);
  //sliderNspinT->set_max(geometry->get_nTime());
    sliderNspinT->set_max(1);
    sliderNspinT->setValue(0);
}

void RADARTranslator::select3dVar(const QString& str)
{
  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\tvar: <" << str.toStdString() << "> is selected." << endl;
  //cout << "\tnsweeps = " << radarcontroller->get_nsweeps() << endl;

    _varname = str.toStdString();

    radarcontroller->set2dvarname(_varname);
    _title = radarcontroller->get_title();
  //_timestr = radarcontroller->get_timestring();

    _minval = radarcontroller->get_minval();
    _maxval = radarcontroller->get_maxval();

    nvoptions->set_zsec(0);

    updateSliders();

    updateGL();
}

void RADARTranslator::update_frame()
{
    if(nvoptions->get_cb(NV_ANIMATIONON))
    {
        ++_glbTime;
 
        if(_glbTime >= _maxTime)
           _glbTime = 0;

        if(_saveJpg && (0 == _glbTime))
            _startSave = true;

        _set_current_time();
      //radarcontroller->set_fileNtime(_curFile, _curTime);
      //_timestr = radarcontroller->get_timestring();

        updateGL();
    }

    updateGL();
}

void RADARTranslator::selectColorMap(const QString& str)
{
  //cout << "\nfile: <" << __FILE__ << ">, function: <" << __PRETTY_FUNCTION__
  //     << ">, line: " << __LINE__ << endl;
  //cout << "ColorMap: <" << str.toStdString() << "> is selected." << endl;

    colorTable->set_colorMap(str.toStdString());

    radarcontroller->set_colorTable(colorTable);

    updateGL();
}

int RADARTranslator::get_ndv(int n)
{
    if(3 == n)
        return radarcontroller->get_varnumber();
    else
        return 0;
}

string* RADARTranslator::get_ndvNames(int n)
{
    unsigned int i;
    string* varnames = NULL;
    vector<string> vn;

    if(3 == n)
    {
        vn = radarcontroller->get_varnames();
        varnames = new string[vn.size()];

        for(i = 0; i < vn.size(); ++i)
            varnames[i] = vn[i];
    }

    return varnames;
}

void RADARTranslator::set_light(Light* l)
{
    light = l;
  //radarcontroller->set_light(l);
}

//Draw vertex in polar coordinates
void RADARTranslator::sphereVertex(int i, int j)
{
    float x = 2.0*Cos(j)*Sin(i);
    float y = 2.0*Sin(j);
    float z = 2.0*Cos(j)*Cos(i);

    glNormal3f(x,y,z);
  //glTexCoord2f(s,t);
    glVertex3f(x,y,z);
}

void RADARTranslator::set_locator(Locator* l)
{
    locator = l;

    locator->set_dim(dim);
    locator->set_fovy(fovy);
    locator->set_zfar(zFar);
    locator->set_znear(zNear);

  //radarcontroller->set_locator(l);
}

void RADARTranslator::writeLocatorMsg()
{
     _locatorinfo = "Location lon="+QString::number(locator->x())
               + ", lat=" + QString::number(locator->y());

    emit locator_msg(_locatorinfo);
}

