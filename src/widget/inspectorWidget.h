#ifndef _InspectorWidget_H
#define _InspectorWidget_H

// $Id: inspectorWidget.h 5315 2015-02-06 21:24:34Z starviewer $

#include <QtGui/QtGui>
#include <QtGui/QWidget>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QCheckBox>
#include <QtGui/QPushButton>
#include <QtGui/QPushButton>

#include <QTimer>

#include <iostream>

#include "locator.h"
#include "sliderNspin.h"
#include "basetranslator.h"

class InspectorWidget : public QWidget
{
    Q_OBJECT                 //  Qt magic macro
    public:
        InspectorWidget(QWidget* parent=0); //  Constructor
       ~InspectorWidget();

        void setup();
        void set_locator(Locator* l) { locator = l; };
        void set_translator(BaseTranslator* t) { translator = t; };
        void set_nx(int n);
        void set_ny(int n);
        void set_nz(int n);
        void set_x(int n) { _x = n; };
        void set_y(int n) { _y = n; };
        void set_z(int n) { _z = n; };

        void set_lon(int n);
        void set_lat(int n);

        void set_distance(float f) { _distance = f; };

    public slots:
        void selectInspector(int n);
        void selectGrid(int n);

        void to_topleft();
        void to_top();
        void to_toprite();
        void to_left();
        void to_center();
        void to_rite();
        void to_lowleft();
        void to_low();
        void to_lowrite();

        void to_far();
        void to_near();

        void increase_zfar();
        void decrease_zfar();
        void increase_znear();
        void decrease_znear();
        void increase_fovy();
        void decrease_fovy();

        void reset();

        void setValueX(int n);
        void setValueY(int n);
        void setValueZ(int n);

        void set_nvoptions(NVOptions* opt) { nvoptions = opt; };

    protected:
        Locator* locator;
        NVOptions* nvoptions;
        BaseTranslator* translator;

        QGridLayout* layout;

        QLabel* locator_msg;

        QCheckBox* inspector_cb;  // Check box for inspector
        QCheckBox* grid_cb;       // Check box for grid

        QPushButton* topleftButton;
        QPushButton* topButton;
        QPushButton* topriteButton;
        QPushButton* leftButton;
        QPushButton* centerButton;
        QPushButton* riteButton;
        QPushButton* lowleftButton;
        QPushButton* lowButton;
        QPushButton* lowriteButton;
        QPushButton* farButton;
        QPushButton* nearButton;

        SliderNSpin* sliderNspinX;
        SliderNSpin* sliderNspinY;
        SliderNSpin* sliderNspinZ;

        QSlider* xslider;
        QSlider* yslider;
        QSlider* zslider;

        QSpinBox* xspinBox;
        QSpinBox* yspinBox;
        QSpinBox* zspinBox;

        int _nx, _ny, _nz;
        int _x, _y, _z;

        float _distance;

        QPushButton* incrZfarButton;
        QPushButton* decrZfarButton;

        QPushButton* incrZnearButton;
        QPushButton* decrZnearButton;

        QPushButton* incrFovyButton;
        QPushButton* decrFovyButton;
};
#endif

