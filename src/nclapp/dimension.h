#ifndef _Dimension_H
#define _Dimension_H

#include <string>

//#include "defs.h"

using namespace std;

class Dimension
{
    public:
        Dimension();
        Dimension(string n);
        Dimension(string n, int v);
       ~Dimension();

        string get_name() { return name; };
        int    get_size() { return size; };

        void set_name(string n) { name = n; };
        void set_size(int v) { size = v; };
 
    private:
        string name;
        int    size;
};
#endif

