#include "ufsgridreader.h"
#include <cstring>
 
// implementation of constructor method from ncgridreader.h
UFSGridReader::UFSGridReader(const char* fname)
	      :NCBaseReader(fname)
{
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
UFSGridReader::~UFSGridReader() {
    if (_geolon)
        delete[] _geolon;
    if (_geolat)
        delete[] _geolat;
    close();
}
 
void UFSGridReader::get_dim_info() {
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

	if (0 == strcmp(recname, "lon")) {
            _nlon = _dimsize[n];
            cout << " _nlon " << _nlon << endl;
	}
	else if (0 == strcmp(recname, "lat")) {
            _nlat = _dimsize[n];
            cout << " _nlat " << _nlat << endl;
	}

        cout << "  - " << dim_names[n] << ": " << dim_length[n] << endl;
    }
    cout << "Leave functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
}

void UFSGridReader::get_var_info() {
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
        if (2 == var_ndims) {
	    v2d_names[num_v2ds] = var_name;
            num_v2ds++;
        }
    }

    v2d_names.resize(num_v2ds);
}

// Function to dimensions, and variables
void UFSGridReader::exploreFile() {
    int i = 0;
    int j = 0;
    int n = 0;
    size_t attr_len;

    cout << "\nEnter functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    num_grps = 1;

    status = nc_inq_attlen(ncid, NC_GLOBAL, "ak", &attr_len);
    if (status != NC_NOERR) handle_error(status);

    get_dim_info();
    cout << "_nlon:" << _nlon << endl;
    cout << "_nlat:" << _nlat << endl;
    cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    get_var_info();

    // Find element with var name
    _geolon = getFloat("lon");
    _geolat = getFloat("lat");
    cout << "Leave functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
}
 
