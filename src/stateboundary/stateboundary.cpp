#include "stateboundary.h"

StateBoundary::StateBoundary()
{
    oneover = 1.0 / 180.0;
    deg2rad = 3.1415926535897932 * oneover;

    minPlotPoints = 100;

    height = 0.0;
    radius = 1.01;

    state_abbr2name = {{"CO", "Colorado"},
                       {"CA", "California"},
                       {"AZ", "Arizona"},
                       {"AR", "Arkansas"},
                       {"ID", "Idaho"},
                       {"IA", "Iowa"},
                       {"GA", "Georgia"},
                       {"DE", "Delaware"},
                       {"DC", "District of Columbia"},
                       {"CT", "Connecticut"},
                       {"IL", "Illinois"},
                       {"IN", "Indiana"},
                       {"KS", "Kansas"},
                       {"KY", "Kentucky"},
                       {"MA", "Massachusetts"},
                       {"ME", "Maine"},
                       {"MI", "Michigan"},
                       {"MN", "Minnesota"},
                       {"MS", "Mississippi"},
                       {"MT", "Montana"},
                       {"NC", "North Carolina"},
                       {"ND", "North Dakota"},
                       {"NE", "Nebraska"},
                       {"NH", "New Hampshire"},
                       {"NJ", "New Jersey"},
                       {"NM", "New Mexico"},
                       {"NV", "Nevada"},
                       {"NY", "New York"},
                       {"OH", "Ohio"},
                       {"OK", "Oklahoma"},
                       {"OR", "Oregon"},
                       {"TN", "Tennessee"},
                       {"SD", "South Dakota"},
                       {"RI", "Rhode Island"},
                       {"PA", "Pennsylvania"},
                       {"TX", "Texas"},
                       {"VT", "Vermont"},
                       {"VA", "Virginia"},
                       {"UT", "Utah"},
                       {"SC", "South Carolina"},
                       {"MD", "Maryland"},
                       {"MO", "Missouri"},
                       {"LA", "Louisiana"},
                       {"AL", "Alabama"},
                       {"FL", "Florida"},
                       {"WA", "Washington"},
                       {"WY", "Wyoming"},
                       {"WV", "West Virginia"},
                       {"WI", "Wisconsin"}};

    // These two states has issues to plot.

#if 1
    for (const auto& pair : state_abbr2name) {
        // cout << pair.first << ": " << pair.second << "\n";
	state_abbr.push_back(pair.first);
    }
#else
    // C++17 and newer
    for (const auto& [key, value] : state_abbr2name) {
        cout << key << ": " << value << "\n";
	state_abbr.push_back(key);
    }
#endif
    _setup();

    // print();
    process();
}

StateBoundary::~StateBoundary()
{
    int n;

    // 6. Cleanup and close dataset
    GDALClose(poDS);
} 

void StateBoundary::_setup()
{
    // cout << "\nEnter functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    int n;
    char shpflnm[1024];
    const char* path = getenv("STARVIEWERHOME");
    if (path == nullptr) {
        cout << "ERROR: STARVIEWERHOME not set!" << endl;
        throw(errno);
    }
    strcpy(shpflnm, path);
    strcat(shpflnm, "/data/stateboundary/s_16ap26.shp");

    // cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    // cout << "\nshpflnm: " << shpflnm << endl;

    // 1. Register all available GIS drivers
    GDALAllRegister();

    poDS = (GDALDataset*) GDALOpenEx(shpflnm, GDAL_OF_VECTOR, NULL, NULL, NULL);
    if (poDS == NULL) {
        cerr << "Error: Open failed for " << shpflnm << endl;
        return;
    }

    // 3. Get the first layer (Shapefiles typically only have 1 layer)
    poLayer = poDS->GetLayer(0);
    if (poLayer == NULL) {
        cerr << "Error: Could not retrieve layer." << endl;
        GDALClose(poDS);
        return;
    }

    // cout << "\tfunctions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    // cout << "Feature count: " << poLayer->GetFeatureCount() << "\n\n";

    // 4. Loop through all features (States/Territories) in the layer
    poLayer->ResetReading();
    // cout << "\tLeave functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
}

void StateBoundary::printPolygonCoordinates(OGRGeometry* poGeometry) {
    // Downcast generic geometry to OGRPolygon
    OGRPolygon* poPolygon = dynamic_cast<OGRPolygon*>(poGeometry);
    if (!poPolygon) return;

    // 1. Get the exterior ring (index 0 is always the outer boundary)
    OGRLinearRing* poExteriorRing = poPolygon->getExteriorRing();
    cout << "  Exterior Ring Points:\n";
    for (int i = 0; i < poExteriorRing->getNumPoints(); i++) {
        OGRPoint point;
        poExteriorRing->getPoint(i, &point);
        cout << "    Pt " << i << ": Long=" << point.getX() << ", Lat=" << point.getY() << "\n";
    }

    // 2. Get interior rings (holes), if any exist
    int numInteriorRings = poPolygon->getNumInteriorRings();
    for (int r = 0; r < numInteriorRings; r++) {
        OGRLinearRing* poInteriorRing = poPolygon->getInteriorRing(r);
        cout << "  Interior Ring " << r << " Points:\n";
        for (int i = 0; i < poInteriorRing->getNumPoints(); i++) {
            // double x, y;
            // poInteriorRing->getPoint(i, &x, &y); // Alternative way to fetch coordinates
	    OGRPoint point;
            poInteriorRing->getPoint(i, &point); // Alternative way to fetch coordinates
            cout << "    Pt " << i << ": Long=" << point.getX() << ", Lat=" << point.getY() << "\n";
        }
    }
}

void StateBoundary::printMultiPolygonCoordinates(OGRGeometry* poGeometry) {
    // Downcast generic geometry to OGRMultiPolygon
    OGRMultiPolygon* poMultiPolygon = dynamic_cast<OGRMultiPolygon*>(poGeometry);
    if (!poMultiPolygon) return;

    int numPolygons = poMultiPolygon->getNumGeometries();
    cout << "MultiPolygon contains " << numPolygons << " sub-polygons.\n";

    // 1. Loop through each individual Polygon inside the MultiPolygon
    for (int p = 0; p < numPolygons; p++) {
        OGRGeometry* poSubGeom = poMultiPolygon->getGeometryRef(p);
        OGRPolygon* poPolygon = dynamic_cast<OGRPolygon*>(poSubGeom);

        if (poPolygon) {
            cout << "  Sub-Polygon " << p << ":\n";

            // 2. Loop through the exterior ring of this sub-polygon
            OGRLinearRing* poExterior = poPolygon->getExteriorRing();
            for (int i = 0; i < poExterior->getNumPoints(); i++) {
                // double x, y;
                // poExterior->getPoint(i, &x, &y);
                OGRPoint point;
                poExterior->getPoint(i, &point);
                cout << "    Poly " << p << ", Pt " << i << ": X=" << point.getX() << ", Y=" << point.getY() << "\n";
            }
        }
    }
}

void StateBoundary::print()
{
    while ((poFeature = poLayer->GetNextFeature()) != NULL) {
        // Fetch attributes. NWS state shapefiles usually have fields like "NAME" or "STATE"
        // Adjust field names based on your specific NWS shapefile schema
        const char* stateName = poFeature->GetFieldAsString("NAME");
        const char* stateAbbr = poFeature->GetFieldAsString("STATE");

        cout << "State: " << stateName << " (" << stateAbbr << ")" << endl;

        // 5. Fetch geometry (e.g., Polygon or MultiPolygon data)
        OGRGeometry* poGeometry = poFeature->GetGeometryRef();
        if (poGeometry != NULL) {
            // Print geometry type (e.g., wkbPolygon, wkbMultiPolygon)
            cout << "  Geometry Type: " << OGRGeometryTypeToName(poGeometry->getGeometryType()) << endl;
            
            // To get raw coordinates, you would cast poGeometry to OGRPolygon/OGRMultiPolygon
            // For brevity, we can check its envelope (bounding box)
            OGREnvelope envelope;
            poGeometry->getEnvelope(&envelope);
            cout << "  Bounding Box: MinX=" << envelope.MinX << ", MaxX=" << envelope.MaxX 
                 << ", MinY=" << envelope.MinY << ", MaxY=" << envelope.MaxY << endl;

	    OGRwkbGeometryType type = wkbFlatten(poGeometry->getGeometryType());

            switch(type) {
                case wkbPolygon:
                    printPolygonCoordinates(poGeometry);
                    break;
            
                case wkbMultiPolygon:
                    printMultiPolygonCoordinates(poGeometry);
                    break;
            
                default:
                    cout << "Unsupported geometry type: " << OGRGeometryTypeToName(type) << endl;
                    break;
            }
        }

        cout << "------------------------------------------" << endl;
        
        // Free feature memory
        OGRFeature::DestroyFeature(poFeature);
    }
}

void StateBoundary::process()
{
    vector<vector<OGRPoint>> vpnt;

    while ((poFeature = poLayer->GetNextFeature()) != NULL) {
        // Fetch attributes. NWS state shapefiles usually have fields like "NAME" or "STATE"
        // Adjust field names based on your specific NWS shapefile schema
        const char* stateName = poFeature->GetFieldAsString("NAME");
        // const char* stateAbbr = poFeature->GetFieldAsString("STATE");
        string stateAbbr = poFeature->GetFieldAsString("STATE");

        cout << "-----------------------------------------------------------" << endl;
        cout << "Processing State: " << stateName << " (" << stateAbbr << ")" << endl;

        // 5. Fetch geometry (e.g., Polygon or MultiPolygon data)
        OGRGeometry* poGeometry = poFeature->GetGeometryRef();
        if (poGeometry != NULL) {
	    OGRwkbGeometryType type = wkbFlatten(poGeometry->getGeometryType());

            switch(type) {
                case wkbPolygon:
                    vpnt = processPolygonCoordinates(poGeometry);
                    state_polygon[stateAbbr] = vpnt;
                    break;
            
                case wkbMultiPolygon:
                    vpnt = processMultiPolygonCoordinates(poGeometry);
                    state_polygon[stateAbbr] = vpnt;
                    break;
            
                default:
                    cout << "Unsupported geometry type: " << OGRGeometryTypeToName(type) << endl;
                    break;
            }
        }
        
        // Free feature memory
        OGRFeature::DestroyFeature(poFeature);
    }
}

vector<vector<OGRPoint>> StateBoundary::processPolygonCoordinates(OGRGeometry* poGeometry) {
    vector<vector<OGRPoint>> vpoint;

    // Downcast generic geometry to OGRPolygon
    OGRPolygon* poPolygon = dynamic_cast<OGRPolygon*>(poGeometry);
    if (!poPolygon) return vpoint;

    // 1. Get the exterior ring (index 0 is always the outer boundary)
    OGRLinearRing* poExteriorRing = poPolygon->getExteriorRing();
    cout << "NumPoints: " << poExteriorRing->getNumPoints() << endl;
    vector<OGRPoint> spoint;
    for (int i = 0; i < poExteriorRing->getNumPoints(); i++) {
        OGRPoint point;
        poExteriorRing->getPoint(i, &point);
        spoint.push_back(point);
    }

    vpoint.push_back(spoint);

#if 0
    // 2. Get interior rings (holes), if any exist
    int numInteriorRings = poPolygon->getNumInteriorRings();
    for (int r = 0; r < numInteriorRings; r++) {
        OGRLinearRing* poInteriorRing = poPolygon->getInteriorRing(r);
        spoint.clear();
        for (int i = 0; i < poInteriorRing->getNumPoints(); i++) {
	    OGRPoint point;
            poInteriorRing->getPoint(i, &point); // Alternative way to fetch coordinates
	    spoint.push_back(point);
        }
        vpoint.push_back(spoint);
    }
#endif

    return vpoint;
}

vector<vector<OGRPoint>> StateBoundary::processMultiPolygonCoordinates(OGRGeometry* poGeometry) {
    vector<vector<OGRPoint>> vpoint;

    // Downcast generic geometry to OGRMultiPolygon
    OGRMultiPolygon* poMultiPolygon = dynamic_cast<OGRMultiPolygon*>(poGeometry);
    if (!poMultiPolygon) return vpoint;

    int numPolygons = poMultiPolygon->getNumGeometries();

    cout << "numPolygons = " << numPolygons << endl;

    vector<OGRPoint> spoint;
    // 1. Loop through each individual Polygon inside the MultiPolygon
    for (int p = 0; p < numPolygons; p++) {
        OGRGeometry* poSubGeom = poMultiPolygon->getGeometryRef(p);
        OGRPolygon* poPolygon = dynamic_cast<OGRPolygon*>(poSubGeom);

        if (poPolygon) {
            // 2. Loop through the exterior ring of this sub-polygon
            OGRLinearRing* poExterior = poPolygon->getExteriorRing();
	    // if (poExterior->getNumPoints() > 100)
	    if (poExterior->getNumPoints() > 300)
	    {
            cout << "\tNumPoints: " << poExterior->getNumPoints() << endl;
            for (int i = 0; i < poExterior->getNumPoints(); i++) {
                OGRPoint point;
                poExterior->getPoint(i, &point);
		spoint.push_back(point);
            }
            vpoint.push_back(spoint);
	    }
        }
    }

    return vpoint;
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

void StateBoundary::draw()
{
    int i, k, n;

    double x, y, z;
    double dlon, dlat;

    GLfloat line_width = 1.0;

    // cout << "\nEnter file: <" << __FILE__ << ">, function: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << endl;

    // --- ADD THIS LINE TO FIX COLOR ---
    glDisable(GL_LIGHTING);
    // glEnable(GL_COLOR_MATERIAL);
    // glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glDisable(GL_TEXTURE_1D);
    glDisable(GL_TEXTURE_2D);

    // Set your color (e.g., Red: 1.0, Green: 0.0, Blue: 0.0)
  //glColor3d(1.0, 1.0, 1.0);
    glColor3d(0.0, 0.0, 0.0);

    glPushMatrix();

    z = 0.01;

    glLineWidth(line_width);

    for(n = 0; n < state_abbr.size(); ++n)
    {
        // cout << "\tfile: <" << __FILE__ << ">, function: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << endl;
        // cout << "\tstate_abbr[" << n << "]: " << state_abbr[n] << endl;
	vector<vector<OGRPoint>> polygons = state_polygon[state_abbr[n]];
	for(k = 0; k < polygons.size(); ++k)
	{
	    vector<OGRPoint> points = polygons[k];
            glBegin(GL_LINE_STRIP);
	    for(i = 0; i < points.size()-1; ++i)
	    {
		dlon = points[i].getX();
		dlat = points[i].getY();
		_mapprojection->lc_llxy(dlon, dlat, x, y);
                glVertex3d(x, y, z);
            }
            glEnd();
        }
    }

    glPopMatrix();
    glEnable(GL_LIGHTING);
    // cout << "Leave file: <" << __FILE__ << ">, function: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << endl;
}

void StateBoundary::drawONplane()
{
    int i, k, n;

    double x, y, xb, yb;
    double z = 0.01;

    GLfloat line_width = 1.5;

  //cout << "\nfile: <" << __FILE__ << ">, function: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << endl;

    glPushMatrix();

  //glColor3f(1.0, 1.0, 1.0);
    glColor3f(0.0, 0.0, 0.0);

    glLineWidth(line_width);

#if 0
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
#endif
    glPopMatrix();
}

