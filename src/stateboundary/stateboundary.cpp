#include "stateboundary.h"

StateBoundary::StateBoundary()
{
    oneover = 1.0 / 180.0;
    deg2rad = 3.1415926535897932 * oneover;

    maxPlotLevel = 1;
    plotLevel = 1;

    minPlotPoints = 100;

    height = 0.0;
    radius = 1.01;

    _setup();

    print();
}

StateBoundary::~StateBoundary()
{
    int n;

    for(n = 0; n < maxPlotLevel; ++n)
    {
        if(NULL != geometry_array[n])
            free(geometry_array[n]);
        if(NULL != segments_array[n])
            free(segments_array[n]);
        if(NULL != lon[n])
            free(lon[n]);
        if(NULL != lat[n])
            free(lat[n]);
    }

    free(geometry_array);
    free(segments_array);
    free(lon);
    free(lat);
} 

void StateBoundary::_setup()
{
    cout << "\tEnter functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    int n;
    char shpflnm[1024];
    const char* path = getenv("STARVIEWERHOME");
    if (path == nullptr) {
        cout << "ERROR: STARVIEWERHOME not set!" << endl;
        throw(errno);
    }
    strcpy(shpflnm, path);
    strcat(shpflnm, "/data/stateboundary/s_16ap26.shp");

    cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    cout << "\nshpflnm: " << shpflnm << endl;

    id = (int**) calloc(maxPlotLevel, sizeof(int*));

    geometry_array = (int**) calloc(maxPlotLevel, sizeof(int*));
    segments_array = (int**) calloc(maxPlotLevel, sizeof(int*));

    lon = (double**) calloc(maxPlotLevel, sizeof(double*));
    lat = (double**) calloc(maxPlotLevel, sizeof(double*));
    cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    cout << "\tmaxPlotLevel = " << maxPlotLevel << endl;

    for(n = 0; n < maxPlotLevel; ++n)
    {
        cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
        cout << "\tn = " << n << endl;

        geometry_array[n] = NULL;
        segments_array[n] = NULL;

        lon[n] = NULL;
        lat[n] = NULL;

        natts[n] = 0;
        ndims[n] = 0;
        nvars[n] = 0;

        geometry[n] = 0;
        segments[n] = 0;

        num_features[n] = 0;
        num_segments[n] = 0;
        num_points[n] = 0;

        _check_atts(n);
        _check_dims(n);
        _check_vars(n);
    }

    cout << "\tLeave functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
}

void StateBoundary::_check_atts(int i)
{
    int n;
    int* iptr;

    char** attnames = NULL;
    char*  cptr;

    natts[i] = 0;
}

void StateBoundary::_check_dims(int i)
{
    int n;
    char* cptr;
 
    ndims[i] = 0;
  //cout << "\nfile: <" << __FILE__ << ">, function: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << endl;
}

void StateBoundary::_check_vars(int i)
{
    int n;
    char* cptr;

  //cout << "\nfile: <" << __FILE__ << ">, function: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << endl;

    nvars[i] = 0;
}

void StateBoundary::print()
{
    int i, n;
    int startSegment, numSegments;
    int seg, startPT, endPT;

    cout << "\nfile: <" << __FILE__ << ">, function: <" << __PRETTY_FUNCTION__
         << ">, line: " << __LINE__ << endl;
    for(n = 0; n < maxPlotLevel; ++n)
    {
        cout << "\nFile " << n << ": <" << flnm[n] << ">" << endl;
        cout << "\tgeometry: <" << geometry[n] << ">" << endl;
        cout << "\tsegments: <" << segments[n] << ">" << endl;
        cout << "\tnum_features: <" << num_features[n] << ">" << endl;
        cout << "\tnum_segments: <" << num_segments[n] << ">" << endl;
        cout << "\tnum_points: <" << num_points[n] << ">" << endl;

        for(i = 0; i < num_features[n]; ++i)
        {
            startSegment = geometry_array[n][i*geometry[n] + geom_segIndex[n]];
            numSegments  = geometry_array[n][i*geometry[n] + geom_numSegs[n]];
            cout << "\t\tgeometry " << i << ": startSegment = " << startSegment << ", numSegments = " << numSegments << endl;
 
            for(seg = startSegment; seg < startSegment+numSegments; ++seg)
            {
                startPT = segments_array[n][seg*segments[n] + segs_xyzIndex[n]];
                endPT   = startPT + segments_array[n][seg*segments[n] + segs_numPnts[n]];
                cout << "\t\t\tsegments " << seg << ": startPT = " << startPT << ", endPT = " << endPT << endl;
                cout << "\t\t\tlon(startPT) = " << lon[n][startPT] << ", lat(startPT) = " << lat[n][startPT] << endl;
            }
        }
    }
}

float* StateBoundary::get_value(char* vn)
{
    float* value;

  //cout << "\nfile: <" << __FILE__ << ">, function: <" << __PRETTY_FUNCTION__
  //     << ">, line: " << __LINE__ << endl;
  //cout << "\tvar name: <" << vn << ">" << endl;

    return value;
}

int* StateBoundary::get_iv(char* vn)
{
    int* value;

  //cout << "\nfile: <" << __FILE__ << ">, function: <" << __PRETTY_FUNCTION__
  //     << ">, line: " << __LINE__ << endl;
  //cout << "\tvar name: <" << vn << ">" << endl;

    return value;
}

double* StateBoundary::get_dv(char* vn)
{
    double* value;

  //cout << "\nfile: <" << __FILE__ << ">, function: <" << __PRETTY_FUNCTION__
  //     << ">, line: " << __LINE__ << endl;
  //cout << "\tvar name: <" << vn << ">" << endl;

    return value;
}

void StateBoundary::_lonlat2xy(double lon, double lat, double &x, double &y)
{
    x = lon * oneover;
    y = lat * oneover;
}

void StateBoundary::_lonlat2xy2(double lon, double lat, double &x, double &y)
{
    if(lon <= 0.0)
        x = (lon + 360.0) * oneover - 1.0;
    else
        x = lon * oneover - 1.0;

    y = lat * oneover;
}

void StateBoundary::_lonlat2xyz(double lon, double lat, double &x, double &y, double &z)
{
    double phi = lat * deg2rad;
    double dist = radius * cos(phi);
    double lamda = lon * deg2rad;

    x = dist * sin(lamda);
    z = dist * cos(lamda);
    y = radius * sin(phi);
}

void StateBoundary::set_plot_level(int n)
{
    if(n <= maxPlotLevel)
        plotLevel = n;
    else
        plotLevel = maxPlotLevel;
}

void StateBoundary::draw(double r, int n)
{
    set_plot_level(n);
    radius = r;
    draw();
}

void StateBoundary::draw(int n)
{
    set_plot_level(n);

    draw();
}

void StateBoundary::draw()
{
    int i, k, n;
    int startSegment, numSegments;
    int seg, startPT, endPT;

    double x, y, z;

    GLfloat line_width = 1.0;

    cout << "\nfile: <" << __FILE__ << ">, function: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << endl;

    glPushMatrix();

  //glColor3f(1.0, 1.0, 1.0);
    glColor3f(0.0, 0.0, 0.0);

    glLineWidth(line_width);

    for(n = 0; n < plotLevel; ++n)
    {
        cout << "\nFile " << n << ": <" << flnm[n] << ">" << endl;
        cout << "\tlayer_name: <" << layer_name[n] << ">" << endl;
        cout << "\tgeometry: <" << geometry[n] << ">" << endl;
        cout << "\tsegments: <" << segments[n] << ">" << endl;
        cout << "\tnum_features: <" << num_features[n] << ">" << endl;
        cout << "\tnum_segments: <" << num_segments[n] << ">" << endl;
        cout << "\tnum_points: <" << num_points[n] << ">" << endl;

        for(i = 0; i < num_features[n]; ++i)
        {
            startSegment = geometry_array[n][i*geometry[n] + geom_segIndex[n]];
            numSegments  = geometry_array[n][i*geometry[n] + geom_numSegs[n]];

          //cout << "\t\tgeometry " << i << ": startSegment = " << startSegment
          //     << ", numSegments = " << numSegments << endl;

            for(seg = startSegment; seg < startSegment+numSegments; ++seg)
            {
                startPT = segments_array[n][seg*segments[n] + segs_xyzIndex[n]];
                k       = segments_array[n][seg*segments[n] + segs_numPnts[n]];
                endPT   = startPT + k;

              //cout << "\t\t\tsegments " << seg << ": startPT = " << startPT
              //     << ", endPT = " << endPT << endl;
              //cout << "\t\t\tlon(startPT) = " << lon[n][startPT]
              //     << ", lat(startPT) = " << lat[n][startPT] << endl;

		if(minPlotPoints > k)
                    continue;

                glBegin(GL_LINE_STRIP);
                    for(k = startPT; k < endPT; ++k)
                    {
                        _lonlat2xyz(lon[n][k], lat[n][k], x, y, z);
                        glVertex3f(x, y, z);
                    }
                glEnd();
            }
        }
    }

    glPopMatrix();
}

void StateBoundary::drawONplane(double hgt, int n)
{
    set_plot_level(n);
    height = hgt;
    drawONplane();
}

void StateBoundary::drawONplane(int n)
{
    set_plot_level(n);

    drawONplane();
}

void StateBoundary::drawONplane()
{
    int i, k, n;
    int startSegment, numSegments;
    int seg, startPT, endPT;

    double x, y;
    double z = height + 0.001;

    GLfloat line_width = 1.5;

    cout << "\nfile: <" << __FILE__ << ">, function: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << endl;

    glPushMatrix();

  //glColor3f(1.0, 1.0, 1.0);
    glColor3f(0.0, 0.0, 0.0);

    glLineWidth(line_width);

    for(n = 0; n < plotLevel; ++n)
    {
      //cout << "\nFile " << n << ": <" << flnm[n] << ">" << endl;
      //cout << "\tgeometry: <" << geometry[n] << ">" << endl;
      //cout << "\tsegments: <" << segments[n] << ">" << endl;
      //cout << "\tnum_features: <" << num_features[n] << ">" << endl;
      //cout << "\tnum_segments: <" << num_segments[n] << ">" << endl;
      //cout << "\tnum_points: <" << num_points[n] << ">" << endl;

        for(i = 0; i < num_features[n]; ++i)
        {
            startSegment = geometry_array[n][i*geometry[n] + geom_segIndex[n]];
            numSegments  = geometry_array[n][i*geometry[n] + geom_numSegs[n]];

          //cout << "\t\tgeometry " << i << ": startSegment = " << startSegment
          //     << ", numSegments = " << numSegments << endl;

            for(seg = startSegment; seg < startSegment+numSegments; ++seg)
            {
                startPT = segments_array[n][seg*segments[n] + segs_xyzIndex[n]];
                k       = segments_array[n][seg*segments[n] + segs_numPnts[n]];
                endPT   = startPT + k;

              //cout << "\t\t\tsegments " << seg << ": startPT = " << startPT
              //     << ", endPT = " << endPT << endl;
              //cout << "\t\t\tlon(startPT) = " << lon[n][startPT]
              //     << ", lat(startPT) = " << lat[n][startPT] << endl;

		if(minPlotPoints > k)
                    continue;

                glBegin(GL_LINE_STRIP);
                    for(k = startPT; k < endPT; ++k)
                    {
                        _lonlat2xy(lon[n][k], lat[n][k], x, y);
                        glVertex3f(x, y, z);
                    }
                glEnd();
            }
        }
    }

    glPopMatrix();
}

void StateBoundary::drawONplane2(double hgt, int m)
{
    int i, k, n;
    int startSegment, numSegments;
    int seg, startPT, endPT;

    double x, y, xb, yb;
  //double z = hgt + 0.001;
    double z = hgt + 0.01;

    set_plot_level(m);

    GLfloat line_width = 1.5;

  //cout << "\nfile: <" << __FILE__ << ">, function: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << endl;

    glPushMatrix();

  //glColor3f(1.0, 1.0, 1.0);
    glColor3f(0.0, 0.0, 0.0);

    glLineWidth(line_width);

    for(n = 0; n < plotLevel; ++n)
    {
        for(i = 0; i < num_features[n]; ++i)
        {
            startSegment = geometry_array[n][i*geometry[n] + geom_segIndex[n]];
            numSegments  = geometry_array[n][i*geometry[n] + geom_numSegs[n]];

            for(seg = startSegment; seg < startSegment+numSegments; ++seg)
            {
                startPT = segments_array[n][seg*segments[n] + segs_xyzIndex[n]];
                k       = segments_array[n][seg*segments[n] + segs_numPnts[n]];
                endPT   = startPT + k;

		if(minPlotPoints > k)
                    continue;

                k = startPT;

              //cout << "\tlon = " << lon[n][k] << ", lat = " << lat[n][k] << ", xb = " << xb << ", yb = " << yb << endl;
              //_lonlat2xy2(lon[n][k], lat[n][k], xb, yb);

                for(k = startPT + 1; k < endPT; ++k)
                {
                    if( (x  >= 0.0) && (x  <= 1.0) &&
                        (xb >= 0.0) && (xb <= 1.0) &&
                        (y  >= 0.0) && (y  <= 1.0) &&
                        (yb >= 0.0) && (yb <= 1.0))
                    {
                        glBegin(GL_LINES);
                            glVertex3f(xb, yb, z);
                            glVertex3f(x , y , z);
                        glEnd();
                    }

                    xb = x;
                    yb = y;
                }
            }
        }
    }

    glPopMatrix();
}

