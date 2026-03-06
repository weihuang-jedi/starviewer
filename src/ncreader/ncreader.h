#ifndef NCREADER_H
#define NCREADER_H
 
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>

#include <netcdf.h>

using namespace std;
 
class ncReader {
  private:
    // declare field or class variable filename to
    // be used in constructor
    string filename; 
    string groupName; 
    int ncid;
    int varid;
    int groupid;
    int status;

    int num_grps;
    int num_gatts;
    int num_vars;
    int num_v2ds;
    int num_v3ds;
    int num_dims;
    int unlimdimid;

    vector<string> grp_names;
    vector<string> var_names;
    vector<string> dim_names;

    vector<string> v2d_names;
    vector<string> v3d_names;

    double* time;

    vector<size_t> dim_length;

    float* grid_xt;
    float* grid_yt;
    float* pfull;
    float* phalf;
    float* lon;
    float* lat;

    void _get_dim_info();
    void _get_var_info();

    bool _isMPAS = false;
    bool _isUFS = true;
 
  public:
    // declare constructor and deconstructor methods
    ncReader(const char*);
    virtual ~ncReader();
 
    void handle_error(int status);

    size_t getVarSize(const char* var_name);

    // instance method(s)
    string getfilename(); 

    // class method(s)
    void exploreFile();
    void close();

    // template <typename T>
    // void printVar(const string var_name, vector<T> value);
    // template <typename T>
    // vector<T> getValue(const string var_name);
    // template <typename T>
    // vector<T> getValue(const char* var_name);

    // float* getFloat(const string var_name);
    // double* getDouble(const string var_name);

    float* getFloat(const char* var_name);
    double* getDouble(const char* var_name);

    int getNumVars() { return num_vars; };
    int getNumV3ds() { return num_v3ds; };
    int getNumV2ds() { return num_v2ds; };
    int getNumDims() { return num_dims; };

    vector<string> getDimNames() { return dim_names; };
    vector<string> getV2dNames() { return v2d_names; };
    vector<string> getV3dNames() { return v3d_names; };

    double* getTim() { return time; };

    float* getGridXt() { return grid_xt; };
    float* getGridYt() { return grid_yt; };
    float* getPfull() { return pfull; };
    float* getPhalf() { return phalf; };
    float* getLon() { return lon; };
    float* getLat() { return lat; };
};
#endif
