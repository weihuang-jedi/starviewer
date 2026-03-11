//$Id: generaltranslator.cpp 4927 2014-01-23 21:49:37Z starviewer $

#include <QtOpenGL>
#include "generaltranslator.h"

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
GeneralTranslator::GeneralTranslator(ColorTable* ct, NVOptions* opt,
                                     string flnm, bool isFileList, QWidget* parent)
                 : BaseTranslator(ct, opt, parent)
{
  //cout << "\tEnter function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;

    _filename = flnm;

    _hasFileList = isFileList;

  //cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__
  //     << ", file: <" << __FILE__ << ">" << endl;
  //cout << "\tOpen file: <" << flnm << ">" << endl;

    controller = NULL;

    _first_time = true;

  //cout << "\tLeave function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
}

GeneralTranslator::~GeneralTranslator()
{
    if(NULL != controller)
        delete controller;
}

void GeneralTranslator::setup()
{
    int n;

    cout << "\tEnter function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;

    controller = new Controller(colorTable, nvoptions, _filename, _hasFileList);
    controller->setup();

    cout << "\tfunction: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;

    _title = controller->get_title();
    _timestr = controller->get_timestring();
    _maxFile = controller->get_nfiles();
    _nTimes  = controller->get_ntimes();

    _maxTime = 0;
    for(n = 0; n < _maxFile; ++n)
        _maxTime += _nTimes[n];

    cout << "\tfunction: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;

    geometry = controller->get_geometry();

    _varname = controller->get_varname();

    cout << "\tfunction: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;

    _isDummy = false;

    _minval = controller->get_minval();
    _maxval = controller->get_maxval();

    if(_first_time)
    {
        nvoptions->set_xsec(-1); 
        nvoptions->set_ysec(-1); 
        nvoptions->set_zsec(0); 

        _first_time = false;
    }

    cout << "\tLeave function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
}

/*******************************************************************/
/*************************  OpenGL Events  *************************/
/*******************************************************************/
//
//  Draw the window
//
void GeneralTranslator::show()
{
    controller->draw();

    if(nvoptions->get_cb(NV_PIXELON))
    {
        writeMouseMsg();
    }
    else
    {
        setLabelColor();

        if(nvoptions->get_cb(NV_TITLEON))
        {
            make_timeNpositionString();
            writeHeader();
        }

        createVarInfo();
        writeVarInfo();
    }
}

void GeneralTranslator::createVarInfo()
{
    _varinfo = "Dim nx="+QString::number(geometry->get_nx())
             + ", ny=" + QString::number(geometry->get_ny())
             + ", nz=" + QString::number(geometry->get_nz())
             + "\nVar min=" + QString::number(controller->get_minval())
             + ", max=" + QString::number(controller->get_maxval());
}

void GeneralTranslator::writeVarInfo()
{
    emit info(_varinfo);
}

void GeneralTranslator::make_timeNpositionString()
{
    int framenumb = 0;

    framenumb = controller->get_curTime();
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
    if(-1 < nvoptions->get_zsec())
        _position += ", Level: " + zstr;
}

void GeneralTranslator::select1dVar(const QString& str)
{
    cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
    cout << "1d var: try to select <" << str.toStdString() << ">." << endl;
}

void GeneralTranslator::select2dVar(const QString& str)
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

        controller->set2dvarname(_varname);
        _title = controller->get_title();
        _timestr = controller->get_timestring();
        _minval = controller->get_minval();
        _maxval = controller->get_maxval();

        update_sliderNspin();
    }

    updateGL();
}

void GeneralTranslator::select3dVar(const QString& str)
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

        controller->set3dvarname(_varname);
        _title = controller->get_title();
        _timestr = controller->get_timestring();
        _minval = controller->get_minval();
        _maxval = controller->get_maxval();

        update_sliderNspin();
    }

    updateGL();
}

void GeneralTranslator::update_sliderNspin()
{
    int intvl = 0;
    int stepsize = 1;

    sliderNspinX->set_max(geometry->get_nx());
    sliderNspinX->setValue(geometry->get_nx());
    intvl = (geometry->get_nx()+9)/10;
    sliderNspinX->set_tickinterval(intvl);
    stepsize = (geometry->get_nx() + 350)/360;
    sliderNspinX->set_step(stepsize);

    sliderNspinY->set_max(geometry->get_ny());
    sliderNspinY->setValue(geometry->get_ny());
    intvl = (geometry->get_ny()+9)/10;
    sliderNspinY->set_tickinterval(intvl);
    stepsize = (geometry->get_ny() + 170)/180;
    sliderNspinY->set_step(stepsize);

    sliderNspinZ->set_max(geometry->get_nz());
    sliderNspinZ->set_step(1);
    sliderNspinZ->setValue(0);

    sliderNspinT->set_max(get_nt());
    sliderNspinT->set_step(1);
    sliderNspinT->setValue(0);
}

void GeneralTranslator::update_frame()
{
    if(nvoptions->get_cb(NV_ANIMATIONON))
    {
        ++_glbTime;
        if(_glbTime >= _maxTime)
           _glbTime = 0;

        _set_current_time();
        controller->set_fileNtime(_curFile, _curTime);
        _timestr = controller->get_timestring();

        updateGL();
    }
}

int GeneralTranslator::get_ndv(int n)
{
    return controller->get_ndv(n);
}

string* GeneralTranslator::get_ndvNames(int n)
{
    string* varnames = controller->get_ndvNames(n);
    return varnames;
}

void GeneralTranslator::set_light(Light* l)
{
    light = l;
  //controller->set_light(l);
}

void GeneralTranslator::set_locator(Locator* l)
{
     locator = l;

   //controller->set_locator(l);
}

void GeneralTranslator::selectColorMap(const QString& str)
{
  //cout << "\nfile: <" << __FILE__ << ">, function: <" << __PRETTY_FUNCTION__
  //     << ">, line: " << __LINE__ << endl;
  //cout << "ColorMap: <" << str.toStdString() << "> is selected." << endl;

    colorTable->set_colorMap(str.toStdString());

    controller->set_colorTable(colorTable);

    updateGL();
}

void GeneralTranslator::writeMouseMsg()
{
    QPoint mouseposition = nvoptions->get_position();

  //QPoint pos = QCursor::pos();

  //cout << "\nfile: <" << __FILE__ << ">, function: <" << __PRETTY_FUNCTION__
  //     << ">, line: " << __LINE__ << endl;
  //cout << "Cursor Position : {" << pos.x() << "," << pos.y() << "}" << endl;

    float sx = 1.0 / (float) nvoptions->get_screenWidth();
    float sy = 1.0 / (float) nvoptions->get_screenHeight();

    int ix = (int) ((float) geometry->get_nx() * (float) mouseposition.x() * sx);
    int jy = (int) ((float) geometry->get_ny() * (1.0 - mouseposition.y() * sy));

    _mouseinfo = "Position x="+QString::number(mouseposition.x())
               + ", y=" + QString::number(mouseposition.y())
               + "\nIndex i="+QString::number(ix)
               + ", j=" + QString::number(jy)
               + "\nValue ="+QString::number(controller->get_valueAt(ix, jy));

    emit mouse_msg(_mouseinfo);
}


void GeneralTranslator::selectClearDataview()
{
    cout << "\nfile: <" << __FILE__ << ">, function: <" << __PRETTY_FUNCTION__
         << ">, line: " << __LINE__ << endl;

    nvoptions->set_cb(NV_CLEARDATAVIEW, true);
}

void GeneralTranslator::selectDataview(int n)
{
    cout << "\nfile: <" << __FILE__ << ">, function: <" << __PRETTY_FUNCTION__
         << ">, line: " << __LINE__ << endl;
    cout << "\tn = " << n << endl;

    if(n)
    {
        nvoptions->set_cb(NV_DATAVIEWON, true);
        controller->setup_spreadsheet();
    }
    else
    {
        nvoptions->set_cb(NV_DATAVIEWON, true);
    }
}

