#ifndef MPASGridTRANSLATOR_H
#define MPASGridTRANSLATOR_H

//$Id: mpastranslator.h 5315 2015-02-06 21:24:34Z starviewer $

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
#include "mpasgridcontroller.h"
#include "basetranslator.h"

class MPASGridTranslator : public BaseTranslator
{
    Q_OBJECT                                             //  Qt magic macro
    public:
        MPASGridTranslator(ColorTable* ct, NVOptions* opt,
	 	           string static_flnm, string data_flnm,
                           QWidget* parent=0);	//  Constructor
       ~MPASGridTranslator();					//  Destructor
    
        void show();
        void setup();

        void set_static_flnm(string s) { _static_flnm = s; };
        void set_data_flnm(string s) { _data_flnm = s; };

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
        MPASGridGeometry* geometry;
        MPASGridController* mpasgridcontroller;

        bool _saveJpg;
        bool _jpgNotSaved;
        bool _startSave;

        bool _flat;
        string _static_flnm;
        string _data_flnm;

        void sphereVertex(int i, int j);

        void createVarInfo();
        void make_timeNpositionString();
        void writeFrameInfo();
};
#endif

