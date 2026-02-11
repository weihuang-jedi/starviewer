#ifndef _Evaluator_H
#define _Evaluator_H

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#define SMALL_NUMBER    0.000001

using namespace std;

class Evaluator
{
    public:
        Evaluator(size_t sz = 0);
       ~Evaluator();

        void set_value(double *v);
        void set_value(size_t sz, double *v);

        void set_FillValue(double dv, bool hfv = true);

        double get_min() { return _minval; };
        double get_max() { return _maxval; };

    private:
        size_t _size;
        bool _hasFillValue;

        double _minval;
        double _maxval;
        double _dblFillValue;
};
#endif

