#ifndef CLIP_H
#define CLIP_H
/**********************************************************************
 *
 *              About Cliping
 * The idea of clipping is to define a plane of:
 *      a.x + b.y + c.z + d = 0
 * Where OpenGL will clip (cut off) anything above the plane.
 *
 **********************************************************************
 */

#include <iostream>
#include <stdio.h>

#include <QGroupBox>       // Ensure this is correct (not QtGui/qgroupbox.h)
#include <QDoubleSpinBox>  // For dsb_a, dsb_b, etc.
#include <QGridLayout>     // For lay_clip
#include <QtGui/QtGui>
#include <QGroupBox>

class QCheckBox;
class QGroupBox;
class QLabel;
class QSpinBox;

class Clip : public QGroupBox
{
    Q_OBJECT		//  Qt magic macro
    private:
        QString _title;
        int _clip_on;

        double _a;
        double _b;
        double _c;
        double _d;

        double _clip_eqn[4];

        void _set_clip_equation();

        QDoubleSpinBox *dsb_a; //  Spinbox for clip plane a
        QDoubleSpinBox *dsb_b; //  Spinbox for clip plane b
        QDoubleSpinBox *dsb_c; //  Spinbox for clip plane c
        QDoubleSpinBox *dsb_d; //  Spinbox for clip plane c

        QCheckBox *clip_cb;    // Check box for clip

    public:
        Clip(const QString &title, QWidget *parent = 0);
       ~Clip();		//  Destructor

        void clipSwitch(int i);

        void lay_clip(QGridLayout* clip_lay, int cols, int cole);

        double *get_clip_equation() { return (_clip_eqn); };

    public slots:
        void setA(double a);
        void setB(double b);
        void setC(double c);
        void setD(double d);
};
#endif

