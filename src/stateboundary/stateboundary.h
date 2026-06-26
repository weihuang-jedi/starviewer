#ifndef _StateBoundary_H
#define _StateBoundary_H

#include <QtOpenGL>

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>

#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include <ogrsf_frmts.h>
#include "mapprojection.h"

using namespace std;

class StateBoundary
{
    public:
        StateBoundary();
       ~StateBoundary();

        void print();
        void process();

        void draw();

        void drawONplane();

	void set_mapprojection(MapProjection* mp) { _mapprojection = mp; };
	MapProjection* get_mapprojection() { return _mapprojection; };

    protected:
	MapProjection* _mapprojection;
        int minPlotPoints;

        double deg2rad;
        double oneover;

        double height;
        double radius;

        void _setup();

        void _lonlat2xy(double lon, double lat, double &x, double &y);
        void _lonlat2xy2(double lon, double lat, double &x, double &y);
        void _lonlat2xyz(double lon, double lat, double &x, double &y, double &z);

	vector<string> state_abbr;
	unordered_map<string, string> state_abbr2name;
	unordered_map<string, vector<vector<OGRPoint>>> state_polygon;

        GDALDataset* poDS;
        OGRLayer* poLayer;
        OGRFeature* poFeature;

	void printPolygonCoordinates(OGRGeometry* poGeometry);
	void printMultiPolygonCoordinates(OGRGeometry* poGeometry);

	vector<vector<OGRPoint>> processPolygonCoordinates(OGRGeometry* poGeometry);
	vector<vector<OGRPoint>> processMultiPolygonCoordinates(OGRGeometry* poGeometry);
};
#endif

