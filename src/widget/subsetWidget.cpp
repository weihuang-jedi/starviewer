// $Id: subsetWidget.cpp 5315 2015-02-06 21:24:34Z starviewer $

#include <QtGui/QApplication>

#include "subsetWidget.h"

template<typename T>
T string2number(string sn)
{
    T value;

    stringstream stream(sn);
    stream >> value;
    if(stream.fail())
    {
        cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
        cout << "\tFailed convert " << sn << " to T." << endl;
        exit (-1);
    }

    return value;
}

SubsetWidget::SubsetWidget(QWidget* parent)
            :  QWidget(parent)
{
}

SubsetWidget::~SubsetWidget()
{
    delete layout;
}

void SubsetWidget::setup()
{
  //Set layout of child widgets
    layout = new QGridLayout;

    layout->setColumnMinimumWidth(0, 2);
    layout->setColumnStretch(0, 2);

    layout->setRowStretch(0, 3);

  //==================================================================================
    subset_cb = new QCheckBox("Subset");
    subset_cb->setChecked(false);
    subset_cb->setCheckState(Qt::Unchecked);
  //connect(subset_cb, SIGNAL(stateChanged(int)), this, SLOT(selectSubset(int)));
    connect(subset_cb, SIGNAL(stateChanged(int)), translator, SLOT(selectSubset(int)));

  //-----------------------------------------------------------------------------------
  //Group Display parameters
    QGroupBox* ssbox = new QGroupBox("Subset");
    QGridLayout* sslay = new QGridLayout;

    sslay->addWidget(subset_cb, 0, 0);

    ssbox->setLayout(sslay);
    layout->addWidget(ssbox, 0, 0);

  //==================================================================================
    minlonEdit = new QLineEdit;
    minlonLabel = new QLabel(tr("Min Lon:"));
    minlonLabel->setBuddy(minlonEdit);

    maxlonEdit = new QLineEdit;
    maxlonLabel = new QLabel(tr("Max Lon:"));
    maxlonLabel->setBuddy(maxlonEdit);

    connect(minlonEdit, SIGNAL(textChanged(const QString &)),
                  this, SLOT(setMinLon(const QString &)));
    connect(maxlonEdit, SIGNAL(textChanged(const QString &)),
                  this, SLOT(setMaxLon(const QString &)));

  //-----------------------------------------------------------------------------------
    QGroupBox* lonbox = new QGroupBox("Longitude: 0 - 360");
    QGridLayout* lonlay = new QGridLayout;

    lonlay->addWidget(minlonLabel, 0, 0);
    lonlay->addWidget(minlonEdit,  0, 1);
    lonlay->addWidget(maxlonLabel, 1, 0);
    lonlay->addWidget(maxlonEdit,  1, 1);

    lonbox->setLayout(lonlay);
    layout->addWidget(lonbox, 1, 0);

  //==================================================================================
    minlatEdit = new QLineEdit;
    minlatLabel = new QLabel(tr("Min Lat:"));
    minlatLabel->setBuddy(minlatEdit);

    maxlatEdit = new QLineEdit;
    maxlatLabel = new QLabel(tr("Max Lat:"));
    maxlatLabel->setBuddy(maxlatEdit);

    connect(minlatEdit, SIGNAL(textChanged(const QString &)),
                  this, SLOT(setMinLat(const QString &)));
    connect(maxlatEdit, SIGNAL(textChanged(const QString &)),
                  this, SLOT(setMaxLat(const QString &)));

  //-----------------------------------------------------------------------------------
    QGroupBox* latbox = new QGroupBox("Latitude: 0 - 180");
    QGridLayout* latlay = new QGridLayout;

    latlay->addWidget(minlatLabel, 0, 0);
    latlay->addWidget(minlatEdit,  0, 1);
    latlay->addWidget(maxlatLabel, 1, 0);
    latlay->addWidget(maxlatEdit,  1, 1);

    latbox->setLayout(latlay);
    layout->addWidget(latbox, 2, 0);

  //==================================================================================

  //Overall layout
    setLayout(layout);
}

#if 0
//subset on/off
void SubsetWidget::selectSubset(int n)
{
  //cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">" << endl;
  //cout << "\tfile: <" << __FILE__ << ">" << ", line: " << __LINE__ << endl;
  //cout << "\tn = " << n << endl;

    if(n)
        nvoptions->set_cb(NV_SUBSET, true);
    else
        nvoptions->set_cb(NV_SUBSET, false);
}
#endif

void SubsetWidget::setMinLon(const QString &text)
{
    double val;

    val = string2number<float>(text.toStdString());

    nvoptions->set_minlon(val);

  //cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">" << endl;
  //cout << "\tfile: <" << __FILE__ << ">" << ", line: " << __LINE__ << endl;
  //cout << "\ttext: " << text.toStdString() << ", val = " << val << endl;
}

void SubsetWidget::setMaxLon(const QString &text)
{
    double val;

    val = string2number<float>(text.toStdString());

    nvoptions->set_maxlon(val);

  //cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">" << endl;
  //cout << "\tfile: <" << __FILE__ << ">" << ", line: " << __LINE__ << endl;
  //cout << "\ttext: " << text.toStdString() << ", val = " << val << endl;
}

void SubsetWidget::setMinLat(const QString &text)
{
    double val;

    val = string2number<float>(text.toStdString());

    nvoptions->set_minlat(val);

  //cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">" << endl;
  //cout << "\tfile: <" << __FILE__ << ">" << ", line: " << __LINE__ << endl;
  //cout << "\ttext: " << text.toStdString() << ", val = " << val << endl;
}

void SubsetWidget::setMaxLat(const QString &text)
{
    double val;

    val = string2number<float>(text.toStdString());

    nvoptions->set_maxlat(val);

  //cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">" << endl;
  //cout << "\tfile: <" << __FILE__ << ">" << ", line: " << __LINE__ << endl;
  //cout << "\ttext: " << text.toStdString() << ", val = " << val << endl;
}

