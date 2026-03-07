#ifndef _UFSTranslator_H
#define _UFSTranslator_H

//$Id: ufstranslator.h 5315 2015-02-06 21:24:34Z starviewer $

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
#include <QtOpenGL/QGLWidget>
#include <QString>
#include <QTimer>

#include "sliderNspin.h"
#include "ufs_controller.h"
#include "generaltranslator.h"

using namespace std;

class UFSTranslator : public GeneralTranslator
{
    Q_OBJECT		//  Qt magic macro
    public:
        UFSTranslator(ColorTable *ct, NVOptions* opt, string flnm,
                      bool isFileList = false, QWidget* parent=0);	//  Constructor
       ~UFSTranslator();						//  Destructor

        void show();
        void setup();
        void update_sliderNspin();

        void set_light(Light* l);
        void set_locator(Locator* l);

        int get_ndv(int n);
        string* get_ndvNames(int n);

    public slots:
        void select2dVar(const QString& str);
        void select3dVar(const QString& str);

        void selectColorMap(const QString& str);

        void update_frame();
        void nextFrame();
        void backFrame();

    protected:
        QGridLayout* layout;
        UFS_Controller* ufs_controller;

        void createVarInfo();
        void writeVarInfo();
        void writeFrameInfo();
        void make_timeNpositionString();
        void writeLocatorMsg();

        bool _first_time;
        bool _displaying2D;
        bool _displaying3D;
};
#endif

