#ifndef _VectorWidget_H
#define _VectorWidget_H

#include <QtGui/QtGui>
#include <QWidget>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QPushButton>
#include <QRadioButton>
#include <QTimer>
#include <QSlider>
#include <QObject>
#include <QLabel>
#include <QGridLayout>
#include <QApplication>

#include <iostream>

#include "nvoptions.h"
#include "colorTable.h"
#include "basetranslator.h"

class VectorWidget : public QWidget
{
    Q_OBJECT                 //  Qt magic macro
    public:
        VectorWidget(QWidget* parent=0); //  Constructor
       ~VectorWidget();

        void setup();

        void set_translator(BaseTranslator* t) { translator = t; };
        void set_nvoptions(NVOptions* opt) { nvoptions = opt; };

    public slots:
        void selectVector(int n);
        void selectVecOnly(int n);
        void selectVec3D(int n);

        void vectorLonger();
        void vectorShorter();
        void vectorWider();
        void vectorNarrower();
        void vectorMore();
        void vectorLess();

    protected:
        BaseTranslator* translator;
        NVOptions* nvoptions;

      //Set layout of child widgets
        QGridLayout* layout;

        QCheckBox* vector_cb;  // Check box for Vector
        QCheckBox* vec_only_cb;  // Check box for Vector
        QCheckBox* vec_3D_cb;  // Check box for Vector

        QPushButton *longerButton;
        QPushButton *shorterButton;

        QPushButton *widerButton;
        QPushButton *narrowerButton;

        QPushButton *moreButton;
        QPushButton *lessButton;
};
#endif

