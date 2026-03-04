#include "clipWidget.h"

//Constructor for clip widget
ClipWidget::ClipWidget(QWidget* parent) : QWidget(parent)
{
  //Set layout of child widgets
    layout = new QGridLayout;

    cliptype = NUMBEROFCLIPS;
}

ClipWidget::~ClipWidget()
{
    delete sliderNspinA;
    delete sliderNspinB;
    delete sliderNspinC;
    delete sliderNspinD;

    delete layout;

    delete sliderNspinA;
    delete sliderNspinB;
    delete sliderNspinC;
    delete sliderNspinD;

    delete clip_cb;

    delete west_cb;
    delete east_cb;
    delete south_cb;
    delete north_cb;
    delete bottom_cb;
    delete top_cb;

    delete west_rb;
    delete east_rb;
    delete south_rb;
    delete north_rb;
    delete bottom_rb;
    delete top_rb;
}

void ClipWidget::setup()
{
    int i, j;

    for(j = 0; j < NUMBEROFCLIPS; ++j)
        for(i = 0; i < 4; ++i)
            clipequation[j][i] = 0.0;

    clipequation[0][0] =  1.0;
    clipequation[1][0] = -1.0;
    clipequation[2][1] =  1.0;
    clipequation[3][1] = -1.0;
    clipequation[4][2] =  1.0;
    clipequation[5][2] = -1.0;

    sliderNspinA = new SliderNSpin(Qt::Horizontal, tr("A"),  this);
    sliderNspinB = new SliderNSpin(Qt::Horizontal, tr("B"),  this);
    sliderNspinC = new SliderNSpin(Qt::Horizontal, tr("C"),  this);
    sliderNspinD = new SliderNSpin(Qt::Horizontal, tr("D"),  this);

    sliderNspinA->set_min(0);
    sliderNspinA->set_max(200);
    sliderNspinA->set_step(1);
    sliderNspinA->set_tickinterval(10);

    aslider = sliderNspinA->get_slider();
    aspinBox = sliderNspinA->get_spinBox();

    sliderNspinB->set_min(0);
    sliderNspinB->set_max(200);
    sliderNspinB->set_step(1);
    sliderNspinB->set_tickinterval(10);

    bslider = sliderNspinB->get_slider();
    bspinBox = sliderNspinB->get_spinBox();

    sliderNspinC->set_min(0);
    sliderNspinC->set_max(200);
    sliderNspinC->set_step(1);
    sliderNspinC->set_tickinterval(10);

    cslider = sliderNspinC->get_slider();
    cspinBox = sliderNspinC->get_spinBox();

    sliderNspinD->set_min(0);
    sliderNspinD->set_max(200);
    sliderNspinD->set_step(1);
    sliderNspinD->set_tickinterval(10);

    dslider = sliderNspinD->get_slider();
    dspinBox = sliderNspinD->get_spinBox();

    clip_cb = new QCheckBox("Clip");
    clip_cb->setChecked(false);
    clip_cb->setCheckState(Qt::Unchecked);

    west_cb   = new QCheckBox("West");
    clip_cb->setChecked(false);
    clip_cb->setCheckState(Qt::Unchecked);

    east_cb   = new QCheckBox("East");
    east_cb->setChecked(false);
    east_cb->setCheckState(Qt::Unchecked);

    south_cb  = new QCheckBox("South");
    south_cb->setChecked(false);
    south_cb->setCheckState(Qt::Unchecked);

    north_cb  = new QCheckBox("North");
    north_cb->setChecked(false);
    north_cb->setCheckState(Qt::Unchecked);

    bottom_cb = new QCheckBox("Bottom");
    bottom_cb->setChecked(false);
    bottom_cb->setCheckState(Qt::Unchecked);

    top_cb    = new QCheckBox("Top");
    top_cb->setChecked(false);
    top_cb->setCheckState(Qt::Unchecked);

    connect(clip_cb,  SIGNAL(stateChanged(int)), this, SLOT(selectClipOn(int)));

    connect(west_cb,   SIGNAL(toggled(bool)), this, SLOT(switchWest(bool)));
    connect(east_cb,   SIGNAL(toggled(bool)), this, SLOT(switchEast(bool)));
    connect(south_cb,  SIGNAL(toggled(bool)), this, SLOT(switchSouth(bool)));
    connect(north_cb,  SIGNAL(toggled(bool)), this, SLOT(switchNorth(bool)));
    connect(bottom_cb, SIGNAL(toggled(bool)), this, SLOT(switchBottom(bool)));
    connect(top_cb,    SIGNAL(toggled(bool)), this, SLOT(switchTop(bool)));

    west_rb   = new QRadioButton("West");
    east_rb   = new QRadioButton("East");
    south_rb  = new QRadioButton("South");
    north_rb  = new QRadioButton("North");
    bottom_rb = new QRadioButton("Bottom");
    top_rb    = new QRadioButton("Top");

    west_rb->setChecked(true);
    east_rb->setChecked(false);
    south_rb->setChecked(false);
    north_rb->setChecked(false);
    bottom_rb->setChecked(false);
    top_rb->setChecked(false);

    connect(west_rb,   SIGNAL(toggled(bool)), this, SLOT(selectWest(bool)));
    connect(east_rb,   SIGNAL(toggled(bool)), this, SLOT(selectEast(bool)));
    connect(south_rb,  SIGNAL(toggled(bool)), this, SLOT(selectSouth(bool)));
    connect(north_rb,  SIGNAL(toggled(bool)), this, SLOT(selectNorth(bool)));
    connect(bottom_rb, SIGNAL(toggled(bool)), this, SLOT(selectBottom(bool)));
    connect(top_rb,    SIGNAL(toggled(bool)), this, SLOT(selectTop(bool)));

  //Group checkbox parameters
    QGroupBox*   clip_box = new QGroupBox("Clip");
    QGridLayout* clip_lay = new QGridLayout;

    clip_lay->addWidget(clip_cb, 0, 0);

    clip_box->setLayout(clip_lay);
    layout->addWidget(clip_box, 0, 0);

  //Group checkbox parameters
    QGroupBox*   opt_box = new QGroupBox("Select Clips");
    QGridLayout* opt_lay = new QGridLayout;

  //opt_box->setCheckable(true);
  //opt_box->setChecked(false);

    opt_lay->addWidget(west_cb,   0, 0);
    opt_lay->addWidget(east_cb,   0, 1);

    opt_lay->addWidget(south_cb,  1, 0);
    opt_lay->addWidget(north_cb,  1, 1);

    opt_lay->addWidget(bottom_cb, 2, 0);
    opt_lay->addWidget(top_cb,    2, 1);

    opt_box->setLayout(opt_lay);
    layout->addWidget(opt_box, 1, 0);

  //Group checkbox parameters
    QGroupBox*   set_box = new QGroupBox("Setup Clip");
    QGridLayout* set_lay = new QGridLayout;

  //set_box->setCheckable(true);
  //set_box->setChecked(false);

    set_lay->addWidget(west_rb,   0, 0);
    set_lay->addWidget(east_rb,   0, 1);

    set_lay->addWidget(south_rb,  1, 0);
    set_lay->addWidget(north_rb,  1, 1);

    set_lay->addWidget(bottom_rb, 2, 0);
    set_lay->addWidget(top_rb,    2, 1);

    set_box->setLayout(set_lay);
    layout->addWidget(set_box, 2, 0);

  //Group slider parameters
    QGroupBox*   slider_box = new QGroupBox("Parameters");
    QGridLayout* slider_lay = new QGridLayout;
    slider_box->setLayout(slider_lay);

    connect(aslider,  SIGNAL(valueChanged(int)), this, SLOT(setValueA(int)));
    connect(aspinBox, SIGNAL(valueChanged(int)), this, SLOT(setValueA(int)));

    connect(bslider,  SIGNAL(valueChanged(int)), this, SLOT(setValueB(int)));
    connect(bspinBox, SIGNAL(valueChanged(int)), this, SLOT(setValueB(int)));

    connect(cslider,  SIGNAL(valueChanged(int)), this, SLOT(setValueC(int)));
    connect(cspinBox, SIGNAL(valueChanged(int)), this, SLOT(setValueC(int)));

    connect(dslider,  SIGNAL(valueChanged(int)), this, SLOT(setValueD(int)));
    connect(dspinBox, SIGNAL(valueChanged(int)), this, SLOT(setValueD(int)));

    sliderNspinA->add2layout(slider_lay, 0, 0);
    sliderNspinB->add2layout(slider_lay, 1, 0);
    sliderNspinC->add2layout(slider_lay, 2, 0);
    sliderNspinD->add2layout(slider_lay, 3, 0);

    layout->addWidget(slider_box, 3, 0);

  //_setup_message();

    layout->setColumnStretch(0, 4);
    layout->setColumnMinimumWidth(0, 4);

    layout->setRowStretch(0, 3);

  //Overall layout
    setLayout(layout);
}

//ClipOn on/off
void ClipWidget::selectClipOn(int n)
{
    if(n)
        nvoptions->set_cb(NV_CLIP_ON, true);
    else
        nvoptions->set_cb(NV_CLIP_ON, false);
}

void ClipWidget::selectWest(bool v)
{
    if(v)
       cliptype = WESTCLIP;
    else
       cliptype = NUMBEROFCLIPS;
}

void ClipWidget::selectEast(bool v)
{
    if(v)
        cliptype = EASTCLIP;
    else
        cliptype = NUMBEROFCLIPS;
}

void ClipWidget::selectSouth(bool v)
{
    if(v)
        cliptype = SOUTHCLIP;
    else
        cliptype = NUMBEROFCLIPS;
}

void ClipWidget::selectNorth(bool v)
{
    if(v)
        cliptype = NORTHCLIP;
    else
        cliptype = NUMBEROFCLIPS;
}

void ClipWidget::selectBottom(bool v)
{
    if(v)
        cliptype = BOTTOMCLIP;
    else
        cliptype = NUMBEROFCLIPS;
}

void ClipWidget::selectTop(bool v)
{
    if(v)
        cliptype = TOPCLIP;
    else
        cliptype = NUMBEROFCLIPS;
}

void ClipWidget::setValueA(int n)
{
    if(NUMBEROFCLIPS != cliptype)
        clipequation[cliptype][0] = 0.01*n - 1.0;
}

void ClipWidget::setValueB(int n)
{
    if(NUMBEROFCLIPS != cliptype)
        clipequation[cliptype][1] = 0.01*n - 1.0;
}

void ClipWidget::setValueC(int n)
{
    if(NUMBEROFCLIPS != cliptype)
        clipequation[cliptype][2] = 0.01*n - 1.0;
}

void ClipWidget::setValueD(int n)
{
    if(NUMBEROFCLIPS != cliptype)
        clipequation[cliptype][3] = 0.01*n - 1.0;
}

