#include <QtOpenGL>
#include "eaglelam_translator.h"
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
EAGLELAM_Translator::EAGLELAM_Translator(ColorTable *ct, NVOptions* opt,
                                 vector<string> datafiles, QWidget* parent)
             : BaseTranslator(ct, opt, parent)
{
    // cout << "\tEnter function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;

    _datafiles = datafiles;

    nvoptions->set_xsec(-1);
    nvoptions->set_ysec(-1);
    nvoptions->set_zsec(0);
    nvoptions->set_tsec(0);

  //dim = 0.25;

  //cout << "\tfunction: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
  //cout << "\t_filename: <" << _filename << ">, _hasFileList = " << _hasFileList << endl;
  //cout << "\t_filename.c_str(): <" << _filename.c_str() << ">, _hasFileList = " << _hasFileList << endl;

    eaglelam_controller = new EAGLELAM_Controller(colorTable, nvoptions, _datafiles);
    eaglelam_controller->setup();

    // cout << "\tLeave function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
}

EAGLELAM_Translator::~EAGLELAM_Translator()
{
  //cout << "\tEnter function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
    delete eaglelam_controller;
  //cout << "\tLeave function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
}

void EAGLELAM_Translator::setup()
{
    int n;

    // cout << "\tEnter function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;

    geometry = eaglelam_controller->get_geometry();

    // cout << "\tfunction: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
    _varname = eaglelam_controller->get_varname();

    // cout << "\tfunction: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
    // cout << "\t_varname: <" << _varname << ">" << endl;

    _title = "EAGLE LAM";
    _timestr = eaglelam_controller->get_timestring();
    _maxFile = 1;
    _maxTime = 1;
    // _nTimes  = eaglelam_controller->get_ntimes();
    _maxval  = eaglelam_controller->get_valmax();
    _minval  = eaglelam_controller->get_valmin();

    // cout << "\tfunction: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
    // cout << "\t_maxval: " << _maxval << ", _minval" << _minval << endl;

    // cout << "\tLeave function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
}

//Draw the window
void EAGLELAM_Translator::show()
{
    // cout << "\nEnter function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
    // cout << "\tnvoptions->get_zsec() = " << nvoptions->get_zsec() << endl;

    eaglelam_controller->draw();

    setLabelColor();

    make_timeNpositionString();
    writeHeader();

    writeVarInfo();

    writeFrameInfo();

    if(locator->on())
        writeLocatorMsg();

    // cout << "Leave function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl << endl;
}

void EAGLELAM_Translator::createVarInfo()
{
    _varinfo = "Dim nx="+QString::number(eaglelam_controller->get_nx())
             + ", ny=" + QString::number(eaglelam_controller->get_ny())
             + "\nVar min=" + QString::number(eaglelam_controller->get_valmin())
             + ", max=" + QString::number(eaglelam_controller->get_valmax());
}

void EAGLELAM_Translator::writeVarInfo()
{
    createVarInfo();
    emit info(_varinfo);
}


void EAGLELAM_Translator::writeFrameInfo()
{
    if(nvoptions->get_cb(NV_ANIMATIONON))
    {
        _frameinfo = "Frame No. "+QString::number(_glbTime);
        emit frame(_frameinfo);
    }
}

void EAGLELAM_Translator::make_timeNpositionString()
{
    _timeinfo = "Time: 0";

    _position = "Position: ";
#if 0
    string xstr = number2string<int>(nvoptions->get_xsec());
    string ystr = number2string<int>(nvoptions->get_ysec());
    string zstr = number2string<int>(nvoptions->get_zsec());

    if(-1 < nvoptions->get_xsec())
        _position += "Xgrid: " + xstr;
    if(-1 < nvoptions->get_ysec())
        _position += ", Ygrid: " + ystr;
    if(0 < nvoptions->get_zsec())
        _position += ", Level: " + zstr;
#endif
}

void EAGLELAM_Translator::selectVar(const QString& vn)
{
  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "2d var: <" << str.toStdString() << "> is selected." << endl;

    _glbTime = 0;

    _varname = vn.toStdString();

    eaglelam_controller->setvarname(_varname);
    _title = "EAGLE LAM";
    _timestr = eaglelam_controller->get_timestring();
    _maxval  = eaglelam_controller->get_valmax();
    _minval  = eaglelam_controller->get_valmin();

    update_sliderNspin();

    updateGL();
}

void EAGLELAM_Translator::update_sliderNspin()
{
    int intvl = 0;
    int stepsize = 1;

    sliderNspinX->set_max(eaglelam_controller->get_nx());
    sliderNspinX->setValue(eaglelam_controller->get_nx());
    intvl = (eaglelam_controller->get_nx()+9)/10;
    sliderNspinX->set_tickinterval(intvl);
    stepsize = (eaglelam_controller->get_nx() + 350)/360;
    sliderNspinX->set_step(stepsize);

    sliderNspinY->set_max(eaglelam_controller->get_ny());
    sliderNspinY->setValue(eaglelam_controller->get_ny());
    intvl = (eaglelam_controller->get_ny()+9)/10;
    sliderNspinY->set_tickinterval(intvl);
    stepsize = (eaglelam_controller->get_ny() + 170)/180;
    sliderNspinY->set_step(stepsize);

    sliderNspinZ->set_max(1);
    sliderNspinZ->set_step(1);
    sliderNspinZ->setValue(0);

  //sliderNspinT->set_max(get_nt());
    sliderNspinT->set_max(_maxTime);
    sliderNspinT->set_step(1);
    sliderNspinT->setValue(0);

  //cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
  //cout << "\tvariable name: <" << _varname << ">" << endl;
  //cout << "\ttitle: <" << _title << ">" << endl;
  //cout << "\teaglelam_controller->get_nx() = " << eaglelam_controller->get_nx() << endl;
  //cout << "\teaglelam_controller->get_ny() = " << eaglelam_controller->get_ny() << endl;
}

void EAGLELAM_Translator::selectColorMap(const QString& str)
{
  //cout << "\nfile: <" << __FILE__ << ">, function: <" << __PRETTY_FUNCTION__
  //     << ">, line: " << __LINE__ << endl;
  //cout << "ColorMap: <" << str.toStdString() << "> is selected." << endl;

    nvoptions->set_cb(NV_UPDATECOLORTABLE, true);

    colorTable->set_colorMap(str.toStdString());

    eaglelam_controller->set_colorTable(colorTable);
    eaglelam_controller->update_colormap();

    updateGL();
}

void EAGLELAM_Translator::update_frame()
{
    ++_glbTime;
    if(_glbTime >= _maxTime)
       _glbTime = 0;

    _set_current_time();
    eaglelam_controller->set_fileNtime(_curFile, _curTime);
    _timestr = eaglelam_controller->get_timestring();

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

                 SaveJpg(0);

               //cout << "\tAfter ave jpeg for time: " << _glbTime << endl;
             }
            _startSave = false;
         }

        if(nvoptions->get_cb(NV_SAVEJPG))
            _startSave = true;

      //cout << "\tFunction: " << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
      //cout << "\t_curTime = " << _curTime << ", _glbTime = " << _glbTime << ", _maxTime = " << _maxTime << endl;

        nvoptions->set_tsec(0);
        _timestr = eaglelam_controller->get_timestring();

      //cout << "Leave Function: " << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
    }

    updateGL();
}

void EAGLELAM_Translator::nextFrame()
{
    ++_glbTime;
    if(_glbTime >= _maxTime)
       _glbTime = 0;

    _set_current_time();
    eaglelam_controller->set_fileNtime(_curFile, _curTime);
    _timestr = eaglelam_controller->get_timestring();

    _curTime = _glbTime;
    nvoptions->set_tsec(_curTime);

    updateGL();
}

void EAGLELAM_Translator::backFrame()
{
    --_glbTime;
    if(_glbTime < 0)
       _glbTime += _maxTime;

    _set_current_time();
    eaglelam_controller->set_fileNtime(_curFile, _curTime);
    _timestr = eaglelam_controller->get_timestring();

    _curTime = _glbTime;
    nvoptions->set_tsec(_curTime);

    updateGL();
}

void EAGLELAM_Translator::set_locator(Locator* l)
{
    locator = l;

    locator->set_dim(dim);
    locator->set_fovy(fovy);
    locator->set_zfar(zFar);
    locator->set_znear(zNear);

    eaglelam_controller->set_locator(l);
}

void EAGLELAM_Translator::writeLocatorMsg()
{
     _locatorinfo = "Location lon="+QString::number(locator->x())
               + ", lat=" + QString::number(locator->y());

    emit locator_msg(_locatorinfo);
}

void EAGLELAM_Translator::set_light(Light* l)
{
    light = l;
  //eaglelam_controller->set_light(l);
}

int EAGLELAM_Translator::get_ndv(int n)
{
    vector<string> vecnames = eaglelam_controller->get_varlist();
    return vecnames.size();
}

string* EAGLELAM_Translator::get_ndvNames(int n)
{
    vector<string> vecnames = eaglelam_controller->get_varlist();
    string* varnames = new string[vecnames.size()];
    for (int i=0; i<vecnames.size(); ++i) {
        varnames[i] = vecnames[i];
    }
    return varnames;
}

