#ifndef _PixelWidget_H
#define _PixelWidget_H

#include <QtGui/QtGui>
#include <QtGui/QWidget>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QCheckBox>
#include <QtGui/QPushButton>
#include <QtGui/QPushButton>

#include <QTimer>

#include <iostream>

#include "nvoptions.h"
#include "basetranslator.h"

class PixelWidget : public QWidget
{
    Q_OBJECT                 //  Qt magic macro
    public:
        PixelWidget(QWidget* parent=0); //  Constructor
       ~PixelWidget();

        void set_translator(BaseTranslator *t) { translator = t; };
        void set_nvoptions(NVOptions *o) { nvoptions = o; };
        void setup();

    public slots:
        void selectPixelOn(int f);

        void selectCopy(bool v);
        void selectSmooth(bool v);
        void selectSharpen(bool v);
        void selectBlur(bool v);
        void selectErosion(bool v);
        void selectDilation(bool v);
        void selectLaplacian(bool v);
        void selectPrewitt(bool v);
        void selectSobel(bool v);
        void selectBlackWhite(bool v);

        void setValueN(int n);

    private:
        QGridLayout* layout;
        QLabel* mouse_msg;

        NVOptions *nvoptions;
        BaseTranslator *translator;

        QCheckBox *pixelmap_cb; // Check box for pixelmap

        QRadioButton *copy_cb;      // Check box for copy
        QRadioButton *smooth_cb;    // Check box for smooth
        QRadioButton *sharpen_cb;   // Check box for sharpen
        QRadioButton *blur_cb;      // Check box for blur
        QRadioButton *erosion_cb;   // Check box for erosion
        QRadioButton *dilation_cb;  // Check box for dilation
        QRadioButton *laplacian_cb; // Check box for laplacian
        QRadioButton *prewitt_cb;   // Check box for prewitt
        QRadioButton *sobel_cb;     // Check box for sobel
        QRadioButton *bNw_cb;       // Check box for bNw

        SliderNSpin *sliderNspinN;
        QSlider  *nslider;
        QSpinBox *nspinBox;

        void _setup_message();
};
#endif

