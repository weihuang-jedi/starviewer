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

    double* _time;
    char* _time_iso;
    string _timestring;

    vector<size_t> dim_length;

    double* grid_xt;
    double* grid_yt;
    double* lon;
    double* lat;
    double* lon2d;
    double* lat2d;

    float* pfull;
    float* phalf;
    float* _ak;
    float* _bk;

    void _get_dim_info();
    void _get_var_info();

    bool _isMPAS = false;
    bool _isUFS = true;

    int* _dimsize;
    int* _ntimes;

    int _nlon;
    int _nlat;
    int _nlev;
    int _ntim;
    int _nfull;
    int _nhalf;
    int _nchars;
 
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

    char* getChar(const char* var_name);
    float* getFloat(const char* var_name);
    double* getDouble(const char* var_name);

    int getNumVars() { return num_vars; };
    int getNumV3ds() { return num_v3ds; };
    int getNumV2ds() { return num_v2ds; };
    int getNumDims() { return num_dims; };

    int getNlon() { return _nlon; };
    int getNlat() { return _nlat; };
    int getNlev() { return _nlev; };
    int getNtim() { return _ntim; };

    vector<string> getDimNames() { return dim_names; };
    vector<string> getV2dNames() { return v2d_names; };
    vector<string> getV3dNames() { return v3d_names; };

    double* getTim() { return _time; };
    string  getTimeString() { return _timestring; };

    double* getGridXt() { return grid_xt; };
    double* getGridYt() { return grid_yt; };
    double* getLon2d() { return lon2d; };
    double* getLat2d() { return lat2d; };
    float* getPfull() { return pfull; };
    float* getPhalf() { return phalf; };
    double* getLon() { return lon; };
    double* getLat() { return lat; };
    float* getLev() { return pfull; };

    // simulate ncl functions.
    double* get_dv( const char* vn ) { return getDouble(vn); };
    float* get_fv( const char* vn ) { return getFloat(vn); };
    int*   get_grdsize() { return _dimsize; };
    int*   get_varsize();
    int*   get_ntimes() { return _ntimes; };

    void select_file(int nf);
};
#endif
