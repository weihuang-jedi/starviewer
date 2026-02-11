// $Id: inspectorWidget.cpp 5315 2015-02-06 21:24:34Z starviewer $

#include <QtGui/QApplication>

#include "inspectorWidget.h"

InspectorWidget::InspectorWidget(QWidget* parent) : QWidget(parent)
{
  //cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__
  //     << ", file: <" << __FILE__ << ">" << endl;

    setWindowTitle(tr("Inspector"));

    layout = new QGridLayout;

    _nx = 1;
    _ny = 1;
    _nz = 1;

    _x = 1;
    _y = 1;
    _z = 1;

    _distance = 8.0;
}

InspectorWidget::~InspectorWidget()
{
    delete layout;
}

//Reset
void InspectorWidget::reset(void)
{
    sliderNspinX->setValue(0);
    sliderNspinY->setValue(0);
    sliderNspinZ->setValue(0);
}

void InspectorWidget::set_nx(int n)
{
    int dx = (9 + n)/10;
    _nx = n;
    sliderNspinX->set_min(0);
    sliderNspinX->set_max(n);
    sliderNspinX->set_step(1);
    sliderNspinX->set_tickinterval(dx);
    sliderNspinX->setValue(0);
}

void InspectorWidget::set_ny(int n)
{
    int dy = (9 + n)/10;
    _ny = n;
    sliderNspinY->set_min(0);
    sliderNspinY->set_max(n);
    sliderNspinY->set_step(1);
    sliderNspinY->set_tickinterval(dy);
    sliderNspinY->setValue(0);
}

void InspectorWidget::set_nz(int n)
{
    int dz = (9 + n)/10;
    _nz = n;
    sliderNspinZ->set_min(0);
    sliderNspinZ->set_max(n);
    sliderNspinZ->set_step(1);
    sliderNspinZ->set_tickinterval(dz);
    sliderNspinZ->setValue(0);
}

void InspectorWidget::setup()
{
  //cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__
  //     << ", file: <" << __FILE__ << ">" << endl;

    inspector_cb  = new QCheckBox("Inspector");
    inspector_cb->setChecked(false);
    inspector_cb->setCheckState(Qt::Unchecked);

    grid_cb  = new QCheckBox("Grid");
    grid_cb->setChecked(false);
    grid_cb->setCheckState(Qt::Unchecked);

    sliderNspinX = new SliderNSpin(Qt::Horizontal, tr("X"),  this);
    sliderNspinY = new SliderNSpin(Qt::Horizontal, tr("Y"),  this);
    sliderNspinZ = new SliderNSpin(Qt::Horizontal, tr("Distance"),  this);

    set_nx(360);
    set_ny(180);
    set_nz(100);

    xslider = sliderNspinX->get_slider();
    yslider = sliderNspinY->get_slider();
    zslider = sliderNspinZ->get_slider();

    xspinBox = sliderNspinX->get_spinBox();
    yspinBox = sliderNspinY->get_spinBox();
    zspinBox = sliderNspinZ->get_spinBox();

    topleftButton = new QPushButton("TopLeft");
    topButton     = new QPushButton("Top");
    topriteButton = new QPushButton("TopRight");

    leftButton   = new QPushButton("Left");
    centerButton = new QPushButton("Center");
    riteButton   = new QPushButton("Right");

    lowleftButton = new QPushButton("LowLeft");
    lowButton     = new QPushButton("Low");
    lowriteButton = new QPushButton("LowRight");

    farButton     = new QPushButton("Farther");
    nearButton    = new QPushButton("Closer");

    connect(inspector_cb, SIGNAL(stateChanged(int)), this, SLOT(selectInspector(int)));
    connect(grid_cb,      SIGNAL(stateChanged(int)), this, SLOT(selectGrid(int)));

  //Group checkbox parameters
    QGroupBox*   inspector_box = new QGroupBox("Inspector");
    QGridLayout* inspector_lay = new QGridLayout;

    inspector_lay->addWidget(inspector_cb, 0, 0);
    inspector_lay->addWidget(grid_cb,      0, 1);

    inspector_box->setLayout(inspector_lay);
    layout->addWidget(inspector_box,0,0);

  //Group slider parameters
    QGroupBox*   slider_box = new QGroupBox("Location:");
    QGridLayout* slider_lay = new QGridLayout;
    slider_box->setLayout(slider_lay);

    connect(xslider,  SIGNAL(valueChanged(int)), this, SLOT(setValueX(int)));
    connect(xspinBox, SIGNAL(valueChanged(int)), this, SLOT(setValueX(int)));

    connect(yslider,  SIGNAL(valueChanged(int)), this, SLOT(setValueY(int)));
    connect(yspinBox, SIGNAL(valueChanged(int)), this, SLOT(setValueY(int)));

    sliderNspinX->add2layout(slider_lay, 0, 0);
    sliderNspinY->add2layout(slider_lay, 1, 0);

    layout->addWidget(slider_box,1,0);

    connect(topleftButton, SIGNAL(clicked()), this, SLOT(to_topleft()));
    connect(topButton,     SIGNAL(clicked()), this, SLOT(to_top()));
    connect(topriteButton, SIGNAL(clicked()), this, SLOT(to_toprite()));

    connect(leftButton,   SIGNAL(clicked()), this, SLOT(to_left()));
    connect(centerButton, SIGNAL(clicked()), this, SLOT(to_center()));
    connect(riteButton,   SIGNAL(clicked()), this, SLOT(to_rite()));

    connect(lowleftButton, SIGNAL(clicked()), this, SLOT(to_lowleft()));
    connect(lowButton,     SIGNAL(clicked()), this, SLOT(to_low()));
    connect(lowriteButton, SIGNAL(clicked()), this, SLOT(to_lowrite()));

  //Group navigation parameters
    QGroupBox*   navigation_box = new QGroupBox("Navagation");
    QGridLayout* navigation_lay = new QGridLayout;

    navigation_lay->addWidget(topleftButton,   0, 0);
    navigation_lay->addWidget(topButton,       0, 1);
    navigation_lay->addWidget(topriteButton,   0, 2);

    navigation_lay->addWidget(leftButton,   1, 0);
    navigation_lay->addWidget(centerButton, 1, 1);
    navigation_lay->addWidget(riteButton,   1, 2);

    navigation_lay->addWidget(lowleftButton,   2, 0);
    navigation_lay->addWidget(lowButton,       2, 1);
    navigation_lay->addWidget(lowriteButton,   2, 2);

    navigation_box->setLayout(navigation_lay);

    layout->addWidget(navigation_box,2,0);

  //Group height parameters
    QGroupBox*   height_box = new QGroupBox("Height:");
    QGridLayout* height_lay = new QGridLayout;
    height_box->setLayout(height_lay);

    connect(zslider,  SIGNAL(valueChanged(int)), this, SLOT(setValueZ(int)));
    connect(zspinBox, SIGNAL(valueChanged(int)), this, SLOT(setValueZ(int)));

    sliderNspinZ->add2layout(height_lay, 0, 0);

    layout->addWidget(height_box,3,0);

    connect(farButton,  SIGNAL(clicked()), this, SLOT(to_far()));
    connect(nearButton, SIGNAL(clicked()), this, SLOT(to_near()));

  //Group distance parameters
    QGroupBox*   distance_box = new QGroupBox("Distance");
    QGridLayout* distance_lay = new QGridLayout;

    distance_lay->addWidget(farButton,  0, 0);
    distance_lay->addWidget(nearButton, 0, 1);

    distance_box->setLayout(distance_lay);

    layout->addWidget(distance_box,4,0);

  //-------------------------------------------------------------------------
  //Group Variable Info
    QGroupBox* dspbox = new QGroupBox("Variable Info");
    QGridLayout* dsplay = new QGridLayout;

  //widget for locator_msg
    QLabel* locator_msg = new QLabel();

  //Connect translator signals to display widgets
    connect(translator, SIGNAL(locator_msg(QString)), locator_msg, SLOT(setText(QString)));

    dsplay->addWidget(locator_msg, 0,0);

    dspbox->setLayout(dsplay);
    layout->addWidget(dspbox,5,0);
#if 0
  //-------------------------------------------------------------------------
  //Group zfar parameters
    QGroupBox*   zfar_box = new QGroupBox("zfar:");
    QGridLayout* zfar_lay = new QGridLayout;
    zfar_box->setLayout(zfar_lay);

    incrZfarButton = new QPushButton("Increase");
    decrZfarButton = new QPushButton("Decrease");

    connect(incrZfarButton, SIGNAL(clicked()), this, SLOT(increase_zfar()));
    connect(decrZfarButton, SIGNAL(clicked()), this, SLOT(decrease_zfar()));

    zfar_lay->addWidget(incrZfarButton,  0, 0);
    zfar_lay->addWidget(decrZfarButton, 0, 1);

    zfar_box->setLayout(zfar_lay);

    layout->addWidget(zfar_box,6,0);

  //-------------------------------------------------------------------------
  //Group znear parameters
    QGroupBox*   znear_box = new QGroupBox("znear:");
    QGridLayout* znear_lay = new QGridLayout;
    znear_box->setLayout(znear_lay);

    incrZnearButton = new QPushButton("Increase");
    decrZnearButton = new QPushButton("Decrease");

    connect(incrZnearButton, SIGNAL(clicked()), this, SLOT(increase_znear()));
    connect(decrZnearButton, SIGNAL(clicked()), this, SLOT(decrease_znear()));

    znear_lay->addWidget(incrZnearButton,  0, 0);
    znear_lay->addWidget(decrZnearButton, 0, 1);

    znear_box->setLayout(znear_lay);

    layout->addWidget(znear_box,7,0);

  //-------------------------------------------------------------------------
  //Group fovy parameters
    QGroupBox*   fovy_box = new QGroupBox("fovy:");
    QGridLayout* fovy_lay = new QGridLayout;
    fovy_box->setLayout(fovy_lay);

    incrFovyButton = new QPushButton("Increase");
    decrFovyButton = new QPushButton("Decrease");

    connect(incrFovyButton, SIGNAL(clicked()), this, SLOT(increase_fovy()));
    connect(decrFovyButton, SIGNAL(clicked()), this, SLOT(decrease_fovy()));

    fovy_lay->addWidget(incrFovyButton,  0, 0);
    fovy_lay->addWidget(decrFovyButton, 0, 1);

    fovy_box->setLayout(fovy_lay);

    layout->addWidget(fovy_box,8,0);
#endif
  //-------------------------------------------------------------------------
    layout->setColumnStretch(0,2);
    layout->setColumnMinimumWidth(0,2);

  //layout->setRowStretch(0,10);

  //Overall layout
    setLayout(layout);
}

void InspectorWidget::selectInspector(int n)
{
  //cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__
  //     << ", file: <" << __FILE__ << ">" << endl;

    if(n)
    {
        nvoptions->set_cb(NV_INSPECTORON, true);
        locator->turnOn();
    }
    else
    {
        nvoptions->set_cb(NV_INSPECTORON, false);
        locator->turnOff();
    }
}

void InspectorWidget::selectGrid(int n)
{
  //cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__
  //     << ", file: <" << __FILE__ << ">" << endl;

    if(n)
    {
        nvoptions->set_cb(NV_GRIDON, true);
    }
    else
    {
        nvoptions->set_cb(NV_GRIDON, false);
    }
}

void InspectorWidget::setValueX(int n)
{
    _x = n;

  //if(0 > _x)
  //    _x += _nx;
  //if(_nx <= _x)
  //    _x -= _nx;

    locator->set_x(_x);

    translator->update_frame();
}

void InspectorWidget::setValueY(int n)
{
    _y = n;

  //if(0 > _y)
  //    _y += _ny;
  //if(_ny <= _y)
  //    _y -= _ny;

    locator->set_y(_y);

    translator->update_frame();
}

void InspectorWidget::setValueZ(int n)
{
    _z = n;

    locator->set_z(_z);

    translator->update_frame();
}

void InspectorWidget::to_left()
{
    --_x;

  //if(0 > _x)
  //    _x += _nx;

    locator->set_x(_x);

    translator->update_frame();
}

void InspectorWidget::to_rite()
{
    ++_x;

  //if(_nx <= _x)
  //    _x -= _nx;

    locator->set_x(_x);

    translator->update_frame();
}

void InspectorWidget::to_low()
{
    --_y;

  //if(0 > _y)
  //    _y += _ny;

    locator->set_y(_y);

    translator->update_frame();
}

void InspectorWidget::to_top()
{
    ++_y;

  //if(_ny <= _y)
  //    _y -= _ny;

    locator->set_y(_y);

    translator->update_frame();
}

void InspectorWidget::to_topleft()
{
    to_left();
    to_top();
}

void InspectorWidget::to_toprite()
{
    to_rite();
    to_top();
}


void InspectorWidget::to_lowleft()
{
    to_left();
    to_low();
}

void InspectorWidget::to_lowrite()
{
    to_rite();
    to_low();
}

void InspectorWidget::to_center()
{
    _x = _nx/2;
  //_y = _ny/2;
    _y = 0;

    locator->set_x(_x);
    locator->set_y(_y);

    translator->update_frame();
}

void InspectorWidget::to_far()
{
    --_z;

    if(0 > _z)
        _z = 0;

    locator->set_z(_z);

    translator->update_frame();
}

void InspectorWidget::to_near()
{
    ++_z;

  //if(_nz <= _z)
  //    _z -= _nz;

    locator->set_z(_z);

    translator->update_frame();
}

void InspectorWidget::increase_zfar()
{
    locator->set_zfar(1.01*locator->get_zfar());

    translator->update_frame();
}

void InspectorWidget::decrease_zfar()
{
    locator->set_zfar(0.99*locator->get_zfar());

    translator->update_frame();
}

void InspectorWidget::increase_znear()
{
    locator->set_znear(1.01*locator->get_znear());

    translator->update_frame();
}

void InspectorWidget::decrease_znear()
{
    locator->set_znear(0.99*locator->get_znear());

    translator->update_frame();
}

void InspectorWidget::increase_fovy()
{
    locator->set_fovy(1.01*locator->get_fovy());

    translator->update_frame();
}

void InspectorWidget::decrease_fovy()
{
    locator->set_fovy(0.99*locator->get_fovy());

    translator->update_frame();
}

void InspectorWidget::set_lon(int n)
{
    int dx = (9 + n)/10;
    _nx = n;
    sliderNspinX->set_min(0);
    sliderNspinX->set_max(n);
    sliderNspinX->set_step(1);
    sliderNspinX->set_tickinterval(dx);
    sliderNspinX->setValue(0);
}

void InspectorWidget::set_lat(int n)
{
    int dy = (9 + n)/10;
    _ny = n;
    sliderNspinY->set_min(-n/2);
    sliderNspinY->set_max(n/2);
    sliderNspinY->set_step(1);
    sliderNspinY->set_tickinterval(dy);
    sliderNspinY->setValue(0);
}

