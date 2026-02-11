#include <QtGui/QApplication>

#include "displayWidget.h"

//Constructor for Viewer widget
DisplayWidget::DisplayWidget(QWidget* parent) : QWidget(parent)
{
  //Set layout of child widgets
    layout = new QGridLayout;
}

DisplayWidget::~DisplayWidget()
{
    delete layout;
}

void DisplayWidget::setup()
{
  //Set window title
  //setWidgetTitle(tr("Display"));

  //Widget widget
    layout->addWidget(translator,0,0);

    layout->setColumnStretch(0,15);
    layout->setColumnMinimumWidth(0,5);

    layout->setRowStretch(0,10);

  //Overall layout
    setLayout(layout);
}

