#ifndef _UFSTRANSLATOR_H
#define _UFSTRANSLATOR_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <assert.h>

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cerrno>

#include <QtGui/QKeyEvent>
#include <QtOpenGL/QGLWidget>
#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core> // Use Core profile structures
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QMatrix4x4>
#include <QString>
#include <QTimer>

#include "sliderNspin.h"
#include "controlWidget.h"
#include "basetranslator.h"
#include "ufs_controller.h"

class UFSTranslator : public BaseTranslator
{
    Q_OBJECT                                             //  Qt magic macro
    public:
        UFSTranslator(ColorTable* ct, NVOptions* opt,
                      YAMLHandler *yamlHandler, QWidget* parent=0);	//  Constructor
       ~UFSTranslator();				//  Destructor
    
        void show();
        void setup();
        void setfilename(string flnm) { _filename = flnm; };
        void set_light(Light* l);
        void set_locator(Locator* l);

        int get_ndv(int n);
        string* get_ndvNames(int n);
        string get_title() { return _title; };

    public slots:
        void select0dVar(const QString& str);
        void select1dVar(const QString& str);
        void select2dVar(const QString& str);
        void select3dVar(const QString& str);

        void selectColorMap(const QString& str);

        void update_frame();
        void nextFrame();
        void backFrame();

    protected:
        void writeVarInfo();
        void writeLocatorMsg();

    private:
        UFSController* ufs_controller;
        UFSGeometry* geometry;
	ColorTable* wvct;

        QOpenGLShaderProgram m_shaderProgram;
        QOpenGLVertexArrayObject m_vao;
        QOpenGLBuffer m_vbo; // Vertex Buffer Object

        // Matrices to replace legacy matrix operations
        QMatrix4x4 m_projectionMatrix;
        QMatrix4x4 m_modelViewMatrix;

#if 0
        bool _jpgNotSaved;
        bool _startSave;

        void SaveJpg(int n);
#endif

        void _initialize();

        void createVarInfo();
        void writeFrameInfo();
        void make_timeNpositionString();

        vector<int> frameList;
};
#endif

