//$Id: vtktranslator.cpp 4865 2014-01-01 15:49:23Z starviewer $
#include <QtOpenGL>

#include "vtktranslator.h"
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
VTKTranslator::VTKTranslator(ColorTable* ct, NVOptions* opt,
                             string flnm, bool isList, QWidget* parent)
             : BaseTranslator(ct, opt, parent)
{
    _filename = flnm;
    _hasFileList = isList;

    _saveJpg = false;
    _jpgNotSaved = true;
    _startSave = false;
}

VTKTranslator::~VTKTranslator()
{
  //delete controller;
}

void VTKTranslator::setup()
{
    nvoptions->set_xsec(-1);
    nvoptions->set_ysec(-1);
    nvoptions->set_zsec(0);

    controller = new VTKController(colorTable, nvoptions,
                                  _filename, _hasFileList);

    _title = controller->get_title();
  //_timestr = controller->get_timestring();
 
    _varname = controller->get_varname();

  //updateSliders();

  //dim = 0.5;

    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
    cout << "\tfunction: " << __PRETTY_FUNCTION__ << endl;
}

//******************************************************************
//*************************  OpenGL Events  ************************
//******************************************************************
//Show image
void VTKTranslator::show()
{
    glShadeModel(GL_SMOOTH);

    if(light->on())
    {
        glEnable(GL_LIGHTING);
        light->draw();
    }
    else
        glDisable(GL_LIGHTING);

    controller->draw();

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

void VTKTranslator::writeFrameInfo()
{
    if(nvoptions->get_cb(NV_ANIMATIONON))
    {
        _frameinfo = "Frame No. "+QString::number(_glbTime);
        emit frame(_frameinfo);
    }
}

void VTKTranslator::createVarInfo()
{
  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;

    _varinfo = "Dim";

  //_varinfo = "Var min=" + QString::number(controller->get_minval())
  //         + ", max=" + QString::number( controller->get_maxval());
}

void VTKTranslator::writeVarInfo()
{
    createVarInfo();
    emit info(_varinfo);
}

void VTKTranslator::make_timeNpositionString()
{
  //int framenumb = 0;

  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;

  //framenumb = controller->get_curTime();
  //if((0 <= framenumb) && (NULL != _timestr))
  //{
      //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
      //cout << "\tframenumb = " << framenumb << endl;
      //cout << "\t_timestr[" << framenumb << "] = <" << _timestr[framenumb] << ">" << endl;
      //_timeinfo = "          " + _timestr[framenumb];
  //    _timeinfo = _timestr[framenumb];
  //}
  //else
  //{
        string tstr = number2string<int>(_glbTime);
        _timeinfo = "Time: ";
        _timeinfo += tstr;
  //}

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

void VTKTranslator::select1dVar(const QString& str)
{
    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
    cout << "1d var: <" << str.toStdString() << "> is selected." << endl;
}

void VTKTranslator::select2dVar(const QString& str)
{
    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
    cout << "1d var: <" << str.toStdString() << "> is selected." << endl;
}

void VTKTranslator::updateSliders()
{
#if 0
    int intvl = 0;
    int stepsize = 1;

    sliderNspinX->set_max(controller->get_nrays());
    sliderNspinX->setValue(0);

    intvl = (controller->get_nrays()+9)/10;
    sliderNspinX->set_tickinterval(intvl);
    stepsize = (controller->get_nrays()+99)/100;
    sliderNspinX->set_step(stepsize);

    sliderNspinY->set_max(controller->get_nbins());
    sliderNspinY->setValue(0);

    intvl = (controller->get_nbins()+9)/10;
    sliderNspinY->set_tickinterval(intvl);
    stepsize = (controller->get_nbins()+99)/100;
    sliderNspinY->set_step(stepsize);

    sliderNspinZ->set_max(controller->get_nsweeps());
    sliderNspinZ->setValue(0);
  //sliderNspinT->set_max(geometry->get_nTime());
#endif
    sliderNspinT->set_max(1);
    sliderNspinT->setValue(0);
}

void VTKTranslator::select3dVar(const QString& str)
{
  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\tvar: <" << str.toStdString() << "> is selected." << endl;
  //cout << "\tnsweeps = " << controller->get_nsweeps() << endl;

    _varname = str.toStdString();

    controller->set2dvarname(_varname);
    _title = controller->get_title();
  //_timestr = controller->get_timestring();

    _minval = controller->get_minval();
    _maxval = controller->get_maxval();

    nvoptions->set_zsec(0);

    updateSliders();

    updateGL();
}

void VTKTranslator::update_frame()
{
    if(nvoptions->get_cb(NV_ANIMATIONON))
    {
        ++_glbTime;
 
        if(_glbTime >= _maxTime)
           _glbTime = 0;

        if(_saveJpg && (0 == _glbTime))
            _startSave = true;

        _set_current_time();
      //controller->set_fileNtime(_curFile, _curTime);
      //_timestr = controller->get_timestring();

        updateGL();
    }

    updateGL();
}

void VTKTranslator::selectColorMap(const QString& str)
{
  //cout << "\nfile: <" << __FILE__ << ">, function: <" << __PRETTY_FUNCTION__
  //     << ">, line: " << __LINE__ << endl;
  //cout << "ColorMap: <" << str.toStdString() << "> is selected." << endl;

    colorTable->set_colorMap(str.toStdString());

    controller->set_colorTable(colorTable);

    updateGL();
}

int VTKTranslator::get_ndv(int n)
{
    if(2 == n)
        return controller->get_varnumber();
    else
        return 0;
}

string* VTKTranslator::get_ndvNames(int n)
{
    unsigned int i;
    string* varnames = NULL;
    vector<string> vn;

    if(2 == n)
    {
        vn = controller->get_varnames();
        varnames = new string[vn.size()];

        for(i = 0; i < vn.size(); ++i)
            varnames[i] = vn[i];
    }

    return varnames;
}

void VTKTranslator::set_light(Light* l)
{
    light = l;
  //controller->set_light(l);
}

//Draw vertex in polar coordinates
void VTKTranslator::sphereVertex(int i, int j)
{
    float x = 2.0*Cos(j)*Sin(i);
    float y = 2.0*Sin(j);
    float z = 2.0*Cos(j)*Cos(i);

    glNormal3f(x,y,z);
  //glTexCoord2f(s,t);
    glVertex3f(x,y,z);
}

void VTKTranslator::set_locator(Locator* l)
{
    locator = l;

    locator->set_dim(dim);
    locator->set_fovy(fovy);
    locator->set_zfar(zFar);
    locator->set_znear(zNear);

  //controller->set_locator(l);
}

void VTKTranslator::writeLocatorMsg()
{
     _locatorinfo = "Location lon="+QString::number(locator->x())
               + ", lat=" + QString::number(locator->y());

    emit locator_msg(_locatorinfo);
}

