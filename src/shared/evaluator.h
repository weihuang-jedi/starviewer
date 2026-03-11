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

        void set_value(float *v);
        void set_value(size_t sz, float *v);

        void set_FillValue(float dv, bool hfv = true);

        float get_min() { return _minval; };
        float get_max() { return _maxval; };

    private:
        size_t _size;
        bool _hasFillValue;

        float _minval;
        float _maxval;
        float _dblFillValue;
};
#endif

