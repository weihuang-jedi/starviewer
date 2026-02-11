#include <clip.h>

//
//  Constructor
//
Clip::Clip(const QString &title, QWidget *parent)
    : QGroupBox(title, parent)
{
    _title = title;
    _clip_on = 0;

    _a = 0.0;
    _b = 0.0;
    _c = 0.0;
    _d = 0.0;

    _set_clip_equation();

 //Create spinboxes
   dsb_a = new QDoubleSpinBox();
   dsb_b = new QDoubleSpinBox();
   dsb_c = new QDoubleSpinBox();
   dsb_d = new QDoubleSpinBox();

   clip_cb = new QCheckBox(title);

   dsb_a->setDecimals(1);
   dsb_a->setSingleStep(1.0);
   dsb_a->setRange(-5000,5000);
   dsb_a->setValue(100);

   dsb_b->setDecimals(1);
   dsb_b->setSingleStep(1.0);
   dsb_b->setRange(-5000,5000);
   dsb_b->setValue(0);

   dsb_c->setDecimals(1);
   dsb_c->setSingleStep(1.0);
   dsb_c->setRange(-100,100);
   dsb_c->setValue(0);

   dsb_d->setDecimals(1);
   dsb_d->setSingleStep(1.0);
   dsb_d->setRange(-1000,1000);
   dsb_d->setValue(0);

   connect(dsb_a, SIGNAL(valueChanged(double)), this, SLOT(setA(double)));
   connect(dsb_b, SIGNAL(valueChanged(double)), this, SLOT(setB(double)));
   connect(dsb_c, SIGNAL(valueChanged(double)), this, SLOT(setC(double)));
   connect(dsb_d, SIGNAL(valueChanged(double)), this, SLOT(setD(double)));

   if(title == tr("Z"))
       connect(clip_cb, SIGNAL(stateChanged(int)),  parent, SLOT(selectClipZ(int)));
   else if(title == tr("Y"))
       connect(clip_cb, SIGNAL(stateChanged(int)),  parent, SLOT(selectClipY(int)));
   else
       connect(clip_cb, SIGNAL(stateChanged(int)),  parent, SLOT(selectClipX(int)));
}

Clip::~Clip()
{
  //delete clip_cb;
}

/********************************************************************/
/*************************  Set parameters  *************************/
/********************************************************************/
//Set a
void Clip::setA(double f)
{
    _a = f;
    _clip_eqn[0] = _a;
  //_set_clip_equation();
}

//Set b
void Clip::setB(double f)
{
    _b = f;
    _clip_eqn[2] = _b;
  //_set_clip_equation();
}

//Set c
void Clip::setC(double f)
{
    _c = f;
    _clip_eqn[1] = _c;
  //_set_clip_equation();
}

//Set d
void Clip::setD(double f)
{
    _d = f;
    _clip_eqn[3] = _d;
  //_set_clip_equation();
}

//select clip
void Clip::clipSwitch(int i)
{
   _clip_on = i;

   if(! i)
   {
       _a = 0.0;
       _b = 0.0;
       _c = 0.0;
       _d = 0.0;

       _set_clip_equation();
   }
}

void Clip::lay_clip(QGridLayout* clip_lay, int cols, int cole)
{
 //clip_lay->addWidget(new QLabel(title), 0, cols);
   clip_lay->addWidget(          clip_cb, 0, cole);
   clip_lay->addWidget(new QLabel("  x"), 1, cols);
   clip_lay->addWidget(            dsb_a, 1, cole);
   clip_lay->addWidget(new QLabel("  y"), 2, cols);
   clip_lay->addWidget(            dsb_b, 2, cole);
   clip_lay->addWidget(new QLabel("  z"), 3, cols);
   clip_lay->addWidget(            dsb_c, 3, cole);
   clip_lay->addWidget(new QLabel("  d"), 4, cols);
   clip_lay->addWidget(            dsb_d, 4, cole);
}

/**********************************************************************
 *
 *              About Cliping
 * The idea of clipping is to define a plane of:
 *      a.x + b.y + c.z + d = 0
 * Where OpenGL will clip (cut off) anything above the plane.
 *
 **********************************************************************
 */

void Clip::_set_clip_equation()
{
    _clip_eqn[0] = _a;
    _clip_eqn[1] = _c;
    _clip_eqn[2] = _b;
    _clip_eqn[3] = _d;
}

