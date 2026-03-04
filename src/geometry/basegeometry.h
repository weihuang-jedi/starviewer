#ifndef _BaseGeometry_H
#define _BaseGeometry_H

//$Id: basegeometry.h 4865 2014-01-01 15:49:23Z starviewer $

#include <string>          // <--- Add this for 'string'
#include <QString>         // <--- Add this if you want to use Qt strings
#include <QOpenGLWidget>

using namespace std;

class BaseGeometry
{
    public:
        BaseGeometry();
        virtual ~BaseGeometry();

        virtual void reset() { };
        virtual void reset_dimension() { };
        virtual void print() { };

        int get_mx() { return _mx; };
        int get_my() { return _my; };
        int get_mz() { return _mz; };

        int get_nx() { return _nx; };
        int get_ny() { return _ny; };
        int get_nz() { return _nz; };
        int get_nt() { return _nt; };
        int get_nm() { return _nm; };

        void set_mx(int mx) { _mx = mx; };
        void set_my(int my) { _my = my; };
        void set_mz(int mz) { _mz = mz; };

        void set_nx(int nx) { _nx = nx; };
        void set_ny(int ny) { _ny = ny; };
        void set_nz(int nz) { _nz = nz; };
        void set_nt(int nt) { _nt = nt; };
        void set_nm(int nm) { _nm = nm; };

        void set_hasFillValue(bool v) { _hasFillValue = v; };
        bool get_hasFillValue() { return _hasFillValue; };

        void  set_fillValue(double v) { _fillValue = v; };
        double get_fillValue() { return _fillValue; };

        void  set_name(string vn) { name = vn; };
        string get_name() { return name; };

    protected:
        string name;

        int  _mx;
        int  _my;
        int  _mz;

        int  _nx;
        int  _ny;
        int  _nz;
        int  _nt;
        int  _nm;

        bool _hasFillValue;
        double _fillValue;

        void _set_default();
};
#endif

