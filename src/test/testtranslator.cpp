//$Id: testtranslator.cpp 4865 2014-01-01 15:49:23Z starviewer $
#include <QtOpenGL>

#include "testtranslator.h"
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
TestTranslator::TestTranslator(ColorTable* ct, NVOptions* opt,
                               string flnm, bool isList, QWidget* parent)
              : BaseTranslator(ct, opt, parent)
{
    _flat = false;

    _filename = flnm;
    _hasFileList = isList;

    topography = new Topography();
    topography->set_nvoptions(opt);

    coastline2 = new CoastLine2();
    coastline2->set_nvoptions(opt);
}

TestTranslator::~TestTranslator()
{
    delete topography;
    delete coastline2;
}

void TestTranslator::setup()
{
    _title = "Test";
    _timestr = new string[1];
    _timestr[0] = "Test";
 
    _varname = "Var";

    _minval = -1.0;
    _maxval =  1.0;

    nvoptions->set_xsec(0);
    nvoptions->set_ysec(0);
    nvoptions->set_zsec(0);

    dim = 0.5;
}

//******************************************************************
//*************************  OpenGL Events  ************************
//******************************************************************
//Show image
void TestTranslator::show()
{
    glShadeModel(GL_SMOOTH);

    if(light->on())
    {
        glEnable(GL_LIGHTING);
        light->draw();
    }
    else
        glDisable(GL_LIGHTING);

    draw();

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

void TestTranslator::writeFrameInfo()
{
    if(nvoptions->get_cb(NV_ANIMATIONON))
    {
        _frameinfo = "Frame No. "+QString::number(_glbTime);
        emit frame(_frameinfo);
    }
}

void TestTranslator::createVarInfo()
{
    _varinfo = "Info";
}

void TestTranslator::writeVarInfo()
{
    createVarInfo();
    emit info(_varinfo);
}

void TestTranslator::make_timeNpositionString()
{
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

void TestTranslator::select1dVar(const QString& str)
{
    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
    cout << "1d var: <" << str.toStdString() << "> is selected." << endl;
}

void TestTranslator::select2dVar(const QString& str)
{
  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "1d var: <" << str.toStdString() << "> is selected." << endl;
 
    _varname = str.toStdString();

    nvoptions->set_zsec(0);

    updateGL();
}

void TestTranslator::select3dVar(const QString& str)
{
    _varname = str.toStdString();

    nvoptions->set_zsec(0);

    updateGL();
}

void TestTranslator::update_frame()
{
    updateGL();
}

void TestTranslator::selectColorMap(const QString& str)
{
  //cout << "\nfile: <" << __FILE__ << ">, function: <" << __PRETTY_FUNCTION__
  //     << ">, line: " << __LINE__ << endl;
  //cout << "ColorMap: <" << str.toStdString() << "> is selected." << endl;

    colorTable->set_colorMap(str.toStdString());

    updateGL();
}

int TestTranslator::get_ndv(int n)
{
  //cout << "\nfile: <" << __FILE__ << ">" << endl;
  //cout << "\tfunction: <" << __PRETTY_FUNCTION__ << ">" << endl;
  //cout << "\tline: " << __LINE__ << endl;
  //cout << "\tn = " << n << endl;

    return 1;
}

string* TestTranslator::get_ndvNames(int n)
{
    string* varnames;

  //cout << "\nfile: <" << __FILE__ << ">" << endl;
  //cout << "\tfunction: <" << __PRETTY_FUNCTION__ << ">" << endl;
  //cout << "\tline: " << __LINE__ << endl;
  //cout << "\tn = " << n << endl;

    varnames = new string[1];
    varnames[0] = "dummy";
    return varnames;
}

void TestTranslator::set_light(Light* l)
{
    light = l;
}

//Draw vertex in polar coordinates
void TestTranslator::sphereVertex(int i, int j)
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

void TestTranslator::set_locator(Locator* l)
{
    locator = l;

    locator->set_dim(dim);
    locator->set_fovy(fovy);
    locator->set_zfar(zFar);
    locator->set_znear(zNear);
}

void TestTranslator::writeLocatorMsg()
{
     _locatorinfo = "Location lon="+QString::number(locator->x())
               + ", lat=" + QString::number(locator->y());

    emit locator_msg(_locatorinfo);
}

void TestTranslator::update_sliderNspin()
{
  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\ttopography->get_level() = " << topography->get_level() << endl;

    sliderNspinX->set_max(topography->get_level());
    sliderNspinX->setValue(0);
    sliderNspinX->set_tickinterval(1);
    sliderNspinX->set_step(1);

    sliderNspinY->set_max(topography->get_level());
    sliderNspinY->setValue(0);
    sliderNspinY->set_tickinterval(1);
    sliderNspinY->set_step(1);

    sliderNspinZ->set_max(1);
    sliderNspinZ->set_step(1);
    sliderNspinZ->setValue(0);

    sliderNspinT->set_max(1);
    sliderNspinT->set_step(1);
    sliderNspinT->setValue(0);
}

void TestTranslator::draw()
{
#if 1
  //draw coastline2
  //coastline2->draw_ball(0.99);
  //coastline2->draw();
  //coastline2->draw_grid(1.01);

    if(nvoptions->get_cb(NV_SUBSET))
    {
        coastline2->drawAregion(0.0001);
        coastline2->drawRegionalGrid(0.0001);
    }
    else
    {
        coastline2->drawONplane(0.0001);
        coastline2->draw_plane_grid(0.0001);
    }
#endif

#if 1
  //draw topography

    if(nvoptions->get_cb(NV_SUBSET))
    {
        topography->drawAregion(0.0);
    }
    else
    {
      //topography->draw();
        topography->draw_plane(0.0);
        topography->draw_plane_grid(0.0001);
    }
#endif
}

