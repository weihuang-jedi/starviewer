#include "licWidget.h"

LICWidget::LICWidget(QWidget* parent) : QWidget(parent)
{
}

LICWidget::~LICWidget()
{
    delete layout;
}

void LICWidget::setup()
{
  //Set layout of child widgets
    layout = new QGridLayout;

  //Group Display parameters
    QGroupBox* licbox = new QGroupBox("Display");
    QGridLayout* liclay = new QGridLayout;

    lic_cb = new QCheckBox("LIC");
    lic_cb->setChecked(false);
    lic_cb->setCheckState(Qt::Unchecked);
    connect(lic_cb, SIGNAL(stateChanged(int)), translator, SLOT(selectLIC(int)));

    lic_only_cb = new QCheckBox("licOnly");
    lic_only_cb->setChecked(false);
    lic_only_cb->setCheckState(Qt::Unchecked);
    connect(lic_only_cb, SIGNAL(stateChanged(int)), translator, SLOT(selectLIConly(int)));

    lic_3D_cb = new QCheckBox("lic3D");
    lic_3D_cb->setChecked(false);
    lic_3D_cb->setCheckState(Qt::Unchecked);
    connect(lic_3D_cb, SIGNAL(stateChanged(int)), translator, SLOT(selectLIC3D(int)));
    
    liclay->addWidget(lic_cb,      0, 0);
    liclay->addWidget(lic_only_cb, 0, 1);
    liclay->addWidget(lic_3D_cb,   0, 2);

    licbox->setLayout(liclay);
    layout->addWidget(licbox,0,0);

    layout->setColumnMinimumWidth(0,2);
    layout->setColumnStretch(0,2);

    layout->setRowStretch(0,2);

  //Overall layout
    setLayout(layout);
}

