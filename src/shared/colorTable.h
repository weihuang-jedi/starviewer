#ifndef _ColorTable_H
#define _ColorTable_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <string.h>
#include <assert.h>

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cerrno>

#include "texture1d.h"

using namespace std;

class ColorTable
{
    public:
        ColorTable();			//  Constructor
        ColorTable(string cmroot, string cmflnm);	//  Constructor
       ~ColorTable();			//  Destructor

        int get_nc() { return _clist.size(); };

        void print();

        vector<string> get_clist() { return _clist; };

        void set_colorMap(string cn);
        void set_opacity(int opt);
        void set_color4v(double v, double* color4v);
        void set_colorMapDir(string cmr) { _cmroot = cmr; };

        int    get_clen() { return _clen; };
        float* get_cmap() { return _cmap; };
        float* get_opacity() { return _opacity; };

        string get_name() { return _name; };

        Texture1d* get_texture1d() { return texture1d; };

    private:
        Texture1d* texture1d;

        vector<string> _clist;

        string _name;
        string _cmroot;

        float* _cmap;
        float* _opacity;
        int    _clen;

        void get_file_contents(const char *cn);
        void _setup();
        void set_default_cmroot();
};
#endif

