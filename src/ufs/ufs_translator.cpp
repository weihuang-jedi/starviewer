//$Id

#include <QtOpenGL>

#include "ufs_translator.h"
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
UFSTranslator::UFSTranslator(ColorTable* ct, NVOptions* opt,
                                 string flnm, bool isList, string mfnm, QWidget* parent)
               : BaseTranslator(ct, opt, parent)
{
  //cout << "\tEnter Funciton: " << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\t\tCAM-SE file: " << flnm << endl;
  //cout << "\t\tCAM-SE mapping file: " << mfnm << endl;

    _filename = flnm;
    _hasFileList = isList;

    nvoptions->set_xsec(-1);
    nvoptions->set_ysec(-91);
    nvoptions->set_zsec(0);
    nvoptions->set_tsec(0);

    ufs_controller = NULL;

    _timestr = new string[2];

  //cout << "Leave Funciton: " << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
}

UFSTranslator::~UFSTranslator()
{
    if(NULL != ufs_controller)
        delete ufs_controller;
    if(NULL != _timestr)
        delete[] _timestr;
    ufs_controller = NULL;
}

void UFSTranslator::setup()
{
    int n;

    cout << "\tEnter Funciton: " << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
    if(NULL != ufs_controller)
        delete ufs_controller;

  //if(_hasFileList)
  //{
      //cout << "\t\tFunciton: " << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
        ufs_controller = new UFSController(colorTable, nvoptions,
                                              _filename.c_str(), _hasFileList);
  //}
  //else
  //{
  //  //cout << "\t\tFunciton: " << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //    ufs_controller = new UFSController(colorTable, nvoptions, _filename.c_str());
  //}

    cout << "\t\tFunciton: " << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
    ufs_controller->setup();

    _jpgNotSaved = true;
    _startSave = false;

    makeCurrent();

    cout << "\t\tFunciton: " << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
    geometry = ufs_controller->get_geometry();

    _varname = string("pressfc");

    ufs_controller->set2dvarname(_varname);

    _minval = ufs_controller->get_minval();
    _maxval = ufs_controller->get_maxval();

    _title   = ufs_controller->get_title();
    _timestr[0] = ufs_controller->get_timestring();
    _maxFile = ufs_controller->get_nfiles();

  //_nTimes  = ufs_controller->get_ntimes();
    _maxTime = 1;
  //for(n = 0; n < _maxFile; ++n)
  //    _maxTime += _nTimes[n];

    if(_maxTime > 12)
        _maxTime = 12;

    cout << "\t_maxFile = " << _maxFile << endl;
    cout << "\t_maxTime = " << _maxTime << endl;
    cout << "\tLeave Funciton: " << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
}

/*******************************************************************/
/*************************  OpenGL Events  *************************/
/*******************************************************************/
//Show the image
void UFSTranslator::show()
{
    glShadeModel(GL_SMOOTH);

    if(nvoptions->get_tsec() != _glbTime)
    {
        _glbTime = nvoptions->get_tsec();

        _set_current_time();

        ufs_controller->set_fileNtime(_curFile, _curTime);
    }

    ufs_controller->draw();

  //_varname = ufs_controller->get_varname();

    setLabelColor();

    make_timeNpositionString();
    writeHeader();

    writeVarInfo();

    writeFrameInfo();

    if(locator->on())
        writeLocatorMsg();
}

void UFSTranslator::createVarInfo()
{
    _varinfo = "Dim nlon="+QString::number(geometry->get_nlon())
             + ", nlat=" + QString::number(geometry->get_nlat())
             + ", nlev=" + QString::number(geometry->get_nlev())
             + "\nVar min=" + QString::number(ufs_controller->get_minval())
             + ", max=" + QString::number(ufs_controller->get_maxval());
}

void UFSTranslator::writeVarInfo()
{
    createVarInfo();
    emit info(_varinfo);
}

void UFSTranslator::writeFrameInfo()
{
    if(nvoptions->get_cb(NV_ANIMATIONON))
    {
        _frameinfo = "Frame No. "+QString::number(_glbTime);
        emit frame(_frameinfo);
    }
}

void UFSTranslator::make_timeNpositionString()
{
    int framenumb = 0;

    framenumb = ufs_controller->get_curTime();
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
void UFSTranslator::SaveJpg(int n)
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
void UFSTranslator::select0dVar(const QString& str)
{
   cout << "0d var: <" << str.toStdString() << "> is selected." << endl;
}

void UFSTranslator::select1dVar(const QString& str)
{
   cout << "0d var: <" << str.toStdString() << "> is selected." << endl;
}

void UFSTranslator::select2dVar(const QString& str)
{
  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "1d var: <" << str.toStdString() << "> is selected." << endl;
 
    _varname = str.toStdString();

    ufs_controller->set1dvarname(_varname);

    sliderNspinX->set_max(360);
    sliderNspinX->setValue(0);
    sliderNspinY->set_max(90);
    sliderNspinY->set_min(-90);
    sliderNspinY->setValue(0);
    sliderNspinZ->set_max(1);
    sliderNspinZ->setValue(0);
    sliderNspinT->set_max(geometry->get_nt());
    sliderNspinT->setValue(0);

    _minval = ufs_controller->get_minval();
    _maxval = ufs_controller->get_maxval();
    _title  = ufs_controller->get_title();

    _jpgNotSaved = true;
    _startSave = false;

    updateGL();
}

void UFSTranslator::select3dVar(const QString& str)
{
  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "2d var: <" << str.toStdString() << "> is selected." << endl;
 
    _varname = str.toStdString();

    ufs_controller->set3dvarname(_varname);

    sliderNspinX->set_max(360);
    sliderNspinX->setValue(0);
    sliderNspinY->set_max(90);
    sliderNspinY->set_min(-90);
    sliderNspinZ->set_max(geometry->get_nlev());
    sliderNspinZ->setValue(0);
  //sliderNspinT->set_max(_ntim);
    sliderNspinT->set_max(1);
    sliderNspinT->setValue(0);

    _minval = ufs_controller->get_minval();
    _maxval = ufs_controller->get_maxval();
    _title  = ufs_controller->get_title();
 
    _jpgNotSaved = true;
    _startSave = false;

    updateGL();
}

void UFSTranslator::selectColorMap(const QString& str)
{
  //cout << "\nfile: <" << __FILE__ << ">, function: <" << __PRETTY_FUNCTION__
  //     << ">, line: " << __LINE__ << endl;
  //cout << "ColorMap: <" << str.toStdString() << "> is selected." << endl;

    colorTable->set_colorMap(str.toStdString());
    ufs_controller->set_colorTable(colorTable);

    updateGL();
}

int UFSTranslator::get_ndv(int n)
{
    return ufs_controller->get_ndv(n);
}

string* UFSTranslator::get_ndvNames(int n)
{
    vector<string> vecnames = ufs_controller->get_ndvNames(n);
    int numbvars = get_ndv(n);
    string* varnames = new string[numbvars];
    cout << "n = " << n << endl;
    cout << "numbvars = " << numbvars << endl;
    cout << "vecnames.size() = " << vecnames.size() << endl;
    for (int i=0; i<numbvars; ++i) {
        cout << "vecnames[" << i << "] = " << vecnames[i] << endl;
	varnames[i] = vecnames[i];
        cout << "varnames[" << i << "] = " << varnames[i] << endl;
    }
    return varnames;
}

void UFSTranslator::update_frame()
{
    ++_glbTime;
    if(_glbTime >= _maxTime)
       _glbTime = 0;

    _set_current_time();
    ufs_controller->set_fileNtime(_curFile, _curTime);
    _timestr[0] = ufs_controller->get_timestring();

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

void UFSTranslator::nextFrame()
{
    ++_glbTime;
    if(_glbTime >= _maxTime)
       _glbTime = 0;

    _set_current_time();
    ufs_controller->set_fileNtime(_curFile, _curTime);
    _timestr[0] = ufs_controller->get_timestring();

    updateGL();
}

void UFSTranslator::backFrame()
{
    --_glbTime;
    if(_glbTime < 0)
       _glbTime += _maxTime;

    _set_current_time();
    ufs_controller->set_fileNtime(_curFile, _curTime);
    _timestr[0] = ufs_controller->get_timestring();

    updateGL();
}


void UFSTranslator::set_light(Light* l)
{
    light = l;
  //ufs_controller->set_light(l);
}

void UFSTranslator::set_locator(Locator* l)
{
    locator = l;

    locator->set_dim(dim);
    locator->set_fovy(fovy);
    locator->set_zfar(zFar);
    locator->set_znear(zNear);

    ufs_controller->set_locator(l);
}

void UFSTranslator::writeLocatorMsg()
{
     _locatorinfo = "Location lon="+QString::number(locator->x())
               + ", lat=" + QString::number(locator->y());

    emit locator_msg(_locatorinfo);
}

