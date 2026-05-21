#include "ufsincrementreader.h"
#include <cstring>
 
// implementation of constructor method from ncgridreader.h
UFSIncrementReader::UFSIncrementReader(const char* fname)
             :NCBaseReader(fname)
{
  //cout << "\nEnter functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    exploreFile();
  //cout << "Leave functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
} 
 
// empty deconstructor method
UFSIncrementReader::~UFSIncrementReader() {
    if (_xaxis_1)
        delete[] _xaxis_1;
    if (_yaxis_1)
        delete[] _yaxis_1;
    if (_zaxis_1)
        delete[] _zaxis_1;
    if (_Time)
        delete[] _Time;
    close();
}
 
void UFSIncrementReader::get_dim_info() {
    int n = 0;
    char recname[NC_MAX_NAME+1];
    size_t length, recs;
    cout << "\nEnter functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    cout << "ncid: " << ncid << endl;
    cout << "num_dims: " << num_dims << endl;

    cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    if (NULL == _dimsize) _dimsize = new int[num_dims];
    cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;

    cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    // Get Dimensions
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

	if (0 == strcmp(recname, "xaxis_1")) {
            _nx = _dimsize[n];
            cout << " _nx " << _nx << endl;
	}
	else if (0 == strcmp(recname, "yaxis_1")) {
            _ny = _dimsize[n];
            cout << " _ny " << _ny << endl;
	}
	else if (0 == strcmp(recname, "zaxis_1")) {
            _nz = _dimsize[n];
            cout << " _nz " << _nz << endl;
	}
	else if (0 == strcmp(recname, "Time")) {
            _nt = _dimsize[n];
            cout << " _nt " << _nt << endl;
	}

        cout << "  - " << dim_names[n] << ": " << dim_length[n] << endl;
    }
    cout << "Leave functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
}

void UFSIncrementReader::get_var_info() {
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
        if (4 == var_ndims) {
	    v3d_names[num_v3ds] = var_name;
            num_v3ds++;
        }
    }

    v3d_names.resize(num_v3ds);
}

// Function to dimensions, and variables
void UFSIncrementReader::exploreFile() {
    int i = 0;
    int j = 0;
    int n = 0;
    size_t attr_len;

    cout << "\nEnter functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    num_grps = 1;

    status = nc_inq_attlen(ncid, NC_GLOBAL, "ak", &attr_len);
    if (status != NC_NOERR) handle_error(status);

    get_dim_info();
    cout << "_nx:" << _nx << endl;
    cout << "_ny:" << _ny << endl;
    cout << "_nz:" << _nz << endl;
    cout << "_nt:" << _nt << endl;
    cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    get_var_info();

    // Find element with var name
    _xaxis_1 = getDouble("xaxis_1");
    _yaxis_1 = getDouble("yaxis_1");
    _zaxis_1 = getDouble("zaxis_1");
    _Time = getDouble("Time");
    cout << "Leave functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
}
 
