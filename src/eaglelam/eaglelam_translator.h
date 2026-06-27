#ifndef _EAGLELAM_Translator_H
#define _EAGLELAM_Translator_H

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

class EAGLELAM_Translator : public BaseTranslator
{
    Q_OBJECT		//  Qt magic macro
    public:
        EAGLELAM_Translator(ColorTable *ct, NVOptions* opt, 
                            vector<string> datafiles, QWidget* parent=0);	//  Constructor
       ~EAGLELAM_Translator();						//  Destructor

        void show();
        void setup();
        void update_sliderNspin();

        void set_light(Light* l);
        void set_locator(Locator* l);

        int get_ndv(int n);
        string* get_ndvNames(int n);

    public slots:
        void selectColorMap(const QString& cmap);

        void selectVar(const QString& vn);
        void select1dVar(const QString& vn) { selectVar(vn); };
        void select2dVar(const QString& vn) { selectVar(vn); };
        void select3dVar(const QString& vn) { selectVar(vn); };

        void update_frame();
        void nextFrame();
        void backFrame();

    protected:
        QGridLayout* layout;
        EAGLELAM_Controller* eaglelam_controller;
	EAGLELAM_Geometry* geometry;

	vector<string> _datafiles;

        void createVarInfo();
        void writeVarInfo();
        void writeFrameInfo();
        void make_timeNpositionString();
        void writeLocatorMsg();
};
#endif

