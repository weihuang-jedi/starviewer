#ifndef _EagleLAMTranslator_H
#define _EagleLAMTranslator_H

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
#include "eaglelam_controller.h"
#include "basetranslator.h"

using namespace std;

class EagleLAMTranslator : public BaseTranslator
{
    Q_OBJECT		//  Qt magic macro
    public:
        EagleLAMTranslator(ColorTable *ct, NVOptions* opt, 
                           vector<string> datafiles, QWidget* parent=0);	//  Constructor
       ~EagleLAMTranslator();						//  Destructor

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
        EAGLELAM_Controller* eaglelam_controller;

	vector<string> _datafiles;

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

