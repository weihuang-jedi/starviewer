#ifndef _IsoSurfaceWidget_H
#define _IsoSurfaceWidget_H

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

class IsoSurfaceWidget : public QWidget
{
    Q_OBJECT                 //  Qt magic macro
    public:
        IsoSurfaceWidget(QWidget* parent=0); //  Constructor
       ~IsoSurfaceWidget();

        void setup();
        void set_translator(BaseTranslator* t) { translator = t; };

    protected:
        BaseTranslator* translator;

      //Set layout of child widgets
        QGridLayout* layout;

        QCheckBox* isosurface_cb;  // Check box for isosurface
        QCheckBox* iso_only_cb;  // Check box for isosurface
};
#endif

