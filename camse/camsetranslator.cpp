//$Id

#include <QtOpenGL>

#include "camsetranslator.h"
#include "colorTable.h"

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
CAMseTranslator::CAMseTranslator(ColorTable* ct, NVOptions* opt,
                                 string flnm, bool isList, string mfnm, QWidget* parent)
               : BaseTranslator(ct, opt, parent)
{
  //cout << "\tEnter Funciton: " << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\t\tCAM-SE file: " << flnm << endl;
  //cout << "\t\tCAM-SE mapping file: " << mfnm << endl;

    _filename = flnm;
    _hasFileList = isList;

    _mappingFilename = mfnm;

    nvoptions->set_xsec(-1);
    nvoptions->set_ysec(-91);
    nvoptions->set_zsec(0);
    nvoptions->set_tsec(0);

    camsecontroller = NULL;

  //cout << "Leave Funciton: " << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
}

CAMseTranslator::~CAMseTranslator()
{
    if(NULL != camsecontroller)
        delete camsecontroller;
    camsecontroller = NULL;
}

void CAMseTranslator::setup()
{
    int n;

  //cout << "\tEnter Funciton: " << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
    if(NULL != camsecontroller)
        delete camsecontroller;

  //if(_hasFileList)
  //{
      //cout << "\t\tFunciton: " << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
        camsecontroller = new CAMseController(colorTable, nvoptions,
                                              _filename.c_str(), _hasFileList);
  //}
  //else
  //{
  //  //cout << "\t\tFunciton: " << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //    camsecontroller = new CAMseController(colorTable, nvoptions, _filename.c_str());
  //}

  //cout << "\t\tFunciton: " << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
    camsecontroller->set_mappingFile(_mappingFilename);
  //cout << "\t\tFunciton: " << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
    camsecontroller->setup();

    _jpgNotSaved = true;
    _startSave = false;

    makeCurrent();

  //cout << "\t\tFunciton: " << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
    geometry = camsecontroller->get_geometry();

    _varname = string("PSL");

    camsecontroller->set2dvarname(_varname);

    _minval = camsecontroller->get_minval();
    _maxval = camsecontroller->get_maxval();

    _title   = camsecontroller->get_title();
    _timestr = camsecontroller->get_timestring();
    _maxFile = camsecontroller->get_nfils();
    _nTimes  = camsecontroller->get_ntimes();

    _maxTime = 0;
    for(n = 0; n < _maxFile; ++n)
        _maxTime += _nTimes[n];

    if(_maxTime > 12)
        _maxTime = 12;

  //cout << "\t_maxFile = " << _maxFile << endl;
  //cout << "\t_maxTime = " << _maxTime << endl;
  //cout << "\tLeave Funciton: " << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
}

/*******************************************************************/
/*************************  OpenGL Events  *************************/
/*******************************************************************/
//Show the image
void CAMseTranslator::show()
{
    glShadeModel(GL_SMOOTH);

    if(nvoptions->get_tsec() != _glbTime)
    {
        _glbTime = nvoptions->get_tsec();

        _set_current_time();

        camsecontroller->set_fileNtime(_curFile, _curTime);
    }

    camsecontroller->draw();

  //_varname = camsecontroller->get_varname();

    setLabelColor();

    make_timeNpositionString();
    writeHeader();

    writeVarInfo();

    writeFrameInfo();

    if(locator->on())
        writeLocatorMsg();
}

void CAMseTranslator::createVarInfo()
{
    _varinfo = "Dim ncol="+QString::number(geometry->get_camse_ncol())
             + ", nlev=" + QString::number(geometry->get_camse_lev())
             + "\nVar min=" + QString::number(camsecontroller->get_minval())
             + ", max=" + QString::number(camsecontroller->get_maxval());
}

void CAMseTranslator::writeVarInfo()
{
    createVarInfo();
    emit info(_varinfo);
}

void CAMseTranslator::writeFrameInfo()
{
    if(nvoptions->get_cb(NV_ANIMATIONON))
    {
        _frameinfo = "Frame No. "+QString::number(_glbTime);
        emit frame(_frameinfo);
    }
}

void CAMseTranslator::make_timeNpositionString()
{
    int framenumb = 0;

    framenumb = camsecontroller->get_curTime();
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

#if 0
void CAMseTranslator::SaveJpg(int n)
{
    bool withAlpha = true;
  //Copy from OpenGL
    QImage *image = new QImage(grabFrameBuffer(withAlpha));

    char flnm[128];

    if(image)
    {
        sprintf(flnm, "img%4.4d.jpg", n);
        image->save(flnm);
        delete image;
    }
}
#endif

//Select a variable
void CAMseTranslator::select0dVar(const QString& str)
{
   cout << "0d var: <" << str.toStdString() << "> is selected." << endl;
}

void CAMseTranslator::select1dVar(const QString& str)
{
   cout << "0d var: <" << str.toStdString() << "> is selected." << endl;
}

void CAMseTranslator::select2dVar(const QString& str)
{
  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "1d var: <" << str.toStdString() << "> is selected." << endl;
 
    _varname = str.toStdString();

    camsecontroller->set1dvarname(_varname);

    sliderNspinX->set_max(360);
    sliderNspinX->setValue(0);
    sliderNspinY->set_max(90);
    sliderNspinY->set_min(-90);
    sliderNspinY->setValue(0);
    sliderNspinZ->set_max(1);
    sliderNspinZ->setValue(0);
    sliderNspinT->set_max(geometry->get_nt());
    sliderNspinT->setValue(0);

    _minval = camsecontroller->get_minval();
    _maxval = camsecontroller->get_maxval();
    _title  = camsecontroller->get_title();

    _jpgNotSaved = true;
    _startSave = false;

    updateGL();
}

void CAMseTranslator::select3dVar(const QString& str)
{
  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "2d var: <" << str.toStdString() << "> is selected." << endl;
 
    _varname = str.toStdString();

    camsecontroller->set3dvarname(_varname);

    sliderNspinX->set_max(360);
    sliderNspinX->setValue(0);
    sliderNspinY->set_max(90);
    sliderNspinY->set_min(-90);
    sliderNspinZ->set_max(geometry->get_camse_lev());
    sliderNspinZ->setValue(0);
    sliderNspinT->set_max(geometry->get_nt());
    sliderNspinT->setValue(0);

    _minval = camsecontroller->get_minval();
    _maxval = camsecontroller->get_maxval();
    _title  = camsecontroller->get_title();
 
    _jpgNotSaved = true;
    _startSave = false;

    updateGL();
}

void CAMseTranslator::selectColorMap(const QString& str)
{
  //cout << "\nfile: <" << __FILE__ << ">, function: <" << __PRETTY_FUNCTION__
  //     << ">, line: " << __LINE__ << endl;
  //cout << "ColorMap: <" << str.toStdString() << "> is selected." << endl;

    colorTable->set_colorMap(str.toStdString());
    camsecontroller->set_colorTable(colorTable);

    updateGL();
}

int CAMseTranslator::get_ndv(int n)
{
    return camsecontroller->get_ndv(n-1);
}

string* CAMseTranslator::get_ndvNames(int n)
{
    string* varnames = camsecontroller->get_ndvNames(n-1);
    return varnames;
}

void CAMseTranslator::update_frame()
{
    ++_glbTime;
    if(_glbTime >= _maxTime)
       _glbTime = 0;

    _set_current_time();
    camsecontroller->set_fileNtime(_curFile, _curTime);
    _timestr = camsecontroller->get_timestring();

    if(nvoptions->get_cb(NV_ANIMATIONON))
    {
      //cout << "\nEnter Function: " << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;

      //cout << "\tAnimate time: " << _glbTime
      //     << ", _startSave = " << _startSave
      //     << ", _jpgNotSaved = " << _jpgNotSaved <<endl;

        if(_startSave)
        {
            if(_jpgNotSaved && nvoptions->get_cb(NV_SAVEJPG))
            {
               //cout << "\tFunction: " << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
               //cout << "\tBefore save jpeg for time: " << _glbTime << endl;

                 SaveJpg(_glbTime);

               //cout << "\tAfter ave jpeg for time: " << _glbTime << endl;
             }

             if((_glbTime + 1) == _maxTime)
             {
                 _startSave = false;
                 if(_jpgNotSaved)
                    _jpgNotSaved = false;
             }
         }

        if(nvoptions->get_cb(NV_SAVEJPG) && (0 == _glbTime))
            _startSave = true;

      //cout << "\tFunction: " << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
      //cout << "\t_curTime = " << _curTime << ", _glbTime = " << _glbTime << ", _maxTime = " << _maxTime << endl;
 
        nvoptions->set_tsec(_curTime);

      //cout << "Leave Function: " << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
    }

    updateGL();
}

void CAMseTranslator::nextFrame()
{
    ++_glbTime;
    if(_glbTime >= _maxTime)
       _glbTime = 0;

    _set_current_time();
    camsecontroller->set_fileNtime(_curFile, _curTime);
    _timestr = camsecontroller->get_timestring();

    updateGL();
}

void CAMseTranslator::backFrame()
{
    --_glbTime;
    if(_glbTime < 0)
       _glbTime += _maxTime;

    _set_current_time();
    camsecontroller->set_fileNtime(_curFile, _curTime);
    _timestr = camsecontroller->get_timestring();

    updateGL();
}


void CAMseTranslator::set_light(Light* l)
{
    light = l;
  //camsecontroller->set_light(l);
}

void CAMseTranslator::set_locator(Locator* l)
{
    locator = l;

    locator->set_dim(dim);
    locator->set_fovy(fovy);
    locator->set_zfar(zFar);
    locator->set_znear(zNear);

    camsecontroller->set_locator(l);
}

void CAMseTranslator::writeLocatorMsg()
{
     _locatorinfo = "Location lon="+QString::number(locator->x())
               + ", lat=" + QString::number(locator->y());

    emit locator_msg(_locatorinfo);
}

