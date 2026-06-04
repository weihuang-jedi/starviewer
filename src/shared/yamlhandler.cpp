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

vector<string> YAMLHandler::get_files(const char* type, YAML::Node config)
{
    // This vector will hold the 6 fully expanded paths
    vector<string> datafiles;

    if (config[type] && config[type]["data"]) {
        auto data_node = config[type]["data"];

        // Verify all required template pieces exist
        if (data_node["dir"] && data_node["prefix"] && data_node["suffix"] && 
            data_node["range_start"] && data_node["range_end"]) {
            
            string dir = data_node["dir"].as<string>();
            string prefix = data_node["prefix"].as<string>();
            string suffix = data_node["suffix"].as<string>();
            int start = data_node["range_start"].as<int>();
            int end = data_node["range_end"].as<int>();

            // Generate the file paths dynamically from start to end (1 to 6)
            for (int i = start; i <= end; ++i) {
                // Construct: /path/to/dir/prefix.tileX.suffix
                string full_path = dir + "/" + prefix + ".tile" + std::to_string(i) + "." + suffix;
                
                // Push into the C++ vector
                datafiles.push_back(full_path);
            }
        } else {
            cerr << "Error: Missing configuration key: <" << type << "> in increment.data template" << endl;
        }
    }

    // Verification: Print out the vector contents and size
    cout << "Vector Size: " << datafiles.size() << endl;
    for (const auto& path : datafiles) {
        std::cout << "Generated Path: " << path << endl;
    }

    return datafiles;
}

void YAMLHandler::read_yaml()
{
    ifstream fin(_flnm.c_str()); // Create a named object (lvalue)
    if (fin.is_open())
    {
        config = YAML::Load(fin);
    }

    if (config["application"].IsDefined())
    {
        _name = config["application"]["name"].as<string>();
        _model = config["application"]["model"].as<string>();
        _version = config["application"]["version"].as<string>();
        // cout << "name: " << _name << endl;
        cout << "model: " << _model << endl;
    }
    else
    {
        cerr << "Warning: application is not defined." << endl;
    }

    if (config["earth"].IsDefined())
    {
        _earth_bmp = config["earth"]["name"].as<string>();
        // cout << "earth_bmp: " << _earth_bmp << endl;
    }
    else
    {
        cerr << "Warning: earch bmp is not defined." << endl;
    }

    if (config["coastline"].IsDefined())
    {
        _coastline_file = config["coastline"]["name"].as<string>();
        _coastline_resolution = config["coastline"]["resolution"].as<string>();
        // cout << "coastline_file: " << _coastline_file << endl;
        // cout << "coastline_resolution: " << _coastline_resolution << endl;
    }
    else
    {
        cerr << "Warning: coastline_file is not defined." << endl;
    }

    if (config["input"].IsDefined())
    {
        _datafiles = get_files("input", config);
    }

    if (config["grid"].IsDefined())
    {
        _gridfiles = get_files("grid", config);
    }

    if (config["increment"].IsDefined())
    {
        _incrfiles = get_files("increment", config);
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

