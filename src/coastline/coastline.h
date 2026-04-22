#ifndef _CoastLine_H
#define _CoastLine_H

#include "gdal/ogrsf_frmts.h" // For OGR vector classes

#include <QtOpenGL>
#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <netcdf>

#include <string.h>
#include <stdlib.h>
#include <assert.h>

using namespace std;

class CoastLine
{
    public:
        CoastLine();
       ~CoastLine();

        // void print();

        void drawOnSphere(double r);
        void drawOnSphere(double r, int n);

        void drawOnPlane(double h);
        void drawOnPlane(double h, int n);

        void set_plot_level(int n);
        void set_min_plot_points(int n) { minPlotPoints = n; };

    protected:
        GDALDataset *poDS;
        // OGRLayer *poLayer;
        // OGRFeature *poFeature;
        // OGRGeometry *poGeometry;
	// OGRPolygon *poPolygon;
	// OGRLinearRing *poRing;

        int plotLevel;
        int maxPlotLevel = 6;
        int minPlotPoints;

        string flnm;

        double deg2rad;
        double oneover;

	double height, radius;
	double x, y, lon, lat;

	int i, n;

	void _lonlat2xyz(double lon, double lat, double radius);
	void _lonlat2xy(double lon, double lat, double height);

	void _drawOnPlane(double height);
        void _drawOnSphere(double radius);

};
#endif

/*
int main() {
    GDALAllRegister(); // 1. Register Drivers

    // 2. Open Dataset
    GDALDataset *poDS = (GDALDataset*) OGROpen("GSHHS_f_L1.shp", FALSE, NULL);
    if (poDS == NULL) {
        std::cerr << "Opening failed." << std::endl;
        return 1;
    }

    // 3. Access Layer
    OGRLayer *poLayer = poDS->GetLayer(0);
    poLayer->ResetReading();

    // 4. Iterate Features
    OGRFeature *poFeature;
    while ((poFeature = poLayer->GetNextFeature()) != NULL) {
        OGRGeometry *poGeometry = poFeature->GetGeometryRef();
        if (poGeometry != NULL && wkbFlatten(poGeometry->getGeometryType()) == wkbPolygon) {
            // 5. Extract Geometry (Polygons)
            OGRPolygon *poPolygon = (OGRPolygon*) poGeometry;
            OGRLinearRing *poRing = poPolygon->getExteriorRing();
            
            // Loop through points and send to plotting function
            for (int i = 0; i < poRing->getNumPoints(); i++) {
                double x = poRing->getX(i);
                double y = poRing->getY(i);
                // Plotting Logic: plotPoint(x, y);
            }
        }
        OGRFeature::DestroyFeature(poFeature);
    }

    // 6. Close Dataset
    GDALClose(poDS);
    return 0;
}
*/
