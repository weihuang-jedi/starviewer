#ifndef _AnimationWidget_H
#define _AnimationWidget_H

#include <QtGui/QtGui>
#include <QWidget>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QPushButton>
#include <QRadioButton>
#include <QTimer>
#include <QLabel>
#include <QGridLayout>
#include <QApplication>

#include <iostream>

#include "basetranslator.h"

class AnimationWidget : public QWidget
{
    Q_OBJECT                 //  Qt magic macro
    public:
        AnimationWidget(QWidget* parent=0); //  Constructor
       ~AnimationWidget();

        void setup();
        void set_translator(BaseTranslator *bt) { translator = bt; };

    public slots:
      //void animateFaster() { if(_time_interval >= 2) _time_interval /= 2; };
      //void animateSlower() { _time_interval *= 2; };
        void animateFaster();
        void animateSlower();

    protected:
        BaseTranslator *translator;

        QCheckBox *animation_cb;  // Check box for animation
        QCheckBox *jpg4movie_cb;  // Check box for jpg to make movie
        QGridLayout* layout;
        QTimer *timer;
        QLabel* frame;

        QPushButton *fastButton;
        QPushButton *slowButton;
        QPushButton *nextButton;
        QPushButton *backButton;

        void _setup_animation();
        void _setup_display();

        int _time_interval;
};
#endif

