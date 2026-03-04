#ifndef _ClipWidget_H
#define _ClipWidget_H

// $Id: clipWidget.h 5315 2015-02-06 21:24:34Z starviewer $

#include <QtGui/QtGui>
#include <QWidget>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QPushButton>
#include <QPushButton>
#include <QRadioButton>
#include <QTimer>
#include <QSlider>
#include <QObject>
#include <QLabel>
#include <QGridLayout>
#include <QApplication>

#include <iostream>

#include "sliderNspin.h"
#include "nvoptions.h"

typedef enum
{
    WESTCLIP = 0,
    EASTCLIP,
    SOUTHCLIP,
    NORTHCLIP,
    BOTTOMCLIP,
    TOPCLIP,
    NUMBEROFCLIPS
} ClipType;

class ClipWidget : public QWidget
{
    Q_OBJECT                 //  Qt magic macro
    public:
        ClipWidget(QWidget* parent=0); //  Constructor
       ~ClipWidget();

        void setup();
        void set_nvoptions(NVOptions* opt) { nvoptions = opt; };

    public slots:
        void setValueA(int n);
        void setValueB(int n);
        void setValueC(int n);
        void setValueD(int n);

        void selectClipOn(int n);

        void switchWest(bool v) { clipat[WESTCLIP] = v; };
        void switchEast(bool v) { clipat[EASTCLIP] = v; };
        void switchSouth(bool v) { clipat[SOUTHCLIP] = v; };
        void switchNorth(bool v) { clipat[NORTHCLIP] = v; };
        void switchBottom(bool v) { clipat[BOTTOMCLIP] = v; };
        void switchTop(bool v) { clipat[TOPCLIP] = v; };

        void selectWest(bool v);
        void selectEast(bool v);
        void selectSouth(bool v);
        void selectNorth(bool v);
        void selectBottom(bool v);
        void selectTop(bool v);

    protected:
        NVOptions* nvoptions;

        SliderNSpin* sliderNspinA;
        SliderNSpin* sliderNspinB;
        SliderNSpin* sliderNspinC;
        SliderNSpin* sliderNspinD;

        QSlider* aslider;
        QSlider* bslider;
        QSlider* cslider;
        QSlider* dslider;

        QSpinBox* aspinBox;
        QSpinBox* bspinBox;
        QSpinBox* cspinBox;
        QSpinBox* dspinBox;

    private:
        ClipType cliptype;

        bool clipat[NUMBEROFCLIPS];

        float clipequation[NUMBEROFCLIPS][4];

        QGridLayout* layout;

        QCheckBox *clip_cb; // Check box for clip

        QCheckBox *west_cb;   // Check box for west
        QCheckBox *east_cb;   // Check box for east
        QCheckBox *south_cb;  // Check box for south
        QCheckBox *north_cb;  // Check box for north
        QCheckBox *bottom_cb; // Check box for bottom
        QCheckBox *top_cb;    // Check box for top

        QRadioButton *west_rb;   // RadioButton for west
        QRadioButton *east_rb;   // RadioButton for east
        QRadioButton *south_rb;  // RadioButton for south
        QRadioButton *north_rb;  // RadioButton for north
        QRadioButton *bottom_rb; // RadioButton for bottom
        QRadioButton *top_rb;    // RadioButton for top
};
#endif

