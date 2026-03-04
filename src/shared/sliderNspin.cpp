#include <QSlider>
#include <QSpinBox>
#include <QGridLayout>
#include <QBoxLayout>

#include "sliderNspin.h"

SliderNSpin::SliderNSpin(Qt::Orientation orientation, const QString &title,
			 QWidget *parent)
                       : QGroupBox(title, parent)
{
    _setup(orientation);
}

SliderNSpin::~SliderNSpin()
{
    delete slider;
    delete spinBox;
}

void SliderNSpin::_setup(Qt::Orientation orientation)
{
    QBoxLayout::Direction direction;
    direction = QBoxLayout::TopToBottom;

    _value = 0;
    _step = 1;
    _min = 0;
    _max = 100;

    slider = new QSlider(orientation);
    slider->setFocusPolicy(Qt::StrongFocus);
    slider->setTickPosition(QSlider::TicksBothSides);
    slider->setTickInterval(10);
    slider->setSingleStep(_step);
    slider->setValue(_value);

    spinBox = new QSpinBox;
    spinBox->setRange(_min, _max);
    spinBox->setSingleStep(_step);
    spinBox->setValue(_value);

    connect(slider,  SIGNAL(valueChanged(int)), spinBox, SLOT(setValue(int)));
    connect(spinBox, SIGNAL(valueChanged(int)), slider,  SLOT(setValue(int)));

    QGridLayout *ss_layout = new QGridLayout;
    ss_layout->addWidget(slider,  0, 0);
    ss_layout->addWidget(spinBox, 0, 1);
    this->setLayout(ss_layout);
}

void SliderNSpin::add2layout(QGridLayout *layout, int row, int col)
{
    layout->addWidget(this, row, col);
}

void SliderNSpin::setValue(int value)
{
    _value = value;
    slider->setValue(value);
    spinBox->setValue(value);
}

void SliderNSpin::set_tickinterval(int value)
{
    slider->setTickInterval(value);
}

void SliderNSpin::set_step(int value)
{
    _step = value;
    slider->setSingleStep(_step);
    spinBox->setSingleStep(_step);
}

void SliderNSpin::set_min(int value)
{
    _min = value;
    spinBox->setRange(_min, _max);
    spinBox->setValue(_value);

    slider->setRange(_min, _max);
    slider->setValue(_value);
}

void SliderNSpin::set_max(int value)
{
    _max = value;
    spinBox->setRange(_min, _max);
    spinBox->setValue(_value);

    slider->setRange(_min, _max);
    slider->setValue(_value);
}

