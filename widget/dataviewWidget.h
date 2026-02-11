#ifndef _DataViewWidget_H
#define _DataViewWidget_H

// $Id: dataviewWidget.h 5315 2015-02-06 21:24:34Z starviewer $

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

class DataViewWidget : public QWidget
{
    Q_OBJECT                 //  Qt magic macro
    public:
        DataViewWidget(QWidget* parent=0); //  Constructor
       ~DataViewWidget();

        void setup();

        void set_translator(BaseTranslator* t) { translator = t; };

    protected:
        BaseTranslator* translator;

      //Set layout of child widgets
        QGridLayout* layout;

        QCheckBox* dataview_cb;  // Check box for Vector
        QPushButton* clear_pb;   // Clear push-button
};
#endif

