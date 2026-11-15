//$Id: mpasgridtranslator.cpp 4927 2014-01-23 21:49:37Z starviewer $
#include <QtOpenGL>

#include "mpasgridtranslator.h"
#include "colorTable.h"
#include "util.h"

//
//  Constructor
//
MPASGridTranslator::MPASGridTranslator(ColorTable* ct, NVOptions* opt,
		                       string static_flnm, string data_flnm, QWidget* parent)
              : BaseTranslator(ct, opt, parent)
{
    _flat = false;
    _saveJpg = false;
    _jpgNotSaved = true;
    _startSave = false;

    set_static_flnm(static_flnm);
    set_data_flnm(data_flnm);

    mpasgridcontroller = NULL;
}

MPASGridTranslator::~MPASGridTranslator()
{
    if(NULL != mpasgridcontroller)
        delete mpasgridcontroller;
}

void MPASGridTranslator::setup()
{
    // cout << "\nEnter Funciton: " << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
    mpasgridcontroller = new MPASGridController(colorTable, nvoptions,
                                                _static_flnm, _data_flnm);
    // cout << "\t" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
    mpasgridcontroller->setup();

    // cout << "\t" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
    _title = mpasgridcontroller->get_title();
 
    // cout << "\t" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
    _jpgNotSaved = true;
    _startSave = false;

    makeCurrent();

    // cout << "\t" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
    geometry = mpasgridcontroller->get_geometry();
    // evaluator = mpasgridcontroller->get_evaluator();
 
    // cout << "\t" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
    _varname = string("pic");

    // cout << "\t" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
    // cout << "\t_varname: " << _varname << endl;
    mpasgridcontroller->set2dvarname(_varname);

    _varname = mpasgridcontroller->get_varname();
    // cout << "\t_varname: " << _varname << endl;

    nvoptions->set_xsec(-1);
    nvoptions->set_ysec(-1);
    nvoptions->set_zsec(0);

    dim = 0.5;
    // cout << "Leave Funciton: " << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
}

//******************************************************************
//*************************  OpenGL Events  ************************
//******************************************************************
//show the image
void MPASGridTranslator::show()
{
    // cout << "\nEnter Funciton: " << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
    // glShadeModel(GL_SMOOTH);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if(light->on())
    {
        glEnable(GL_LIGHTING);
        light->draw();
    }
    else
        glDisable(GL_LIGHTING);

    mpasgridcontroller->draw();

    if(light->on())
        glDisable(GL_LIGHTING);

    setLabelColor();

    make_timeNpositionString();
    writeHeader();

    writeVarInfo();

    writeFrameInfo();

    if(locator->on())
        writeLocatorMsg();
    // cout << "Leave Funciton: " << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
}

void MPASGridTranslator::writeFrameInfo()
{
    if(nvoptions->get_cb(NV_ANIMATIONON))
    {
        _frameinfo = "Frame No. "+QString::number(_glbTime);
        emit frame(_frameinfo);
    }
}

void MPASGridTranslator::createVarInfo()
{
    _varinfo = "Dim cells="+QString::number(geometry->get_nCells());
}

void MPASGridTranslator::writeVarInfo()
{
    createVarInfo();
    emit info(_varinfo);
}

void MPASGridTranslator::make_timeNpositionString()
{
    int framenumb = 0;

    framenumb = mpasgridcontroller->get_curTime();
    _timeinfo = "Time: ";

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

void MPASGridTranslator::select1dVar(const QString& str)
{
    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
    cout << "1d var: <" << str.toStdString() << "> is selected." << endl;
}

void MPASGridTranslator::select2dVar(const QString& str)
{
  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "1d var: <" << str.toStdString() << "> is selected." << endl;
 
    _varname = str.toStdString();

    mpasgridcontroller->set1dvarname(_varname);
    _title = mpasgridcontroller->get_title();

    sliderNspinX->set_max(360);
    sliderNspinX->set_min(360);
    sliderNspinX->setValue(360);
    sliderNspinY->set_max(90);
    sliderNspinY->set_min(-90);
    sliderNspinY->setValue(90);
    sliderNspinZ->set_max(1);
    sliderNspinZ->setValue(0);
    sliderNspinT->set_max(1);
    sliderNspinT->setValue(0);

    nvoptions->set_zsec(0);

    updateGL();
}

void MPASGridTranslator::select3dVar(const QString& str)
{
    _varname = str.toStdString();

    mpasgridcontroller->set3dvarname(_varname);
    _title = mpasgridcontroller->get_title();

  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\t2d var: <" << str.toStdString() << "> is selected." << endl;
 
    sliderNspinX->set_max(360);
    sliderNspinX->set_min(0);
    sliderNspinX->setValue(360);
    sliderNspinY->set_max(90);
    sliderNspinY->set_min(-90);
    sliderNspinY->setValue(90);
    sliderNspinZ->set_max(1);
    sliderNspinZ->setValue(0);
    sliderNspinT->set_max(1);
    sliderNspinT->setValue(0);

    nvoptions->set_zsec(0);

    updateGL();
}

void MPASGridTranslator::update_frame()
{
    if(nvoptions->get_cb(NV_ANIMATIONON))
    {
        ++_glbTime;
 
        if(_glbTime >= _maxTime)
           _glbTime = 0;

        if(_saveJpg && (0 == _glbTime))
            _startSave = true;

        _set_current_time();
        mpasgridcontroller->set_fileNtime(_curFile, _curTime);

        updateGL();
    }

    updateGL();
}

void MPASGridTranslator::selectColorMap(const QString& str)
{
  //cout << "\nfile: <" << __FILE__ << ">, function: <" << __PRETTY_FUNCTION__
  //     << ">, line: " << __LINE__ << endl;
  //cout << "ColorMap: <" << str.toStdString() << "> is selected." << endl;

    colorTable->set_colorMap(str.toStdString());

    mpasgridcontroller->set_colorTable(colorTable);

    updateGL();
}

int MPASGridTranslator::get_ndv(int n)
{
    return mpasgridcontroller->get_ndv(n);
}

string* MPASGridTranslator::get_ndvNames(int n)
{
    vector<string> vnlist = mpasgridcontroller->get_ndvNames(n);
    int ns = vnlist.size();
    if(0 >= ns)
        ns = 1;

    string* ts = new string[ns];

    if(0 == vnlist.size())
    {
        ts[0] = string("Unknown");
    }
    else
    {
        for(n = 0; n < ns; ++n)
	    ts[n] = vnlist[n];
    }
    return ts;
}

void MPASGridTranslator::set_light(Light* l)
{
    light = l;
  //mpasgridcontroller->set_light(l);
}

//Draw vertex in polar coordinates
void MPASGridTranslator::sphereVertex(int i, int j)
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

void MPASGridTranslator::set_locator(Locator* l)
{
    locator = l;

    locator->set_dim(dim);
    locator->set_fovy(fovy);
    locator->set_zfar(zFar);
    locator->set_znear(zNear);

    mpasgridcontroller->set_locator(l);
}

void MPASGridTranslator::writeLocatorMsg()
{
     _locatorinfo = "Location lon="+QString::number(locator->x())
               + ", lat=" + QString::number(locator->y());

    emit locator_msg(_locatorinfo);
}

