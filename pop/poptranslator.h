#ifndef _POPTRANSLATOR_H
#define _POPTRANSLATOR_H

//$Id: poptranslator.h 5315 2015-02-06 21:24:34Z starviewer $

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <assert.h>

#include <QtGui/QKeyEvent>
#include <QGLWidget>
#include <QString>
#include <QTimer>

#include "util.h"
#include "popcontroller.h"
#include "basetranslator.h"

class POPTranslator : public BaseTranslator
{
    Q_OBJECT                                             //  Qt magic macro
    public:
        POPTranslator(ColorTable* ct, NVOptions* opt, string flnm,
                      bool isList = false, QWidget* parent=0);	//  Constructor
       ~POPTranslator();					//  Destructor
    
        void show();
        void setup();
        void setfilename(string flnm) { _filename = flnm; };

        int get_ndv(int n);
        string* get_ndvNames(int n);
        string get_title() { return _title; };

        void set_light(Light* l);
        void set_locator(Locator* l);

    public slots:
        void select1dVar(const QString& str);
        void select2dVar(const QString& str);
        void select3dVar(const QString& str);

        void selectColorMap(const QString& str);

        void update_frame();

    protected:
        void writeVarInfo();
        void writeLocatorMsg();

    private:
        POPGeometry* geometry;
        POPController* controller;

        void createVarInfo();
        void make_timeNpositionString();
        void writeFrameInfo();
};
#endif

