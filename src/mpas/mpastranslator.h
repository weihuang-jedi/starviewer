#ifndef MPASTRANSLATOR_H
#define MPASTRANSLATOR_H

//$Id: mpastranslator.h 5315 2015-02-06 21:24:34Z starviewer $

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <assert.h>

#include <QtGui/QKeyEvent>
#include <QtOpenGL/QGLWidget>
#include <QString>
#include <QTimer>

#include "sliderNspin.h"
#include "mpascontroller.h"
#include "basetranslator.h"

class MPASTranslator : public BaseTranslator
{
    Q_OBJECT                                             //  Qt magic macro
    public:
        MPASTranslator(ColorTable* ct, NVOptions* opt, string flnm,
                       bool isList = false, QWidget* parent=0);	//  Constructor
       ~MPASTranslator();					//  Destructor
    
        void show();
        void setup();
        void setfilename(string flnm) { _filename = flnm; };

        void set_light(Light* l);
        void set_locator(Locator* l);

        int get_ndv(int n);
        string* get_ndvNames(int n);

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
        MPASGeometry* geometry;
        MPASController* mpascontroller;

        bool _saveJpg;
        bool _jpgNotSaved;
        bool _startSave;

        void sphereVertex(int i, int j);

        bool _flat;

        void createVarInfo();
        void make_timeNpositionString();
        void writeFrameInfo();
};
#endif

