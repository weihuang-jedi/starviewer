#ifndef _VTKTRANSLATOR_H_
#define _VTKTRANSLATOR_H_

//$Id: vtktranslator.h 5315 2015-02-06 21:24:34Z starviewer $

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <assert.h>

#include <QtGui/QKeyEvent>

#include <QGLWidget>
#include <QString>
#include <QTimer>

#include "sliderNspin.h"
#include "coastline.h"
#include "vtkcontroller.h"
#include "basetranslator.h"

class VTKTranslator : public BaseTranslator
{
    Q_OBJECT                                             //  Qt magic macro
    public:
        VTKTranslator(ColorTable* ct, NVOptions* opt,
                        string flnm, bool isList = false,
                        QWidget* parent=0);		//  Constructor
       ~VTKTranslator();				//  Destructor
    
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
        void updateSliders();

    protected:
        void writeVarInfo();
        void writeLocatorMsg();

    private:
        CoastLine* coastline;
        VTKReader* reader;
        VTKController* controller;

        bool _saveJpg;
        bool _jpgNotSaved;
        bool _startSave;

        void sphereVertex(int i, int j);

        void createVarInfo();
        void make_timeNpositionString();
        void writeFrameInfo();
};
#endif

