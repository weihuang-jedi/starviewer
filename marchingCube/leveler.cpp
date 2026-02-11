#include "leveler.h"

Leveler::Leveler()
{
    int n;

    _nl = 0;
    _max_levels = 5;

    _minval = 0.0;
    _maxval = 0.0;

    _levels = new float[_max_levels];

    _cmap = (float **) calloc(_max_levels, sizeof(float *));
    
    for(n = 0; n < _max_levels; ++n)
    {
        _cmap[n] = (float *) calloc(4, sizeof(float));
    }
}

Leveler::~Leveler()
{
    int n;

    delete _levels;

    for(n = 0; n < _max_levels; ++n)
    {
        free(_cmap[n]);
    }

    free(_cmap);
}

void Leveler::setup(float minval, float maxval)
{
    setup(_max_levels, minval, maxval);
}

void Leveler::setup(int n, float minval, float maxval)
{
    int m;
    float dlt;

    if(n > _max_levels)
        set_nl(_max_levels);
    else if(n < 1)
        set_nl(1);
    else
        set_nl(n);

    _maxval = maxval;
    _minval = minval;

    dlt = (_maxval - _minval) / _nl;

  //cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__
  //     << ", file: <" << __FILE__ << ">" << endl;
  //cout << "\t_nl = " << _nl << ", minval = " << minval << ", maxval = " << maxval << endl;
  //cout << "\tdlt = " << dlt << endl;

    for(m = 0; m < _nl; m++)
    {
        _levels[m] = _minval + (m + 0.5) * dlt;
      //cout << "\t_levels[" << m << "] = " << _levels[m] << endl;
    }
}

void Leveler::set_nl(int nl)
{
    _nl = nl;

    if(_nl < 1)
        _nl = 1;

    if(_nl > _max_levels)
    {
        _nl = _max_levels;
    }
}

void Leveler::reset_nl(int nl)
{
    int m;
    float dlt;

    if(nl > _max_levels)
        set_nl(_max_levels);
    else if(nl < 1)
        set_nl(1);
    else
        set_nl(nl);

    dlt = (_maxval - _minval) / nl;

    for(m = 0; m < _nl; m++)
    {
        _levels[m] = _minval + (0.5 + m) * dlt;
    }
}

void Leveler::set_cmap(ColorTable* ct)
{
    int i, m, n;
    int clen = ct->get_clen();

    float dlt, fct;
    float *colors = ct->get_cmap();

    fct = 1.0 / (_maxval - _minval + 1.0e-20);

  //cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__
  //     << ", file: <" << __FILE__ << ">" << endl;
  //cout << "\t_nl = " << _nl << ", clen = " << clen << endl;
  //cout << "\t_minval = " << _minval << ", _maxval = " << _maxval << endl;

    for(m = 0; m < _nl; m++)
    {
        dlt = fct * (_levels[m] - _minval) * clen;

        n = floor(dlt);
        dlt = dlt - n;

      //cout << "\t_levels[" << m << "] = " << _levels[m] << endl;
      //cout << "\tn = " << n << ", dlt = " << dlt << endl;

        for(i = 0; i < 3; ++i)
            _cmap[m][i] = (1.0 - dlt) * colors[3*(n+2) + i] + dlt * colors[3*(n+3) + i];

        _cmap[m][3] = 0.1 + 0.2 * m;
    }

    if(0 > _maxval * _minval)
    {
        n = (1 + _nl) / 2;
        _cmap[n][3] = 0.1;
        m = n;
        while(m > 0)
        {
            --m;
            _cmap[m][3] = _cmap[m + 1][3] + 0.2;
        }

        m = n + 1;
        while(m < _nl)
        {
            _cmap[m][3] = _cmap[m - 1][3] + 0.2;
            ++m;
        }
    }
}

