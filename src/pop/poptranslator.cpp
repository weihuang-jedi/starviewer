//$Id: poptranslator.cpp 4927 2014-01-23 21:49:37Z starviewer $

#include <QtOpenGL>

#include "poptranslator.h"

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
POPTranslator::POPTranslator(ColorTable* ct, NVOptions* opt,
                             string flnm, bool isList, QWidget* parent)
             : BaseTranslator(ct, opt, parent)
{
    _filename = flnm;
    _hasFileList = isList;

    _timestr = NULL;
    controller = NULL;
}

POPTranslator::~POPTranslator()
{
    if(NULL != controller)
        delete controller;
}

void POPTranslator::setup()
{
    controller = new POPController(colorTable, nvoptions,
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
void POPTranslator::show()
{
    controller->draw();

    _varname = controller->get_varname();

    setLabelColor();

    make_timeNpositionString();
    writeHeader();

    writeVarInfo();

    writeFrameInfo();

    if(locator->on())
        writeLocatorMsg();
}

void POPTranslator::writeFrameInfo()
{
    if(nvoptions->get_cb(NV_ANIMATIONON))
    {
        _frameinfo = "Frame No. "+QString::number(_glbTime);
        emit frame(_frameinfo);
    }
}

void POPTranslator::createVarInfo()
{
    _varinfo = "Dim nlon="+QString::number(geometry->get_pop_nlon())
             + ", nlat=" + QString::number(geometry->get_pop_nlat())
             + ", nlev=" + QString::number(geometry->get_pop_nlev())
             + "\nVar min=" + QString::number(controller->get_minval())
             + ", max=" + QString::number(controller->get_maxval());
}

void POPTranslator::writeVarInfo()
{
    createVarInfo();
    emit info(_varinfo);
}

void POPTranslator::make_timeNpositionString()
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

void POPTranslator::select1dVar(const QString& str)
{
   cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
   cout << "\tTry to selected 1d var: <" << str.toStdString() << ">." << endl;
}

//
//Select a variable
//
void POPTranslator::select2dVar(const QString& str)
{
 //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
 //cout << "2d var: <" << str.toStdString() << "> is selected." << endl;
 
   _varname = str.toStdString();

   controller->set2dvarname(_varname);

   _minval = controller->get_minval();
   _maxval = controller->get_maxval();

   sliderNspinX->set_max(geometry->get_pop_nlon());
   sliderNspinX->setValue(0);
   sliderNspinY->set_max(geometry->get_pop_nlat());
   sliderNspinY->setValue(0);
   sliderNspinZ->set_max(geometry->get_pop_nlev());
   sliderNspinZ->setValue(0);
   sliderNspinT->set_max(geometry->get_nTime());
   sliderNspinT->setValue(0);
   updateGL();
}

void POPTranslator::select3dVar(const QString& str)
{
   _varname = str.toStdString();

 //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
 //cout << "3d var: <" << str.toStdString() << "> is selected." << endl;

   controller->set3dvarname(_varname);

   _minval = controller->get_minval();
   _maxval = controller->get_maxval();

   sliderNspinX->set_max(geometry->get_pop_nlon());
   sliderNspinX->setValue(0);
   sliderNspinY->set_max(geometry->get_pop_nlat());
   sliderNspinY->setValue(0);
   sliderNspinZ->set_max(geometry->get_pop_nlev());
   sliderNspinZ->setValue(0);
   sliderNspinT->set_max(geometry->get_nTime());
   sliderNspinT->setValue(0);
   updateGL();
}

void POPTranslator::update_frame()
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

void POPTranslator::selectColorMap(const QString& str)
{
  //cout << "\nfile: <" << __FILE__ << ">, function: <" << __PRETTY_FUNCTION__
  //     << ">, line: " << __LINE__ << endl;
  //cout << "ColorMap: <" << str.toStdString() << "> is selected." << endl;

    colorTable->set_colorMap(str.toStdString());

    controller->set_colorTable(colorTable);

    updateGL();
}

int POPTranslator::get_ndv(int n)
{
    return controller->get_ndv(n);
}

string* POPTranslator::get_ndvNames(int n)
{
    string* varnames = controller->get_ndvNames(n);
    return varnames;
}

void POPTranslator::set_locator(Locator* l)
{
    locator = l;

    locator->set_dim(dim);
    locator->set_fovy(fovy);
    locator->set_zfar(zFar);
    locator->set_znear(zNear);

    controller->set_locator(l);
}

void POPTranslator::writeLocatorMsg()
{
     _locatorinfo = "Location lon="+QString::number(locator->x())
               + ", lat=" + QString::number(locator->y());

    emit locator_msg(_locatorinfo);
}

void POPTranslator::set_light(Light* l)
{
    light = l;
  //mpascontroller->set_light(l);
}

