#ifndef RADARTRANSLATOR_H
#define RADARTRANSLATOR_H

//$Id: radartranslator.h 14561 2013-09-23 16:07:00Z starviewer $

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <assert.h>

#include <QtOpenGL/QGLWidget>
#include <QString>
#include <QKeyEvent>
#include <QTimer>

#include "sliderNspin.h"
#include "coastline2.h"
#include "radarcontroller.h"
#include "basetranslator.h"

class RADARTranslator : public BaseTranslator
{
    Q_OBJECT                                             //  Qt magic macro
    public:
        RADARTranslator(ColorTable* ct, NVOptions* opt,
                        string flnm, bool isList = false,
                        string rid = "", QWidget* parent=0);	//  Constructor
       ~RADARTranslator();					//  Destructor
    
        void setup();

        void paintGL();

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
        RADARReader* reader;
        RADARController* radarcontroller;

        string radarid;

        bool _saveJpg;
        bool _jpgNotSaved;
        bool _startSave;

        double _base_lon, _base_lat;

        void sphereVertex(int i, int j);

        void createVarInfo();
        void make_timeNpositionString();
        void writeFrameInfo();
};
#endif

