// $Id: minmaxWidget.cpp 5315 2015-02-06 21:24:34Z starviewer $

#include <QtGui/QApplication>

#include "minmaxWidget.h"

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

MinMaxWidget::MinMaxWidget(QWidget* parent)
            :  QWidget(parent)
{
}

MinMaxWidget::~MinMaxWidget()
{
}

void MinMaxWidget::setup()
{
  //Set layout of child widgets
    layout = new QGridLayout;

    layout->setColumnMinimumWidth(0, 2);
    layout->setColumnStretch(0, 2);

    layout->setRowStretch(0, 3);

  //==================================================================================
    minmax_cb = new QCheckBox("MinMax");
    minmax_cb->setChecked(false);
    minmax_cb->setCheckState(Qt::Unchecked);
    connect(minmax_cb, SIGNAL(stateChanged(int)), translator, SLOT(selectMinMax(int)));

  //-----------------------------------------------------------------------------------
  //Group Display parameters
    QGroupBox* mmbox = new QGroupBox("User Define Minimum and Maximum");
    QGridLayout* mmlay = new QGridLayout;

    mmlay->addWidget(minmax_cb, 0, 0);

    mmbox->setLayout(mmlay);
    layout->addWidget(mmbox, 0, 0);

  //==================================================================================
    minEdit = new QLineEdit;
    minLabel = new QLabel(tr("Minimum:"));
    minLabel->setBuddy(minEdit);

    maxEdit = new QLineEdit;
    maxLabel = new QLabel(tr("Maximum:"));
    maxLabel->setBuddy(maxEdit);

    connect(minEdit, SIGNAL(textChanged(const QString &)),
               this, SLOT(setMin(const QString &)));
    connect(maxEdit, SIGNAL(textChanged(const QString &)),
               this, SLOT(setMax(const QString &)));

  //-----------------------------------------------------------------------------------
    QGroupBox* minbox = new QGroupBox("Minimum");
    QGridLayout* minlay = new QGridLayout;

    minlay->addWidget(minLabel, 0, 0);
    minlay->addWidget(minEdit,  0, 1);

    minbox->setLayout(minlay);
    layout->addWidget(minbox, 1, 0);

  //==================================================================================
    QGroupBox* maxbox = new QGroupBox("Maximum");
    QGridLayout* maxlay = new QGridLayout;

    maxlay->addWidget(maxLabel, 0, 0);
    maxlay->addWidget(maxEdit,  0, 1);

    maxbox->setLayout(maxlay);
    layout->addWidget(maxbox, 2, 0);

  //==================================================================================

  //Overall layout
    setLayout(layout);
}

void MinMaxWidget::setMin(const QString &text)
{
    double val;

    val = string2number<float>(text.toStdString());

    nvoptions->set_userminimum(val);

  //cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">" << endl;
  //cout << "\tfile: <" << __FILE__ << ">" << ", line: " << __LINE__ << endl;
  //cout << "\ttext: " << text.toStdString() << ", val = " << val << endl;
}

void MinMaxWidget::setMax(const QString &text)
{
    double val;

    val = string2number<float>(text.toStdString());

    nvoptions->set_usermaximum(val);

  //cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">" << endl;
  //cout << "\tfile: <" << __FILE__ << ">" << ", line: " << __LINE__ << endl;
  //cout << "\ttext: " << text.toStdString() << ", val = " << val << endl;
}

void MinMaxWidget::set_translator(BaseTranslator* t)
{
    translator = t;
    nvoptions = translator->get_nvoptions();
}

