#include "dataviewWidget.h"

DataViewWidget::DataViewWidget(QWidget* parent)
              :  QWidget(parent)
{
}

DataViewWidget::~DataViewWidget()
{
    delete layout;
}

void DataViewWidget::setup()
{
  //Set layout of child widgets
    layout = new QGridLayout;

  //Group Display parameters
    QGroupBox* dv_box = new QGroupBox("Display");
    QGridLayout* dv_lay = new QGridLayout;

    dataview_cb = new QCheckBox("Dataview");
    dataview_cb->setChecked(false);
    dataview_cb->setCheckState(Qt::Unchecked);

    clear_pb = new QPushButton("Clear");
    clear_pb->setChecked(false);

    connect(dataview_cb, SIGNAL(stateChanged(int)), translator, SLOT(selectDataview(int)));
    connect(clear_pb,    SIGNAL(clicked()),         translator, SLOT(selectClearDataview()));

    dv_lay->addWidget(dataview_cb, 0, 0);
    dv_lay->addWidget(clear_pb,    0, 1);

    dv_box->setLayout(dv_lay);
    layout->addWidget(dv_box, 0, 0);

    layout->setColumnMinimumWidth(0, 2);
    layout->setColumnStretch(0, 2);

    layout->setRowStretch(0, 2);

  //Overall layout
    setLayout(layout);
}

