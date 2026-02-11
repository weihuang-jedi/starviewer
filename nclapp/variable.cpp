#include "variable.h"

Variable::Variable()
{
    name = "unknown";
    natts = 0;
    ndims = 0;

    catt = 0;
    cdim = 0;
}

Variable::Variable(string n)
{
    name = n;
    natts = 0;
    ndims = 0;

    catt = 0;
    cdim = 0;
}

Variable::Variable(string n, int na, int nd)
{
    name = n;
    natts = na;
    ndims = nd;

    catt = 0;
    cdim = 0;

    if(natts > 0)
        atts = new Attribute[natts];
    if(ndims > 0)
        dims = new Dimension[ndims];
}

Variable::~Variable()
{
    if(natts > 0)
        delete [] atts;
    if(ndims > 0)
        delete [] dims;
}

void Variable::set_natts(int na)
{
    if(natts != na)
    {
       if(natts > 0)
       {
           delete [] atts;
           catt = 0;
       }

       natts = na;
       if(natts > 0)
       {
           atts = new Attribute[natts];
       }
    }
}

void Variable::set_ndims(int nd)
{
    if(ndims != nd)
    {
       if(ndims > 0)
       {
           delete [] dims;
           cdim = 0;
       }

       ndims = nd;
       if(ndims > 0)
       {
           dims = new Dimension[ndims];
       }
    }
}

void Variable::push_att(string n, string v)
{
    if(natts > catt)
    {
       atts[catt].set_name(n);
       atts[catt].set_value(v);
       ++catt;
    }
}

void Variable::push_att(string n, const char* cn)
{
    string v = cn;
    push_att(n, v);
}

void Variable::push_att(string nm, short s)
{
    stringstream ss;
    ss << s;
    string v = ss.str();
    push_att(nm, v);
}

void Variable::push_att(string nm, int i)
{
    stringstream ss;
    ss << i;
    string v = ss.str();
    push_att(nm, v);
}

void Variable::push_att(string nm, float f)
{
    stringstream ss;
    ss << f;
    string v = ss.str();
    push_att(nm, v);
}

void Variable::push_att(string nm, double d)
{
    stringstream ss;
    ss << d;
    string v = ss.str();
    push_att(nm, v);
}

void Variable::push_att(const char* nc, const char* vc)
{
    string n = nc;
    string v = vc;
    push_att(n, v);
}

void Variable::push_att(const char* nc, int i)
{
    stringstream ss;
    ss << i;
    string n = nc;
    string v = ss.str();
    push_att(n, v);
}

void Variable::push_att(const char* nc, float f)
{
    stringstream ss;
    ss << f;
    string n = nc;
    string v = ss.str();
    push_att(n, v);
}

void Variable::push_att(const char* nc, double d)
{
    stringstream ss;
    ss << d;
    string n = nc;
    string v = ss.str();
    push_att(n, v);
}

void Variable::push_dim(string n, int s)
{
    dims[cdim].set_name(n);
    dims[cdim].set_size(s);
    ++cdim;
}

void Variable::push_dim(const char* nc, int s)
{
    string n = nc;
    push_dim(n, s);
}

void Variable::print()
{
    int n;

    cout << "\nfile: " << __FILE__
         << ", function: " << __PRETTY_FUNCTION__
         << ", line: " << __LINE__ << endl;
    cout << "\t var name: " << name << endl;

    for(n = 0; n < cdim; ++n)
        cout << "\tDim " << n << ": name: <"
             << dims[n].get_name() << ">, size: "
             << dims[n].get_size() << endl;

    for(n = 0; n < catt; ++n)
        cout << "\tAtt " << n << ": name: <"
             << atts[n].get_name() << ">, value: <"
             << atts[n].get_value() << ">" << endl;
}

void Variable::set_fillValue(double v)
{
  //cout << "\nfile: " << __FILE__
  //     << ", function: " << __PRETTY_FUNCTION__
  //     << ", line: " << __LINE__ << endl;
  //cout << "\t var name: " << name << ", _fillValue = " << v << endl;

    _fillValue = v;
}

double Variable::get_fillValue()
{
  //cout << "\nfile: " << __FILE__
  //     << ", function: " << __PRETTY_FUNCTION__
  //     << ", line: " << __LINE__ << endl;
  //cout << "\t var name: " << name << ", _fillValue = " << _fillValue << endl;

    return _fillValue;
}

