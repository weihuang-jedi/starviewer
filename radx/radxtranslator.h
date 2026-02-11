#ifndef RadxTRANSLATOR_H
#define RadxTRANSLATOR_H

//$Id: radxtranslator.h 5315 2015-02-06 21:24:34Z starviewer $

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
#include "coastline2.h"
#include "radxcontroller.h"
#include "basetranslator.h"

class RadxTranslator : public BaseTranslator
{
    Q_OBJECT                                             //  Qt magic macro
    public:
        RadxTranslator(ColorTable* ct, NVOptions* opt,
                       string flnm, bool isList = false,
                       QWidget* parent=0);		//  Constructor
       ~RadxTranslator();				//  Destructor
    
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
        CoastLine2* coastline2;
        RadxReader* reader;
        RadxController* radxcontroller;

        bool _saveJpg;
        bool _jpgNotSaved;
        bool _startSave;

        double _base_lon, _base_lat, _base_alt;

        void sphereVertex(int i, int j);

        void createVarInfo();
        void make_timeNpositionString();
        void writeFrameInfo();
};
#endif

