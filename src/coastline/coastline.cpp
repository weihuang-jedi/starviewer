#include "coastline.h"

CoastLine::CoastLine()
{
    int n;
    char flnm[1024];

    GDALAllRegister(); // 1. Register Drivers

    height = 0.0;
    radius = 1.01;
    oneover = 1.0 / 180.0;
    deg2rad = 3.1415926535897932 * oneover;

    const char* path = getenv("STARVIEWERHOME");
    if (path == nullptr) {
        cout << "ERROR: STARVIEWERHOME not set!" << endl;
        throw(errno);
    }
    strcpy(flnm, path);
    strcat(flnm, "/data/gshhg-shp-2.3.7/GSHHS_shp/c/GSHHS_c_L1.shp");

    // cout << "\tshapefile flnm: " << flnm << endl;

    // Open Dataset
    poDS = (GDALDataset*) OGROpen(flnm, FALSE, NULL);
    if (poDS == NULL) {
        cerr << "Opening failed." << endl;
        exit -1;
    }
}

CoastLine::CoastLine(string clfn)
{
    int n;

    GDALAllRegister(); // 1. Register Drivers

    height = 0.0;
    radius = 1.01;
    oneover = 1.0 / 180.0;
    deg2rad = 3.1415926535897932 * oneover;

    flnm = clfn;

    // Open Dataset
    poDS = (GDALDataset*) OGROpen(flnm.c_str(), FALSE, NULL);
    if (poDS == NULL) {
        cerr << "Opening failed." << endl;
        exit -1;
    }
}

CoastLine::~CoastLine()
{
    // Close Dataset
    GDALClose(poDS);
} 

/*
void CoastLine::print()
{
    int i, n;
}
*/

void CoastLine::_lonlat2xyz(double lon, double lat, double radius)
{
    double x, y, z;
    double phi = lat * deg2rad;
    double dist = radius * cos(phi);
    double lamda = lon * deg2rad;

    x = dist * sin(lamda);
    z = dist * cos(lamda);
    y = radius * sin(phi);
    glVertex3f(x, y, z);
}

void CoastLine::set_plot_level(int n)
{
    if(n <= maxPlotLevel)
        plotLevel = n;
    else
        plotLevel = maxPlotLevel;
}

void CoastLine::drawOnSphere(double r)
{
    GLfloat line_width = 1.0;
  //glColor3f(1.0, 1.0, 1.0);
    glColor3f(0.0, 0.0, 0.0);

    _drawOnSphere(r);
}


void CoastLine::drawOnSphere(double r, int n)
{
    set_plot_level(n);

    GLfloat line_width = 1.0;
  //glColor3f(1.0, 1.0, 1.0);
    glColor3f(0.0, 0.0, 0.0);
    _drawOnSphere(r);
}

void CoastLine::_drawOnSphere(double radius)
{
    // Access Layer
    OGRLayer *poLayer = poDS->GetLayer(0);
    poLayer->ResetReading();

    n = 0;
    OGRFeature *poFeature;
    while ((poFeature = poLayer->GetNextFeature()) != NULL) {
        OGRGeometry *poGeometry = poFeature->GetGeometryRef();
        if (poGeometry != NULL && wkbFlatten(poGeometry->getGeometryType()) == wkbPolygon) {
            // Extract Geometry (Polygons)
            OGRPolygon *poPolygon = (OGRPolygon*) poGeometry;
            OGRLinearRing *poRing = poPolygon->getExteriorRing();

            if(poRing->getNumPoints() < 5)
                continue;

	    ++n;
            // Loop through points and send to plotting function
            glBegin(GL_LINE_STRIP);
            for (i = 0; i < poRing->getNumPoints(); i++) {
                lon = poRing->getX(i);
                lat = poRing->getY(i);
                _lonlat2xyz(lon, lat, radius);
            }
            glEnd();
        }
        OGRFeature::DestroyFeature(poFeature);
    }
}

void CoastLine::drawOnPlane(double hgt, int n)
{
    set_plot_level(n);
    GLfloat line_width = 1.0;
  //glColor3f(1.0, 1.0, 1.0);
    glColor3f(0.0, 0.0, 0.0);

    _drawOnPlane(hgt);
}

void CoastLine::drawOnPlane(double hgt)
{
    GLfloat line_width = 1.0;
  //glColor3f(1.0, 1.0, 1.0);
    glColor3f(0.0, 0.0, 0.0);


    _drawOnPlane(hgt);
}

void CoastLine::_lonlat2xy(double lon, double lat, double z)
{
    double x, y;
    x = lon * oneover;
    if(x > 1.0)
       x -= 2.0;
    y = lat * oneover;
    glVertex3f(x, y, z);
}

void CoastLine::_drawOnPlane(double z)
{
    // Access Layer
    OGRLayer *poLayer = poDS->GetLayer(0);
    poLayer->ResetReading();

    n = 0;
    OGRFeature *poFeature;
    while ((poFeature = poLayer->GetNextFeature()) != NULL) {
        OGRGeometry *poGeometry = poFeature->GetGeometryRef();
        if (poGeometry != NULL && wkbFlatten(poGeometry->getGeometryType()) == wkbPolygon) {
            // Extract Geometry (Polygons)
            OGRPolygon *poPolygon = (OGRPolygon*) poGeometry;
            OGRLinearRing *poRing = poPolygon->getExteriorRing();

            if(poRing->getNumPoints() < 5)
                continue;

            ++n;
            // Loop through points and send to plotting function
            glBegin(GL_LINE_STRIP);
            for (i = 0; i < poRing->getNumPoints(); i++) {
                lon = poRing->getX(i);
                lat = poRing->getY(i);
                _lonlat2xy(lon, lat, z);
            }
            glEnd();
        }
        OGRFeature::DestroyFeature(poFeature);
    }
}
