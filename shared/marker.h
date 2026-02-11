#ifndef MARKER_H
#define MARKER_H

#include <QtOpenGL>
#include <QGLWidget>
#include <QString>

#include <iostream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

using namespace std;

class Marker : public QGLWidget
{
    Q_OBJECT				//  Qt magic macro
    private:
        string _varname;
        string _timestr;

        double _length;
        double _height;

        int _nc;

        void _setup();

    public:
        Marker(QWidget* parent=0);
       ~Marker();			//  Destructor

        void draw();

        void set_varname(string vn) { _varname = vn; };
        void set_timestr(string ts) { _timestr = ts; };
        void set_nc(int n) { _nc = n; };

        int get_nc() { return _nc; };
};
#endif

