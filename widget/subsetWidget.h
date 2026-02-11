#ifndef _Subsetidget_H
#define _Subsetidget_H

// $Id: subsetWidget.h 5315 2015-02-06 21:24:34Z starviewer $

#include <QtGui/QtGui>
#include <QtGui/QWidget>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QCheckBox>
#include <QtGui/QPushButton>
#include <QtGui/QPushButton>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>

#include <QTimer>

#include <string>
#include <iostream>
#include <sstream>

#include "nvoptions.h"
#include "basetranslator.h"

using namespace std;

class SubsetWidget : public QWidget
{
    Q_OBJECT                 //  Qt magic macro
    public:
        SubsetWidget(QWidget* parent=0); //  Constructor
       ~SubsetWidget();

        void setup();

        void set_translator(BaseTranslator* t) { translator = t; };
        void set_nvoptions(NVOptions *o) { nvoptions = o; };

    private slots:
      //void selectSubset(int n);
        void setMinLon(const QString &text);
        void setMaxLon(const QString &text);
        void setMinLat(const QString &text);
        void setMaxLat(const QString &text);

    protected:
        BaseTranslator* translator;
        NVOptions *nvoptions;

      //Set layout of child widgets
        QGridLayout* layout;

        QCheckBox* subset_cb;  // Check box for Subset

        QLabel* minlonLabel;
        QLabel* maxlonLabel;
        QLabel* minlatLabel;
        QLabel* maxlatLabel;

        QLineEdit* minlonEdit;
        QLineEdit* maxlonEdit;
        QLineEdit* minlatEdit;
        QLineEdit* maxlatEdit;
};
#endif

