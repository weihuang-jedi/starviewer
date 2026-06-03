#include "mpasstaticreader.h"
#include <cstring>
 
// implementation of constructor method from ncgridreader.h
MPASStaticReader::MPASStaticReader(const char* fname)
	         :NCBaseReader(fname)
{
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
    cout << "\nEnter functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    cout << "ncid: " << ncid << endl;
    cout << "num_dims: " << num_dims << endl;

    cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    if (NULL == _dimsize) _dimsize = new int[num_dims];

    _nVertLevels = 1;
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

	if (0 == strcmp(recname, "nCells")) {
            _nCells = _dimsize[n];
            cout << " _nCells " << _nCells << endl;
	}
	else if (0 == strcmp(recname, "nVertices")) {
            _nVertices = _dimsize[n];
            cout << " _nVertices " << _nVertices << endl;
	}
	else if (0 == strcmp(recname, "nEdges")) {
            _nEdges = _dimsize[n];
            cout << " _nEdges " << _nEdges << endl;
	}
	else if (0 == strcmp(recname, "maxEdges")) {
            _maxEdges = _dimsize[n];
            cout << " _maxEdges " << _maxEdges << endl;
	}
	else if (0 == strcmp(recname, "maxEdges2")) {
            _maxEdges2 = _dimsize[n];
            cout << " _maxEdges2 " << _maxEdges2 << endl;
	}
	else if (0 == strcmp(recname, "TWO")) {
            _TWO = _dimsize[n];
            cout << " _TWO " << _TWO << endl;
	}
	else if (0 == strcmp(recname, "vertexDegree")) {
            _vertexDegree = _dimsize[n];
            cout << " _vertexDegree " << _vertexDegree << endl;
	}
	else if (0 == strcmp(recname, "Strlen")) {
            _Strlen = _dimsize[n];
            cout << " _Strlen " << _Strlen << endl;
	}
	else if (0 == strcmp(recname, "Time")) {
            _nTime = _dimsize[n];
            cout << " _nTime " << _nTime << endl;
	}
	else if (0 == strcmp(recname, "R3")) {
            _R3 = _dimsize[n];
            cout << " _R3 " << _R3 << endl;
	}
	else if (0 == strcmp(recname, "nMonth")) {
            _nMonth = _dimsize[n];
            cout << " _nMonth " << _nMonth << endl;
	}
	else if (0 == strcmp(recname, "FIFTEEN")) {
            _FIFTEEN = _dimsize[n];
            cout << " _FIFTEEN " << _FIFTEEN << endl;
	}
	else if (0 == strcmp(recname, "TWENTYONE")) {
            _TWENTYONE = _dimsize[n];
            cout << " _TWENTYONE " << _TWENTYONE << endl;
	}

        cout << "  - " << dim_names[n] << ": " << dim_length[n] << endl;
    }
    cout << "Leave functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
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
        cout << "  - " << var_name << " Type: " << var_type << ", ndims: " << var_ndims << endl;
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
 
