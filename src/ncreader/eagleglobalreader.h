#ifndef _EAGLEGLOBALREADER_H
#define _EAGLEGLOBALREADER_H
 
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>

#include <netcdf.h>

using namespace std;
 
class EagleGlobalReader {
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

    double* _longitude;
    double* _latitude;

    void _get_dim_info();
    void _get_var_info();

    int* _dimsize = NULL;
    long long int* _time = NULL;
    long long int forecast_reference_time;
    long long int CRS;

    int _nlon;
    int _nlat;
    int _ntime;
    int _nforecast_reference_time;
 
  public:
    // declare constructor and deconstructor methods
    EagleGlobalReader(string flnm);
    virtual ~EagleGlobalReader();
 
    void handle_error(int status);

    size_t getVarSize(const char* var_name);

    // instance method(s)
    string getfilename(); 

    // class method(s)
    void exploreFile();
    void close();

    long long int* getInt64(const char* var_name);
    float* getFloat(const char* var_name);
    double* getDouble(const char* var_name);

    int getNumVars() { return num_vars; };
    int getNumDims() { return num_dims; };

    int getNlon() { return _nlon; };
    int getNlat() { return _nlat; };
    int getNtime() { return _ntime; };

    vector<string> getDimNames() { return dim_names; };
    vector<string> getVarNames() { return var_names; };

    string  getTimeString() { return string(""); };

    double* getLongitude() { return _longitude; };
    double* getLatitude() { return _latitude; };

    // simulate ncl functions.
    double* get_dv( const char* vn ) { return getDouble(vn); };
    float* get_fv( const char* vn ) { return getFloat(vn); };
    int*   get_grdsize() { return _dimsize; };
    int*   get_varsize();
    long long int* get_ntimes() { return _time; };

    void select_file(int nf);
};
#endif
