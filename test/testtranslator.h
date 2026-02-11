#ifndef TestTRANSLATOR_H
#define TestTRANSLATOR_H

//$Id: testtranslator.h 5315 2015-02-06 21:24:34Z starviewer $

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
#include "basetranslator.h"
#include "topography.h"
#include "coastline2.h"

class TestTranslator : public BaseTranslator
{
    Q_OBJECT                                             //  Qt magic macro
    public:
        TestTranslator(ColorTable* ct, NVOptions* opt, string flnm,
                       bool isList = false, QWidget* parent=0);	//  Constructor
       ~TestTranslator();					//  Destructor
    
        void show();
        void setup();
        void update_sliderNspin();
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
        Topography* topography;
        CoastLine2* coastline2;

        bool _flat;

        void sphereVertex(int i, int j);
        void createVarInfo();
        void make_timeNpositionString();
        void writeFrameInfo();
        void draw();
};
#endif

