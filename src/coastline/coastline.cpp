#include <QtOpenGL>
#include <string.h>
#include "coastline.h"

CoastLine::CoastLine()
{
    oneover = 1.0 / 180.0;
    deg2rad = 3.1415926535897932 * oneover;

    maxPlotLevel = 3;
    plotLevel = 3;

    minPlotPoints = 100;

    height = 0.0;
    radius = 1.01;

    _setup();

  //print();
}

CoastLine::~CoastLine()
{
    int n;

    for(n = 0; n < maxPlotLevel; ++n)
    {
        if(NULL != geometry_array[n])
            free(geometry_array[n]);
        if(NULL != segments_array[n])
            free(segments_array[n]);
        if(NULL != level[n])
            free(level[n]);
        if(NULL != lon[n])
            free(lon[n]);
        if(NULL != lat[n])
            free(lat[n]);
    }

    free(geometry_array);
    free(segments_array);
    free(level);
    free(lon);
    free(lat);
} 

void CoastLine::_setup()
{
    int n;
    char shpflnm[1024];

  //cout << "\tEnter functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;

  //NclAddFileFormats();
  //initializeNcl();

  //cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;

  //nclvar = NULL;

  //flnm[0] = "$NV_DATA/coastline/WDBII_shp/l/WDBII_border_l_L1.shp";
  //flnm[1] = "$NV_DATA/coastline/WDBII_shp/l/WDBII_border_l_L2.shp";
  //flnm[2] = "$NV_DATA/coastline/WDBII_shp/l/WDBII_border_l_L3.shp";

  //flnm[0] = "$NV_DATA/coastline/WDBII_shp/c/WDBII_border_c_L1.shp";
  //flnm[1] = "$NV_DATA/coastline/WDBII_shp/c/WDBII_border_c_L2.shp";
  //flnm[2] = "$NV_DATA/coastline/WDBII_shp/c/WDBII_border_c_L3.shp";

  //flnm[0] = "$NV_DATA/coastline/GSHHS_shp/l/GSHHS_l_L1.shp";
  //flnm[1] = "$NV_DATA/coastline/GSHHS_shp/l/GSHHS_l_L2.shp";
  //flnm[2] = "$NV_DATA/coastline/GSHHS_shp/l/GSHHS_l_L3.shp";

    flnm[0] = "$NV_DATA/coastline/GSHHS_shp/l/GSHHS_l_L1.shp";
    flnm[1] = "$NV_DATA/coastline/WDBII_shp/l/WDBII_border_l_L1.shp";
    flnm[2] = "$NV_DATA/coastline/WDBII_shp/l/WDBII_border_l_L2.shp";

  //cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
  //cout << "\nflnm[0]: " << flnm[0] << endl;
  //cout << "\nflnm[1]: " << flnm[1] << endl;
  //cout << "\nflnm[2]: " << flnm[2] << endl;

  //guiSetAdvancedFileStructure("shp");

    id = (int**) calloc(maxPlotLevel, sizeof(int*));

    geometry_array = (int**) calloc(maxPlotLevel, sizeof(int*));
    segments_array = (int**) calloc(maxPlotLevel, sizeof(int*));
    level          = (int**) calloc(maxPlotLevel, sizeof(int*));

    lon = (double**) calloc(maxPlotLevel, sizeof(double*));
    lat = (double**) calloc(maxPlotLevel, sizeof(double*));
  //cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
  //cout << "\tmaxPlotLevel = " << maxPlotLevel << endl;

    for(n = 0; n < maxPlotLevel; ++n)
    {
        geometry_array[n] = NULL;
        segments_array[n] = NULL;
        level[n] = NULL;

        lon[n] = NULL;
        lat[n] = NULL;

      //cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
      //cout << "\tfile name " << n << ": <" << flnm[n] << ">" << endl;

     // strcpy(shpflnm, flnm[n].c_str());
     // nclfile = NclCreateFile(shpflnm);

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

      //delete nclfile;
    }

  //cout << "\tLeave functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
}

void CoastLine::_check_atts(int i)
{
    int n;
    int* iptr;

    char** attnames = NULL;
    char*  cptr;
 
    natts[i] = 0;

    /*

    NclMultiDValData attMV = NULL;
    attnames = guiGetNclFileAttNames(nclfile, &natts[i]);

  //cout << "\nfile: <" << __FILE__ << ">, function: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << endl;
  //cout << "\tnatts[" << i << "] = " << natts[i] << "." << endl;

    for(n = 0; n < natts[i]; ++n)
    {
      //cout << "\tAtt " << n << ": <" << attnames[n] << ">" << endl;
        attMV = guiGetFileAtt(nclfile, attnames[n]);

        if(NCL_string == attMV->multidval.data_type)
        {
            if(0 == strcmp("layer_name", attnames[n]))
            {
                cptr = guiQuarkToString(*(NclQuark*)attMV->multidval.val);
              //cout << "\t\tAtt " << n << ": <" << attnames[n] << "> = <" << cptr << ">" << endl;

                layer_name[i] = cptr;
            }
            else if(0 == strcmp("geometry_type", attnames[n]))
            {
                cptr = guiQuarkToString(*(NclQuark*)attMV->multidval.val);
              //cout << "\t\tAtt " << n << ": <" << attnames[n] << "> = <" << cptr << ">" << endl;

                geometry_type[i] = cptr;
            }
        }
        else if(NCL_int == attMV->multidval.data_type)
        {
            if(0 == strcmp("geom_segIndex", attnames[n]))
            {
                iptr = (int*)attMV->multidval.val;
              //cout << "\t\tAtt " << n << ": <" << attnames[n] << "> = <" << *iptr << ">" << endl;

                geom_segIndex[i] = *iptr;
            }
            else if(0 == strcmp("geom_numSegs", attnames[n]))
            {
                iptr = (int*)attMV->multidval.val;
              //cout << "\t\tAtt " << n << ": <" << attnames[n] << "> = <" << *iptr << ">" << endl;

                geom_numSegs[i] = *iptr;
            }
            else if(0 == strcmp("segs_xyzIndex", attnames[n]))
            {
                iptr = (int*)attMV->multidval.val;
              //cout << "\t\tAtt " << n << ": <" << attnames[n] << "> = <" << *iptr << ">" << endl;

                segs_xyzIndex[i] = *iptr;
            }
            else if(0 == strcmp("segs_numPnts", attnames[n]))
            {
                iptr = (int*)attMV->multidval.val;
              //cout << "\t\tAtt " << n << ": <" << attnames[n] << "> = <" << *iptr << ">" << endl;

                segs_numPnts[i] = *iptr;
            }
        }

      //guiDestroyObj((NclObj) attMV);
    }
    */
}

void CoastLine::_check_dims(int i)
{
    int n;
    char* cptr;
 
    ndims[i] = 0;
  //cout << "\nfile: <" << __FILE__ << ">, function: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << endl;

    /*
    if(nclfile->file.advanced_file_structure)
    {
      //Advanced file strucuture
        NclAdvancedFile theadvancedfile = NULL;
        NclFileGrpNode* grpnode = NULL;

        theadvancedfile = (NclAdvancedFile) nclfile;
        grpnode = theadvancedfile->advancedfile.grpnode;
        if(NULL != grpnode->dim_rec)
            ndims[i] = grpnode->dim_rec->n_dims;
    }
    else
    {
        ndims[i] = nclfile->file.n_file_dims;
    }

  //cout << "\tndims[" << i << "] = " << ndims[i] << "." << endl;
  
    if(nclfile->file.advanced_file_structure)
    {
      //Advanced file strucuture
      //cout << "\nfile: <" << __FILE__ << ">, function: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << endl;
      //for(n = 0; n < ndims[i]; ++n)
      //{
      //    cout << "\tDim " << n << ": <" << _dimnames[n] << ">, size: " << _dimsizes[n] << endl;
      //}
    }
    else
    {
      //cout << "\nfile: <" << __FILE__ << ">, function: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << endl;
        for(n = 0; n < ndims[i]; ++n)
        {
            cptr = guiQuarkToString(nclfile->file.file_dim_info[n]->dim_name_quark);
            if(0 == strcmp("geometry", cptr))
                geometry[i] = (int) nclfile->file.file_dim_info[n]->dim_size;
            else if(0 == strcmp("segments", cptr))
                segments[i] = (int) nclfile->file.file_dim_info[n]->dim_size;
            else if(0 == strcmp("num_features", cptr))
                num_features[i] = (int) nclfile->file.file_dim_info[n]->dim_size;
            else if(0 == strcmp("num_segments", cptr))
                num_segments[i] = (int) nclfile->file.file_dim_info[n]->dim_size;
            else if(0 == strcmp("num_points", cptr))
                num_points[i] = (int) nclfile->file.file_dim_info[n]->dim_size;

          //cout << "\tDim " << n << ": <" << cptr
          //     << ">, size: " << nclfile->file.file_dim_info[n]->dim_size << endl;
        }
    }
    */
}

void CoastLine::_check_vars(int i)
{
    int n;
    char* cptr;

  //cout << "\nfile: <" << __FILE__ << ">, function: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << endl;

    nvars[i] = 0;
    /*
    if(nclfile->file.advanced_file_structure)
    {
      //Advanced file strucuture
        NclAdvancedFile theadvancedfile = NULL;
        NclFileGrpNode* grpnode = NULL;
      //NclFileVarNode* varnode = NULL;
      //NclFileDimNode* dimnode = NULL;
 
        theadvancedfile = (NclAdvancedFile) nclfile;
        grpnode = theadvancedfile->advancedfile.grpnode;
        if(NULL != grpnode->var_rec)
            nvars[i] = grpnode->var_rec->n_vars;
    }
    else
    {
        nvars[i] = nclfile->file.n_vars;
    }
    */

  //cout << "\tnvars[" << i << "] = " << nvars[i] << "." << endl;

    if(0 == nvars[i])
        return;

/*
    if(nclfile->file.advanced_file_structure)
    {
      //Advanced file strucuture
    }
    else
    {
      //cout << "\nfile: <" << __FILE__ << ">, function: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << endl;
        for(n = 0; n < nvars[i]; ++n)
        {
          //cptr = guiQuarkToString(nclfile->file.var_info[n]->var_name_quark);
          //cout << "\tVar " << n << ": <" << cptr << ">" << endl;

            if(0 == strcmp("x", cptr))
            {
                lon[i] = get_dv(cptr);
            }
            else if(0 == strcmp("y", cptr))
            {
                lat[i] = get_dv(cptr);
            }
            else if(0 == strcmp("geometry", cptr))
            {
                geometry_array[i] = get_iv(cptr);
            }
            else if(0 == strcmp("segments", cptr))
            {
                segments_array[i] = get_iv(cptr);
            }
            else if(0 == strcmp("level", cptr))
            {
                level[i] = get_iv(cptr);
            }
#if 0
            else if(0 == strcmp("id", cptr))
            {
                int*  iptr;
                iptr = get_iv(cptr);

                cout << "\nfile: <" << __FILE__ << ">, function: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << endl;
                cout << "\t\tiptr = " << iptr[0] << endl;
            }
#endif
        }
    }
*/
}

void CoastLine::print()
{
    int i, n;
    int startSegment, numSegments;
    int seg, startPT, endPT;

    cout << "\nfile: <" << __FILE__ << ">, function: <" << __PRETTY_FUNCTION__
         << ">, line: " << __LINE__ << endl;
    for(n = 0; n < maxPlotLevel; ++n)
    {
        cout << "\nFile " << n << ": <" << flnm[n] << ">" << endl;
        cout << "\tlayer_name: <" << layer_name[n] << ">" << endl;
        cout << "\tgeometry_type: <" << geometry_type[n] << ">" << endl;
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

float* CoastLine::get_value(char* vn)
{
    float* value;

  //cout << "\nfile: <" << __FILE__ << ">, function: <" << __PRETTY_FUNCTION__
  //     << ">, line: " << __LINE__ << endl;
  //cout << "\tvar name: <" << vn << ">" << endl;

  //nclvar = readNclFileVar(nclfile, vn, NULL);

  //value = guiGetValue(nclvar);

    return value;
}

int* CoastLine::get_iv(char* vn)
{
    int* value;

  //cout << "\nfile: <" << __FILE__ << ">, function: <" << __PRETTY_FUNCTION__
  //     << ">, line: " << __LINE__ << endl;
  //cout << "\tvar name: <" << vn << ">" << endl;

  //nclvar = readNclFileVar(nclfile, vn, NULL);

  //value = guiGetIntArray(nclvar);

    return value;
}

double* CoastLine::get_dv(char* vn)
{
    double* value;

  //cout << "\nfile: <" << __FILE__ << ">, function: <" << __PRETTY_FUNCTION__
  //     << ">, line: " << __LINE__ << endl;
  //cout << "\tvar name: <" << vn << ">" << endl;

  //nclvar = readNclFileVar(nclfile, vn, NULL);

  //value = guiGetDoubleArray(nclvar);

    return value;
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

void CoastLine::draw(int n)
{
    set_plot_level(n);

    draw();
}

void CoastLine::draw()
{
    int i, k, n;
    int startSegment, numSegments;
    int seg, startPT, endPT;

    double x, y, z;

    GLfloat line_width = 1.0;

  //cout << "\nfile: <" << __FILE__ << ">, function: <" << __PRETTY_FUNCTION__
  //     << ">, line: " << __LINE__ << endl;

    glPushMatrix();

  //glColor3f(1.0, 1.0, 1.0);
    glColor3f(0.0, 0.0, 0.0);

    glLineWidth(line_width);

    for(n = 0; n < plotLevel; ++n)
    {
      //cout << "\nFile " << n << ": <" << flnm[n] << ">" << endl;
      //cout << "\tlayer_name: <" << layer_name[n] << ">" << endl;
      //cout << "\tgeometry_type: <" << geometry_type[n] << ">" << endl;
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
                        _lonlat2xyz(lon[n][k], lat[n][k], x, y, z);
                        glVertex3f(x, y, z);
                    }
                glEnd();
            }
        }
    }

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

    for(n = 0; n < plotLevel; ++n)
    {
      //cout << "\nFile " << n << ": <" << flnm[n] << ">" << endl;
      //cout << "\tlayer_name: <" << layer_name[n] << ">" << endl;
      //cout << "\tgeometry_type: <" << geometry_type[n] << ">" << endl;
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
                _lonlat2xy2(lon[n][k], lat[n][k], xb, yb);

                for(k = startPT + 1; k < endPT; ++k)
                {
                    _lonlat2xy2(lon[n][k], lat[n][k], x, y);

                    if(((0.9 < xb) && (-0.9 > x)) ||
                       ((0.9 < x) && (-0.9 > xb)))
                    {
                        xb = x;
                        yb = y;
                    }
                    else
                    {
                        glBegin(GL_LINES);
                            glVertex3f(xb, yb, z);
                            glVertex3f(x , y , z);
                        glEnd();

                        xb = x;
                        yb = y;
                    }
                }
            }
        }
    }

    glPopMatrix();
}

