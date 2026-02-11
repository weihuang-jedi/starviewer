#ifndef _GeneralTranslator_H
#define _GeneralTranslator_H

//$Id: generaltranslator.h 5315 2015-02-06 21:24:34Z starviewer $

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <assert.h>

#ifdef LINUX
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>
#endif

#include <QtGui/QKeyEvent>

#include <QGLWidget>
#include <QString>
#include <QTimer>

#include "sliderNspin.h"
#include "geometry.h"
#include "controller.h"
#include "util.h"
#include "basetranslator.h"

class GeneralTranslator : public BaseTranslator
{
    Q_OBJECT		//  Qt magic macro
    public:
        GeneralTranslator(ColorTable* ct,
                          NVOptions* opt,
                          string flnm,
                          bool isFileList = false,
                          QWidget* parent=0);	//  Constructor
        virtual ~GeneralTranslator();						//  Destructor

        void setup();

        virtual void show();                   //  Draw widget

        void setfilename(string flnm) { _filename = flnm; };

        void set_light(Light* l);
        void set_locator(Locator* l);

        int get_ndv(int n);
        string* get_ndvNames(int n);
        string get_title() { return _title; };

    public slots:
        void update_frame();

        void select1dVar(const QString& str);
        void select2dVar(const QString& str);
        void select3dVar(const QString& str);

        void selectColorMap(const QString& str);

        void selectDataview(int n);
        void selectClearDataview();

    protected:
        Geometry*   geometry;
        Controller* controller;

        void update_sliderNspin();

        void createVarInfo();
        void writeVarInfo();
        void make_timeNpositionString();

        void writeMouseMsg();

    private:
        bool _first_time;
};
#endif

