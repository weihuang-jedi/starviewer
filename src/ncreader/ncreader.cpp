#include "ncreader.h"
 
// implementation of constructor method from ncReader.h
ncReader::ncReader(const char* fname) {
    filename = (string)fname;
    // Open the file in read-only mode.
    status = nc_open(fname, NC_NOWRITE, &ncid);
    if (status != NC_NOERR) handle_error(status);
    status = nc_inq(ncid, &num_dims, &num_vars, &num_gatts, &unlimdimid);
    if (status != NC_NOERR) handle_error(status);
} 
 
// empty deconstructor method
ncReader::~ncReader() {
    if (_isUFS) {
        delete[] grid_xt;
        delete[] grid_yt;
        delete[] pfull;
        delete[] phalf;
        delete[] time;
        delete[] lon;
        delete[] lat;
        delete[] lon2d;
        delete[] lat2d;
    }

    if (NULL != _dimsize) delete[] _dimsize;
    if (NULL != _ntimes) delete[] _ntimes;
}
 
void ncReader::_get_dim_info() {
    int n = 0;
    char recname[NC_MAX_NAME+1];
    size_t length, recs;
    cout << "ncid: " << ncid << endl;

    if (NULL == _dimsize) _dimsize = new int(num_dims);
    if (NULL == _ntimes) _dimsize = new int(1);

    // Get Dimensions
    cout << " Dimensions (" << num_dims << "):" << endl;
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

	if (recname == "grid_xt")
            _nlon = (int) length;
	if (recname == "grid_yt")
            _nlat = (int) length;
	if (recname == "phalf")
            _nhalf = (int) length;
	if (recname == "pfull") {
            _nfull = (int) length;
            _nlev = (int) length;
	}
	if (recname == "time")
            _ntim = (int) length;

        cout << "  - " << dim_names[n] << ": " << dim_length[n] << endl;
    }
}

void ncReader::_get_var_info() {
    int n = 0; 
    int  var_id;
    nc_type var_type;
    int var_ndims;
    int var_dimids[NC_MAX_VAR_DIMS];
    int var_natts;
    char var_name[NC_MAX_NAME+1];

    num_v2ds = 0;
    num_v3ds = 0;
    v2d_names.resize(num_vars);
    v3d_names.resize(num_vars);
    // Get Variables
    var_names.resize(num_vars);
    cout << " Variables (" << num_vars << "):" << endl;
    for (n = 0; n < num_vars; ++n) {
        status = nc_inq_var (ncid, n, 0, &var_type, &var_ndims, var_dimids, &var_natts);
        if (status != NC_NOERR) handle_error(status);


        status = nc_inq_varname(ncid, n, var_name);
        if (status != NC_NOERR) handle_error(status);

	var_names[n] = var_name;
        cout << "  - " << var_name << " Type: " << var_type << endl;
        if (3 == var_ndims) {
	    v2d_names[num_v2ds] = var_name;
            num_v2ds++;
        } else if (4 == var_ndims) {
	    v3d_names[num_v3ds] = var_name;
            num_v3ds++;
        }
    }

    v2d_names.resize(num_v2ds);
    v3d_names.resize(num_v3ds);
}

// static or class method to return filename of NetCDF passed-in
string ncReader::getfilename() {
  return this->filename;
}
 
void ncReader::close() {
   // Close the file.
   status = nc_close(ncid);
   if (status != NC_NOERR) handle_error(status);
}
 
void ncReader::handle_error(int status) {
    if (status != NC_NOERR) {
        fprintf(stderr, "%s\n", nc_strerror(status));
        exit(-1);
    }
}
 
// Function to dimensions, and variables
void ncReader::exploreFile() {
    int i = 0;
    int j = 0;
    int n = 0;
    size_t attr_len;

    num_grps = 1;

    status = nc_inq_attlen(ncid, NC_GLOBAL, "ak", &attr_len);
    if (status != NC_NOERR) handle_error(status);

    _ak = new float[attr_len];
    _bk = new float[attr_len];

    status = nc_get_att_float(ncid, NC_GLOBAL, "ak", _ak);
    if (status != NC_NOERR) handle_error(status);
    status = nc_get_att_float(ncid, NC_GLOBAL, "bk", _bk);
    if (status != NC_NOERR) handle_error(status);

    _get_dim_info();
    _get_var_info();

    cout << "Dimensions:" << endl;
    for (n=0; n<num_dims; ++n) {
	cout << "dim " << n << " name: <" << dim_names[n] << ">" << endl;
    }

    cout << "2D vars:" << endl;
    for (n=0; n<num_v2ds; ++n) {
	cout << "var " << n << " name: <" << v2d_names[n] << ">" << endl;
    }

    cout << "3D vars:" << endl;
    for (n=0; n<num_v3ds; ++n) {
	cout << "var " << n << " name: <" << v3d_names[n] << ">" << endl;
    }


    // Find element with var name
    grid_xt = getDouble("grid_xt");
    grid_yt = getDouble("grid_yt");
    lon2d = getDouble("lon");
    lat2d = getDouble("lat");
    time = getDouble("time");

    pfull = getFloat("pfull");
    phalf = getFloat("phalf");

    double* lon = new double[_nlon];
    double* lat = new double[_nlat];

    for (i=0; i<_nlon; ++i) {
	lon[i] = lon2d[i];
	cout << "lon[" << i << "]= " << lon[i] << endl;
    }
    for (j=0; j<_nlat; ++j) {
	n = j*_nlon;
	lat[j] = lat2d[n];
	cout << "lat[" << j << "]= " << lat[j] << endl;
    }

   /*
    double* DBLlev = new double[_nlev];
    for (n=0; n<_nlev; ++n) {
	DBLlev[n] = pfull[n];
	cout << "pfull[" << n << "]= " << pfull[n] << endl;
    }
    */
}
 
/*
float* ncReader::getFloat(const string var_name) {
    return getFloat(var_name.c_str());
}
 
double* ncReader::getDouble(const string var_name) {
    return getDouble(var_name.c_str());
}
*/
 
float* ncReader::getFloat(const char* var_name) {
    int var_id;
    size_t var_length = 1;
    size_t length = 1;

    status = nc_inq_varid (ncid, var_name, &var_id);
    if (status != NC_NOERR) handle_error(status);

    var_length = getVarSize(var_name);

    float* value = new float[var_length];
    
    status = nc_get_var_float(ncid, var_id, value);
    if (status != NC_NOERR) handle_error(status);

    return value;
}

double* ncReader::getDouble(const char* var_name) {
    int var_id;
    size_t var_length = 1;
    size_t length = 1;

    status = nc_inq_varid (ncid, var_name, &var_id);
    if (status != NC_NOERR) handle_error(status);
    
    var_length = getVarSize(var_name);

    double* value = new double[var_length];
    
    status = nc_get_var_double(ncid, var_id, value);
    if (status != NC_NOERR) handle_error(status);

    return value;
}

/* 
template <typename T>
void ncReader::printVar(const string var_name, T* value) {
    cout << "var name: " << var_name << endl;
    for (const auto& element : value) {
        cout << element << " ";
    }
    cout << endl;
}
*/
 
// Function to recursively explore groups, dimensions, and variables
size_t ncReader::getVarSize(const char* var_name) {
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

void ncReader::select_file(int nf) {
    cout << "Select file No.: " << nf << endl;
}

