#include "eaglereader.h"
#include <cstring>
 
// implementation of constructor method from EagleReader.h
EagleReader::EagleReader(const char* fname) {
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
EagleReader::~EagleReader() {
    // delete[] _x;
    // delete[] _y;
    delete[] _time;
    delete[] _longitude;
    delete[] _latitude;

    if (NULL != _dimsize) delete[] _dimsize;
    if (NULL != _time) delete[] _time;

    dim_names.clear();
    var_names.clear();

    close();
}
 
void EagleReader::_get_dim_info() {
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

	if (0 == strcmp(recname, "x")) {
            _nx = _dimsize[n];
	}
	else if (0 == strcmp(recname, "y")) {
            _ny = _dimsize[n];
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

void EagleReader::_get_var_info() {
    int n = 0; 
    int  varid;
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
string EagleReader::getfilename() {
  return this->filename;
}
 
void EagleReader::close() {
   // Close the file.
   status = nc_close(ncid);
   if (status != NC_NOERR) handle_error(status);
}
 
void EagleReader::handle_error(int status) {
    if (status != NC_NOERR) {
        fprintf(stderr, "%s\n", nc_strerror(status));
        exit(-1);
    }
}
 
// Function to dimensions, and variables
void EagleReader::exploreFile() {
    int i = 0;
    int j = 0;
    int n = 0;
    size_t attr_len;

    // cout << "Enter functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;

    _get_dim_info();
    // cout << "_nx:" << _nx << endl;
    // cout << "_ny:" << _ny << endl;
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
    // _x = getDouble("x");
    // _y = getDouble("y");
    _longitude = getFloat("longitude");
    _latitude = getFloat("latitude");
    _time = getInt64("time");

//  int64 CRS ;
//      CRS:false_easting = 0. ;
//      CRS:false_northing = 0. ;
//      CRS:grid_mapping_name = "lambert_conformal_conic" ;
//      CRS:latitude_of_projection_origin = 38.5 ;
//      CRS:longitude_of_central_meridian = 262.5 ;
//      CRS:standard_parallel = 38.5, 38.5 ;

    grid_mapping_name = "lambert_conformal_conic" ;
    latitude_of_projection_origin = 38.5 ;
    longitude_of_central_meridian = 262.5 ;
    standard_parallel.push_back(38.5);
    standard_parallel.push_back(38.5);

    grid_map_info();

    // cout << "Leave functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
}

// Function to recursively explore groups, dimensions, and variables
void EagleReader::grid_map_info() {
    int varid, n;
    nc_type var_type;
    int var_ndims;
    int var_dimids[NC_MAX_VAR_DIMS];
    int var_natts;
    size_t att_len = 1;

    // cout << "Enter functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    status = nc_inq_varid (ncid, "CRS", &varid);
    if (status != NC_NOERR) handle_error(status);
    
    // cout << "functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    // cout << "var CRS id: " << varid << endl;
    status = nc_inq_var (ncid, varid, 0, &var_type, &var_ndims, var_dimids, &var_natts);
    if (status != NC_NOERR) handle_error(status);

    // 1. Get the length of the attribute array
    status = nc_inq_attlen(ncid, varid, "grid_mapping_name", &att_len);
    if (status != NC_NOERR) handle_error(status);

    // 2. Allocate buffer (add 1 for string null-terminator)
    char *buffer = (char *)malloc(att_len + 1);

    // 3. Read the attribute text data
    status = nc_get_att_text(ncid, varid, "grid_mapping_name", buffer);
    if (status == NC_NOERR) {
        buffer[att_len] = '\0'; // Properly terminate C-string
        // cout << "Variable Attribute <grid_mapping_name> = " << buffer << endl;
        grid_mapping_name = buffer;
    }
    free(buffer);

    nc_get_att_float(ncid, varid, "latitude_of_projection_origin", &latitude_of_projection_origin);
    nc_get_att_float(ncid, varid, "longitude_of_central_meridian", &longitude_of_central_meridian);

    // cout << "latitude_of_projection_origin: " << latitude_of_projection_origin << endl;
    // cout << "longitude_of_central_meridian: " << longitude_of_central_meridian << endl;

    float *buf = new float[2];
    nc_get_att_float(ncid, varid, "standard_parallel", buf);
    standard_parallel[0] = buf[0];
    standard_parallel[1] = buf[1];
    delete[] buf;

    // cout << "standard_parallel[0]: " << standard_parallel[0] << endl;
    // cout << "standard_parallel[1]: " << standard_parallel[1] << endl;
    // cout << "Leave functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
}
 
long long int* EagleReader::getInt64(const char* var_name) {
    int varid;
    size_t var_length = 1;
    size_t length = 1;

    status = nc_inq_varid (ncid, var_name, &varid);
    if (status != NC_NOERR) handle_error(status);

    var_length = getVarSize(var_name);

    long long int* value = new long long int[var_length];

    status = nc_get_var_longlong(ncid, varid, value);
    if (status != NC_NOERR) handle_error(status);

    return value;
}

float* EagleReader::getFloat(const char* var_name) {
    int varid;
    size_t var_length = 1;
    size_t length = 1;

  //cout << "Enter functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    status = nc_inq_varid (ncid, var_name, &varid);
    if (status != NC_NOERR) handle_error(status);

    var_length = getVarSize(var_name);
  //cout << "var_name: " << var_name << ", ncid = " << ncid << ", varid = " << varid << ", var_length = " << var_length << endl;

    float* value = new float[var_length];
    
    status = nc_get_var_float(ncid, varid, value);
    if (status != NC_NOERR) handle_error(status);
  //cout << "value[0] = " << value[0] << endl;

  //cout << "Leave functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    return value;
}

double* EagleReader::getDouble(const char* var_name) {
    int varid;
    size_t var_length = 1;
    size_t length = 1;

  //cout << "Enter functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    status = nc_inq_varid (ncid, var_name, &varid);
    if (status != NC_NOERR) handle_error(status);
    
    var_length = getVarSize(var_name);
  //cout << "var_name: " << var_name << ", ncid = " << ncid << ", varid = " << varid << ", var_length = " << var_length << endl;

    double* value = new double[var_length];
    
    status = nc_get_var_double(ncid, varid, value);
    if (status != NC_NOERR) handle_error(status);
  //cout << "value[0] = " << value[0] << endl;

  //cout << "Leave functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    return value;
}

// Function to recursively explore groups, dimensions, and variables
size_t EagleReader::getVarSize(const char* var_name) {
    int varid, n;
    nc_type var_type;
    int var_ndims;
    int var_dimids[NC_MAX_VAR_DIMS];
    int var_natts;
    size_t var_length = 1;
    size_t length = 1;

    status = nc_inq_varid (ncid, var_name, &varid);
    if (status != NC_NOERR) handle_error(status);
    
    status = nc_inq_var (ncid, varid, 0, &var_type, &var_ndims, var_dimids, &var_natts);
    if (status != NC_NOERR) handle_error(status);

    for (n = 0; n < var_ndims; ++n) {
        status = nc_inq_dimlen(ncid, var_dimids[n], &length);
        if (status != NC_NOERR) handle_error(status);
        
        var_length *= length;
    }

    return var_length;
}

void EagleReader::select_file(int nf) {
    cout << "Select file No.: " << nf << endl;
}

