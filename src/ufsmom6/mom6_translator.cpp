//$Id

#include <QtOpenGL>

#include "mom6_translator.h"
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
MOM6Translator::MOM6Translator(ColorTable* ct, NVOptions* opt,
                             string flnm, bool isList, string mfnm, QWidget* parent)
               : BaseTranslator(ct, opt, parent)
{
  //cout << "\nEnter Funciton: " << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;

    _filename = flnm;
    _hasFileList = isList;

    nvoptions->set_xsec(-1);
    nvoptions->set_ysec(-1);
    nvoptions->set_zsec(0);
    nvoptions->set_tsec(0);

    mom6_controller = NULL;

    _timestr = new string[2];
    _timestr = "  ";

  //cout << "Leave Funciton: " << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
}

MOM6Translator::~MOM6Translator()
{
    if(NULL != mom6_controller)
        delete mom6_controller;
    if(NULL != _timestr)
        delete[] _timestr;
    mom6_controller = NULL;
}

void MOM6Translator::setup()
{
    int n;

    cout << "\nEnter Funciton: " << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
    if(NULL != mom6_controller)
        delete mom6_controller;

    cout << "\tFunciton: " << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
    mom6_controller = new MOM6Controller(colorTable, nvoptions, _filename.c_str());

    cout << "\tFunciton: " << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
    mom6_controller->setup();

    _jpgNotSaved = true;
    _startSave = false;

    makeCurrent();

    cout << "\tFunciton: " << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
    geometry = mom6_controller->get_geometry();

    _varname = string("SST");

    mom6_controller->set2dvarname(_varname);

    _minval = mom6_controller->get_minval();
    _maxval = mom6_controller->get_maxval();

    _title   = mom6_controller->get_title();
    // _timestr[0] = mom6_controller->get_timestring();
    _timestr[0] = "  ";
    // _maxFile = mom6_controller->get_nfiles();
    _maxFile = 1

  //_nTimes  = mom6_controller->get_ntimes();
    _maxTime = 1;
  //for(n = 0; n < _maxFile; ++n)
  //    _maxTime += _nTimes[n];

    if(_maxTime > 12)
        _maxTime = 12;

    cout << "Leave Funciton: " << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
}

/*******************************************************************/
/*************************  OpenGL Events  *************************/
/*******************************************************************/
//Show the image
void MOM6Translator::show()
{
    // cout << "\nEnter Funciton: " << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //glShadeModel(GL_SMOOTH);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if(nvoptions->get_tsec() != _glbTime)
    {
        _glbTime = nvoptions->get_tsec();

        _set_current_time();

        mom6_controller->set_fileNtime(_curFile, _curTime);
    }

  //cout << "\t" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
    mom6_controller->draw();

    _varname = mom6_controller->get_varname();

  //cout << "\t_varname : " << _varname << endl;
  //cout << "\t" << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
    setLabelColor();

    make_timeNpositionString();
    writeHeader();

    writeVarInfo();

    writeFrameInfo();

    if(locator->on())
        writeLocatorMsg();
    // cout << "Leave Funciton: " << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
}

void MOM6Translator::createVarInfo()
{
    _varinfo = QString(_varname.c_str()) + "\n"
             + QString(_timeinfo.c_str()) + "\n"
             + "Dim nx="+QString::number(geometry->get_nlon())
             + ", nlat=" + QString::number(geometry->get_nlat())
             + ", nlev=" + QString::number(geometry->get_nlev())
             + "\nVar min=" + QString::number(mom6_controller->get_minval())
             + ", max=" + QString::number(mom6_controller->get_maxval());
}

void MOM6Translator::writeVarInfo()
{
    createVarInfo();
    emit info(_varinfo);
}

void MOM6Translator::writeFrameInfo()
{
    if(nvoptions->get_cb(NV_ANIMATIONON))
    {
        _frameinfo = "Frame No. "+QString::number(_glbTime);
        emit frame(_frameinfo);
    }
}

void MOM6Translator::make_timeNpositionString()
{
    int framenumb = 0;

    framenumb = mom6_controller->get_curTime();
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
void MOM6Translator::SaveJpg(int n)
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
void MOM6Translator::select0dVar(const QString& str)
{
   cout << "0d var: <" << str.toStdString() << "> is selected." << endl;
}

void MOM6Translator::select1dVar(const QString& str)
{
   cout << "0d var: <" << str.toStdString() << "> is selected." << endl;
}

void MOM6Translator::select2dVar(const QString& str)
{
    // cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
    // cout << "1d var: <" << str.toStdString() << "> is selected." << endl;
 
    _varname = str.toStdString();

    mom6_controller->set1dvarname(_varname);

    sliderNspinX->set_max(geometry->get_nxh());
    sliderNspinX->setValue(0);
    sliderNspinY->set_max(geometry->get_nyh());
    sliderNspinY->set_min(0);
    sliderNspinY->setValue(0);
    sliderNspinZ->set_max(1);
    sliderNspinZ->setValue(0);
    sliderNspinT->set_max(geometry->get_ntime());
    sliderNspinT->setValue(0);

    _minval = mom6_controller->get_minval();
    _maxval = mom6_controller->get_maxval();
    _title  = mom6_controller->get_title();

    _jpgNotSaved = true;
    _startSave = false;

    updateGL();
}

void MOM6Translator::select3dVar(const QString& str)
{
  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "2d var: <" << str.toStdString() << "> is selected." << endl;
 
    _varname = str.toStdString();

    mom6_controller->set3dvarname(_varname);

    sliderNspinX->set_max(geometry->get_nxh());
    sliderNspinX->setValue(0);
    sliderNspinY->set_max(geometry->get_nyh());
    sliderNspinY->set_min(0);
    sliderNspinZ->set_max(geometry->get_nzl());
    sliderNspinZ->setValue(0);
  //sliderNspinT->set_max(_ntim);
    sliderNspinT->set_max(1);
    sliderNspinT->setValue(0);

    _minval = mom6_controller->get_minval();
    _maxval = mom6_controller->get_maxval();
    _title  = mom6_controller->get_title();
 
    _jpgNotSaved = true;
    _startSave = false;

    updateGL();
}

void MOM6Translator::selectColorMap(const QString& str)
{
  //cout << "\nfile: <" << __FILE__ << ">, function: <" << __PRETTY_FUNCTION__
  //     << ">, line: " << __LINE__ << endl;
  //cout << "ColorMap: <" << str.toStdString() << "> is selected." << endl;

    colorTable->set_colorMap(str.toStdString());
    mom6_controller->set_colorTable(colorTable);

    updateGL();
}

int MOM6Translator::get_ndv(int n)
{
    return mom6_controller->get_ndv(n);
}

string* MOM6Translator::get_ndvNames(int n)
{
    vector<string> vecnames = mom6_controller->get_ndvNames(n);
    int numbvars = get_ndv(n);
    string* varnames = new string[numbvars];
  //cout << "n = " << n << endl;
  //cout << "numbvars = " << numbvars << endl;
  //cout << "vecnames.size() = " << vecnames.size() << endl;
    for (int i=0; i<numbvars; ++i) {
      //cout << "vecnames[" << i << "] = " << vecnames[i] << endl;
	varnames[i] = vecnames[i];
      //cout << "varnames[" << i << "] = " << varnames[i] << endl;
    }
    return varnames;
}

void MOM6Translator::update_frame()
{
    ++_glbTime;
    if(_glbTime >= _maxTime)
       _glbTime = 0;

    _set_current_time();
    mom6_controller->set_fileNtime(_curFile, _curTime);
    _timestr[0] = mom6_controller->get_timestring();

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

void MOM6Translator::nextFrame()
{
    ++_glbTime;
    if(_glbTime >= _maxTime)
       _glbTime = 0;

    _set_current_time();
    mom6_controller->set_fileNtime(_curFile, _curTime);
    _timestr[0] = mom6_controller->get_timestring();

    updateGL();
}

void MOM6Translator::backFrame()
{
    --_glbTime;
    if(_glbTime < 0)
       _glbTime += _maxTime;

    _set_current_time();
    mom6_controller->set_fileNtime(_curFile, _curTime);
    _timestr[0] = mom6_controller->get_timestring();

    updateGL();
}


void MOM6Translator::set_light(Light* l)
{
    light = l;
  //mom6_controller->set_light(l);
}

void MOM6Translator::set_locator(Locator* l)
{
    locator = l;

    locator->set_dim(dim);
    locator->set_fovy(fovy);
    locator->set_zfar(zFar);
    locator->set_znear(zNear);

    mom6_controller->set_locator(l);
}

void MOM6Translator::writeLocatorMsg()
{
     _locatorinfo = "Location lon="+QString::number(locator->x())
               + ", lat=" + QString::number(locator->y());

    emit locator_msg(_locatorinfo);
}

void MOM6Translator::paintGL()
{
    // cout << "\nEnter Funciton: " << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
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

    drawColorBar();

    // cout << "line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;

    // if(! nvoptions->get_cb(NV_PIXELON))
    //     drawAxis();

    if(nvoptions->get_cb(NV_STATUS_CHANGED))
        save_status();

  //Done
    glFlush();
    // cout << "Leave Funciton: " << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
}
