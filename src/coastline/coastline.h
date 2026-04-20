#ifndef _CoastLine_H
#define _CoastLine_H

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
using namespace netCDF;

class CoastLine
{
    public:
        CoastLine();
       ~CoastLine();

        void print();

        float*  get_value(char *vn);
        double* get_dv(char *vn);
        int*    get_iv(char *vn);

        void drawOnSphere(double r);
        void drawOnSphere(double r, int n);

        void drawOnPlane(double h);
        void drawOnPlane(double h, int n);

        void set_plot_level(int n);
        void set_min_plot_points(int n) { minPlotPoints = n; };

    protected:
        int plotLevel;
        int maxPlotLevel;
        int minPlotPoints;

        string flnm;

        double height;
        double radius;
        double deg2rad;
        double oneover;

        size_t _getDim(const char* dn);
        void _setup();

        void _lonlat2xy(double lon, double lat, double hgt);
        void _drawOnPlane(double height);

        void _drawOnSphere(double radius);
        void _lonlat2xyz(double lon, double lat, double radius);

	// for file: gshhg-gmt-2.3.7/binned_GSHHS_i.nc
	size_t n_of_scalar; // = 1 ;
        size_t n_of_polygon_array; // = 41230 ;
        size_t n_of_node_arrays; // = 2701 ;
        size_t n_of_bin_arrays; // = 2592 ;
        size_t n_of_segment_arrays; // = 45515 ;
        size_t n_of_point_arrays; // = 472443 ;
	int nLonBins;
	int nLatBins;

        NcFile* ncfl;  // dataFile("gshhg-gmt-2.3.7/binned_GSHHS_i.nc", NcFile::read);

        // Metadata for scaling
        int binSizeMinutes = 300; // 5 degrees = 300 minutes
        double binSizeDeg = 5.0;
        double scale = binSizeDeg / 65535.0;

	vector<short> relLons, relLats;
        int Bin_size_in_minutes;	// (Dimension_of_scalar) ;
        int N_bins_in_360_longitude_range;	// (Dimension_of_scalar) ;
        int N_bins_in_180_degree_latitude_range;	// (Dimension_of_scalar) ;
        int N_bins_in_file;	// (Dimension_of_scalar) ;
        int N_segments_in_file;	// (Dimension_of_scalar) ;
        int N_points_in_file;	// (Dimension_of_scalar) ;
        vector<int> Id_of_first_segment_in_a_bin;	// (Dimension_of_bin_arrays) ;
        vector<short> N_segments_in_a_bin;	// (Dimension_of_bin_arrays) ;
        vector<int> Id_of_first_point_in_a_segment;	// (Dimension_of_segment_arrays) ;

/*
	netcdf binned_GSHHS_i {
dimensions:
        Dimension_of_scalar = 1 ;
        Dimension_of_polygon_array = 41230 ;
        Dimension_of_node_arrays = 2701 ;
        Dimension_of_bin_arrays = 2592 ;
        Dimension_of_segment_arrays = 45515 ;
        Dimension_of_point_arrays = 472443 ;
variables:
        int Bin_size_in_minutes(Dimension_of_scalar) ;
        int N_bins_in_360_longitude_range(Dimension_of_scalar) ;
        int N_bins_in_180_degree_latitude_range(Dimension_of_scalar) ;
        int N_bins_in_file(Dimension_of_scalar) ;
        int N_polygons_in_file(Dimension_of_scalar) ;
        int N_segments_in_file(Dimension_of_scalar) ;
        int N_points_in_file(Dimension_of_scalar) ;
        int N_nodes_in_file(Dimension_of_scalar) ;
        int Id_of_parent_polygons(Dimension_of_polygon_array) ;
        double The_km_squared_area_of_polygons(Dimension_of_polygon_array) ;
        int Micro_fraction_of_full_resolution_area(Dimension_of_polygon_array) ;
        int Id_of_node_polygons(Dimension_of_node_arrays) ;
        int Id_of_first_segment_in_a_bin(Dimension_of_bin_arrays) ;
        short Embedded_node_levels_in_a_bin(Dimension_of_bin_arrays) ;
        short Embedded_node_levels_in_a_bin_ANT(Dimension_of_bin_arrays) ;
        short N_segments_in_a_bin(Dimension_of_bin_arrays) ;
        int Embedded_npts_levels_exit_entry_for_a_segment(Dimension_of_segment_arrays) ;
        int Id_of_first_point_in_a_segment(Dimension_of_segment_arrays) ;
        int Id_of_GSHHS_ID(Dimension_of_segment_arrays) ;
        byte Embedded_ANT_flag(Dimension_of_segment_arrays) ;
        short Relative_longitude_from_SW_corner_of_bin(Dimension_of_point_arrays) ;
                Relative_longitude_from_SW_corner_of_bin:units = "1/65535 of 5 degrees relative to south-west corner of bin" ;
        short Relative_latitude_from_SW_corner_of_bin(Dimension_of_point_arrays) ;
                Relative_latitude_from_SW_corner_of_bin:units = "1/65535 of 5 degrees relative to south-west corner of bin" ;

// global attributes:
                :title = "Derived from World Vector Shoreline, CIA WDB-II, and Atlas of the Cryosphere" ;
                :source = "Processed by Paul Wessel and Walter H. F. Smith, 1994-2017" ;
                :version = "2.3.7" ;
*/
};
#endif

