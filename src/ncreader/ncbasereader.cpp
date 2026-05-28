#include "ncbasereader.h"
#include <cstring>
 
// implementation of constructor method from ncbasereader.h
NCBaseReader::NCBaseReader(const char* fname) {
  //cout << "Enter functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    filename = (string)fname;
    // Open the file in read-only mode.
    status = nc_open(fname, NC_NOWRITE, &ncid);
    if (status != NC_NOERR) handle_error(status);
    status = nc_inq(ncid, &num_dims, &num_vars, &num_gatts, &unlimdimid);
    if (status != NC_NOERR) handle_error(status);
  //cout << "Leave functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
} 
 
// static or class method to return filename of NetCDF passed-in
string NCBaseReader::getfilename() {
  return this->filename;
}
 
void NCBaseReader::close() {
   // Close the file.
   status = nc_close(ncid);
   if (status != NC_NOERR) handle_error(status);
}
 
void NCBaseReader::handle_error(int status) {
    if (status != NC_NOERR) {
        fprintf(stderr, "%s\n", nc_strerror(status));
        exit(-1);
    }
}
 
int* NCBaseReader::getInt(const char* var_name) {
    int var_id;
    size_t var_length = 1;
    size_t length = 1;

    status = nc_inq_varid (ncid, var_name, &var_id);
    if (status != NC_NOERR) handle_error(status);

    var_length = getVarSize(var_name);

    int* value = new int[var_length];

    status = nc_get_var_int(ncid, var_id, value);
    if (status != NC_NOERR) handle_error(status);

    return value;
}

char* NCBaseReader::getChar(const char* var_name) {
    int var_id;
    size_t var_length = 1;
    size_t length = 1;

    status = nc_inq_varid (ncid, var_name, &var_id);
    if (status != NC_NOERR) handle_error(status);

    var_length = getVarSize(var_name);

    char* value = new char[var_length];

    status = nc_get_var_text(ncid, var_id, value);
    if (status != NC_NOERR) handle_error(status);

    return value;
}

float* NCBaseReader::getFloat(const char* var_name) {
    int var_id;
    size_t var_length = 1;
    size_t length = 1;

    // cout << "\nEnter functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    status = nc_inq_varid (ncid, var_name, &var_id);
    if (status != NC_NOERR) handle_error(status);

    var_length = getVarSize(var_name);
    // cout << "var_name: " << var_name << ", ncid = " << ncid << ", var_id = " << var_id << ", var_length = " << var_length << endl;

    float* value = new float[var_length];
    
    status = nc_get_var_float(ncid, var_id, value);
    if (status != NC_NOERR) handle_error(status);
    // cout << "value[0] = " << value[0] << endl;

    _has_missing_value = false;
    // Read the _FillValue attribute
    // Note: Use the function matching your variable's data type
    status = nc_get_att_float(ncid, varid, "missing_value", &_missing_value);

    if (status == NC_NOERR) {
        // cout << "The missing value is: " << _missing_value << endl;
        _has_missing_value = true;
    }
    else
    {
        // cout << "No missing_value found.\n" << endl;
        // handle_error(status);

        status = nc_get_att_float(ncid, varid, "_FillValue", &_missing_value);

        if (status == NC_NOERR) {
            // cout << "The missing_value is: " << _missing_value << endl;
            _has_missing_value = true;
        }
        // else
        // {
        //     cout << "No _FillValue found.\n" << endl;
        // }
    }

    // cout << "Leave functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    return value;
}

double* NCBaseReader::getDouble(const char* var_name) {
    int var_id;
    size_t var_length = 1;
    size_t length = 1;

  //cout << "Enter functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    status = nc_inq_varid (ncid, var_name, &var_id);
    if (status != NC_NOERR) handle_error(status);
    
    var_length = getVarSize(var_name);
  //cout << "var_name: " << var_name << ", ncid = " << ncid << ", var_id = " << var_id << ", var_length = " << var_length << endl;

    double* value = new double[var_length];
    
    status = nc_get_var_double(ncid, var_id, value);
    if (status != NC_NOERR) handle_error(status);
  //cout << "value[0] = " << value[0] << endl;

  //cout << "Leave functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    return value;
}

/* 
template <typename T>
void NCBaseReader::printVar(const string var_name, T* value) {
    cout << "var name: " << var_name << endl;
    for (const auto& element : value) {
        cout << element << " ";
    }
    cout << endl;
}
*/
 
// Function to recursively explore groups, dimensions, and variables
size_t NCBaseReader::getVarSize(const char* var_name) {
    int var_id, n;
    nc_type var_type;
    int var_ndims;
    int var_dimids[NC_MAX_VAR_DIMS];
    int var_natts;
    size_t var_length = 1;
    size_t length = 1;

    status = nc_inq_varid (ncid, var_name, &var_id);
    if (status != NC_NOERR) handle_error(status);
    
    status = nc_inq_var (ncid, var_id, 0, &var_type, &var_ndims, var_dimids, &var_natts);
    if (status != NC_NOERR) handle_error(status);

    for (n = 0; n < var_ndims; ++n) {
        status = nc_inq_dimlen(ncid, var_dimids[n], &length);
        if (status != NC_NOERR) handle_error(status);
        
        var_length *= length;
    }

    return var_length;
}

void NCBaseReader::select_file(int nf) {
    cout << "Select file No.: " << nf << endl;
}

