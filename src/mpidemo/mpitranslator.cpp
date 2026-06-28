//$Id: mpitranslator.cpp 4927 2014-01-23 21:49:37Z starviewer $

#include <QtOpenGL>

#include "mpitranslator.h"

//
//  Constructor
//
MPITranslator::MPITranslator(ColorTable* ct, NVOptions* opt, QWidget* parent)
             : BaseTranslator(ct, opt, parent)
{
    _glbTime = 0;
    _maxTime = 100;
    _curFile = 0;
    _maxFile = 1;

    _nTimes = new int[1];
    _nTimes[0] = 100;

    viewer = new MPIViewer();
}

MPITranslator::~MPITranslator()
{
    delete [] _nTimes;
    delete viewer;
}

void MPITranslator::setup()
{
    nvoptions->set_xsec(-1);
    nvoptions->set_ysec(-1);
    nvoptions->set_zsec(0);

    _title = "MPI Demo";
    _varname = "Cube";
}

//******************************************************************
//*************************  OpenGL Events  ************************
//******************************************************************
//Show the image
void MPITranslator::show()
{
    // cout << "\nfile: <" << __FILE__ << ">, function: <" << __PRETTY_FUNCTION__
    //      << ">, line: " << __LINE__ << endl;
    // cout << "locator->on(): " << locator->on() << endl;
    // cout << "light->on(): " << light->on() << endl;
    
    glShadeModel(GL_SMOOTH);

  //Clear screen and Z-buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  //Enable Z-buffering in OpenGL
    glEnable(GL_DEPTH_TEST);

    if(light->on())
    {
        glEnable(GL_LIGHTING);
        light->draw();
    }
    else
        glDisable(GL_LIGHTING);

    glColor4d(1.0, 0.0, 1.0, 0.50);

  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\t_glbTime = " << _glbTime << endl;

    viewer->set_step(_glbTime);
    viewer->draw();

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

void MPITranslator::writeFrameInfo()
{
    if(nvoptions->get_cb(NV_ANIMATIONON))
    {
        _frameinfo = "Frame No. "+QString::number(_glbTime);
        emit frame(_frameinfo);
    }
}

void MPITranslator::createVarInfo()
{
  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;

    _varinfo = "Dim";
}

void MPITranslator::writeVarInfo()
{
    createVarInfo();
    emit info(_varinfo);
}

void MPITranslator::make_timeNpositionString()
{
    string tstr = number2string<int>(_glbTime);
    _timeinfo = "Time: ";
    _timeinfo += tstr;

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

void MPITranslator::select1dVar(const QString& str)
{
    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
    cout << "1d var: <" << str.toStdString() << "> is selected." << endl;
}

void MPITranslator::select2dVar(const QString& str)
{
    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
    cout << "1d var: <" << str.toStdString() << "> is selected." << endl;
}

void MPITranslator::updateSliders()
{
    sliderNspinX->set_max(10);
    sliderNspinX->setValue(0);
    sliderNspinX->set_tickinterval(1);
    sliderNspinX->set_step(1);

    sliderNspinY->set_max(10);
    sliderNspinY->setValue(0);
    sliderNspinY->set_tickinterval(1);
    sliderNspinY->set_step(1);

    sliderNspinZ->set_max(10);
    sliderNspinZ->setValue(0);
    sliderNspinZ->set_tickinterval(1);
    sliderNspinZ->set_step(1);

    sliderNspinT->set_max(100);
    sliderNspinT->setValue(0);
    sliderNspinT->set_tickinterval(10);
    sliderNspinT->set_step(1);
}

void MPITranslator::select3dVar(const QString& str)
{
  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\tvar: <" << str.toStdString() << "> is selected." << endl;

    _varname = str.toStdString();

    _minval = -1.0;
    _maxval =  1.0;

    nvoptions->set_zsec(0);

    updateSliders();

    updateGL();
}

void MPITranslator::selectColorMap(const QString& str)
{
  //cout << "\nfile: <" << __FILE__ << ">, function: <" << __PRETTY_FUNCTION__
  //     << ">, line: " << __LINE__ << endl;
  //cout << "ColorMap: <" << str.toStdString() << "> is selected." << endl;

    colorTable->set_colorMap(str.toStdString());

    updateGL();
}

int MPITranslator::get_ndv(int n)
{
    if(0 < n)
        return 2;
    else
        return 1;
}

string* MPITranslator::get_ndvNames(int n)
{
    string* varnames = new string[2];
    vector<string> vn;

    if(0 < n)
    {
        varnames[0] = "Dummy";
        varnames[1] = "MPI";
    }

    return varnames;
}

void MPITranslator::writeLocatorMsg()
{
     _locatorinfo = "Location lon="+QString::number(locator->x())
               + ", lat=" + QString::number(locator->y());

    emit locator_msg(_locatorinfo);
}

void MPITranslator::update_frame()
{
    ++_glbTime;

    // cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
    // cout << "\t_glbTime = " << _glbTime << endl;
 
    if(_glbTime >= _maxTime)
       _glbTime = 0;

    viewer->set_step(_glbTime);

    updateGL();
}


void MPITranslator::nextFrame()
{
    ++_glbTime;
    if(_glbTime >= _maxTime)
       _glbTime = 0;

    viewer->set_step(_glbTime);

    updateGL();
}

void MPITranslator::backFrame()
{
    --_glbTime;
    if(_glbTime < 0)
       _glbTime += _maxTime;

    viewer->set_step(_glbTime);

    updateGL();
}

void MPITranslator::paintGL()
{
    // cout << "Enter Functions: <" << __PRETTY_FUNCTION__
    //      << ">, line: " << __LINE__
    //      << ", file: <" << __FILE__ << ">" << endl;

    set_modelview();

    // cout << "line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;

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

    // cout << "line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;

    setBackgroundColor();

    // cout << "line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;

    show();

    // cout << "line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;

    // drawColorBar();

    // cout << "line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;

    if(! nvoptions->get_cb(NV_PIXELON))
        drawAxis();

    if(nvoptions->get_cb(NV_STATUS_CHANGED))
        save_status();

  //Done
    glFlush();

    // cout << "Leave Functions: <" << __PRETTY_FUNCTION__
    //      << ">, line: " << __LINE__
    //      << ", file: <" << __FILE__ << ">" << endl;
}
