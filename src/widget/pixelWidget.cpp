#include "pixelWidget.h"

//Constructor for Viewer widget
PixelWidget::PixelWidget(QWidget* parent) : QWidget(parent)
{
  //Set layout of child widgets
    layout = new QGridLayout;

    nvoptions = NULL;
}

PixelWidget::~PixelWidget()
{
    delete mouse_msg;
    delete sliderNspinN;
    delete layout;
}

void PixelWidget::setup()
{
    sliderNspinN = new SliderNSpin(Qt::Horizontal, tr("NumbPass"),  this);

    sliderNspinN->set_min(0);
    sliderNspinN->set_max(100);
    sliderNspinN->set_step(1);
    sliderNspinN->set_tickinterval(10);

    nslider = sliderNspinN->get_slider();
    nspinBox = sliderNspinN->get_spinBox();

    pixelmap_cb = new QCheckBox("PixelMap");
    pixelmap_cb->setChecked(false);
    pixelmap_cb->setCheckState(Qt::Unchecked);

    copy_cb      = new QRadioButton("Copy");
    smooth_cb    = new QRadioButton("Smooth");
    sharpen_cb   = new QRadioButton("Sharpen");
    blur_cb      = new QRadioButton("Blur");
    erosion_cb   = new QRadioButton("Erosion");
    dilation_cb  = new QRadioButton("Dilation");
    laplacian_cb = new QRadioButton("Laplacian");
    prewitt_cb   = new QRadioButton("Prewitt");
    sobel_cb     = new QRadioButton("Sobel");
    bNw_cb       = new QRadioButton("BlackWhite");

    copy_cb->setChecked(true);
    smooth_cb->setChecked(false);
    sharpen_cb->setChecked(false);
    blur_cb->setChecked(false);
    erosion_cb->setChecked(false);
    dilation_cb->setChecked(false);
    laplacian_cb->setChecked(false);
    prewitt_cb->setChecked(false);
    sobel_cb->setChecked(false);
    bNw_cb->setChecked(false);

    connect(pixelmap_cb,  SIGNAL(stateChanged(int)), this, SLOT(selectPixelOn(int)));

    connect(copy_cb,      SIGNAL(toggled(bool)), this, SLOT(selectCopy(bool)));
    connect(smooth_cb,    SIGNAL(toggled(bool)), this, SLOT(selectSmooth(bool)));
    connect(sharpen_cb,   SIGNAL(toggled(bool)), this, SLOT(selectSharpen(bool)));
    connect(blur_cb,      SIGNAL(toggled(bool)), this, SLOT(selectBlur(bool)));
    connect(erosion_cb,   SIGNAL(toggled(bool)), this, SLOT(selectErosion(bool)));
    connect(dilation_cb,  SIGNAL(toggled(bool)), this, SLOT(selectDilation(bool)));
    connect(laplacian_cb, SIGNAL(toggled(bool)), this, SLOT(selectLaplacian(bool)));
    connect(prewitt_cb,   SIGNAL(toggled(bool)), this, SLOT(selectPrewitt(bool)));
    connect(sobel_cb,     SIGNAL(toggled(bool)), this, SLOT(selectSobel(bool)));
    connect(bNw_cb,       SIGNAL(toggled(bool)), this, SLOT(selectBlackWhite(bool)));

  //Group checkbox parameters
    QGroupBox*   pix_box = new QGroupBox("PixelMap");
    QGridLayout* pix_lay = new QGridLayout;

    pix_lay->addWidget(pixelmap_cb, 0, 0);

    pix_box->setLayout(pix_lay);
    layout->addWidget(pix_box, 0, 0);

  //Group checkbox parameters
    QGroupBox*   opt_box = new QGroupBox("Options");
    QGridLayout* opt_lay = new QGridLayout;

  //opt_box->setCheckable(true);
  //opt_box->setChecked(false);

    opt_lay->addWidget(copy_cb,      0, 0);

    opt_lay->addWidget(smooth_cb,    1, 0);
    opt_lay->addWidget(sharpen_cb,   1, 1);
    opt_lay->addWidget(blur_cb,      1, 2);

    opt_lay->addWidget(erosion_cb,   2, 0);
    opt_lay->addWidget(dilation_cb,  2, 1);
    opt_lay->addWidget(laplacian_cb, 2, 2);

    opt_lay->addWidget(prewitt_cb,   3, 0);
    opt_lay->addWidget(sobel_cb,     3, 1);
    opt_lay->addWidget(bNw_cb,       3, 2);

    opt_box->setLayout(opt_lay);
    layout->addWidget(opt_box, 1, 0);

  //Group slider parameters
    QGroupBox*   slider_box = new QGroupBox("Pass:");
    QGridLayout* slider_lay = new QGridLayout;
    slider_box->setLayout(slider_lay);

    connect(nslider,  SIGNAL(valueChanged(int)), this, SLOT(setValueN(int)));
    connect(nspinBox, SIGNAL(valueChanged(int)), this, SLOT(setValueN(int)));

    sliderNspinN->add2layout(slider_lay, 0, 0);

    layout->addWidget(slider_box, 2, 0);

    _setup_message();

    layout->setColumnStretch(0, 2);
    layout->setColumnMinimumWidth(0, 2);

    layout->setRowStretch(0, 5);

  //Overall layout
    setLayout(layout);
}

//PixelOn on/off
void PixelWidget::selectPixelOn(int f)
{
    if(f)
        nvoptions->set_cb(NV_PIXELON, true);
    else
        nvoptions->set_cb(NV_PIXELON, false);
}

//copy on/off
void PixelWidget::selectCopy(bool v)
{
    nvoptions->set_cb(NV_COPYON, v);
}

//smooth on/off
void PixelWidget::selectSmooth(bool v)
{
    nvoptions->set_cb(NV_SMOOTHON, v);
}

//sharpen on/off
void PixelWidget::selectSharpen(bool v)
{
    nvoptions->set_cb(NV_SHARPENON, v);
}

//blur on/off
void PixelWidget::selectBlur(bool v)
{
    nvoptions->set_cb(NV_BLURON, v);
}

//erosion on/off
void PixelWidget::selectErosion(bool v)
{
    nvoptions->set_cb(NV_EROSIONON, v);
}

//dilation on/off
void PixelWidget::selectDilation(bool v)
{
    nvoptions->set_cb(NV_DILATIONON, v);
}

//laplacian on/off
void PixelWidget::selectLaplacian(bool v)
{
    nvoptions->set_cb(NV_LAPLACIANON, v);
}

//prewitt on/off
void PixelWidget::selectPrewitt(bool v)
{
    nvoptions->set_cb(NV_PREWITTON, v);
}

//Sobel on/off
void PixelWidget::selectSobel(bool v)
{
    nvoptions->set_cb(NV_SOBELON, v);
}

//BlackWhite on/off
void PixelWidget::selectBlackWhite(bool v)
{
    nvoptions->set_cb(NV_BLACKWHITEON, v);
}

void PixelWidget::setValueN(int n)
{
    nvoptions->set_ShaderPasses(n);
}

void PixelWidget::_setup_message()
{
  //-------------------------------------------------------------------------
  //Group Variable Info
    QGroupBox* msgbox = new QGroupBox("Variable Info");
    QGridLayout* msglay = new QGridLayout;

  //widget for mouse_msg
    mouse_msg = new QLabel();

  //Connect translator signals to display widgets
    connect(translator, SIGNAL(mouse_msg(QString)), mouse_msg, SLOT(setText(QString)));

    msglay->addWidget(mouse_msg, 0,0);

    msgbox->setLayout(msglay);
    layout->addWidget(msgbox,3,0);
}

