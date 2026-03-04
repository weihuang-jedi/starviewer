#ifndef _LightWidget_H
#define _LightWidget_H

// $Id: lightWidget.h 5315 2015-02-06 21:24:34Z starviewer $

#include <QtGui/QtGui>
#include <QtGui/QWidget>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QCheckBox>
#include <QtGui/QPushButton>
#include <QtGui/QPushButton>

#include <QTimer>

#include <iostream>

#include "sliderNspin.h"
#include "light.h"
#include "basetranslator.h"

class LightWidget : public QWidget
{
    Q_OBJECT                 //  Qt magic macro
    public:
        LightWidget(QWidget* parent=0); //  Constructor
       ~LightWidget();

        void setup();
        void set_light(Light *l) { light = l; };
        void set_translator(BaseTranslator* t) { translator = t; };

    public slots:
        void update_frame();
        void selectAnimation(int n);
        void selectLight(int n);
        void selectBall(int n);
        void animateFaster() { if(_time_interval >= 2) _time_interval /= 2; };
        void animateSlower() { _time_interval *= 2; };
        void nextFrame();
        void backFrame();

        void reset();

        void setValueLongitude(int n);
        void setValueLatitude(int n);
        void setValueAltitude(int n);

    protected:
        BaseTranslator* translator;
        Light *light;

        QGridLayout* layout;

        QTimer* timer;
        QLabel* info;

        QCheckBox *animation_cb;  // Check box for animation
        QCheckBox *light_cb; // Check box for light
        QCheckBox *ball_cb; // Check box for ball

        QPushButton *fastButton;
        QPushButton *slowButton;
        QPushButton *nextButton;
        QPushButton *backButton;

        SliderNSpin *sliderNspinLon;
        SliderNSpin *sliderNspinLat;
        SliderNSpin *sliderNspinAlt;

        QSlider* lon_slider;
        QSlider* lat_slider;
        QSlider* alt_slider;

        QSpinBox* lon_spinBox;
        QSpinBox* lat_spinBox;
        QSpinBox* alt_spinBox;

        int _time_interval;
        bool _animationOn;
        bool _lightOn;
        bool _drawBall;

        float _latitude;
        float _longitude;
        float _altitude;
        float _longitude_step;
};
#endif

