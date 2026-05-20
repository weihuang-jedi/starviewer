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
  //cout << "\nEnter function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
  //Set window title
  //setWidgetTitle(tr("Display"));

  //Widget widget
    layout->addWidget(translator,0,0);

  //cout << "\tfunction: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;

    layout->setColumnStretch(0,15);
    layout->setColumnMinimumWidth(0,5);

  //cout << "\tfunction: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;

    layout->setRowStretch(0,10);

  //cout << "\tfunction: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;

  //Overall layout
    setLayout(layout);
  //cout << "Leave function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
}

