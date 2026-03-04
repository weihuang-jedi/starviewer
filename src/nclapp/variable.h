#ifndef _Variable_H
#define _Variable_H

#include <string>
#include <sstream>
#include <iostream>

//#include "defs.h"

#include "attribute.h"
#include "dimension.h"

using namespace std;

class Variable
{
    public:
        Variable();
        Variable(string n);
        Variable(string n, int natts, int ndims);
       ~Variable();

        void set_name(string n) { name = n; };
        void set_value(void* v) { value = v; };

        string get_name() { return name; };
        void* get_value() { return value; };

        void set_natts(int n);
        void set_ndims(int n);

        int get_natts() { return natts; };
        int get_ndims() { return ndims; };

        void push_att(string nm, string v);
        void push_att(string nm, const char* cn);
        void push_att(string nm, short s);
        void push_att(string nm, int i);
        void push_att(string nm, float f);
        void push_att(string nm, double d);
        void push_att(const char* nc, const char* vc);
        void push_att(const char* nc, int i);
        void push_att(const char* nc, float f);
        void push_att(const char* nc, double d);

        void push_dim(string nm, int s);
        void push_dim(const char* nc, int s);

        void print();

        Attribute* get_atts() { return atts; };
        Dimension* get_dims() { return dims; };

        void  set_hasFillValue(bool v) { _hasFillValue = v; };
        bool  get_hasFillValue() { return _hasFillValue; };

        void  set_fillValue(double v);
        double get_fillValue();

      //NV_DataType get_type() { return type; };
 
    private:
        string name;
        void*  value;

        Attribute* atts;
        Dimension* dims;

        int natts;
        int ndims;

        int catt;
        int cdim;

        bool _hasFillValue;
        double _fillValue;

      //NV_DataType type;
};
#endif

