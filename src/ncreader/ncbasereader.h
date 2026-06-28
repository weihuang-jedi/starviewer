#ifndef _NCBASEREADER_H
#define _NCBASEREADER_H
 
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>

#include <netcdf.h>

using namespace std;
 
class NCBaseReader {
  public:
    // declare constructor and deconstructor methods
    NCBaseReader(const char*);
    virtual ~NCBaseReader() = default;
 
    void handle_error(int status);

    size_t getVarSize(const char* var_name);

    // instance method(s)
    string getfilename(); 

    // class method(s)
    virtual void exploreFile() = 0;
    virtual void get_dim_info() = 0;
    virtual void get_var_info() = 0;

    void close();

    long long int* getInt64(const char* var_name);
    int* getInt(const char* var_name);
    char* getChar(const char* var_name);
    float* getFloat(const char* var_name);
    double* getDouble(const char* var_name);

    int getNumVars() { return num_vars; };
    int getNumV3ds() { return num_v3ds; };
    int getNumV2ds() { return num_v2ds; };
    int getNumDims() { return num_dims; };

    vector<string> getDimNames() { return dim_names; };
    vector<string> getV2dNames() { return v2d_names; };
    vector<string> getV3dNames() { return v3d_names; };

    // simulate ncl functions.
    double* get_dv( const char* vn ) { return getDouble(vn); };
    float* get_fv( const char* vn ) { return getFloat(vn); };
    int* get_iv( const char* vn ) { return getInt(vn); };
    int*   get_grdsize() { return _dimsize; };
    int*   get_varsize();

    void select_file(int nf);

    bool has_missing_value() { return _has_missing_value; };
    float get_missing_value() { return _missing_value; };

  protected:
    // declare field or class variable filename to
    // be used in constructor
    string filename; 
    string groupName; 
    int ncid;
    int varid;
    int groupid;
    int status;

    float _missing_value;
    bool _has_missing_value;

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

    vector<size_t> dim_length;

    int* _dimsize = NULL;
};
#endif
