#ifndef _EAGLELAM_Geometry_H
#define _EAGLELAM_Geometry_H

#include <QtOpenGL>

#include <iostream>
#include <vector>

#include <stdlib.h>
#include <assert.h>

#include <geometry.h>

using namespace std;

class EAGLELAM_Geometry : public Geometry {
    public:
        EAGLELAM_Geometry();
       ~EAGLELAM_Geometry();


        string get_name() { return name; };

        void  set_name(string vn) { name = vn; };

        void print();
        void reset();
        void reset_dimension();

        double* get_xSphere() { return _xSphere; };
        double* get_ySphere() { return _ySphere; };
        double* get_zSphere() { return _zSphere; };

        double* get_xFlat() { return _xFlat; };
        double* get_yFlat() { return _yFlat; };

        float* get_longitude() { return _longitude; };
        float* get_latitude() { return _latitude; };

	void set_longitude(float *v) { _longitude = v; };
	void set_latitude(float *v) { _latitude = v; };

        double* get_xGrid() { return _xGrid; };
        double* get_yGrid() { return _yGrid; };

	void set_xGrid(double *v) { _xGrid = v; };
	void set_yGrid(double *v) { _yGrid = v; };

      //EAGLE
        void setup();

	void set_ntime(int v) { _ntime = v; };
	int get_ntime() { return _ntime; };

	string get_grid_mapping_name() { return _grid_mapping_name; };
        float get_latitude_of_projection_origin() { return _latitude_of_projection_origin; };
        float get_longitude_of_central_meridian() { return _longitude_of_central_meridian; };
        vector<float> get_standard_parallel() { return _standard_parallel; };

	void set_grid_mapping_name(string v) { _grid_mapping_name = v; };
        void set_latitude_of_projection_origin(float v) { _latitude_of_projection_origin = v; };
        void set_longitude_of_central_meridian(float v) { _longitude_of_central_meridian = v; };
        void set_standard_parallel(vector<float> v) { _standard_parallel = v; };

    protected:
        string name;

        double* _xSphere;
        double* _ySphere;
        double* _zSphere;

        double* _xFlat;
        double* _yFlat;

        float* _longitude;
        float* _latitude;

        double* _xGrid;
        double* _yGrid;

    private:
        void _set_default();

	int _ntime;

	string _grid_mapping_name;
        float _latitude_of_projection_origin;
        float _longitude_of_central_meridian;
        vector<float> _standard_parallel;
};
#endif

