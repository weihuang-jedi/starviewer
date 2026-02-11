#ifndef _RADARReader_H
#define _RADARReader_H

//$Id: radarreader.h 14561 2013-09-23 16:07:00Z starviewer $

#include <QtOpenGL>

#include <iostream>
#include <vector>

#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#define USE_RSL_VARS
#include "rsl.h"

using namespace std;

class RADARReader
{
    public:
        RADARReader(string flnm, string id);
       ~RADARReader();

        void setup();
        void reset();
        void to_gif();

        string get_time_string() { return time_string; };
        string get_varname() { return _varname; };

        vector<string> get_types() { return _types; };
        vector<string> get_varnames() { return _types; };

        int get_varnumber() { return _types.size(); };

        Volume* get_volume(string vn);

        int get_nsweeps() { return nsweeps; };
        int get_nrays() { return nrays; };
        int get_nbins() { return nbins; };

        double get_base_lon() { return _base_lon; };
        double get_base_lat() { return _base_lat; };

    protected:
        string _filename;
        string _id;
        string _varname;

        Radar *radar;
        Sweep *sweep;
        Ray   *ray;

        string time_string;

        bool verbose;
        int wholevolume;
        int xdim;
        float range;
        int dbz_black;

        double _base_lon, _base_lat;

        vector<string> _types;
        vector<int> _nsweeps;
        vector<int> _sweepid;

        unsigned char *get_pixel_from_sweep(Sweep *s, int xdim, int ydim, float range);

        int nsweeps;
        int nrays;
        int nbins;
};
#endif

