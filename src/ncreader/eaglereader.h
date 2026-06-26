#ifndef _EAGLEREADER_H
#define _EAGLEREADER_H
 
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>

#include <netcdf.h>

using namespace std;
 
class EagleReader {
  private:
    // declare field or class variable filename to
    // be used in constructor
    string filename; 
    int ncid;
    int varid;
    int status;

    int num_gatts;
    int num_vars;
    int num_dims;
    int unlimdimid;

    vector<string> var_names;
    vector<string> dim_names;
    vector<size_t> dim_length;

    // double* _x;
    // double* _y;

    float* _longitude;
    float* _latitude;

    void _get_dim_info();
    void _get_var_info();

    int* _dimsize = NULL;
    long long int* _time = NULL;
    long long int forecast_reference_time;
    long long int CRS;

    int _nx;
    int _ny;
    int _ntime;
    int _nforecast_reference_time;

    string grid_mapping_name;
    float latitude_of_projection_origin;
    float longitude_of_central_meridian;
    vector<float> standard_parallel;
 
  public:
    // declare constructor and deconstructor methods
    EagleReader(const char*);
    virtual ~EagleReader();
 
    void handle_error(int status);

    size_t getVarSize(const char* var_name);

    // instance method(s)
    string getfilename(); 

    string get_grid_mapping_name() { return grid_mapping_name; };
    float get_latitude_of_projection_origin() { return latitude_of_projection_origin; };
    float get_longitude_of_central_meridian() { return longitude_of_central_meridian; };
    vector<float> get_standard_parallel() { return standard_parallel; };

    // class method(s)
    void exploreFile();
    void close();

    long long int* getInt64(const char* var_name);
    float* getFloat(const char* var_name);
    double* getDouble(const char* var_name);

    int getNumVars() { return num_vars; };
    int getNumDims() { return num_dims; };

    int getNx() { return _nx; };
    int getNy() { return _ny; };
    int getNtime() { return _ntime; };

    vector<string> getDimNames() { return dim_names; };
    vector<string> getVarNames() { return var_names; };

    string  getTimeString() { return string(""); };

    // double* getX() { return _x; };
    // double* getY() { return _y; };
    float* getLongitude() { return _longitude; };
    float* getLatitude() { return _latitude; };

    // simulate ncl functions.
    double* get_dv( const char* vn ) { return getDouble(vn); };
    float* get_fv( const char* vn ) { return getFloat(vn); };
    int*   get_grdsize() { return _dimsize; };
    int*   get_varsize();
    long long int* get_ntimes() { return _time; };

    void select_file(int nf);

    void grid_map_info();
};
#endif
