#ifndef _NclFileHandler_H
#define _NclFileHandler_H

#include <istream>
#include <iostream>
#include <vector>
#include <string>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include <netcdf.h>

#include "nclInterface.h"
#include "variable.h"

#define MAXDIM		64
#define MAXCOORDS	64

using namespace std;

class NclFileHandler
{
    public:
        NclFileHandler();
       ~NclFileHandler();
  
        string filename() { return _flnm; };

        void open(string fn);
        void print();

        int get_ndv(int n);

        int get_natts() { return _natts; };
        int get_ndims() { return _ndims; };
        int get_nvars() { return _nvars; };
        int get_ncoords() { return _ncoords; };

        string* get_ndvNames(int n);

        string getAndvName(int n);

        int*    get_iv(string vn);
        float*  get_fv(string vn);
        double* get_dv(string vn);
        string  get_sv(string vn);

        Attribute* get_atts() { return _atts; };
        Dimension* get_dims() { return _dims; };
        Variable*  get_vars() { return _vars; };
        Variable*  get_coordvars() { return _coordvars; };

        bool hascoords() { return _hascoords; };

    private:
        NclFile _nclfile;
        NclVar  _nclvar;

        Attribute* _atts;
        Dimension* _dims;
        Variable*  _vars;
        Variable*  _coordvars;

        string _flnm;

        int _ndims;
        int _nvars;
        int _natts;
        int _ncoords;
        int _idx;

        int* _ndv;

        bool _hasTime;

        void _default();
        void _check_atts();
        void _check_dims();
        void _check_vars();

        bool _setvar(string vn);

        string* _get_coordnames(string coordstring, int &nc);
        void _check_coordvars(string coordstring);
        bool _hascoords;
        string _oldcoordstring;
};
#endif

