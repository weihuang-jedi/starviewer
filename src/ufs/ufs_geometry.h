#ifndef _UFSGeometry_H
#define _UFSGeometry_H

#include <QtOpenGL>

#include <iostream>
#include <vector>

#include <stdlib.h>
#include <assert.h>

#include <geometry.h>

using namespace std;

struct VertexPoint {
    float x, y, z;    // Position
    float nx, ny, nz; // Normal
    float r, g, b, a; // Color (or texture coordinate data)
};

class UFSGeometry : public Geometry {
    public:
        UFSGeometry();
       ~UFSGeometry();


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

      //UFS
        void setup();

	void set_ntim(int v) { _ntim = v; };
	int get_ntim() { return _ntim; };
	int get_hlon() { return _hlon; };

        void set_sphereVertex(int lvl);
        void set_flatVertex(int lvl);

        vector<VertexPoint> get_sphereVertex() { return _sphereVertex; };
        vector<VertexPoint> get_flatVertex() { return _flatVertex; };

    protected:
        string name;

        double* _xSphere;
        double* _ySphere;
        double* _zSphere;

        double* _xFlat;
        double* _yFlat;

        float _hmax;
        float _hmin;

	int _hlon;

    private:
	vector<VertexPoint> _sphereVertex;
	vector<VertexPoint> _flatVertex;

        int _current_sphere_level;
        int _current_flat_level;
	int _ntim;

        void _set_default();
        double _k2h(int k);
        double _k2r(int k);

	void _fillFlatVertex(double x, double y, double z, VertexPoint& vp);
	void _fillSphereVertex(double xs, double ys, double zs, double radius, VertexPoint& vp);
};
#endif

