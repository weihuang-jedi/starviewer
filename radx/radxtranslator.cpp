//$Id: radxtranslator.cpp 5323 2015-02-20 17:38:57Z starviewer $
#include <QtOpenGL>

#include "radxtranslator.h"
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

//Constructor
RadxTranslator::RadxTranslator(ColorTable* ct, NVOptions* opt,
                                 string flnm, bool isList, QWidget* parent)
               : BaseTranslator(ct, opt, parent)
{
    _filename = flnm;
    _hasFileList = isList;

    _saveJpg = false;
    _jpgNotSaved = true;
    _startSave = false;

    coastline2 = new CoastLine2("KTLX");
    coastline2->set_nvoptions(opt);

    radxcontroller = NULL;
}

RadxTranslator::~RadxTranslator()
{
    delete coastline2;

    if(NULL != radxcontroller)
        delete radxcontroller;
}

void RadxTranslator::setup()
{
    double range = 500.0 * 1000.0;
    double scale = 111177.0 / range;

    nvoptions->set_xsec(-1);
    nvoptions->set_ysec(-1);
    nvoptions->set_zsec(0);

    radxcontroller = new RadxController(colorTable, nvoptions,
                                          _filename, _hasFileList);

    _title = radxcontroller->get_title();
  //_timestr = radxcontroller->get_timestring();
 
    _varname = radxcontroller->get_varname();

  //updateSliders();

    dim = 0.5;

    radxcontroller->get_location(_base_lon, _base_lat, _base_alt);
    coastline2->set_base_lon(_base_lon);
    coastline2->set_base_lat(_base_lat);
    coastline2->set_scale(scale);

  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\tfunction: " << __PRETTY_FUNCTION__ << endl;
  //cout << "\t_base_lon = " << _base_lon;
  //cout << "\t_base_lat = " << _base_lat;
  //cout << "\t_base_alt = " << _base_alt << endl;
}

//******************************************************************
//*************************  OpenGL Events  ************************
//******************************************************************
//Show the image
void RadxTranslator::show()
{
    glShadeModel(GL_SMOOTH);

    if(light->on())
    {
        glEnable(GL_LIGHTING);
        light->draw();
    }
    else
        glDisable(GL_LIGHTING);

    coastline2->drawLimitedArea(0.0);
    radxcontroller->draw();

    if(light->on())
        glDisable(GL_LIGHTING);

    setLabelColor();

    make_timeNpositionString();
    writeHeader();

    writeVarInfo();

    writeFrameInfo();

    if(locator->on())
        writeLocatorMsg();
}

void RadxTranslator::writeFrameInfo()
{
    if(nvoptions->get_cb(NV_ANIMATIONON))
    {
        _frameinfo = "Frame No. "+QString::number(_glbTime);
        emit frame(_frameinfo);
    }
}

void RadxTranslator::createVarInfo()
{
  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;

    _varinfo = "Dim";

  //_varinfo = "Var min=" + QString::number(radxcontroller->get_minval())
  //         + ", max=" + QString::number( radxcontroller->get_maxval());
}

void RadxTranslator::writeVarInfo()
{
    createVarInfo();
    emit info(_varinfo);
}

void RadxTranslator::make_timeNpositionString()
{
  //int framenumb = 0;

  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;

  //framenumb = radxcontroller->get_curTime();
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

void RadxTranslator::select1dVar(const QString& str)
{
    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
    cout << "1d var: <" << str.toStdString() << "> is selected." << endl;
}

void RadxTranslator::select2dVar(const QString& str)
{
    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
    cout << "1d var: <" << str.toStdString() << "> is selected." << endl;
}

void RadxTranslator::updateSliders()
{
    int intvl = 0;
    int stepsize = 1;

    sliderNspinX->set_max(radxcontroller->get_nrays());
    sliderNspinX->setValue(0);

    intvl = (radxcontroller->get_nrays()+9)/10;
    sliderNspinX->set_tickinterval(intvl);
    stepsize = (radxcontroller->get_nrays()+99)/100;
    sliderNspinX->set_step(stepsize);

    sliderNspinY->set_max(radxcontroller->get_nbins());
    sliderNspinY->setValue(0);

    intvl = (radxcontroller->get_nbins()+9)/10;
    sliderNspinY->set_tickinterval(intvl);
    stepsize = (radxcontroller->get_nbins()+99)/100;
    sliderNspinY->set_step(stepsize);

    sliderNspinZ->set_max(radxcontroller->get_nsweeps());
    sliderNspinZ->setValue(0);
  //sliderNspinT->set_max(geometry->get_nTime());
    sliderNspinT->set_max(1);
    sliderNspinT->setValue(0);
}

void RadxTranslator::select3dVar(const QString& str)
{
  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\tvar: <" << str.toStdString() << "> is selected." << endl;
  //cout << "\tnsweeps = " << radxcontroller->get_nsweeps() << endl;

    _varname = str.toStdString();

    radxcontroller->set2dvarname(_varname);
    _title = radxcontroller->get_title();
  //_timestr = radxcontroller->get_timestring();

    _minval = radxcontroller->get_minval();
    _maxval = radxcontroller->get_maxval();

    nvoptions->set_zsec(0);

    updateSliders();

    updateGL();
}

void RadxTranslator::update_frame()
{
    if(nvoptions->get_cb(NV_ANIMATIONON))
    {
        ++_glbTime;
 
        if(_glbTime >= _maxTime)
           _glbTime = 0;

        if(_saveJpg && (0 == _glbTime))
            _startSave = true;

        _set_current_time();
      //radxcontroller->set_fileNtime(_curFile, _curTime);
      //_timestr = radxcontroller->get_timestring();

        updateGL();
    }

    updateGL();
}

void RadxTranslator::selectColorMap(const QString& str)
{
  //cout << "\nfile: <" << __FILE__ << ">, function: <" << __PRETTY_FUNCTION__
  //     << ">, line: " << __LINE__ << endl;
  //cout << "ColorMap: <" << str.toStdString() << "> is selected." << endl;

    colorTable->set_colorMap(str.toStdString());

    radxcontroller->set_colorTable(colorTable);

    updateGL();
}

int RadxTranslator::get_ndv(int n)
{
    if(3 == n)
        return radxcontroller->get_varnumber();
    else
        return 0;
}

string* RadxTranslator::get_ndvNames(int n)
{
    unsigned int i;
    string* varnames = NULL;
    vector<string> vn;

    if(3 == n)
    {
        vn = radxcontroller->get_varnames();
        varnames = new string[vn.size()];

        for(i = 0; i < vn.size(); ++i)
            varnames[i] = vn[i];
    }

    return varnames;
}

void RadxTranslator::set_light(Light* l)
{
    light = l;
  //radxcontroller->set_light(l);
}

//Draw vertex in polar coordinates
void RadxTranslator::sphereVertex(int i, int j)
{
    float x = 2.0*Cos(j)*Sin(i);
    float y = 2.0*Sin(j);
    float z = 2.0*Cos(j)*Cos(i);

    glNormal3f(x,y,z);
  //glTexCoord2f(s,t);
    glVertex3f(x,y,z);
}

void RadxTranslator::set_locator(Locator* l)
{
    locator = l;

    locator->set_dim(dim);
    locator->set_fovy(fovy);
    locator->set_zfar(zFar);
    locator->set_znear(zNear);

  //radxcontroller->set_locator(l);
}

void RadxTranslator::writeLocatorMsg()
{
     _locatorinfo = "Location lon="+QString::number(locator->x())
               + ", lat=" + QString::number(locator->y());

    emit locator_msg(_locatorinfo);
}

