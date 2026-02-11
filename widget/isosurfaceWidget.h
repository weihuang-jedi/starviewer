#ifndef _IsoSurfaceWidget_H
#define _IsoSurfaceWidget_H

// $Id: isosurfaceWidget.h 5315 2015-02-06 21:24:34Z starviewer $

#include <QtGui/QtGui>
#include <QtGui/QWidget>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QCheckBox>
#include <QtGui/QPushButton>
#include <QtGui/QPushButton>

#include <QTimer>

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

