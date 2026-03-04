#ifndef _MinMaxidget_H
#define _MinMaxidget_H

// $Id: minmaxWidget.h 5315 2015-02-06 21:24:34Z starviewer $

#include <QtGui/QtGui>
#include <QWidget>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QPushButton>
#include <QRadioButton>
#include <QLabel>
#include <QLineEdit>
#include <QTimer>
#include <QSlider>
#include <QObject>
#include <QLabel>
#include <QGridLayout>
#include <QApplication>

#include <string>
#include <iostream>
#include <sstream>

#include "basetranslator.h"

using namespace std;

class MinMaxWidget : public QWidget
{
    Q_OBJECT                 //  Qt magic macro
    public:
        MinMaxWidget(QWidget* parent=0); //  Constructor
       ~MinMaxWidget();

        void setup();

        void set_translator(BaseTranslator* t);

    private slots:
        void setMin(const QString &text);
        void setMax(const QString &text);

    protected:
        BaseTranslator* translator;
        NVOptions* nvoptions;

      //Set layout of child widgets
        QGridLayout* layout;

        QCheckBox* minmax_cb;  // Check box for MinMax

        QLabel* minLabel;
        QLabel* maxLabel;

        QLineEdit* minEdit;
        QLineEdit* maxEdit;
};
#endif

