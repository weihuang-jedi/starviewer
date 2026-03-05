#ifndef LEVELER_H
#define LEVELER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

using namespace std;

#include <math.h>
#include "colorTable.h"

class Leveler
{
    public:
        Leveler();
       ~Leveler();

        int  get_nl() { return (_nl); };

        void set_nl(int nl);
        void reset_nl(int nl);
        void setup(float minval, float maxval);
        void setup(int n, float minval, float maxval);
        void set_cmap(ColorTable* ct);

        float *get_levels() { return _levels; };
        float **get_cmap() { return _cmap; };

    protected:
        int _nl;
        int _max_levels;

        float _minval;
        float _maxval;

        float *_levels;
        float **_cmap;
};
#endif

