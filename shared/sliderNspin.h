#ifndef SLIDERNSPIN_H
#define SLIDERNSPIN_H

#include <iostream>
#include <stdio.h>

#include <QtGui/QtGui>
#include <QGroupBox>

class QCheckBox;
class QGroupBox;
class QLabel;
class QSlider;
class QSpinBox;

class SliderNSpin : public QGroupBox
{
    Q_OBJECT
    public:
        SliderNSpin(Qt::Orientation orientation, const QString &title,
                    QWidget *parent = 0);
       ~SliderNSpin();

        void add2layout(QGridLayout *layout, int row, int col);

        int  get_value() { return (_value); };

        void set_min(int value);
        void set_max(int value);
        void set_step(int value);
        void set_tickinterval(int value);

        QSlider  *get_slider() { return slider; };
        QSpinBox *get_spinBox() { return spinBox; };

    signals:
        void valueChanged(int value);

    public slots:
        void setValue(int value);

    private:
        QGroupBox *controlBox;
        QSlider *slider;
        QSpinBox *spinBox;

        int _value;
        int _step;
        int _min;
        int _max;

        void _setup(Qt::Orientation orientation);
};
#endif

