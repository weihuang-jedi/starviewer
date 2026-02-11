#include <evaluator.h>

Evaluator::Evaluator(size_t sz)
{
    _size = sz;

    _hasFillValue = false;
    _dblFillValue = -1.0e200;

    _minval = 1.0e200;
    _maxval = -_minval;
}

Evaluator::~Evaluator()
{
}

void Evaluator::set_FillValue(double dv, bool hfv)
{
    _hasFillValue = hfv;
    _dblFillValue = dv;
}

void Evaluator::set_value(size_t sz, double *v)
{
    _size = sz;
    set_value(v);
}

void Evaluator::set_value(double *v)
{
    size_t i;
    double nearFillValue;

    _minval = 1.0e200;
    _maxval = -_minval;

  //cout << "\nFunction: " << __PRETTY_FUNCTION__
  //     << ", file: " << __FILE__
  //     << ", line: " << __LINE__ << endl;
  //cout << "\t_hasFillValue = " << _hasFillValue
  //     << ", _dblFillValue = " << _dblFillValue << endl;

    if(_hasFillValue)
    {
        nearFillValue = 0.95 * _dblFillValue;

        if(0.0 > _dblFillValue)
        {
            for(i = 0; i < _size; ++i)
            {
                if(v[i] > nearFillValue)
                {
                    if(_minval > v[i])
                       _minval = v[i];
                    if(_maxval < v[i])
                       _maxval = v[i];
                }
            }
        }
        else
        {
            for(i = 0; i < _size; ++i)
            {
                if(v[i] < nearFillValue)
                {
                    if(_minval > v[i])
                       _minval = v[i];
                    if(_maxval < v[i])
                       _maxval = v[i];
                }
            }
        }
    }
    else
    {
        for(i = 0; i < _size; ++i)
        {
            if(_minval > v[i])
               _minval = v[i];
            if(_maxval < v[i])
               _maxval = v[i];
        }
    }

  //cout << "\nFunction: " << __PRETTY_FUNCTION__
  //     << ", file: " << __FILE__
  //     << ", line: " << __LINE__ << endl;
  //cout << "\t_minval = " << _minval
  //     << ", _maxval = " << _maxval << endl;
}

