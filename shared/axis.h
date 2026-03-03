#ifndef AXIS_H
#define AXIS_H

#include <QGLWidget>
#include <QString>

#ifndef GL_PI
#define GL_PI   3.1415926535897932
#endif

#define Cos(th) cos(3.1415926/180*(th))
#define Sin(th) sin(3.1415926/180*(th))

//class Axis : public QGLWidget
class Axis
{
    // Q_OBJECT				//  Qt magic macro
    private:
        double _ratio;
        double _axis_length;
        double _arrow_angle;

        void _setup();
        void _x_axis(double x, double length, double angle);
        void _y_axis(double y, double length, double angle);
        void _z_axis(double z, double length, double angle);

    protected:
        void drawOneLine(double x1, double y1, double z1,
                         double x2, double y2, double z2);

    public:
      //Axis(QWidget* parent=0);	//  Constructor
        Axis();
       ~Axis();				//  Destructor

        void draw();

        void set_axis_length(double alen) { _axis_length = alen; };
        void set_axis_angle(double ang) { _arrow_angle = ang; };
        void set_axis_ratio(double rat) { _ratio = rat; };
};
#endif

