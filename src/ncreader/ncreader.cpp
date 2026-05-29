#include "ncreader.h"
#include <cstring>
 
// implementation of constructor method from ncReader.h
ncReader::ncReader(const char* fname) {
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
ncReader::~ncReader() {
    if (_isUFS) {
        delete[] _grid_xt;
        delete[] _grid_yt;
        delete[] _pfull;
        delete[] _phalf;
        delete[] _time;
        delete[] _time_iso;
        delete[] _lon;
        delete[] _lat;
        delete[] _lon2d;
        delete[] _lat2d;
    }

    if (NULL != _dimsize) delete[] _dimsize;
    if (NULL != _ntimes) delete[] _ntimes;
    close();
}
 
void ncReader::_get_dim_info() {
    int n = 0;
    char recname[NC_MAX_NAME+1];
    size_t length, recs;
  //cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
  //cout << "ncid: " << ncid << endl;
  //cout << "num_dims: " << num_dims << endl;

  //cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    if (NULL == _dimsize) _dimsize = new int[num_dims];
  //cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    if (NULL == _ntimes) _ntimes = new int[2];
     _ntimes[0] = 1;
     _ntimes[1] = 1;

  //cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    // Get Dimensions
  //cout << " Dimensions (" << num_dims << "):" << endl;
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

	if (0 == strcmp(recname, "grid_xt")) {
            _nlon = _dimsize[n];
          //cout << " _nlon " << _nlon << endl;
	}
	else if (0 == strcmp(recname, "grid_yt")) {
            _nlat = _dimsize[n];
          //cout << " _nlat " << _nlat << endl;
	}
	else if (0 == strcmp(recname, "phalf")) {
            _nhalf = _dimsize[n];
          //cout << " _nhalf " << _nhalf << endl;
	}
	else if (0 == strcmp(recname, "pfull")) {
            _nfull = _dimsize[n];
            _nlev = _dimsize[n];
          //cout << " _nfull " << _nfull << endl;
          //cout << " _nlev " << _nlev << endl;
	}
	else if (0 == strcmp(recname, "time")) {
            _ntim = _dimsize[n];
          //cout << " _ntim " << _ntim << endl;
	}
	else if (0 == strcmp(recname, "nchars")) {
            _nchars = _dimsize[n];
            _time_iso = new char[_nchars];
          //cout << " _nchars " << _nchars << endl;
	}

      //cout << "  - " << dim_names[n] << ": " << dim_length[n] << endl;
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
  //cout << " Variables (" << num_vars << "):" << endl;
    for (n = 0; n < num_vars; ++n) {
        status = nc_inq_var (ncid, n, 0, &var_type, &var_ndims, var_dimids, &var_natts);
        if (status != NC_NOERR) handle_error(status);

        status = nc_inq_varname(ncid, n, var_name);
        if (status != NC_NOERR) handle_error(status);

	var_names[n] = var_name;
      //cout << "  - " << var_name << " Type: " << var_type << endl;
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

  //cout << "Enter functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    num_grps = 1;

    status = nc_inq_attlen(ncid, NC_GLOBAL, "ak", &attr_len);
    if (status != NC_NOERR) handle_error(status);

  //cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
  //cout << "\tattr_len = " << attr_len << endl;

    _ak = new float[attr_len];
    _bk = new float[attr_len];

    status = nc_get_att_float(ncid, NC_GLOBAL, "ak", _ak);
    if (status != NC_NOERR) handle_error(status);
    status = nc_get_att_float(ncid, NC_GLOBAL, "bk", _bk);
    if (status != NC_NOERR) handle_error(status);

  //cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    _get_dim_info();
  //cout << "_nlon:" << _nlon << endl;
  //cout << "_nlat:" << _nlat << endl;
  //cout << "_nlev:" << _nlev << endl;
  //cout << "_ntim:" << _ntim << endl;
  //cout << "_nhalf:" << _nhalf << endl;
  //cout << "_nfull:" << _nfull << endl;
  //cout << "_nchars:" << _nchars << endl;
  //cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    _get_var_info();

   /*
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
    */


    // Find element with var name
    _grid_xt = getDouble("grid_xt");
    _grid_yt = getDouble("grid_yt");
    _lon2d = getDouble("lon");
    _lat2d = getDouble("lat");
    _time = getDouble("time");
    _time_iso = getChar("time_iso");
    _timestring = _time_iso;

  //cout << " _timestring:" << _timestring << endl;

    _pfull = getFloat("pfull");
    _phalf = getFloat("phalf");

    _lon = new double[_nlon];
    _lat = new double[_nlat];

    for (i=0; i<_nlon; ++i) {
	_lon[i] = _lon2d[i];
      //cout << "lon[" << i << "]= " << _lon[i] << endl;
    }
    for (j=0; j<_nlat; ++j) {
	n = j*_nlon;
	_lat[j] = _lat2d[n];
      //cout << "lat[" << j << "]= " << _lat[j] << endl;
    }

  //cout << "Leave functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
}
 
/*
float* ncReader::getFloat(const string var_name) {
    return getFloat(var_name.c_str());
}
 
double* ncReader::getDouble(const string var_name) {
    return getDouble(var_name.c_str());
}
*/
 
char* ncReader::getChar(const char* var_name) {
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

float* ncReader::getFloat(const char* var_name) {
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

double* ncReader::getDouble(const char* var_name) {
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

