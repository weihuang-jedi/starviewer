#ifndef _Attribute_H
#define _Attribute_H

#include <string>
#include <sstream>

//#include "defs.h"

using namespace std;

class Attribute
{
    public:
        Attribute();
        Attribute(string n);
        Attribute(string n, string v);
       ~Attribute();

        string get_name() { return name; };
        string get_value() { return value; };

        void set_name(string n) { name = n; };
        void set_name(const char* nc);

        void set_value(string v) { value = v; };
        void set_value(const char* vc);
        void set_value(short s);
        void set_value(int i);
        void set_value(float f);
        void set_value(double d);

      //NV_DataType get_type() { return type; };
      //int get_nelem() { return nelem; };
      //void* get_value() { return value; };
 
    private:
        string name;
        string value;

      //NV_DataType type;
      //void*       value;
      //int         nelem;
};
#endif

