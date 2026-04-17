#include "coastline.h"

// using namespace std;
// using namespace netCDF;

CoastLine::CoastLine()
{
    oneover = 1.0 / 180.0;
    deg2rad = 3.1415926535897932 * oneover;

    height = 0.0;
    radius = 1.01;

    _setup();

  //print();
}

CoastLine::~CoastLine()
{
} 

size_t CoastLine::_getDim(const char* dn)
{
    size_t varSize = 0;
    NcDim varDim = ncfl->getDim(dn);

    if (varDim.isNull()) {
        cerr << "Dimension 'lat' not found!" << endl;
    } else {
        varSize = varDim.getSize();
        cout << "Dimension Name: " << varDim.getName() << endl;
        cout << "Dimension Size: " << varSize << endl;
    }
    return varSize;
} 

void CoastLine::_setup()
{
    int n;
    char ncflnm[1024];

    cout << "\tEnter functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;

    const char* path = getenv("STARVIEWERHOME");
    if (path == nullptr) {
        cout << "ERROR: STARVIEWERHOME not set!" << endl;
        throw(errno);
    }
    strcpy(ncflnm, path);
  //strcat(ncflnm, "/data/gshhg-gmt-2.3.7/binned_GSHHS_f.nc");
  //strcat(ncflnm, "/data/gshhg-gmt-2.3.7/binned_GSHHS_h.nc");
  //strcat(ncflnm, "/data/gshhg-gmt-2.3.7/binned_GSHHS_i.nc");
    strcat(ncflnm, "/data/gshhg-gmt-2.3.7/binned_GSHHS_l.nc");
  //strcat(ncflnm, "/data/gshhg-gmt-2.3.7/binned_GSHHS_c.nc");

    cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    cout << "\tncflnm: " << ncflnm << endl;

    try {
        // Use the constructor to re-initialize the ncfl object
        ncfl = new NcFile(ncflnm, NcFile::read);

        cout << "Successfully opened: " << ncflnm << endl;
    } catch (netCDF::exceptions::NcException& e) {
        cerr << "Error opening file: " << e.what() << endl;
    }

    // Get dimension size:
    n_of_scalar = _getDim("Dimension_of_scalar");
    n_of_polygon_array = _getDim("Dimension_of_polygon_array");
    n_of_node_arrays = _getDim("Dimension_of_node_arrays");
    n_of_bin_arrays = _getDim("Dimension_of_bin_arrays");
    n_of_segment_arrays = _getDim("Dimension_of_segment_arrays");
    n_of_point_arrays = _getDim("Dimension_of_point_arrays");

    // Load necessary variables
    NcVar segStartVar = ncfl->getVar("Id_of_first_point_in_a_segment");
    size_t nSegs = segStartVar.getDim(0).getSize();

    NcVar nBinSizeVar = ncfl->getVar("Bin_size_in_minutes");
    NcVar nLonBinsVar = ncfl->getVar("N_bins_in_360_longitude_range");
    NcVar nLatBinsVar = ncfl->getVar("N_bins_in_180_degree_latitude_range");
    NcVar nBinInFileVar = ncfl->getVar("N_bins_in_file");
    NcVar nSegInFileVar = ncfl->getVar("N_segments_in_file");
    NcVar nPntInFileVar = ncfl->getVar("N_points_in_file");

    nBinSizeVar.getVar(&Bin_size_in_minutes);
    nLonBinsVar.getVar(&N_bins_in_360_longitude_range);
    nLatBinsVar.getVar(&N_bins_in_180_degree_latitude_range);
    nBinInFileVar.getVar(&N_bins_in_file);
    nSegInFileVar.getVar(&N_segments_in_file);
    nPntInFileVar.getVar(&N_points_in_file);

    cout << "\tBin_size_in_minutes = " << Bin_size_in_minutes << endl;
    cout << "\tN_bins_in_360_longitude_range = " << N_bins_in_360_longitude_range << endl;
    cout << "\tN_bins_in_180_degree_latitude_range = " << N_bins_in_180_degree_latitude_range << endl;
    cout << "\tN_bins_in_file = " << N_bins_in_file << endl;
    cout << "\tN_segments_in_file = " << N_segments_in_file << endl;
    cout << "\tN_points_in_file = " << N_points_in_file << endl;

    // Read relative points
    NcVar relLonVar = ncfl->getVar("Relative_longitude_from_SW_corner_of_bin");
    NcVar relLatVar = ncfl->getVar("Relative_latitude_from_SW_corner_of_bin");
    size_t nPoints = relLonVar.getDim(0).getSize();
    relLons.resize(nPoints);
    relLats.resize(nPoints);
    relLonVar.getVar(relLons.data());
    relLatVar.getVar(relLats.data());

    // Read bin and seg info
    NcVar fsibVar = ncfl->getVar("Id_of_first_segment_in_a_bin");
    Id_of_first_segment_in_a_bin.resize(n_of_bin_arrays);
    fsibVar.getVar(Id_of_first_segment_in_a_bin.data());

    NcVar sibVar = ncfl->getVar("N_segments_in_a_bin");
    N_segments_in_a_bin.resize(n_of_bin_arrays);
    sibVar.getVar(N_segments_in_a_bin.data());

    NcVar fpisVar = ncfl->getVar("Id_of_first_point_in_a_segment");
    Id_of_first_point_in_a_segment.resize(n_of_segment_arrays);
    fpisVar.getVar(Id_of_first_point_in_a_segment.data());

    cout << "\tLeave functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
}

void CoastLine::print()
{
    int i, n;
}

void CoastLine::_lonlat2xy(double lon, double lat, double &x, double &y)
{
    x = lon * oneover;
    y = lat * oneover;
}

void CoastLine::_lonlat2xy2(double lon, double lat, double &x, double &y)
{
    if(lon <= 0.0)
        x = (lon + 360.0) * oneover - 1.0;
    else
        x = lon * oneover - 1.0;

    y = lat * oneover;
}

void CoastLine::_lonlat2xyz(double lon, double lat, double &x, double &y, double &z)
{
    double phi = lat * deg2rad;
    double dist = radius * cos(phi);
    double lamda = lon * deg2rad;

    x = dist * sin(lamda);
    z = dist * cos(lamda);
    y = radius * sin(phi);
}

void CoastLine::set_plot_level(int n)
{
    if(n <= maxPlotLevel)
        plotLevel = n;
    else
        plotLevel = maxPlotLevel;
}

void CoastLine::draw(double r, int n)
{
    set_plot_level(n);
    radius = r;
    draw();
}

void CoastLine::_drawit()
{
    int i, j, k, n, ns, nb, ifseg;
    int startPT, endPT;

    double lon, lat;
    double x, y, z;
    double binSizeDeg = Bin_size_in_minutes/60.0;

    ns = 0;
    for(j = 0; j < N_bins_in_180_degree_latitude_range; ++j)
    {
        for(i = 0; i < N_bins_in_360_longitude_range; ++i)
        {
            nb = j*N_bins_in_360_longitude_range + i;

          //cout << "\t\ti " << i << ", j = " << j << ", nseg = " << N_segments_in_a_bin[nb] << endl;

            for(n = 0; n < N_segments_in_a_bin[nb]; ++n)
            {
                startPT = Id_of_first_point_in_a_segment[ns];
                if(ns == (n_of_segment_arrays-1))
                    endPT = n_of_point_arrays;
		else
                    endPT = startPT + Id_of_first_point_in_a_segment[ns+1];

              //cout << "\t\t\tsegments " << ns+n << ": startPT = " << startPT
              //     << ", endPT = " << endPT << endl;

                glBegin(GL_LINE_STRIP);
                for(k = startPT; k < endPT; ++k)
                {
		    lon = i*binSizeDeg + (relLons[k] * scale);
		    lat = j*binSizeDeg + (relLats[k] * scale) - 90.0;
                    _lonlat2xyz(lon, lat, x, y, z);
                    glVertex3f(x, y, z);
                  //cout << "\t\t\tk = " << k << ", lon = " << lon << ", lat = " << lat << endl;
                }
                glEnd();
            }
	    ns += N_segments_in_a_bin[nb];
        }
    }
}

void CoastLine::draw(int n)
{
    set_plot_level(n);

    draw();
}

void CoastLine::draw()
{
    int i, j, k, n, ns, nb, ifseg;
    int startPT, endPT;

    double lon, lat;
    double x, y, z;
    double binSizeDeg = Bin_size_in_minutes/60.0;

    GLfloat line_width = 1.0;

  //cout << "\nfile: <" << __FILE__ << ">, function: <" << __PRETTY_FUNCTION__
  //     << ">, line: " << __LINE__ << endl;

    glPushMatrix();

  //glColor3f(1.0, 1.0, 1.0);
    glColor3f(0.0, 0.0, 0.0);

    glLineWidth(line_width);

    _drawit();

    glPopMatrix();
}

void CoastLine::drawONplane(double hgt, int n)
{
    set_plot_level(n);
    height = hgt;
    drawONplane();
}

void CoastLine::drawONplane(int n)
{
    set_plot_level(n);

    drawONplane();
}

void CoastLine::drawONplane()
{
    int i, k, n;
    int startSegment, numSegments;
    int seg, startPT, endPT;

    double x, y;
    double z = height + 0.001;

    GLfloat line_width = 1.5;

  //cout << "\nfile: <" << __FILE__ << ">, function: <" << __PRETTY_FUNCTION__
  //     << ">, line: " << __LINE__ << endl;

    glPushMatrix();

  //glColor3f(1.0, 1.0, 1.0);
    glColor3f(0.0, 0.0, 0.0);

    glLineWidth(line_width);

    _drawit();

    glPopMatrix();
}

void CoastLine::drawONplane2(double hgt, int m)
{
    int i, k, n;
    int startSegment, numSegments;
    int seg, startPT, endPT;

    double x, y, xb, yb;
    double z = hgt + 0.001;

    set_plot_level(m);

    GLfloat line_width = 1.5;

  //cout << "\nfile: <" << __FILE__ << ">, function: <" << __PRETTY_FUNCTION__
  //     << ">, line: " << __LINE__ << endl;

    glPushMatrix();

  //glColor3f(1.0, 1.0, 1.0);
    glColor3f(0.0, 0.0, 0.0);

    glLineWidth(line_width);

    _drawit();

    glPopMatrix();
}

