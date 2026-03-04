#ifndef _TrajectoryWidget_H
#define _TrajectoryWidget_H

// $Id: trajectoryWidget.h 5315 2015-02-06 21:24:34Z starviewer $

#include <QtGui/QtGui>
#include <QWidget>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QPushButton>
#include <QRadioButton>
#include <QTimer>
#include <QSlider>
#include <QObject>
#include <QLabel>
#include <QGridLayout>
#include <QApplication>

#include <iostream>

#include "nvoptions.h"
#include "basetranslator.h"

class TrajectoryWidget : public QWidget
{
    Q_OBJECT                 //  Qt magic macro
    public:
        TrajectoryWidget(QWidget* parent=0); //  Constructor
       ~TrajectoryWidget();

        void setup();

        void set_translator(BaseTranslator* t) { translator = t; };
        void set_nvoptions(NVOptions* opt) { nvoptions = opt; };

    public slots:
        void selectTrajectory(int n);
        void selectTrajectoryOnly(int n);

        void trajectoryWider();
        void trajectoryNarrower();

    protected:
        BaseTranslator* translator;
        NVOptions* nvoptions;

      //Set layout of child widgets
        QGridLayout* layout;

        QCheckBox* trajectory_cb;  // Check box for Trajectory
        QCheckBox* traj_only_cb;  // Check box for Trajectory only

        QPushButton *widerButton;
        QPushButton *narrowerButton;
};
#endif

