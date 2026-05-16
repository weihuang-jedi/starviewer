#include <QOpenGLWidget>
#include "yamlhandler.h"
#include <fstream> // Ensure fstream is included

//Constructor
YAMLHandler::YAMLHandler(const char* flnm)
{
    _flnm = flnm;
    ifstream fin(flnm); // Create a named object (lvalue)
    if (fin.is_open())
    {
        config = YAML::Load(fin);
    }
    else
    {
        cerr << "Error: Could not open file " << flnm << endl;
    }
}

YAMLHandler::~YAMLHandler()
{
}

void YAMLHandler::read_yaml()
{
    ifstream fin("config.yaml"); // Create a named object (lvalue)
    if (fin.is_open())
    {
        config = YAML::Load(fin);
    }

    if (config["application"].IsDefined())
    {
        _name = config["application"]["name"].as<string>();
        _model = config["application"]["name"].as<string>();
        _version = config["application"]["name"].as<string>();
        cout << "name: " << _name << endl;
        cout << "model: " << _model << endl;
    }
    else
    {
        cerr << "Warning: application is not defined." << endl;
    }

    if (config["earth"].IsDefined())
    {
        _earth_bmp = config["earth"]["name"].as<string>();
        cout << "earth_bmp: " << _earth_bmp << endl;
    }
    else
    {
        cerr << "Warning: earch bmp is not defined." << endl;
    }

    if (config["coastline"].IsDefined())
    {
        _coastline_file = config["coastline"]["name"].as<string>();
        _coastline_resolution = config["coastline"]["resolution"].as<string>();
        cout << "coastline_file: " << _coastline_file << endl;
        cout << "coastline_resolution: " << _coastline_resolution << endl;
    }
    else
    {
        cerr << "Warning: coastline_file is not defined." << endl;
    }

    try
    {
        int n=0;

        _datafiles = config["data"]["increment"].as<vector<string>>();

        for (const auto& df : _datafiles)
	{
            ++n;
            cout << "Data file #" << n << ": " << df << endl;
        }
    } catch (const YAML::TypedBadConversion<vector<string>>& e) {
        cerr << "Error: 'data/increment' is not a list of strings!" << endl;
    }

    try
    {
        _atmfile = config["data"]["background"]["atm"].as<string>();

        cout << "atm file: " << _atmfile << endl;
    } catch (const YAML::TypedBadConversion<string>& e) {
        cerr << "Error: '_atmfile' is not a string!" << endl;
    }

    try
    {
        _sfcfile = config["data"]["background"]["sfc"].as<string>();

        cout << "sfc file: " << _sfcfile << endl;
    } catch (const YAML::TypedBadConversion<string>& e) {
        cerr << "Error: '_sfcfile' is not a string!" << endl;
    }
}

void YAMLHandler::write_yaml()
{
    YAML::Node output_config;
    output_config["database"]["host"] = "localhost";
    output_config["database"]["port"] = 5432;
    output_config["database"]["credentials"]["user"] = "admin";
    
    ofstream out_file("output.yaml");
    out_file << output_config;
}

