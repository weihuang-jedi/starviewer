#include "mpasstaticreader.h"
#include <cstring>
 
// implementation of constructor method from ncgridreader.h
MPASStaticReader::MPASStaticReader(const char* fname)
	         :NCBaseReader(fname)
{
    _dimMap =
    {
        {"nCells",        &_nCells},
        {"nVertices",     &_nVertices},
        {"nEdges",        &_nEdges},
        {"maxEdges",      &_maxEdges},
        {"maxEdges2",     &_maxEdges2},
        {"TWO",           &_TWO},
        {"vertexDegree",  &_vertexDegree},
        {"Strlen",        &_Strlen},
        {"Time",          &_nTime},
        {"R3",            &_R3},
        {"nMonth",        &_nMonth},
        {"FIFTEEN",       &_FIFTEEN},
        {"TWENTYONE",     &_TWENTYONE},
        {"nVertLevels",   &_nVertLevels}
    };

    exploreFile();
} 
 
// empty deconstructor method
MPASStaticReader::~MPASStaticReader() {
    if (_lonCell)
        delete[] _lonCell;
    if (_latCell)
        delete[] _latCell;
    if (_xCell)
        delete[] _xCell;
    if (_yCell)
        delete[] _yCell;
    if (_zCell)
        delete[] _zCell;

    if (_xtime)
        delete[] _xtime;

    if (_lonVertex)
        delete[] _lonVertex;
    if (_latVertex)
        delete[] _latVertex;
    if (_xVertex)
        delete[] _xVertex;
    if (_yVertex)
        delete[] _yVertex;
    if (_zVertex)
        delete[] _zVertex;

    if (_indexToCellID)
        delete[] _indexToCellID;
    if (_indexToVertexID)
        delete[] _indexToVertexID;
    if (_cellsOnCell)
        delete[] _cellsOnCell;
    if (_verticesOnCell)
        delete[] _verticesOnCell;
    if (_cellsOnVertex)
        delete[] _cellsOnVertex;
    close();
}
 
void MPASStaticReader::get_dim_info() {
    int n = 0;
    char recname[NC_MAX_NAME+1];
    size_t length, recs;
    // cout << "\nEnter functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    // cout << "ncid: " << ncid << endl;
    // cout << "num_dims: " << num_dims << endl;

    // cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    if (NULL == _dimsize) _dimsize = new int[num_dims];

    _nVertLevels = 1;
    // cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
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

	auto it = _dimMap.find(recname);
        if (it != _dimMap.end()) {
            *(it->second) = _dimsize[n]; // Dereference the variable pointer and assign the size!
        }

        // cout << "  - " << dim_names[n] << ": " << dim_length[n] << endl;
    }
    // cout << "Leave functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
}

void MPASStaticReader::get_var_info() {
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
    // cout << " Variables (" << num_vars << "):" << endl;
    for (n = 0; n < num_vars; ++n) {
        status = nc_inq_var (ncid, n, 0, &var_type, &var_ndims, var_dimids, &var_natts);
        if (status != NC_NOERR) handle_error(status);

        status = nc_inq_varname(ncid, n, var_name);
        if (status != NC_NOERR) handle_error(status);

	var_names[n] = var_name;
        // cout << "  - " << var_name << " Type: " << var_type << ", ndims: " << var_ndims << endl;
        if (1 == var_ndims) {
	    v2d_names[num_v2ds] = var_name;
            num_v2ds++;
        }
        if (2 == var_ndims) {
	    v3d_names[num_v3ds] = var_name;
            num_v3ds++;
        }
    }

    v2d_names.resize(num_v2ds);
    v3d_names.resize(num_v3ds);
}

// Function to dimensions, and variables
void MPASStaticReader::exploreFile() {
    int i = 0;
    int j = 0;
    int n = 0;
    size_t attr_len;

    // cout << "\nEnter functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    num_grps = 1;

    get_dim_info();
    // cout << "_nCells:" << _nCells << endl;
    // cout << "_nVertices:" << _nVertices << endl;
    // cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    get_var_info();

    // Find element with var name
    _lonCell = getDouble("lonCell");
    _latCell = getDouble("latCell");
    _xCell = getDouble("xCell");
    _yCell = getDouble("yCell");
    _zCell = getDouble("zCell");

    _lonVertex = getDouble("lonVertex");
    _latVertex = getDouble("latVertex");
    _xVertex = getDouble("xVertex");
    _yVertex = getDouble("yVertex");
    _zVertex = getDouble("zVertex");

    _indexToCellID = getInt("indexToCellID");
    _indexToVertexID = getInt("indexToVertexID");

    _cellsOnCell = getInt("cellsOnCell");
    _verticesOnCell = getInt("verticesOnCell");
    _cellsOnVertex = getInt("cellsOnVertex");

    _xtime = getChar("xtime");

    // cout << "Leave functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
}
 
