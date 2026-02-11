// $Id: animationWidget.cpp 5315 2015-02-06 21:24:34Z starviewer $

#include <QtGui/QApplication>

#include "animationWidget.h"

AnimationWidget::AnimationWidget(QWidget* parent)
             :  QWidget(parent)
{
    setWindowTitle(tr("Animation"));

    layout = new QGridLayout;

    _time_interval = 128;

    timer = new QTimer(this);
    timer->start(_time_interval);
}

AnimationWidget::~AnimationWidget()
{
   delete timer;
   delete layout;
}

void AnimationWidget::setup()
{
    _setup_animation();
    _setup_display();
}

void AnimationWidget::_setup_animation()
{
  //Display widget for frame (number)
    frame = new QLabel();

    animation_cb  = new QCheckBox("Animation");
    animation_cb->setChecked(false);

    jpg4movie_cb  = new QCheckBox("SaveJPG4movie");
    jpg4movie_cb->setChecked(false);

    fastButton = new QPushButton("Fast");
    slowButton = new QPushButton("Slow");
    nextButton = new QPushButton("Next");
    backButton = new QPushButton("Back");

    connect(animation_cb, SIGNAL(stateChanged(int)), translator, SLOT(selectAnimation(int)));
    connect(jpg4movie_cb, SIGNAL(stateChanged(int)), translator, SLOT(selectJPG4Movie(int)));

  //Connect translator signals to display widgets
    connect(translator, SIGNAL(frame(QString)),  frame, SLOT(setText(QString)));

    connect(fastButton, SIGNAL(clicked()),       this, SLOT(animateFaster()));
    connect(slowButton, SIGNAL(clicked()),       this, SLOT(animateSlower()));
    connect(nextButton, SIGNAL(clicked()), translator, SLOT(nextFrame()));
    connect(backButton, SIGNAL(clicked()), translator, SLOT(backFrame()));

    connect(timer, SIGNAL(timeout()), translator, SLOT(update_frame()));

  //Group animation parameters
    QGroupBox*   animation_box = new QGroupBox("Animation");
    QGridLayout* animation_lay = new QGridLayout;
    animation_lay->addWidget(animation_cb, 0, 0);
    animation_lay->addWidget(frame,        0, 1);
    animation_lay->addWidget(nextButton,   1, 0);
    animation_lay->addWidget(backButton,   1, 1);
    animation_lay->addWidget(fastButton,   2, 0);
    animation_lay->addWidget(slowButton,   2, 1);
    animation_lay->addWidget(jpg4movie_cb, 3, 0);
    animation_box->setLayout(animation_lay);

    layout->addWidget(animation_box,0,0);
}

void AnimationWidget::_setup_display()
{
  //cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__
  //     << ", file: <" << __FILE__ << ">" << endl;

    layout->setColumnStretch(0,4);
    layout->setColumnMinimumWidth(0,2);

    layout->setRowStretch(0,2);

  //Overall layout
    setLayout(layout);
}

void AnimationWidget::animateFaster()
{
    if(_time_interval > 1)
        _time_interval /= 2;
    timer->start(_time_interval);
}

void AnimationWidget::animateSlower()
{
    if(_time_interval < 1500)
        _time_interval *= 2;
    timer->start(_time_interval);
}

