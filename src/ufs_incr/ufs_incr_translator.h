#ifndef _UFSincrTRANSLATOR_H
#define _UFSincrTRANSLATOR_H

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

#include "sliderNspin.h"
#include "basetranslator.h"
#include "ufs_incr_controller.h"

class UFSincrTranslator : public BaseTranslator
{
    Q_OBJECT                                             //  Qt magic macro
    public:
        UFSincrTranslator(ColorTable* ct, NVOptions* opt,
                          string flnm, bool isList=false,
                          string mfnm = "unknown", QWidget* parent=0);	//  Constructor
        UFSincrTranslator(ColorTable* ct, NVOptions* opt,
                          string atmfile, string sfcfile,
                          vector<string> datafiles, QWidget* parent=0);
       ~UFSincrTranslator();				//  Destructor
    
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
        UFSincrController* ufs_controller;
        UFSincrGeometry* incr_geometry;

	string _atmfile;
       	string _sfcfile;
        vector<string> _datafiles;

    private:
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

