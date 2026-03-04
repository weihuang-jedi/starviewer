#ifndef _LICWidget_H
#define _LICWidget_H

// $Id: licWidget.h 5315 2015-02-06 21:24:34Z starviewer $

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

#include "basetranslator.h"
#include "colorTable.h"

class LICWidget : public QWidget
{
    Q_OBJECT                 //  Qt magic macro
    public:
        LICWidget(QWidget* parent=0); //  Constructor
       ~LICWidget();

        void setup();

        void set_translator(BaseTranslator* t) { translator = t; };

    protected:
        BaseTranslator* translator;

      //Set layout of child widgets
        QGridLayout* layout;

        QCheckBox* lic_cb;  // Check box for LIC
        QCheckBox* lic_only_cb;  // Check box for LIC
        QCheckBox* lic_3D_cb;  // Check box for LIC
};
#endif

