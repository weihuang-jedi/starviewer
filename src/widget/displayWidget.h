#ifndef _DisplayWidget_H
#define _DisplayWidget_H

#include <QtGui/QtGui>
#include <QtGui/QWidget>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QCheckBox>
#include <QtGui/QPushButton>

#include <iostream>

#include "basetranslator.h"

class DisplayWidget : public QWidget
{
    Q_OBJECT                 //  Qt magic macro
    public:
        DisplayWidget(QWidget* parent=0); //  Constructor
       ~DisplayWidget();

        void set_translator(BaseTranslator *t) { translator = t; };
        void setup();

    private:
        BaseTranslator* translator;
        QGridLayout* layout;
};
#endif

