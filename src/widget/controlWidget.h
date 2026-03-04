#ifndef _ControlWidget_H
#define _ControlWidget_H

// $Id: controlWidget.h 5315 2015-02-06 21:24:34Z starviewer $

#include <QtGui/QtGui>
#include <QtGui/QWidget>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QCheckBox>
#include <QtGui/QPushButton>
#include <QtGui/QPushButton>

#include <QTimer>

#include <iostream>

#include "basetranslator.h"
#include "colorTable.h"

class ControlWidget : public QWidget
{
    Q_OBJECT                 //  Qt magic macro
    public:
        ControlWidget(QWidget* parent=0); //  Constructor
       ~ControlWidget();

        virtual void setup();

        void set_colortable(ColorTable* ct) { colorTable = ct; };
        void set_translator(BaseTranslator* t) { translator = t; };

        void disable_ncl() { ncl_cb->setEnabled(false); };
        void disable_bump() { bump_cb->setEnabled(false); };
        void disable_sphere() { sphere_cb->setEnabled(false); };
        void disable_flat() { flat_cb->setEnabled(false); };
        void disable_onmap() { onmap_cb->setEnabled(false); };
      //void disable_surface() { surface_cb->setEnabled(false); };
        void disable_title() { title_cb->setEnabled(false); };
        void disable_coastline() { coastline_cb->setEnabled(false); };
        void disable_x2() { x2_cb->setEnabled(false); };
        void disable_y2() { y2_cb->setEnabled(false); };
        void disable_z2() { z2_cb->setEnabled(false); };
        void disable_t2() { t2_cb->setEnabled(false); };

        void selectNCL();

    protected:
        BaseTranslator* translator;
        ColorTable* colorTable;

      //Set layout of child widgets
        QGridLayout* layout;

        QCheckBox* proj_cb;  // Check box for projection
        QCheckBox* axis_cb;  // Check box for axis
        QCheckBox* ncl_cb;   // Check box for NCL

        QCheckBox* bump_cb;       // Check box for bump
        QCheckBox* surface_cb;    // Check box for surface
        QCheckBox* sphere_cb;     // Check box for sphere
        QCheckBox* flat_cb;       // Check box for flat
        QCheckBox* bgBW_cb;       // Check box for background color
        QCheckBox* onmap_cb;      // Check box for on-map
        QCheckBox* title_cb;      // Check box for title
        QCheckBox* coastline_cb;  // Check box for coastline

        QCheckBox* x2_cb;  // Check box for X2
        QCheckBox* y2_cb;  // Check box for Y2
        QCheckBox* z2_cb;  // Check box for Z2
        QCheckBox* t2_cb;  // Check box for T2

        QCheckBox* colorbar_cb;  // Check box for colorbar

        QPushButton* save_image;
        QPushButton* reset;
        QLabel* info;

        SliderNSpin* sliderNspinX;
        SliderNSpin* sliderNspinY;
        SliderNSpin* sliderNspinZ;
        SliderNSpin* sliderNspinT;

        QSlider* xslider;
        QSlider* yslider;
        QSlider* zslider;
        QSlider* tslider;

        QSpinBox* xspinBox;
        QSpinBox* yspinBox;
        QSpinBox* zspinBox;
        QSpinBox* tspinBox;

        void _setup_buttons();
        void _setup_colormap();
        void _setup_variables();
        void _setup_sliderNspin();
        void _setup_message();
};
#endif

