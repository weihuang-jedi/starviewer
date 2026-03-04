#include <QtGui/QApplication>

// $Id: lightWidget.cpp 5315 2015-02-06 21:24:34Z starviewer $

#include "lightWidget.h"

LightWidget::LightWidget(QWidget* parent) : QWidget(parent)
{
  //cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__
  //     << ", file: <" << __FILE__ << ">" << endl;

    setWindowTitle(tr("Light"));

    layout = new QGridLayout;
    light = NULL;

    _time_interval = 100;
    _latitude = 0.0;
    _altitude = 0.0;
    _longitude = 180.0;
    _longitude_step = 2.5;

    timer = new QTimer(this);
    timer->start(_time_interval);

    _animationOn = false;
    _lightOn = false;
    _drawBall = false;
}

LightWidget::~LightWidget()
{
   delete timer;
   if(NULL != light)
       light->turnOff();
   delete layout;
}

//Reset
void LightWidget::reset(void)
{
    sliderNspinLon->setValue(0);
    sliderNspinLat->setValue(0);
    sliderNspinAlt->setValue(100);
  //light->reset();
}

void LightWidget::setup()
{
  //cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__
  //     << ", file: <" << __FILE__ << ">" << endl;

  //Display info
    info = new QLabel();

    animation_cb  = new QCheckBox("Animation");
    animation_cb->setChecked(_animationOn);

    sliderNspinLon = new SliderNSpin(Qt::Horizontal, tr("Longitude"),  this);
    sliderNspinLat = new SliderNSpin(Qt::Horizontal, tr("Latitude"),  this);
    sliderNspinAlt = new SliderNSpin(Qt::Horizontal, tr("Altitude"),  this);

    sliderNspinLon->set_min(0);
    sliderNspinLon->set_max(360);
    sliderNspinLon->set_step(1);
    sliderNspinLon->set_tickinterval(30);
    sliderNspinLon->setValue(0);

    sliderNspinLat->set_min(-90);
    sliderNspinLat->set_max(90);
    sliderNspinLat->set_step(1);
    sliderNspinLat->set_tickinterval(30);
    sliderNspinLat->setValue(0);

    sliderNspinAlt->set_min(0);
    sliderNspinAlt->set_max(1000);
    sliderNspinAlt->set_step(1);
    sliderNspinAlt->set_tickinterval(100);
    sliderNspinAlt->setValue(500);

    lon_slider = sliderNspinLon->get_slider();
    lat_slider = sliderNspinLat->get_slider();
    alt_slider = sliderNspinAlt->get_slider();

    lon_spinBox = sliderNspinLon->get_spinBox();
    lat_spinBox = sliderNspinLat->get_spinBox();
    alt_spinBox = sliderNspinAlt->get_spinBox();

  //Pushbutton to reset view angle
    QPushButton* reset = new QPushButton("Reset");

    light_cb = new QCheckBox("Light");
    light_cb->setChecked(_lightOn);

    ball_cb = new QCheckBox("LightBall");
    ball_cb->setChecked(false);
    ball_cb->setCheckState(Qt::Unchecked);

    fastButton = new QPushButton("Fast");
    slowButton = new QPushButton("Slow");
    nextButton = new QPushButton("Next");
    backButton = new QPushButton("Back");

    connect(animation_cb, SIGNAL(stateChanged(int)), this, SLOT(selectAnimation(int)));
    connect(light_cb,     SIGNAL(stateChanged(int)), this, SLOT(selectLight(int)));
    connect(ball_cb,      SIGNAL(stateChanged(int)), this, SLOT(selectBall(int)));

  //Connect light signals to display widgets
    connect(reset, SIGNAL(clicked()),       this, SLOT(reset(void)));

  //Group checkbox parameters
    QGroupBox*   light_box = new QGroupBox("Light");
    QGridLayout* light_lay = new QGridLayout;

    light_lay->addWidget(light_cb, 0, 0);
    light_lay->addWidget(ball_cb,  0, 1);
    light_lay->addWidget(reset,    1, 0);

    light_box->setLayout(light_lay);
    layout->addWidget(light_box,0,0);

  //Group slider parameters
    QGroupBox*   slider_box = new QGroupBox("Cross-Sections:");
    QGridLayout* slider_lay = new QGridLayout;
    slider_box->setLayout(slider_lay);

    connect(lon_slider,  SIGNAL(valueChanged(int)), this, SLOT(setValueLongitude(int)));
    connect(lon_spinBox, SIGNAL(valueChanged(int)), this, SLOT(setValueLongitude(int)));

    connect(lat_slider,  SIGNAL(valueChanged(int)), this, SLOT(setValueLatitude(int)));
    connect(lat_spinBox, SIGNAL(valueChanged(int)), this, SLOT(setValueLatitude(int)));

    connect(alt_slider,  SIGNAL(valueChanged(int)), this, SLOT(setValueAltitude(int)));
    connect(alt_spinBox, SIGNAL(valueChanged(int)), this, SLOT(setValueAltitude(int)));

    sliderNspinLon->add2layout(slider_lay, 0, 0);
    sliderNspinLat->add2layout(slider_lay, 1, 0);
    sliderNspinAlt->add2layout(slider_lay, 2, 0);

    layout->addWidget(slider_box,1,0);

    connect(fastButton, SIGNAL(clicked()), this, SLOT(animateFaster()));
    connect(slowButton, SIGNAL(clicked()), this, SLOT(animateSlower()));
    connect(nextButton, SIGNAL(clicked()), this, SLOT(nextFrame()));
    connect(backButton, SIGNAL(clicked()), this, SLOT(backFrame()));

  //Group animation parameters
    QGroupBox*   animation_box = new QGroupBox("Animation");
    QGridLayout* animation_lay = new QGridLayout;
    animation_lay->addWidget(animation_cb, 0, 0);
    animation_lay->addWidget(info,         0, 1);
    animation_lay->addWidget(nextButton,   1, 0);
    animation_lay->addWidget(backButton,   1, 1);
    animation_lay->addWidget(fastButton,   2, 0);
    animation_lay->addWidget(slowButton,   2, 1);
    animation_box->setLayout(animation_lay);

    layout->addWidget(animation_box,2,0);

    connect(timer, SIGNAL(timeout()), this, SLOT(update_frame()));

    layout->setColumnStretch(0,2);
    layout->setColumnMinimumWidth(0,2);

  //layout->setRowStretch(0,10);

  //Overall layout
    setLayout(layout);
}

void LightWidget::selectAnimation(int n)
{
  //cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__
  //     << ", file: <" << __FILE__ << ">" << endl;

    if(n)
        _animationOn = true;
    else
        _animationOn = false;
}

void LightWidget::selectLight(int n)
{
  //cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__
  //     << ", file: <" << __FILE__ << ">" << endl;

    if(n)
    {
        _lightOn = true;
        light->turnOn();
    }
    else
    {
        _lightOn = false;
        light->turnOff();
    }

    update_frame();
}

void LightWidget::selectBall(int n)
{
  //cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__
  //     << ", file: <" << __FILE__ << ">" << endl;

    if(n)
        _drawBall = true;
    else
        _drawBall = false;

    light->set_drawBall(_drawBall);
    update_frame();
}

void LightWidget::update_frame()
{
    if(_animationOn)
    {
        _longitude -= _longitude_step;
        if(0.0 > _longitude)
            _longitude += 360.0;
        light->set_longitude(_longitude);
    }

    translator->update_frame();
}

void LightWidget::nextFrame()
{
    _longitude -= _longitude_step;
    light->set_longitude(_longitude);
    update_frame();
}

void LightWidget::backFrame()
{
    _longitude += _longitude_step;
    light->set_longitude(_longitude);
    update_frame();
}

void LightWidget::setValueLongitude(int n)
{
    _longitude = (float) n;
    light->set_longitude(_longitude);
    update_frame();
}

void LightWidget::setValueLatitude(int n)
{
    _latitude = (float) n;
    light->set_latitude(_latitude);
    update_frame();
}

void LightWidget::setValueAltitude(int n)
{
    _altitude = 2.0 + 0.02 * (float) (n+1);
    light->set_altitude(_altitude);
    update_frame();
}

