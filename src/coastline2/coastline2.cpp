#include <QtOpenGL>
#include "coastline2.h"

CoastLine2::CoastLine2()
{
  //cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__
  //     << ", file: <" << __FILE__ << ">" << endl;

    oneover = 1.0 / 180.0;
    deg2rad = 3.1415926535897932 * oneover;

    kind = 3;
    height = 0.0;
    radius = 1.01;

    grid_lon_space = 15;
    grid_lat_space = 15;

    _setup();

  //print();
}

CoastLine2::CoastLine2(string idstr)
{
  //cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__
  //     << ", file: <" << __FILE__ << ">" << endl;
  //cout << "\tidstr: <" << idstr << ">" << endl;

    oneover = 1.0 / 180.0;
    deg2rad = 3.1415926535897932 * oneover;

    kind = 3;
    height = 0.0;
    radius = 1.01;

    grid_lon_space = 5;
    grid_lat_space = 5;

    _setup(idstr);

  //print();
}

CoastLine2::~CoastLine2()
{
} 

void CoastLine2::_setup()
{
    int n;

  //cout << "\nIn functions: <" << __PRETTY_FUNCTION__
  //     << ">, line: " << __LINE__
  //     << ", file: <" << __FILE__ << ">" << endl;

    cntlon =    0.0;
    minlon = -180.0;
    maxlon =  180.0;
    minlat = -90.0;
    maxlat =  90.0;

    xwest = -1.0;
    xeast =  1.0;
    xcenter = 0.0;

    ysouth = -0.5;
    ynorth =  0.5;
    ycenter = 0.0;

  //_NclAddFileFormats();
    initializeNcl();

    guiSetAdvancedFileStructure("shp");

  //flnm[0] = "/Users/starviewer/work/nv/data/coastline/GSHHS_shp/l/GSHHS_l_L1.shp";
    flnm[0] = "$NV_DATA/coastline/GSHHS_shp/l/GSHHS_l_L1.shp";
    flnm[1] = "$NV_DATA/coastline/WDBII_shp/l/WDBII_border_l_L1.shp";
  //flnm[1] = "$NV_DATA/coastline/WDBII_shp/c/WDBII_border_c_L2.shp";
    flnm[2] = "$NV_DATA/coastline/WDBII_shp/l/WDBII_river_l_L01.shp";

    level = (int **) calloc(kind, sizeof(int *));
    seg_sizes = (int **) calloc(kind, sizeof(int *));
    area  = (double **) calloc(kind, sizeof(double *));
    segments = (double ***) calloc(kind, sizeof(double **));

    for(n = 0; n < kind; ++n)
    {
        nclfile = NclCreateAdvancedFile(flnm[n].c_str());

      //cout << "\tfile name " << n << ": <" << flnm[n] << ">" << endl;

        natts[n] = 0;
        ndims[n] = 0;
        nvars[n] = 0;

        num_features[n] = 0;
        num_segments[n] = 0;
        num_points[n] = 0;

        _check_atts(n);
        _check_dims(n);
        _check_vars(n);

        delete nclfile;
    }
}

void CoastLine2::_setup(string idstr)
{
    int n;

  //cout << "\nIn functions: <" << __PRETTY_FUNCTION__
  //     << ">, line: " << __LINE__
  //     << ", file: <" << __FILE__ << ">" << endl;

    kind = 2;
    cntlon =    0.0;
    minlon = -180.0;
    maxlon =  180.0;
    minlat = -90.0;
    maxlat =  90.0;

    xwest = -1.0;
    xeast =  1.0;
    xcenter = 0.0;

    ysouth = -0.5;
    ynorth =  0.5;
    ycenter = 0.0;

  //_NclAddFileFormats();
    initializeNcl();

    guiSetAdvancedFileStructure("shp");

    if(0 == idstr.compare("KTLX"))
    {
        flnm[0] = "$NV_DATA/states/ok_eco_l3.shp";
      //flnm[0] = "$NV_DATA/states/ok_eco_l4.shp";
        flnm[1] = "$NV_DATA/states/ok_county.shp";
    }
    else
    {
      //flnm[0] = "$NV_DATA/coastline/GSHHS_shp/l/GSHHS_l_L1.shp";
      //flnm[0] = "$NV_DATA/states/countyp010.shp";
        flnm[0] = "$NV_DATA/states/state_bounds.shp";
    }

    level = (int **) calloc(kind, sizeof(int *));
    seg_sizes = (int **) calloc(kind, sizeof(int *));
    area  = (double **) calloc(kind, sizeof(double *));
    segments = (double ***) calloc(kind, sizeof(double **));

    for(n = 0; n < kind; ++n)
    {
        nclfile = NclCreateAdvancedFile(flnm[n].c_str());

      //cout << "\tfile name " << n << ": <" << flnm[n] << ">" << endl;

        natts[n] = 0;
        ndims[n] = 0;
        nvars[n] = 0;

        num_features[n] = 0;
        num_segments[n] = 0;
        num_points[n] = 0;

        _check_atts(n);
        _check_dims(n);
        _check_vars(n);

        delete nclfile;
    }
}

void CoastLine2::_check_atts(int i)
{
    int n;
    int* iptr;
    char* cptr;
    char attname[1024];

    NclAdvancedFile advfile = (NclAdvancedFile) nclfile;
    NclFileAttRecord* attrec = advfile->advancedfile.grpnode->att_rec;
    NclFileAttNode* attnode;
 
    if(NULL == attrec)
    {
        natts[i] = 0;
        return;
    }

    natts[i] = attrec->n_atts;

  //cout << "\nfile: <" << __FILE__ << ">" << endl;
  //cout << "\tline: " << __LINE__ << endl;
  //cout << "\tfunction: <" << __PRETTY_FUNCTION__ << ">" << endl; 
  //cout << "\tnatts[" << i << "] = " << natts[i] << "." << endl;

    for(n = 0; n < natts[i]; ++n)
    {
        attnode = &attrec->att_node[n];
        strcpy(attname, guiQuarkToString(attnode->name));

      //cout << "\t\tAtt " << n << ": name <" << attname << ">" << endl;

        if(NCL_string == attnode->type)
        {
            cptr = guiQuarkToString(*(NclQuark*)attnode->value);

          //cout << "\t\tAtt " << n << " value: <" << cptr << ">" << endl;

            if(0 == strcmp("layer_name", attname))
                layer_name[i] = cptr;
            else if(0 == strcmp("geometry_type", attname))
                geometry_type[i] = cptr;
        }
        else if(NCL_int == attnode->type)
        {
            iptr = (int*)attnode->value;

          //cout << "\t\tAtt " << n << " value: " << *iptr << endl;

            if(0 == strcmp("numSegs", attname))
                num_segments[i] = *iptr;
            else if(0 == strcmp("numPnts", attname))
                num_points[i] = *iptr;
            else if(0 == strcmp("numGeom", attname))
                num_features[i] = *iptr;
        }
    }
}

void CoastLine2::_check_dims(int i)
{
  //int n;

    NclAdvancedFile advfile = NULL;
    NclFileGrpNode* grpnode = NULL;
  //NclFileDimNode* dimnode = NULL;
 
  //cout << "\nfile: <" << __FILE__ << ">" << endl;
  //cout << "\tline: " << __LINE__ << endl;
  //cout << "\tfunction: <" << __PRETTY_FUNCTION__ << ">" << endl;
  //cout << "\tnclfile->file.advanced_file_structure: ";
  //cout << nclfile->file.advanced_file_structure << endl;

    ndims[i] = 0;

    if(!nclfile->file.advanced_file_structure)
        return;

    advfile = (NclAdvancedFile) nclfile;
    grpnode = advfile->advancedfile.grpnode;
    if(NULL == grpnode->dim_rec)
        return;

    ndims[i] = grpnode->dim_rec->n_dims;

  //cout << "\tndims[" << i << "] = " << ndims[i] << "." << endl;
  
  //Advanced file strucuture
  //cout << "\nfile: <" << __FILE__ << ">" << endl;
  //cout << "\tline: " << __LINE__ << endl;
  //cout << "\tfunction: <" << __PRETTY_FUNCTION__ << ">" << endl;

  //for(n = 0; n < ndims[i]; ++n)
  //{
  //    dimnode = &(grpnode->dim_rec->dim_node[n]);
  //  //_dimnames[n] = guiQuarkToString(dimnode->name);
  //  //_dimsizes[n] = dimnode->size;
  //  //cout << "\tDim " << n << ": <" << _dimnames[n] << ">, size: " << _dimsizes[n] << endl;
  //  //cout << "\tDim " << n << ": <" << guiQuarkToString(dimnode->name) << ">";
  //  //cout << " size: " << dimnode->size << endl;
  //}
}

void CoastLine2::_check_vars(int i)
{
    int n;
    char* cptr;

    NclAdvancedFile advfile = NULL;
    NclFileGrpNode* grpnode = NULL;
    NclFileVarNode* varnode = NULL;
 
  //cout << "\nfile: <" << __FILE__ << ">" << endl;
  //cout << "\tline: " << __LINE__ << endl;
  //cout << "\tfunction: <" << __PRETTY_FUNCTION__ << ">" << endl << endl;

    nvars[i] = 0;
    if(!nclfile->file.advanced_file_structure)
        return;

    advfile = (NclAdvancedFile) nclfile;
    grpnode = advfile->advancedfile.grpnode;
    if(NULL == grpnode->var_rec)
        return;

    nvars[i] = grpnode->var_rec->n_vars;

  //cout << "\tnvars[" << i << "] = " << nvars[i] << "." << endl;

    if(0 == nvars[i])
        return;

  //cout << "\nfile: <" << __FILE__ << ">" << endl;
  //cout << "\tline: " << __LINE__ << endl;
  //cout << "\tfunction: <" << __PRETTY_FUNCTION__ << ">" << endl << endl;

    for(n = 0; n < nvars[i]; ++n)
    {
        varnode = &grpnode->var_rec->var_node[n];
        cptr = guiQuarkToString(varnode->name);

      //cout << "\tVar " << n << ": <" << cptr << ">" << endl;

        if(0 == strcmp("level", cptr))
            level[i] = get_iv(cptr);
        else if(0 == strcmp("area", cptr))
            area[i] = get_dv(cptr);
        else if(0 == strcmp("segments", cptr))
            segments[i] = get_lv(cptr, &seg_sizes[i]);
    }

  //cout << "\nfile: <" << __FILE__ << ">" << endl;
  //cout << "\tline: " << __LINE__ << endl;
  //cout << "\tfunction: <" << __PRETTY_FUNCTION__ << ">" << endl << endl;
  //cout << "\tsegments[" << i << "][0][0]=" << segments[i][0][0] << endl;
}

void CoastLine2::print()
{
    int n;

  //cout << "\nfile: <" << __FILE__ << ">" << endl;
  //cout << "\tline: " << __LINE__ << endl;
  //cout << "\tfunction: <" << __PRETTY_FUNCTION__ << ">" << endl << endl;

    for(n = 0; n < kind; ++n)
    {
        cout << "\nFile " << n << ": <" << flnm[n] << ">" << endl;
        cout << "\tlayer_name: <" << layer_name[n] << ">" << endl;
        cout << "\tgeometry_type: <" << geometry_type[n] << ">" << endl;
        cout << "\tnum_features: <" << num_features[n] << ">" << endl;
        cout << "\tnum_segments: <" << num_segments[n] << ">" << endl;
        cout << "\tnum_points: <" << num_points[n] << ">" << endl;
    }
}

float* CoastLine2::get_fv(char* vn)
{
    float* value;

  //cout << "\nfile: <" << __FILE__ << ">" << endl;
  //cout << "\tline: " << __LINE__ << endl;
  //cout << "\tfunction: <" << __PRETTY_FUNCTION__ << ">" << endl;
  //cout << "\tvar name: <" << vn << ">" << endl;

    nclvar = readNclFileVar(nclfile, vn, NULL);

    value = guiGetValue(nclvar);

    return value;
}

int* CoastLine2::get_iv(char* vn)
{
    int* value;

    nclvar = readNclFileVar(nclfile, vn, NULL);

    value = guiGetIntArray(nclvar);

  //cout << "\nfile: <" << __FILE__ << ">" << endl;
  //cout << "\tline: " << __LINE__ << endl;
  //cout << "\tfunction: <" << __PRETTY_FUNCTION__ << ">" << endl;
  //cout << "\tvar name: <" << vn << ">" << endl;
  //cout << "\tvar value[0]: <" << value[0] << ">" << endl;

    return value;
}

double* CoastLine2::get_dv(char* vn)
{
    double* value;

    nclvar = readNclFileVar(nclfile, vn, NULL);

    value = guiGetDoubleArray(nclvar);

    guiDestroyObj((NclObj) nclvar);

  //cout << "\nfile: <" << __FILE__ << ">" << endl;
  //cout << "\tline: " << __LINE__ << endl;
  //cout << "\tfunction: <" << __PRETTY_FUNCTION__ << ">" << endl;
  //cout << "\tvar name: <" << vn << ">" << endl;
  //cout << "\tvar value[0]: <" << value[0] << ">" << endl;

    return value;
}

double** CoastLine2::get_lv(char* vn, int** seg_sizes)
{
    double** value;

    nclvar = readNclFileVar(nclfile, vn, NULL);

    value = guiGetListArray(nclvar, seg_sizes);

    guiDestroyObj((NclObj) nclvar);

  //cout << "\nfile: <" << __FILE__ << ">" << endl;
  //cout << "\tline: " << __LINE__ << endl;
  //cout << "\tfunction: <" << __PRETTY_FUNCTION__ << ">" << endl;
  //cout << "\tvar name: <" << vn << ">" << endl;
  //cout << "\tvar seg_sizes[0]: <" << seg_sizes[0] << ">" << endl;

    return value;
}

void CoastLine2::_lonlat2xy(double lon, double lat, double &x, double &y)
{
    x = (lon - cntlon) * oneover;

    if(-1.0 > x)
        x += 2.0;

    if(1.0 < x)
        x -= 2.0;

    y = lat * oneover;
}

void CoastLine2::_lonlat2xyz(double lon, double lat, double &x, double &y, double &z)
{
    double phi = lat * deg2rad;
    double dist = radius * cos(phi);
    double lamda = lon * deg2rad;

    x = dist * sin(lamda);
    z = dist * cos(lamda);
    y = radius * sin(phi);
}

void CoastLine2::draw(double r)
{
    double ord = radius;
    radius = r;
    draw();
    radius = ord;
}

void CoastLine2::draw()
{
    int i, k, n;

    double* lon;
    double* lat;
    double x, y, z;

    GLfloat line_width = 1.5;

  //cout << "\nfile: <" << __FILE__ << ">, function: <" << __PRETTY_FUNCTION__
  //     << ">, line: " << __LINE__ << endl;
  //cout << "\tkind = " << kind  << endl;

    glPushMatrix();

    if(nvoptions->get_cb(NV_BGBLACK))
        glColor3f(1.0, 1.0, 1.0);
    else
        glColor3f(0.0, 0.0, 0.0);

    glLineWidth(line_width);

    for(n = 0; n < kind; ++n)
    {
      //cout << "\nFile " << n << ": <" << flnm[n] << ">" << endl;
      //cout << "\tlayer_name: <" << layer_name[n] << ">" << endl;
      //cout << "\tgeometry_type: <" << geometry_type[n] << ">" << endl;
      //cout << "\tnum_features: <" << num_features[n] << ">" << endl;
      //cout << "\tnum_segments: <" << num_segments[n] << ">" << endl;
      //cout << "\tnum_points: <" << num_points[n] << ">" << endl;

        for(i = 0; i < num_features[n]; ++i)
        {
          //cout << "\t\tseg_sizes[" << n << "][" << i << "] = " << seg_sizes[n][i] << endl;

            if(5 > seg_sizes[n][i])
                continue;

            lon = &segments[n][i][0];
            lat = &segments[n][i][seg_sizes[n][i]];

            glBegin(GL_LINE_STRIP);
            for(k = 0; k < seg_sizes[n][i]; ++k)
            {
                _lonlat2xyz(lon[k], lat[k], x, y, z);
                glVertex3f(x, y, z);
            }
            glEnd();
        }
    }

    glPopMatrix();
}

void CoastLine2::drawONplane(double hgt)
{
    double oh = height;
    height = hgt;
    drawONplane();
    height = oh;
}

void CoastLine2::drawONplane()
{
    int i, k, n;

    double* lon;
    double* lat;
    double x, y, xb, yb;
    double z = height + 0.001;

    GLfloat line_width = 1.5;

  //cout << "\nfile: <" << __FILE__ << ">" << endl;
  //cout << "\tline: " << __LINE__ << endl;
  //cout << "\tfunction: <" << __PRETTY_FUNCTION__ << ">" << endl << endl;

    glPushMatrix();

    if(nvoptions->get_cb(NV_BGBLACK))
        glColor3f(1.0, 1.0, 1.0);
    else
        glColor3f(0.0, 0.0, 0.0);

    glLineWidth(line_width);

    drawFrame(-1.0, 1.0, -0.5, 0.5, z);

    for(n = 0; n < kind; ++n)
    {
      //cout << "\nFile " << n << ": <" << flnm[n] << ">" << endl;
      //cout << "\tlayer_name: <" << layer_name[n] << ">" << endl;
      //cout << "\tgeometry_type: <" << geometry_type[n] << ">" << endl;
      //cout << "\tnum_features: <" << num_features[n] << ">" << endl;
      //cout << "\tnum_segments: <" << num_segments[n] << ">" << endl;
      //cout << "\tnum_points: <" << num_points[n] << ">" << endl;

        for(i = 0; i < num_features[n]; ++i)
        {
          //cout << "\t\tseg_sizes[" << n << "][" << i << "] = " << seg_sizes[n][i] << endl;

            if(5 > seg_sizes[n][i])
                continue;

            lon = &segments[n][i][0];
            lat = &segments[n][i][seg_sizes[n][i]];

            k = 0;
            _lonlat2xy(lon[k], lat[k], xb, yb);

            for(k = 1; k < seg_sizes[n][i]; ++k)
            {
                _lonlat2xy(lon[k], lat[k], x, y);

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

    glPopMatrix();
}

void CoastLine2::draw_plane_grid(double z)
{
    int i, j;
    double x, y;

    glColor4f(0.90, 0.90, 0.10, 0.50);

    for(j = 0; j <= 18; ++j)
    {
       y = (double)j/18.0 - 0.5;
       glBegin(GL_LINE_STRIP);
       for(i = 0; i <= 36; ++i)
       {
           x = (double)i/18.0 - 1.0;
           glVertex3d(x, y, z);
       }
       glEnd();
    }

    for(i = 0; i <= 36; ++i)
    {
       x = (double)i/18.0 - 1.0;
       glBegin(GL_LINE_STRIP);
       for(j = 0; j <= 18; ++j)
       {
           y = (double)j/18.0 - 0.5;
           glVertex3d(x, y, z);
       }
       glEnd();
    }
}

void CoastLine2::draw_grid(double r)
{
    int i, j;

    double x, y, z;
    double lon, lat;
    double ord = radius;

    GLfloat line_width = 2.0;

  //cout << "\nfile: <" << __FILE__ << ">" << endl;
  //cout << "\tline: " << __LINE__ << endl;
  //cout << "\tfunction: <" << __PRETTY_FUNCTION__ << ">" << endl << endl;

    radius = r;

    glPushMatrix();

    glColor4f(0.90, 0.90, 0.10, 0.50);

    glLineWidth(line_width);

    for(j = -75; j < 90; j += 15)
    {
        lat = (double) j;

        glBegin(GL_LINE_STRIP);
        for(i = 0; i <= 360; i += 15)
        {
            lon = (double) i;
            _lonlat2xyz(lon, lat, x, y, z);
            glVertex3f(x, y, z);
        }
        glEnd();
    }

    for(i = 0; i <= 360; i += 90)
    {
        lon = (double) i;

        glBegin(GL_LINE_STRIP);
        for(j = -90; j <= -75; j += 1)
        {
            lat = (double) j;
            _lonlat2xyz(lon, lat, x, y, z);
            glVertex3f(x, y, z);
        }
        glEnd();

        glBegin(GL_LINE_STRIP);
        for(j = 75; j <= 90; j += 1)
        {
            lat = (double) j;
            _lonlat2xyz(lon, lat, x, y, z);
            glVertex3f(x, y, z);
        }
        glEnd();
    }

    for(i = 0; i <= 360; i += 45)
    {
        lon = (double) i;

        glBegin(GL_LINE_STRIP);
        for(j = -75; j <= -60; j += 1)
        {
            lat = (double) j;
            _lonlat2xyz(lon, lat, x, y, z);
            glVertex3f(x, y, z);
        }
        glEnd();

        glBegin(GL_LINE_STRIP);
        for(j = 60; j <= 75; j += 1)
        {
            lat = (double) j;
            _lonlat2xyz(lon, lat, x, y, z);
            glVertex3f(x, y, z);
        }
        glEnd();
    }

    for(i = 0; i <= 360; i += 15)
    {
        lon = (double) i;

        glBegin(GL_LINE_STRIP);
        for(j = -60; j <= 60; j += 1)
        {
            lat = (double) j;
            _lonlat2xyz(lon, lat, x, y, z);
            glVertex3f(x, y, z);
        }
        glEnd();
    }

    glPopMatrix();

    radius = ord;
}

void CoastLine2::draw_ball(double r)
{
    int i, j;

    double x, y, z;
    double lon, lat1, lat2;
    double ord = radius;

    radius = r;

    glPushMatrix();

    glColor4f(0.10, 0.90, 0.90, 0.50);

    for(j = -90; j < 90; j += 5)
    {
        lat1 = (double) j;
        lat2 = lat1 + 5.0;

        glBegin(GL_QUAD_STRIP);
        for(i = 0; i <= 360; i += 5)
        {
            lon = (double) i;
            _lonlat2xyz(lon, lat1, x, y, z);
            glVertex3f(x, y, z);
            _lonlat2xyz(lon, lat2, x, y, z);
            glVertex3f(x, y, z);
        }
        glEnd();
    }

    glPopMatrix();

    radius = ord;
}

void CoastLine2::_setupRegionInfo()
{
    double v;

    minlon = nvoptions->get_minlon();
    maxlon = nvoptions->get_maxlon();

    if(minlon > 180.0)
        minlon -= 360.0;
    if(maxlon > 180.0)
        maxlon -= 360.0;

    if((minlon > 0.0) && (maxlon < 0.0))
        cntlon = 180.0;
    else
    {
        cntlon = 0.0;

        if(minlon > maxlon)
        {
           v = minlon;
           minlon = maxlon;
           maxlon = v;
        }
    }

    minlat = nvoptions->get_minlat() - 90.0;
    maxlat = nvoptions->get_maxlat() - 90.0;

    if(minlat > maxlat)
    {
       v = minlat;
       minlat = maxlat;
       maxlat = v;
    }

    _lonlat2xy(minlon, minlat, xwest, ysouth);
    _lonlat2xy(maxlon, maxlat, xeast, ynorth);

    xcenter = 0.5 * (xwest + xeast);
    ycenter = 0.5 * (ysouth + ynorth);
}

void CoastLine2::drawAregion(double hgt)
{
    _setupRegionInfo();

    glPushMatrix();

    glTranslated(-xcenter, -ycenter, 0.0);

    drawFrame(xwest, xeast, ysouth, ynorth, hgt);

    if((minlon > 0.0) && (maxlon < 0.0))
    {
        _drawAregion(minlon,  180.0, minlat, maxlat, hgt);
        _drawAregion(-180.0, maxlon, minlat, maxlat, hgt);
    }
    else
    {
        _drawAregion(minlon, maxlon, minlat, maxlat, hgt);
    }

    glPopMatrix();
}

void CoastLine2::drawFrame(double xleft, double xrite,
                           double ybottom, double ytop, double z)
{
    glBegin(GL_LINES);
        glVertex3f(xleft, ybottom, z);
        glVertex3f(xrite, ybottom, z);

        glVertex3f(xleft, ytop, z);
        glVertex3f(xrite, ytop, z);

        glVertex3f(xleft, ybottom, z);
        glVertex3f(xleft, ytop, z);

        glVertex3f(xrite, ybottom, z);
        glVertex3f(xrite, ytop, z);
    glEnd();
}

void CoastLine2::drawRegionalGrid(double z)
{
    _setupRegionInfo();

    glPushMatrix();

    glTranslated(-xcenter, -ycenter, 0.0);

    if((minlon > 0.0) && (maxlon < 0.0))
    {
        _drawRegionalGrid(minlon,  180.0, minlat, maxlat, z);
        _drawRegionalGrid(-180.0, maxlon, minlat, maxlat, z);
    }
    else
    {
        _drawRegionalGrid(minlon, maxlon, minlat, maxlat, z);
    }

    glPopMatrix();
}

void CoastLine2::_drawAregion(double slon, double elon,
                              double slat, double elat, double hgt)
{
    int i, k, n;

    double* lon;
    double* lat;

    double x, y, xb, yb;
    double z = hgt + 0.001;

    GLfloat line_width = 1.5;

    bool lastin = false;

    if(nvoptions->get_cb(NV_BGBLACK))
        glColor3f(1.0, 1.0, 1.0);
    else
        glColor3f(0.0, 0.0, 0.0);

    _lonlat2xy(slon, elat, xwest, ysouth);
    _lonlat2xy(slon, elat, xeast, ynorth);

    glLineWidth(line_width);

    for(n = 0; n < kind; ++n)
    {
        for(i = 0; i < num_features[n]; ++i)
        {
          //cout << "\t\tseg_sizes[" << n << "][" << i << "] = " << seg_sizes[n][i] << endl;

            lon = &segments[n][i][0];
            lat = &segments[n][i][seg_sizes[n][i]];

            k = 0;
            _lonlat2xy(lon[k], lat[k], xb, yb);

            if((lon[k] >= slon) && (lon[k] <= elon) &&
               (lat[k] >= slat) && (lat[k] <= elat))
            {
                if(xb < xwest)
                     xb += 2.0;
                lastin = true;
            }
            else
                lastin = false;

            for(k = 1; k < seg_sizes[n][i]; ++k)
            {
                _lonlat2xy(lon[k], lat[k], x, y);

                if((lon[k] >= slon) && (lon[k] <= elon) &&
                   (lat[k] >= slat) && (lat[k] <= elat))
                {
                    if(x < xwest)
                         x += 2.0;

                    if(lastin)
                    {
                        glBegin(GL_LINES);
                            glVertex3f(xb, yb, z);
                            glVertex3f(x , y , z);
                        glEnd();
                    }

                    lastin = true;
                }
                else
                    lastin = false;

                xb = x;
                yb = y;
            }
        }
    }
}

void CoastLine2::_drawRegionalGrid(double slon, double elon,
                                   double slat, double elat, double z)
{
    int i, j;
    double x, y;
    double lon, lat;
    double xb, yb;
    bool lastin = false;

    glColor4f(0.90, 0.90, 0.10, 0.50);

    for(j = -90; j <= 90; j += grid_lat_space)
    {
        lon = -180.0;
        lat = (double)j;

        if((lat < slat) || (lat > elat))
            continue;

        _lonlat2xy(lon, lat, xb, yb);

        if((lon >= slon) && (lon <= elon))
        {
            lastin = true;
        }
        else
            lastin = false;

        for(i = -179; i <= 180; ++i)
        {
            lon = (double)i;
            _lonlat2xy(lon, lat, x, y);

            if((lon >= slon) && (lon <= elon))
            {
                if(lastin)
                {
                    glBegin(GL_LINES);
                        glVertex3f(xb, yb, z);
                        glVertex3f(x , y , z);
                    glEnd();
                }

                lastin = true;
            }
            else
                lastin = false;

            xb = x;
            yb = y;
        }
    }

    for(i = -180; i <= 180; i += grid_lon_space)
    {
        lon = (double)i;

        if((lon < slon) || (lon > elon))
            continue;

        glBegin(GL_LINE_STRIP);
        for(j = -90; j <= 90; ++j)
        {
            lat = (double)j;
 
            if(lat < slat)
                 continue;
    
            if(lat > elat)
                 break;
    
            _lonlat2xy(lon, lat, x, y);

            glVertex3d(x, y, z);
        }
        glEnd();
    }
}

void CoastLine2::_local_ll2xy(double lon, double lat, double &x, double &y)
{
    x = scale * (lon - base_lon);
    y = scale * (lat - base_lat);
}

void CoastLine2::drawLimitedArea(double hgt)
{
    int i, k;
    int n = 0;

    double* lon;
    double* lat;

    double x, y;
    double z = hgt + 0.001;

    GLfloat line_width = 1.0;

    glPushMatrix();

    if(nvoptions->get_cb(NV_BGBLACK))
        glColor3f(1.0, 1.0, 1.0);
    else
        glColor3f(0.0, 0.0, 0.0);

    glLineWidth(line_width);

    for(n = 0; n < kind; ++n)
    { 
        for(i = 0; i < num_features[n]; ++i)
        {
          //cout << "\t\tseg_sizes[" << n << "][" << i << "] = " << seg_sizes[n][i] << endl;

            lon = &segments[n][i][0];
            lat = &segments[n][i][seg_sizes[n][i]];

            glBegin(GL_LINE_STRIP);
            for(k = 0; k < seg_sizes[n][i]; ++k)
            {
                _local_ll2xy(lon[k], lat[k], x, y);
                glVertex3f(x , y , z);
            }
            glEnd();
        }
    }

    glPopMatrix();
}

