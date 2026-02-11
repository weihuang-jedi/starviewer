#ifndef _NVFile_h
#define _NVFile_h

#include <stdio.h>
#include <ctype.h>

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cerrno>

#include "variable.h"
#include "nclfilehandler.h"

#define MAX_DMN_SIZE	3

using namespace std;

class NVFile
{
    public:
        NVFile(string flnm, bool isList = false);
        NVFile(string flnm, bool isList, bool isWRFmodel);
       ~NVFile();

        void set_flnm(string s) { flnm = s; };

        string get_flnm() { return flnm; };

        int get_natts() { return natts; };
        int get_ndims() { return ndims; };
        int get_nvars() { return nvars; };
        int get_nfils() { return nfils; };

        Variable* get_var(int n);
        Variable* get_varWithName(string vn);

        Dimension* get_dim(int n);
        Dimension* get_dimWithName(string dn);

        int get_dim_size(const char* dn);

        Attribute* get_att(int n);
        Attribute* get_attWithName(string an);

        int* get_iv(string vn);
        double* get_dv(string vn);
        float* get_fv(string vn);
        int*   get_grdsize() { return domsize; };
        int*   get_varsize();
        int*   get_ntimes() { return ntimes; };

        string get_sv(string vn);
        string get_varname(int n);

        NclFileHandler* get_filehandler(int n) { return &(filehandler[n]); };
 
        int get_ndv(int n);
        string* get_ndvNames(int n);
        string get_title() { return title; };
        string* get_timestr();

        void select_file(int nf);
        void set_file(int nf) { currfile = nf; };

        bool get_has1dLon() { return has1dLon; };
        bool get_has1dLat() { return has1dLat; };
        bool get_has1dLev() { return has1dLev; };
        bool get_hasFillValue() { return hasFillValue; };

        bool get_has2dLon() { return has2dLon; };
        bool get_has2dLat() { return has2dLat; };

        double* get_lon_value() { return lon_value; };
        double* get_lat_value() { return lat_value; };
        double* get_lev_value() { return lev_value; };

        double  get_fillValue() { return fillValue; };

        string get_att_value(string an);
        string get_att_value(const char* cn);

    private:
        string flnm;
        string title;
        string timevarname;

        NclFileHandler* filehandler;

        Attribute* atts;
        Dimension* dims;
        Variable*  vars;
        Variable*  coordvars;

        vector<string> fileList;

        int* ntimes;

        int domsize[3];
        int varsize[3];

        int natts;
        int ndims;
        int nvars;
        int nfils;
        int ncoords;

        int prevfile;
        int currfile;

        bool hasFillValue;

        bool hasTime;
        bool hasTimeVar;

        bool has1dLon;
        bool has1dLat;
        bool has1dLev;

        bool WRFmodel;

        string* timetitle;

        double* lon_value;
        double* lat_value;
        double* lev_value;
        double* timevalue;

        double  fillValue;

        int _get_file_time_size(int n);

        void _setup(string flnm, bool isList);
        void _read_filelist();
        void _set_file_property();
        void _check_coords();
        void _recheck_coords();
        void _build_timestring(bool first);

        string _tolower(string is);

        bool _isVar(string vn);

        bool has2dLon;
        bool has2dLat;
};
#endif

