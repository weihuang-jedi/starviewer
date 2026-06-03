//$Id: mpastranslator.cpp 4927 2014-01-23 21:49:37Z starviewer $
#include <QtOpenGL>

#include "mpastranslator.h"
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
MPASTranslator::MPASTranslator(ColorTable* ct, NVOptions* opt, string flnm, QWidget* parent)
              : BaseTranslator(ct, opt, parent)
{
    _flat = false;
    _saveJpg = false;
    _jpgNotSaved = true;
    _startSave = false;

    _filename = flnm;

    mpascontroller = NULL;
}

MPASTranslator::~MPASTranslator()
{
    if(NULL != mpascontroller)
        delete mpascontroller;
}

void MPASTranslator::setup()
{
    cout << "\nEnter Funciton: " << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
    mpascontroller = new MPASController(colorTable, nvoptions,
                                        _filename.c_str());

    _title = mpascontroller->get_title();
    _timestr = mpascontroller->get_timestring();
 
     geometry = mpascontroller->get_geometry();
   //evaluator = mpascontroller->get_evaluator();

    _varname = mpascontroller->get_varname();
    _minval = mpascontroller->get_minval();
    _maxval = mpascontroller->get_maxval();

    nvoptions->set_xsec(-1);
    nvoptions->set_ysec(-1);
    nvoptions->set_zsec(0);

    dim = 0.5;
    cout << "Leave Funciton: " << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
}

//******************************************************************
//*************************  OpenGL Events  ************************
//******************************************************************
void MPASTranslator::paintGL()
{
    cout << "\nEnter Funciton: " << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
    set_modelview();

  //Clear screen and Z-buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  //Enable Z-buffering in OpenGL
    glEnable(GL_DEPTH_TEST);

    // cout << "line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;

    // if(!locator)
    // {
    //     cout << "WARNING: locator is null. Skipping view configuration until initialized." << endl;
    //     return; // Exits safely, preventing the segmentation fault!
    // }

    setViewOptions();

    cout << "line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;

    setBackgroundColor();

    cout << "line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;

    show();

    cout << "line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;

    drawColorBar();

    // cout << "line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;

    // if(! nvoptions->get_cb(NV_PIXELON))
    //     drawAxis();

    if(nvoptions->get_cb(NV_STATUS_CHANGED))
        save_status();

  //Done
    glFlush();
    cout << "Leave Funciton: " << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
}
//show the image
void MPASTranslator::show()
{
    cout << "\nEnter Funciton: " << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
    glShadeModel(GL_SMOOTH);

    if(light->on())
    {
        glEnable(GL_LIGHTING);
        light->draw();
    }
    else
        glDisable(GL_LIGHTING);

    mpascontroller->draw();

    if(light->on())
        glDisable(GL_LIGHTING);

    setLabelColor();

    make_timeNpositionString();
    writeHeader();

    writeVarInfo();

    writeFrameInfo();

    if(locator->on())
        writeLocatorMsg();
    cout << "Leave Funciton: " << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
}

void MPASTranslator::writeFrameInfo()
{
    if(nvoptions->get_cb(NV_ANIMATIONON))
    {
        _frameinfo = "Frame No. "+QString::number(_glbTime);
        emit frame(_frameinfo);
    }
}

void MPASTranslator::createVarInfo()
{
    _varinfo = "Dim cells="+QString::number(geometry->get_nCells())
             + ", level=" + QString::number(geometry->get_nVertLevels())
             + "\nVar min=" + QString::number(mpascontroller->get_minval())
             + ", max=" + QString::number( mpascontroller->get_maxval());
}

void MPASTranslator::writeVarInfo()
{
    createVarInfo();
    emit info(_varinfo);
}

void MPASTranslator::make_timeNpositionString()
{
    int framenumb = 0;

    framenumb = mpascontroller->get_curTime();
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

void MPASTranslator::select1dVar(const QString& str)
{
    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
    cout << "1d var: <" << str.toStdString() << "> is selected." << endl;
}

void MPASTranslator::select2dVar(const QString& str)
{
  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "1d var: <" << str.toStdString() << "> is selected." << endl;
 
    _varname = str.toStdString();

    mpascontroller->set1dvarname(_varname);
    _title = mpascontroller->get_title();
    _timestr = mpascontroller->get_timestring();

    _minval = mpascontroller->get_minval();
    _maxval = mpascontroller->get_maxval();

    sliderNspinX->set_max(360);
    sliderNspinX->set_min(360);
    sliderNspinX->setValue(360);
    sliderNspinY->set_max(90);
    sliderNspinY->set_min(-90);
    sliderNspinY->setValue(90);
    sliderNspinZ->set_max(1);
    sliderNspinZ->setValue(0);
    sliderNspinT->set_max(geometry->get_nTime());
    sliderNspinT->setValue(0);

    nvoptions->set_zsec(0);

    updateGL();
}

void MPASTranslator::select3dVar(const QString& str)
{
    _varname = str.toStdString();

    mpascontroller->set2dvarname(_varname);
    _title = mpascontroller->get_title();
    _timestr = mpascontroller->get_timestring();

    _minval = mpascontroller->get_minval();
    _maxval = mpascontroller->get_maxval();

  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\t2d var: <" << str.toStdString() << "> is selected." << endl;
  //cout << "\tgeometry->get_nVertLevels() = " << geometry->get_nVertLevels() << endl;
 
    sliderNspinX->set_max(360);
    sliderNspinX->set_min(0);
    sliderNspinX->setValue(360);
    sliderNspinY->set_max(90);
    sliderNspinY->set_min(-90);
    sliderNspinY->setValue(90);
    sliderNspinZ->set_max(geometry->get_nVertLevels());
    sliderNspinZ->setValue(0);
    sliderNspinT->set_max(geometry->get_nTime());
    sliderNspinT->setValue(0);

    nvoptions->set_zsec(0);

    updateGL();
}

void MPASTranslator::update_frame()
{
    if(nvoptions->get_cb(NV_ANIMATIONON))
    {
        ++_glbTime;
 
        if(_glbTime >= _maxTime)
           _glbTime = 0;

        if(_saveJpg && (0 == _glbTime))
            _startSave = true;

        _set_current_time();
        mpascontroller->set_fileNtime(_curFile, _curTime);
        _timestr = mpascontroller->get_timestring();

        updateGL();
    }

    updateGL();
}

void MPASTranslator::selectColorMap(const QString& str)
{
  //cout << "\nfile: <" << __FILE__ << ">, function: <" << __PRETTY_FUNCTION__
  //     << ">, line: " << __LINE__ << endl;
  //cout << "ColorMap: <" << str.toStdString() << "> is selected." << endl;

    colorTable->set_colorMap(str.toStdString());

    mpascontroller->set_colorTable(colorTable);

    updateGL();
}

int MPASTranslator::get_ndv(int n)
{
    return mpascontroller->get_ndv(n-1);
}

string* MPASTranslator::get_ndvNames(int n)
{
    string* ts = new string[1];
    ts[0] = string("Unknown");
    return ts;
}

void MPASTranslator::set_light(Light* l)
{
    light = l;
  //mpascontroller->set_light(l);
}

//Draw vertex in polar coordinates
void MPASTranslator::sphereVertex(int i, int j)
{
  //float s = i/360.0;
  //float t = j/180.0 + 0.5;

    float x = 2.0*Cos(j)*Sin(i);
    float y = 2.0*Sin(j);
    float z = 2.0*Cos(j)*Cos(i);

    glNormal3f(x,y,z);
  //glTexCoord2f(s,t);
    glVertex3f(x,y,z);
}

void MPASTranslator::set_locator(Locator* l)
{
    locator = l;

    locator->set_dim(dim);
    locator->set_fovy(fovy);
    locator->set_zfar(zFar);
    locator->set_znear(zNear);

    mpascontroller->set_locator(l);
}

void MPASTranslator::writeLocatorMsg()
{
     _locatorinfo = "Location lon="+QString::number(locator->x())
               + ", lat=" + QString::number(locator->y());

    emit locator_msg(_locatorinfo);
}

