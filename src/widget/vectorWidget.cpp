#include "vectorWidget.h"

VectorWidget::VectorWidget(QWidget* parent)
            :  QWidget(parent)
{
    translator = NULL;
}

VectorWidget::~VectorWidget()
{
    delete layout;
}

void VectorWidget::setup()
{
  //Set layout of child widgets
    layout = new QGridLayout;

  //Group Display parameters
    QGroupBox* vecbox = new QGroupBox("Display");
    QGridLayout* veclay = new QGridLayout;

    vector_cb = new QCheckBox("Vector");
    vector_cb->setChecked(false);
    vector_cb->setCheckState(Qt::Unchecked);
    connect(vector_cb, SIGNAL(stateChanged(int)), this, SLOT(selectVector(int)));

    vec_only_cb = new QCheckBox("VecOnly");
    vec_only_cb->setChecked(false);
    vec_only_cb->setCheckState(Qt::Unchecked);
    connect(vec_only_cb, SIGNAL(stateChanged(int)), this, SLOT(selectVecOnly(int)));

    vec_3D_cb = new QCheckBox("Vec3D");
    vec_3D_cb->setChecked(false);
    vec_3D_cb->setCheckState(Qt::Unchecked);
    connect(vec_3D_cb, SIGNAL(stateChanged(int)), this, SLOT(selectVec3D(int)));

    veclay->addWidget(vector_cb,   0, 0);
    veclay->addWidget(vec_only_cb, 0, 1);
    veclay->addWidget(vec_3D_cb,   0, 2);

    longerButton = new QPushButton("Longer Arrow");
    shorterButton = new QPushButton("Shorter Arrow");
    widerButton = new QPushButton("Wider Arrow");
    narrowerButton = new QPushButton("Narrower Arrow");
    moreButton = new QPushButton("More Vectors");
    lessButton = new QPushButton("Less Vectors");

    connect(longerButton,   SIGNAL(clicked()), this, SLOT(vectorLonger()));
    connect(shorterButton,  SIGNAL(clicked()), this, SLOT(vectorShorter()));
    connect(widerButton,    SIGNAL(clicked()), this, SLOT(vectorWider()));
    connect(narrowerButton, SIGNAL(clicked()), this, SLOT(vectorNarrower()));
    connect(moreButton,     SIGNAL(clicked()), this, SLOT(vectorMore()));
    connect(lessButton,     SIGNAL(clicked()), this, SLOT(vectorLess()));

    veclay->addWidget(longerButton,   1, 0);
    veclay->addWidget(shorterButton,  1, 1);
    veclay->addWidget(widerButton,    2, 0);
    veclay->addWidget(narrowerButton, 2, 1);
    veclay->addWidget(moreButton,     3, 0);
    veclay->addWidget(lessButton,     3, 1);

    vecbox->setLayout(veclay);
    layout->addWidget(vecbox,0,0);

    layout->setColumnMinimumWidth(0,2);
    layout->setColumnStretch(0,2);
    layout->setRowStretch(0,2);

  //Overall layout
    setLayout(layout);
}

void VectorWidget::selectVector(int n)
{
  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\tn = " << n << endl;

    nvoptions->set_cb(NV_STATUS_CHANGED, true);
    if(n)
        nvoptions->set_cb(NV_VECTORON, true);
    else
        nvoptions->set_cb(NV_VECTORON, false);

    if(NULL != translator)
        translator->updateGL();
}

void VectorWidget::selectVecOnly(int n)
{
  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\tn = " << n << endl;

    nvoptions->set_cb(NV_STATUS_CHANGED, true);
    if(n)
        nvoptions->set_cb(NV_VECTORONLY, true);
    else
        nvoptions->set_cb(NV_VECTORONLY, false);

    if(NULL != translator)
        translator->updateGL();
}

void VectorWidget::selectVec3D(int n)
{
  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\tn = " << n << endl;

    nvoptions->set_cb(NV_STATUS_CHANGED, true);
    if(n)
        nvoptions->set_cb(NV_VEC3D, true);
    else
        nvoptions->set_cb(NV_VEC3D, false);

    if(NULL != translator)
        translator->updateGL();
}

void VectorWidget::vectorLonger()
{
  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;

    nvoptions->set_cb(NV_STATUS_CHANGED, true);
    nvoptions->set_cb(NV_VECTOR_LONGER, true);

    if(NULL != translator)
        translator->updateGL();
}

void VectorWidget::vectorShorter()
{
  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;

    nvoptions->set_cb(NV_STATUS_CHANGED, true);
    nvoptions->set_cb(NV_VECTOR_SHORTER, true);

    if(NULL != translator)
        translator->updateGL();
}

void VectorWidget::vectorWider()
{
  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;

    nvoptions->set_cb(NV_STATUS_CHANGED, true);
    nvoptions->set_cb(NV_VECTOR_WIDER, true);

    if(NULL != translator)
        translator->updateGL();
}

void VectorWidget::vectorNarrower()
{
  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;

    nvoptions->set_cb(NV_STATUS_CHANGED, true);
    nvoptions->set_cb(NV_VECTOR_NARROWER, true);

    if(NULL != translator)
        translator->updateGL();
}

void VectorWidget::vectorMore()
{
  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;

    nvoptions->set_cb(NV_STATUS_CHANGED, true);
    nvoptions->set_cb(NV_VECTOR_MORE, true);

    if(NULL != translator)
        translator->updateGL();
}

void VectorWidget::vectorLess()
{
  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;

    nvoptions->set_cb(NV_STATUS_CHANGED, true);
    nvoptions->set_cb(NV_VECTOR_LESS, true);

    if(NULL != translator)
        translator->updateGL();
}

