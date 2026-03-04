#include "isosurfaceWidget.h"

IsoSurfaceWidget::IsoSurfaceWidget(QWidget* parent)
                :  QWidget(parent)
{
}

IsoSurfaceWidget::~IsoSurfaceWidget()
{
    delete layout;
}

void IsoSurfaceWidget::setup()
{
  //Set layout of child widgets
    layout = new QGridLayout;

  //Group Display parameters
    QGroupBox* isobox = new QGroupBox("Display");
    QGridLayout* isolay = new QGridLayout;

    isosurface_cb = new QCheckBox("IsoSurface");
    isosurface_cb->setChecked(false);
    isosurface_cb->setCheckState(Qt::Unchecked);
    connect(isosurface_cb, SIGNAL(stateChanged(int)), translator, SLOT(selectIsoSurface(int)));

    iso_only_cb = new QCheckBox("IsoOnly");
    iso_only_cb->setChecked(false);
    iso_only_cb->setCheckState(Qt::Unchecked);
    connect(iso_only_cb, SIGNAL(stateChanged(int)), translator, SLOT(selectIsoOnly(int)));

    isolay->addWidget(isosurface_cb, 0,0);
    isolay->addWidget(iso_only_cb,   0,1);

    isobox->setLayout(isolay);
    layout->addWidget(isobox,0,0);

    layout->setColumnMinimumWidth(0,2);
    layout->setColumnStretch(0,2);

    layout->setRowStretch(0,2);

  //Overall layout
    setLayout(layout);
}

