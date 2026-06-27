#ifndef _EagleGlobalTRANSLATOR_H
#define _EagleGlobalTRANSLATOR_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <assert.h>

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cerrno>

#include <QtGui/QKeyEvent>
#include <QtOpenGL/QGLWidget>
#include <QString>
#include <QTimer>

#include "yamlhandler.h"
#include "sliderNspin.h"
#include "controlWidget.h"
#include "basetranslator.h"
#include "eagleglobal_controller.h"

class EagleGlobalTranslator : public BaseTranslator
{
    Q_OBJECT                                             //  Qt magic macro
    public:
        EagleGlobalTranslator(ColorTable* ct, NVOptions* opt,
                              vector<string> datafiles, QWidget* parent=0);	//  Constructor
       ~EagleGlobalTranslator();				//  Destructor
    
        void show();
        void setup();
        void setfilename(string flnm) { _filename = flnm; };
        void set_light(Light* l);
        void set_locator(Locator* l);

        int get_ndv(int n);
        string* get_ndvNames(int n);
        string get_title() { return _title; };

    public slots:
        void select0dVar(const QString& str);
        void select1dVar(const QString& str);
        void select2dVar(const QString& str);
        void select3dVar(const QString& str);

        void selectColorMap(const QString& str);

        void update_frame();
        void nextFrame();
        void backFrame();

    protected:
        void writeVarInfo();
        void writeLocatorMsg();

    private:
        EagleGlobalController* eagleglobal_controller;
        EagleGlobalGeometry* geometry;

        vector<string> _datafiles;

#if 0
        bool _jpgNotSaved;
        bool _startSave;

        void SaveJpg(int n);
#endif

        void _initialize();

        void createVarInfo();
        void writeFrameInfo();
        void make_timeNpositionString();

        vector<int> frameList;
};
#endif

