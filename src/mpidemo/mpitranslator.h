#ifndef MPITRANSLATOR_H
#define MPITRANSLATOR_H

//$Id: mpitranslator.h 5315 2015-02-06 21:24:34Z starviewer $

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
#include "basetranslator.h"
#include "mpiviewer.h"

class MPITranslator : public BaseTranslator
{
    Q_OBJECT                                             //  Qt magic macro
    public:
        MPITranslator(ColorTable* ct, NVOptions* opt,
                        QWidget* parent=0);	//  Constructor
       ~MPITranslator();					//  Destructor
    
        void show();
        void setup();

        int get_ndv(int n);
        string* get_ndvNames(int n);

	void paintGL();

    public slots:
        void select1dVar(const QString& str);
        void select2dVar(const QString& str);
        void select3dVar(const QString& str);

        void selectColorMap(const QString& str);

        void updateSliders();
        void update_frame();
        void nextFrame();
        void backFrame();

    protected:
        void writeVarInfo();
        void writeLocatorMsg();

    private:
        MPIViewer* viewer;

        void createVarInfo();
        void make_timeNpositionString();
        void writeFrameInfo();
};
#endif

