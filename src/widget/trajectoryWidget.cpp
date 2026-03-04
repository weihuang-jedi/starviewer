#include "trajectoryWidget.h"

TrajectoryWidget::TrajectoryWidget(QWidget* parent)
                :  QWidget(parent)
{
    translator = NULL;
}

TrajectoryWidget::~TrajectoryWidget()
{
    delete layout;
}

void TrajectoryWidget::setup()
{
  //Set layout of child widgets
    layout = new QGridLayout;

  //Group Display parameters
    QGroupBox* trajbox = new QGroupBox("Display");
    QGridLayout* trajlay = new QGridLayout;

    trajectory_cb = new QCheckBox("Trajectory");
    trajectory_cb->setChecked(false);
    trajectory_cb->setCheckState(Qt::Unchecked);
    connect(trajectory_cb, SIGNAL(stateChanged(int)), this, SLOT(selectTrajectory(int)));

    traj_only_cb = new QCheckBox("TrajectoryOnly");
    traj_only_cb->setChecked(false);
    traj_only_cb->setCheckState(Qt::Unchecked);
    connect(traj_only_cb, SIGNAL(stateChanged(int)), this, SLOT(selectTrajectoryOnly(int)));

    trajlay->addWidget(trajectory_cb, 0, 0);
    trajlay->addWidget(traj_only_cb,  0, 1);

    widerButton = new QPushButton("Wider");
    narrowerButton = new QPushButton("Narrower");

    connect(widerButton,    SIGNAL(clicked()), this, SLOT(trajectoryWider()));
    connect(narrowerButton, SIGNAL(clicked()), this, SLOT(trajectoryNarrower()));

    trajlay->addWidget(widerButton,    1, 0);
    trajlay->addWidget(narrowerButton, 1, 1);

    trajbox->setLayout(trajlay);
    layout->addWidget(trajbox,0,0);

    layout->setColumnMinimumWidth(0,2);
    layout->setColumnStretch(0,2);
    layout->setRowStretch(0,2);

  //Overall layout
    setLayout(layout);
}

void TrajectoryWidget::selectTrajectory(int n)
{
  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\tn = " << n << endl;

    nvoptions->set_cb(NV_STATUS_CHANGED, true);
    if(n)
        nvoptions->set_cb(NV_TRAJECTORY_ON, true);
    else
        nvoptions->set_cb(NV_TRAJECTORY_ON, false);

    if(NULL != translator)
        translator->updateGL();
}

void TrajectoryWidget::selectTrajectoryOnly(int n)
{
  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\tn = " << n << endl;

    nvoptions->set_cb(NV_STATUS_CHANGED, true);
    if(n)
        nvoptions->set_cb(NV_TRAJECTORY_ONLY, true);
    else
        nvoptions->set_cb(NV_TRAJECTORY_ONLY, false);

    if(NULL != translator)
        translator->updateGL();
}

void TrajectoryWidget::trajectoryWider()
{
  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\tn = " << n << endl;

    nvoptions->set_cb(NV_STATUS_CHANGED, true);
    nvoptions->set_cb(NV_TRAJECTORY_WIDER, true);

    if(NULL != translator)
        translator->updateGL();
}

void TrajectoryWidget::trajectoryNarrower()
{
  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\tn = " << n << endl;

    nvoptions->set_cb(NV_STATUS_CHANGED, true);
    nvoptions->set_cb(NV_TRAJECTORY_NARROWER, true);

    if(NULL != translator)
        translator->updateGL();
}

