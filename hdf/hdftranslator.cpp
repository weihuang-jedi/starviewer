//$Id: hdftranslator.cpp 5314 2015-02-06 20:58:52Z starviewer $

#include <QtOpenGL>

#include "hdftranslator.h"

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
HDFTranslator::HDFTranslator(ColorTable* ct, NVOptions* opt,
                             string flnm, bool isList, QWidget* parent)
             : BaseTranslator(ct, opt, parent)
{
    _filename = flnm;
    _hasFileList = isList;

    _timestr = NULL;
    controller = NULL;
}

HDFTranslator::~HDFTranslator()
{
    if(NULL != controller)
        delete controller;
}

void HDFTranslator::setup()
{
    controller = new HDFController(colorTable, nvoptions,
                                   _filename.c_str(), _hasFileList);

    geometry = controller->get_geometry();

    _varname = controller->get_varname();

    dim = 0.375;

   nvoptions->set_xsec(-1);
   nvoptions->set_ysec(-1);
   nvoptions->set_zsec(0);
}

/*******************************************************************/
/*************************  OpenGL Events  *************************/
/*******************************************************************/
//
//Draw the window
//
void HDFTranslator::show()
{
    controller->draw();

    _varname = controller->get_varname();
    _title = controller->get_varname();

  //cout << "\n" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\t_varname; " << _varname << endl;

    setLabelColor();

    make_timeNpositionString();
    writeHeader();

    writeVarInfo();

    writeFrameInfo();

    if(locator->on())
        writeLocatorMsg();
}

void HDFTranslator::writeFrameInfo()
{
    if(nvoptions->get_cb(NV_ANIMATIONON))
    {
        _frameinfo = "Frame No. "+QString::number(_glbTime);
        emit frame(_frameinfo);
    }
}

void HDFTranslator::createVarInfo()
{
    _varinfo = "Dim nlon="+QString::number(geometry->get_nlon())
             + ", nlat=" + QString::number(geometry->get_nlat())
             + ", nlev=" + QString::number(geometry->get_nlev())
             + "\nVar min=" + QString::number(controller->get_minval())
             + ", max=" + QString::number(controller->get_maxval());
}

void HDFTranslator::writeVarInfo()
{
    createVarInfo();
    emit info(_varinfo);
}

void HDFTranslator::make_timeNpositionString()
{
    int framenumb = 0;

    framenumb = controller->get_curTime();
    if((0 <= framenumb) && (NULL != _timestr))
    {
      //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
      //cout << "\tframenumb = " << framenumb << endl;
      //cout << "\t_timestr[" << framenumb << "] = <" << _timestr[framenumb] << ">" << endl;
      //_timeinfo = "          " + _timestr[framenumb];
        _timeinfo = _timestr[framenumb];
    }
    else
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
        _position += "Longitude: " + xstr;
    if(-1 < nvoptions->get_ysec())
        _position += ", Latitude: " + ystr;
    if(0 < nvoptions->get_zsec())
        _position += ", Level: " + zstr;
}

void HDFTranslator::select1dVar(const QString& str)
{
   cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
   cout << "\tTry to selected 1d var: <" << str.toStdString() << ">." << endl;
}

//
//Select a variable
//
void HDFTranslator::select2dVar(const QString& str)
{
 //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
 //cout << "2d var: <" << str.toStdString() << "> is selected." << endl;
 
   _varname = str.toStdString();

   controller->set2dvarname(_varname);

   _minval = controller->get_minval();
   _maxval = controller->get_maxval();

   sliderNspinX->set_max(geometry->get_nlon());
   sliderNspinX->setValue(0);
   sliderNspinY->set_max(geometry->get_nlat());
   sliderNspinY->setValue(0);
   sliderNspinZ->set_max(geometry->get_nlev());
   sliderNspinZ->setValue(0);
   sliderNspinT->set_max(geometry->get_nTime());
   sliderNspinT->setValue(0);
   updateGL();
}

void HDFTranslator::select3dVar(const QString& str)
{
   _varname = str.toStdString();

 //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
 //cout << "3d var: <" << str.toStdString() << "> is selected." << endl;

   controller->set3dvarname(_varname);

   _minval = controller->get_minval();
   _maxval = controller->get_maxval();

   sliderNspinX->set_max(geometry->get_nlon());
   sliderNspinX->setValue(0);
   sliderNspinY->set_max(geometry->get_nlat());
   sliderNspinY->setValue(0);
   sliderNspinZ->set_max(geometry->get_nlev());
   sliderNspinZ->setValue(0);
   sliderNspinT->set_max(geometry->get_nTime());
   sliderNspinT->setValue(0);
   updateGL();
}

void HDFTranslator::update_frame()
{
    if(nvoptions->get_cb(NV_ANIMATIONON))
    {
        ++_glbTime;
        if(_glbTime >= _maxTime)
           _glbTime = 0;

        _set_current_time();
        controller->set_fileNtime(_curFile, _curTime);
        _timestr = controller->get_timestring();
    }

    updateGL();
}

void HDFTranslator::selectColorMap(const QString& str)
{
  //cout << "\nfile: <" << __FILE__ << ">, function: <" << __PRETTY_FUNCTION__
  //     << ">, line: " << __LINE__ << endl;
  //cout << "ColorMap: <" << str.toStdString() << "> is selected." << endl;

    colorTable->set_colorMap(str.toStdString());

    controller->set_colorTable(colorTable);

    updateGL();
}

int HDFTranslator::get_ndv(int n)
{
    return controller->get_ndv(n);
}

string* HDFTranslator::get_ndvNames(int n)
{
    string* varnames = controller->get_ndvNames(n);
    return varnames;
}

void HDFTranslator::set_locator(Locator* l)
{
    locator = l;

    locator->set_dim(dim);
    locator->set_fovy(fovy);
    locator->set_zfar(zFar);
    locator->set_znear(zNear);

    controller->set_locator(l);
}

void HDFTranslator::writeLocatorMsg()
{
     _locatorinfo = "Location lon="+QString::number(locator->x())
               + ", lat=" + QString::number(locator->y());

    emit locator_msg(_locatorinfo);
}

void HDFTranslator::set_light(Light* l)
{
    light = l;
  //mpascontroller->set_light(l);
}

