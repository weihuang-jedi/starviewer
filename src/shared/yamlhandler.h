#ifndef _YAMLHandler_H
#define _YAMLHandler_H

#include <yaml-cpp/yaml.h>
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

class YAMLHandler
{
    public:
        YAMLHandler(const char* flnm); 	// Constructor
       ~YAMLHandler();			// Destructor

        void read_yaml();
        void write_yaml();

	string get_flnm() { return _flnm; };
	string get_name() { return _name; };
	string get_model() { return _model; };
	string get_version() { return _version; };

	string get_earth_bmp() { return _earth_bmp; };
	string get_coastline_file() { return _coastline_file; };
	string get_coastline_resolution() { return _coastline_resolution; };

	vector<string> get_files(const char* type, YAML::Node config);

	vector<string> get_datafiles() { return _datafiles; };
	vector<string> get_gridfiles() { return _gridfiles; };
	vector<string> get_incrfiles() { return _incrfiles; };

    private:
        YAML::Node config;

        string _flnm;
        string _name;
        string _model;
        string _version;

	string _earth_bmp;
	string _coastline_file;
	string _coastline_resolution;

	vector<string> _datafiles;
	vector<string> _gridfiles;
	vector<string> _incrfiles;
};
#endif

