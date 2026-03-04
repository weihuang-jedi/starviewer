#ifndef _DataViewWidget_H
#define _DataViewWidget_H

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
#include <QComboBox>
#include <QGridLayout>
#include <QApplication>

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

