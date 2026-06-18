#include "eagleglobalreader.h"
#include <cstring>
 
// implementation of constructor method from EagleGlobalReader.h
EagleGlobalReader::EagleGlobalReader(const char* fname) {
  //cout << "Enter functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    filename = (string)fname;
    // Open the file in read-only mode.
    status = nc_open(fname, NC_NOWRITE, &ncid);
    if (status != NC_NOERR) handle_error(status);
    status = nc_inq(ncid, &num_dims, &num_vars, &num_gatts, &unlimdimid);
    if (status != NC_NOERR) handle_error(status);

    exploreFile();
  //cout << "Leave functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
} 
 
// empty deconstructor method
EagleGlobalReader::~EagleGlobalReader() {
    delete[] _time;
    delete[] _longitude;
    delete[] _latitude;

    if (NULL != _dimsize) delete[] _dimsize;
    if (NULL != _time) delete[] _time;

    dim_names.clear();
    var_names.clear();

    close();
}
 
void EagleGlobalReader::_get_dim_info() {
    int n = 0;
    char recname[NC_MAX_NAME+1];
    size_t length, recs;
    // cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;

    if (NULL == _dimsize) _dimsize = new int[num_dims];
    // Get Dimensions
    // cout << " Dimensions (" << num_dims << "):" << endl;
    dim_names.resize(num_dims);
    dim_length.resize(num_dims);
    for (n = 0; n < num_dims; ++n) {
        status = nc_inq_dimlen(ncid, n, &length);
        if (status != NC_NOERR) handle_error(status);
	
	dim_length[n] = length;
	_dimsize[n] = (int) length;
 
        status = nc_inq_dim(ncid, n, recname, &recs);
        if (status != NC_NOERR) handle_error(status);
	dim_names[n] = recname;

	if (0 == strcmp(recname, "longitude")) {
            _nlon = _dimsize[n];
	}
	else if (0 == strcmp(recname, "latitude")) {
            _nlat = _dimsize[n];
	}
	else if (0 == strcmp(recname, "time")) {
            _ntime = _dimsize[n];
	}
	else if (0 == strcmp(recname, "forecast_reference_time")) {
            _nforecast_reference_time = _dimsize[n];
	}
        // cout << "  - " << dim_names[n] << ": " << dim_length[n] << endl;
    }
}

void EagleGlobalReader::_get_var_info() {
    int n = 0; 
    int  var_id;
    nc_type var_type;
    int var_ndims;
    int var_dimids[NC_MAX_VAR_DIMS];
    int var_natts;
    char var_name[NC_MAX_NAME+1];

    var_names.resize(num_vars);
    // Get Variables
    // cout << " Variables (" << num_vars << "):" << endl;
    for (n = 0; n < num_vars; ++n) {
        status = nc_inq_var (ncid, n, 0, &var_type, &var_ndims, var_dimids, &var_natts);
        if (status != NC_NOERR) handle_error(status);

        status = nc_inq_varname(ncid, n, var_name);
        if (status != NC_NOERR) handle_error(status);

	var_names[n] = var_name;
    }
}

// static or class method to return filename of NetCDF passed-in
string EagleGlobalReader::getfilename() {
  return this->filename;
}
 
void EagleGlobalReader::close() {
   // Close the file.
   status = nc_close(ncid);
   if (status != NC_NOERR) handle_error(status);
}
 
void EagleGlobalReader::handle_error(int status) {
    if (status != NC_NOERR) {
        fprintf(stderr, "%s\n", nc_strerror(status));
        exit(-1);
    }
}
 
// Function to dimensions, and variables
void EagleGlobalReader::exploreFile() {
    int i = 0;
    int j = 0;
    int n = 0;
    size_t attr_len;

    // cout << "Enter functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;

    _get_dim_info();
    // cout << "_nlon:" << _nlon << endl;
    // cout << "_nlat:" << _nlat << endl;
    // cout << "_ntime:" << _ntime << endl;

    _get_var_info();

   /*
    cout << "Dimensions:" << endl;
    for (n=0; n<num_dims; ++n) {
	cout << "dim " << n << " name: <" << dim_names[n] << ">" << endl;
    }

    cout << "Vars:" << endl;
    for (n=0; n<num_vars; ++n) {
	cout << "var " << n << " name: <" << var_names[n] << ">" << endl;
    }
    */


    // Find element with var name
    _longitude = getDouble("longitude");
    _latitude = getDouble("latitude");
    _time = getInt64("time");

    // cout << "Leave functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
}
 
long long int* EagleGlobalReader::getInt64(const char* var_name) {
    int var_id;
    size_t var_length = 1;
    size_t length = 1;

    status = nc_inq_varid (ncid, var_name, &var_id);
    if (status != NC_NOERR) handle_error(status);

    var_length = getVarSize(var_name);

    long long int* value = new long long int[var_length];

    status = nc_get_var_longlong(ncid, var_id, value);
    if (status != NC_NOERR) handle_error(status);

    return value;
}

float* EagleGlobalReader::getFloat(const char* var_name) {
    int var_id;
    size_t var_length = 1;
    size_t length = 1;

  //cout << "Enter functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    status = nc_inq_varid (ncid, var_name, &var_id);
    if (status != NC_NOERR) handle_error(status);

    var_length = getVarSize(var_name);
  //cout << "var_name: " << var_name << ", ncid = " << ncid << ", var_id = " << var_id << ", var_length = " << var_length << endl;

    float* value = new float[var_length];
    
    status = nc_get_var_float(ncid, var_id, value);
    if (status != NC_NOERR) handle_error(status);
  //cout << "value[0] = " << value[0] << endl;

  //cout << "Leave functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    return value;
}

double* EagleGlobalReader::getDouble(const char* var_name) {
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

// Function to recursively explore groups, dimensions, and variables
size_t EagleGlobalReader::getVarSize(const char* var_name) {
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

void EagleGlobalReader::select_file(int nf) {
    cout << "Select file No.: " << nf << endl;
}

