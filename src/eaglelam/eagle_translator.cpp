//$Id: eagletranslator.cpp 4947 2014-02-04 16:09:40Z starviewer $

#include <QtOpenGL>
#include "eagletranslator.h"
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
EAGLETranslator::EAGLETranslator(ColorTable *ct, NVOptions* opt, string flnm,
                             bool isFileList, QWidget* parent)
             : GeneralTranslator(ct, opt, flnm, isFileList, parent)
{
  //cout << "\tEnter function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;

    _varname = "HGT";
  //dim = 0.25;

    _preTime = 0;
    _filename = flnm;
    _hasFileList = isFileList;

    _first_time = true;
    _displaying2D = false;
    _displaying3D = false;

  //cout << "\tfunction: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
  //cout << "\t_filename: <" << _filename << ">, _hasFileList = " << _hasFileList << endl;
  //cout << "\t_filename.c_str(): <" << _filename.c_str() << ">, _hasFileList = " << _hasFileList << endl;

    eagle_controller = new EAGLE_Controller(colorTable, nvoptions,
                                        _filename.c_str(), _hasFileList);

  //cout << "\tLeave function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
}

EAGLETranslator::~EAGLETranslator()
{
  //cout << "\tEnter function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
    delete eagle_controller;
  //cout << "\tLeave function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
}

void EAGLETranslator::setup()
{
    int n;

  //cout << "\tEnter function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;

  //cout << "\t_filename: <" << _filename << ">, _hasFileList = " << _hasFileList << endl;

    eagle_controller->setup();

  //cout << "\tfunction: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;

    geometry = eagle_controller->get_geometry();

  //cout << "\tfunction: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;

  //_varname = eagle_controller->get_varname();
    _title = eagle_controller->get_title();
    _timestr = eagle_controller->get_timestring();
    _maxFile = eagle_controller->get_nfils();
    _nTimes  = eagle_controller->get_ntimes();
    _maxval  = eagle_controller->get_maxval();
    _minval  = eagle_controller->get_minval();

  //cout << "\tfunction: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;

    _preTime = 0;
    _curTime = 0;
    _glbTime = 0;
    _maxTime = 0;
    if(NULL != _nTimes)
    {
        for(n = 0; n < _maxFile; ++n)
            _maxTime += _nTimes[n];
    }
    else
        _maxTime = 1;

    _isDummy = false;

    _varname = "HGT";

  //cout << "\t_maxFile = " << _maxFile << ", _maxTime = " << _maxTime << endl;
  //cout << "\tLeave function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
}

//Draw the window
void EAGLETranslator::show()
{
  //cout << "\n\tEnter function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
  //cout << "\t_glbTime = " << _glbTime << ", _curFile = " << _curFile << endl;
  //cout << "\t_preTime = " << _preTime << ", nvoptions->get_tsec() = " << nvoptions->get_tsec() << endl;

  //cout << "\tnvoptions->get_xsec() = " << nvoptions->get_xsec() << endl;
  //cout << "\tnvoptions->get_ysec() = " << nvoptions->get_ysec() << endl;
  //cout << "\tnvoptions->get_zsec() = " << nvoptions->get_zsec() << endl;

  //if(nvoptions->get_tsec() != _glbTime)
    if(_preTime != _glbTime)
    {
        _glbTime = nvoptions->get_tsec();

        _set_current_time();

        eagle_controller->set_fileNtime(_curFile, _curTime);

        _preTime = _glbTime;
    }

  //cout << "\tFunction: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
  //cout << "\t_glbTime = " << _glbTime << ", _curFile = " << _curFile << endl;
  //cout << "\t_preTime = " << _preTime << ", nvoptions->get_tsec() = " << nvoptions->get_tsec() << endl;

  //cout << "\tnvoptions->get_xsec() = " << nvoptions->get_xsec() << endl;
  //cout << "\tnvoptions->get_ysec() = " << nvoptions->get_ysec() << endl;
  //cout << "\tnvoptions->get_zsec() = " << nvoptions->get_zsec() << endl;

    eagle_controller->draw();

    setLabelColor();

    make_timeNpositionString();
    writeHeader();

    writeVarInfo();

    writeFrameInfo();

    if(locator->on())
        writeLocatorMsg();

  //cout << "\tLeave function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl << endl;
}

void EAGLETranslator::createVarInfo()
{
    _varinfo = "Dim nx="+QString::number(eagle_controller->get_nx())
             + ", ny=" + QString::number(eagle_controller->get_ny())
             + ", nz=" + QString::number(eagle_controller->get_nz())
             + "\nVar min=" + QString::number(eagle_controller->get_minval())
             + ", max=" + QString::number(eagle_controller->get_maxval());
}

void EAGLETranslator::writeVarInfo()
{
    createVarInfo();
    emit info(_varinfo);
}


void EAGLETranslator::writeFrameInfo()
{
    if(nvoptions->get_cb(NV_ANIMATIONON))
    {
        _frameinfo = "Frame No. "+QString::number(_glbTime);
        emit frame(_frameinfo);
    }
}

void EAGLETranslator::make_timeNpositionString()
{
    int framenumb = 0;

    framenumb = eagle_controller->get_curTime();
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
        _position += "Xgrid: " + xstr;
    if(-1 < nvoptions->get_ysec())
        _position += ", Ygrid: " + ystr;
    if(0 < nvoptions->get_zsec())
        _position += ", Level: " + zstr;
}

void EAGLETranslator::select2dVar(const QString& str)
{
  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "2d var: <" << str.toStdString() << "> is selected." << endl;

    _glbTime = 0;

    _varname = str.toStdString();

    if(0 == _varname.compare("dummy"))
        _isDummy = true;
    else
    {
        _isDummy = false;

        eagle_controller->set2dvarname(_varname);
        _title = eagle_controller->get_title();
        _timestr = eagle_controller->get_timestring();
        _maxval  = eagle_controller->get_maxval();
        _minval  = eagle_controller->get_minval();

        if((_displaying3D) || (! _displaying2D))
        {
            _first_time = true;
            update_sliderNspin();
            _displaying2D = true;
            _displaying3D = false;
        }
    }

    updateGL();
}

void EAGLETranslator::select3dVar(const QString& str)
{
  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "3d var: <" << str.toStdString() << "> is selected." << endl;

    _glbTime = 0;

    _varname = str.toStdString();

    if(0 == _varname.compare("dummy"))
        _isDummy = true;
    else
    {
        _isDummy = false;

        eagle_controller->set3dvarname(_varname);
        _title = eagle_controller->get_title();
        _timestr = eagle_controller->get_timestring();
        _maxval  = eagle_controller->get_maxval();
        _minval  = eagle_controller->get_minval();

        if((_displaying2D) || (! _displaying3D))
        {
            _first_time = true;
            update_sliderNspin();
            _displaying2D = false;
            _displaying3D = true;
        }
    }

    updateGL();
}

void EAGLETranslator::update_sliderNspin()
{
    int intvl = 0;
    int stepsize = 1;
    if(! _first_time)
        return;

    _first_time = false;

    sliderNspinX->set_max(eagle_controller->get_nx());
    sliderNspinX->setValue(eagle_controller->get_nx());
    intvl = (eagle_controller->get_nx()+9)/10;
    sliderNspinX->set_tickinterval(intvl);
    stepsize = (eagle_controller->get_nx() + 350)/360;
    sliderNspinX->set_step(stepsize);

    sliderNspinY->set_max(eagle_controller->get_ny());
    sliderNspinY->setValue(eagle_controller->get_ny());
    intvl = (eagle_controller->get_ny()+9)/10;
    sliderNspinY->set_tickinterval(intvl);
    stepsize = (eagle_controller->get_ny() + 170)/180;
    sliderNspinY->set_step(stepsize);

    sliderNspinZ->set_max(eagle_controller->get_nz());
    sliderNspinZ->set_step(1);
    sliderNspinZ->setValue(0);

  //sliderNspinT->set_max(get_nt());
    sliderNspinT->set_max(_maxTime);
    sliderNspinT->set_step(1);
    sliderNspinT->setValue(0);

  //cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
  //cout << "\tvariable name: <" << _varname << ">" << endl;
  //cout << "\ttitle: <" << _title << ">" << endl;
  //cout << "\teagle_controller->get_nx() = " << eagle_controller->get_nx() << endl;
  //cout << "\teagle_controller->get_ny() = " << eagle_controller->get_ny() << endl;
  //cout << "\teagle_controller->get_nz() = " << eagle_controller->get_nz() << endl;
}

void EAGLETranslator::selectColorMap(const QString& str)
{
  //cout << "\nfile: <" << __FILE__ << ">, function: <" << __PRETTY_FUNCTION__
  //     << ">, line: " << __LINE__ << endl;
  //cout << "ColorMap: <" << str.toStdString() << "> is selected." << endl;

    nvoptions->set_cb(NV_UPDATECOLORTABLE, true);

    colorTable->set_colorMap(str.toStdString());

    eagle_controller->set_colorTable(colorTable);
    eagle_controller->update_colormap();

    updateGL();
}

void EAGLETranslator::update_frame()
{
    ++_glbTime;
    if(_glbTime >= _maxTime)
       _glbTime = 0;

    _set_current_time();
    eagle_controller->set_fileNtime(_curFile, _curTime);
    _timestr = eagle_controller->get_timestring();

    if(nvoptions->get_cb(NV_ANIMATIONON))
    {
      //cout << "\nEnter Function: " << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;

      //cout << "\tAnimate time: " << _glbTime << ", _startSave = " << _startSave << ", _jpgNotSaved = " << _jpgNotSaved <<endl;
      //cout << "\tnvoptions->get_xsec() = " << nvoptions->get_xsec() << endl;
      //cout << "\tnvoptions->get_ysec() = " << nvoptions->get_ysec() << endl;
      //cout << "\tnvoptions->get_zsec() = " << nvoptions->get_zsec() << endl;

      //nvoptions->set_xsec(nvoptions->get_xsec());
      //nvoptions->set_ysec(nvoptions->get_ysec());
      //nvoptions->set_zsec(nvoptions->get_zsec());

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

        _curTime = _glbTime;
        nvoptions->set_tsec(_curTime);
        _timestr = eagle_controller->get_timestring();

      //cout << "Leave Function: " << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
    }

    updateGL();
}

void EAGLETranslator::nextFrame()
{
    ++_glbTime;
    if(_glbTime >= _maxTime)
       _glbTime = 0;

    _set_current_time();
    eagle_controller->set_fileNtime(_curFile, _curTime);
    _timestr = eagle_controller->get_timestring();

    _curTime = _glbTime;
    nvoptions->set_tsec(_curTime);

    updateGL();
}

void EAGLETranslator::backFrame()
{
    --_glbTime;
    if(_glbTime < 0)
       _glbTime += _maxTime;

    _set_current_time();
    eagle_controller->set_fileNtime(_curFile, _curTime);
    _timestr = eagle_controller->get_timestring();

    _curTime = _glbTime;
    nvoptions->set_tsec(_curTime);

    updateGL();
}

void EAGLETranslator::set_locator(Locator* l)
{
    locator = l;

    locator->set_dim(dim);
    locator->set_fovy(fovy);
    locator->set_zfar(zFar);
    locator->set_znear(zNear);

    eagle_controller->set_locator(l);
}

void EAGLETranslator::writeLocatorMsg()
{
     _locatorinfo = "Location lon="+QString::number(locator->x())
               + ", lat=" + QString::number(locator->y());

    emit locator_msg(_locatorinfo);
}

void EAGLETranslator::set_light(Light* l)
{
    light = l;
  //eagle_controller->set_light(l);
}

int EAGLETranslator::get_ndv(int n)
{
    return eagle_controller->get_ndv(n);
}

string* EAGLETranslator::get_ndvNames(int n)
{
    string* varnames = eagle_controller->get_ndvNames(n);
    return varnames;
}

