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

        void setup(string vn, float* var);
        void set_geometry(Geometry* gm);
        void set_level(int n) { level = n; };

        float get_minval() { return vMinimum; };
        float get_maxval() { return vMaximum; };

        void set_fillValue(float v) { fillValue = v; };
        void set_hasFillValue(bool v) { hasFillValue = v; };

        void clear();

        void resize(int width, int height);  //  Resize widget

    private:
        QTableWidget *table;
        Evaluator* evaluator;
        Geometry* geometry;

        float* _var;
        float* pltvar;

        double* lon;
        double* lat;
        float* lev;

        float fillValue;
        bool hasFillValue;

        int level;
        int nx, ny, nz;
        int widgetWidth, widgetHeight;
        float vMinimum, vMaximum;
        string name;
};
#endif

