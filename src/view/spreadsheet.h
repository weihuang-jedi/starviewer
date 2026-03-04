//$Id: spreadsheet.h 5315 2015-02-06 21:24:34Z starviewer $

#ifndef _SpreadSheet_H
#define _SpreadSheet_H

#include <QtGui/QtGui>
#include <QWidget>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <iostream>

#include "evaluator.h"
#include "geometry.h"

using namespace std;

class SpreadSheet : public QWidget
{
    public:
        SpreadSheet(QWidget* parent=0);       	// Constructor
       ~SpreadSheet();		// Destructor

        void setup(string vn, double* var);
        void set_geometry(Geometry* gm);
        void set_level(int n) { level = n; };

        double get_minval() { return vMinimum; };
        double get_maxval() { return vMaximum; };

        void set_fillValue(double v) { fillValue = v; };
        void set_hasFillValue(bool v) { hasFillValue = v; };

        void clear();

        void resize(int width, int height);  //  Resize widget

    private:
        QTableWidget *table;
        Evaluator* evaluator;
        Geometry* geometry;

        double* _var;
        double* pltvar;

        double* lon;
        double* lat;
        double* lev;

        double fillValue;
        bool hasFillValue;

        int level;
        int nx, ny, nz;
        int widgetWidth, widgetHeight;
        double vMinimum, vMaximum;
        string name;
};
#endif

