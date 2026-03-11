#ifndef _WRF_Geometry_H
#define _WRF_Geometry_H

//$Id: wrf_geometry.h 4833 2013-11-05 15:49:31Z starviewer $

#include <QtOpenGL>

#include <iostream>
#include <vector>

#include <stdlib.h>
#include <assert.h>

#include "geometry.h"

using namespace std;

class WRF_Geometry : public Geometry
{
    public:
        WRF_Geometry();
       ~WRF_Geometry();

        void set_Xstaggered() { Xstaggered = true; };
        void set_Ystaggered() { Ystaggered = true; };
        void set_Zstaggered() { Zstaggered = true; };

        void unset_Xstaggered() { Xstaggered = false; };
        void unset_Ystaggered() { Ystaggered = false; };
        void unset_Zstaggered() { Zstaggered = false; };

        bool isXstaggered() { return Xstaggered; }
        bool isYstaggered() { return Ystaggered; }
        bool isZstaggered() { return Zstaggered; }

        void set_dx(float fdx) { dx = fdx; };
        void set_dy(float fdy) { dy = fdy; };
        void set_dt(float fdt) { dt = fdt; };
        void set_cen_lat(float fcen_lat) { cen_lat = fcen_lat; };
        void set_cen_lon(float fcen_lon) { cen_lon = fcen_lon; };
        void set_truelat1(float ftruelat1) { truelat1 = ftruelat1; };
        void set_truelat2(float ftruelat2) { truelat2 = ftruelat2; };
        void set_moad_cen_lat(float fmoad_cen_lat) { moad_cen_lat = fmoad_cen_lat; };
        void set_stand_lon(float fstand_lon) { stand_lon = fstand_lon; };
        void set_pole_lat(float fpole_lat) { pole_lat = fpole_lat; };
        void set_pole_lon(float fpole_lon) { pole_lon = fpole_lon; };
        void set_gmt(float fgmt) { gmt = fgmt; };

        void set_map_proj(int imap_proj) { map_proj = imap_proj; };
        void set_grid_id(int igrid_id) { grid_id = igrid_id; };
        void set_parent_id(int iparent_id) { parent_id = iparent_id; };
        void set_parent_grid_ratio(int iparent_grid_ratio) { parent_grid_ratio = iparent_grid_ratio; };
        void set_i_parent_start(int ii_parent_start) { i_parent_start = ii_parent_start; };
        void set_j_parent_start(int ij_parent_start) { j_parent_start = ij_parent_start; };

        void set_julyr(int ijulyr) { julyr = ijulyr; };
        void set_julday(int ijulday) { julday = ijulday; };

        void set_num_land_cat(int inum_land_cat) { num_land_cat = inum_land_cat; };
        void set_iswater(int iiswater) { iswater = iiswater; };
        void set_islake(int iislake) { islake = iislake; };
        void set_isice(int iisice) { isice = iisice; };
        void set_isurban(int iisurban) { isurban = iisurban; };
        void set_issoilwater(int iissoilwater) { issoilwater = iissoilwater; };

        void set_mminlu(const char *cmminlu) { strcpy(mminlu, cmminlu); };

        float get_dx() { return dx; };
        float get_dy() { return dy; };
        float get_dt() { return dt; };
        float get_cen_lat() { return cen_lat; };
        float get_cen_lon() { return cen_lon; };
        float get_truelat1() { return truelat1; };
        float get_truelat2() { return truelat2; };
        float get_moad_cen_lat() { return moad_cen_lat; };
        float get_stand_lon() { return stand_lon; };
        float get_pole_lat() { return pole_lat; };
        float get_pole_lon() { return pole_lon; };
        float get_gmt() { return gmt; };

        int get_map_proj() { return map_proj; };
        int get_grid_id() { return grid_id; };
        int get_parent_id() { return parent_id; };
        int get_parent_grid_ratio() { return parent_grid_ratio; };
        int get_i_parent_start() { return i_parent_start; };
        int get_j_parent_start() { return j_parent_start; };

        int get_julyr() { return julyr; };
        int get_julday() { return julday; };

        int get_num_land_cat() { return num_land_cat; };
        int get_iswater() { return iswater; };
        int get_islake() { return islake; };
        int get_isice() { return isice; };
        int get_isurban() { return isurban; };
        int get_issoilwater() { return issoilwater; };

        char *get_mminlu() { return mminlu; };

        void reset_dimension();
        void reset();
        void print();

        void set_lon(double* v) { _lon = v; };
        void set_lat(double* v) { _lat = v; };
        void set_hgt(float* v) { _hgt = v; };

        void set_ulon(double* v) { _ulon = v; };
        void set_ulat(double* v) { _ulat = v; };

        void set_vlon(double* v) { _vlon = v; };
        void set_vlat(double* v) { _vlat = v; };

        double* get_lon() { return _lon; };
        double* get_lat() { return _lat; };
        float* get_hgt() { return _hgt; };

        double* get_ulon() { return _ulon; };
        double* get_ulat() { return _ulat; };

        double* get_vlon() { return _vlon; };
        double* get_vlat() { return _vlat; };

        int  get_nx() { return _nx; };
        int  get_ny() { return _ny; };
        int  get_nz() { return _nz; };
        int  get_nt() { return _nt; };
        int  get_nm() { return _nm; };

        void  set_nx(int nx) { _nx = nx; };
        void  set_ny(int ny) { _ny = ny; };
        void  set_nz(int nz) { _nz = nz; };
        void  set_nt(int nt) { _nt = nt; };
        void  set_nm(int nm) { _nm = nm; };

    protected:
        void _set_default();

        int  _nx;
        int  _ny;
        int  _nz;
        int  _nt;
        int  _nm;

        double* _lon;
        double* _lat;
        float* _hgt;

        double* _ulon;
        double* _ulat;

        double* _vlon;
        double* _vlat;

        bool Xstaggered;
        bool Ystaggered;
        bool Zstaggered;

        float dx;              //DX = 15000.f ;
        float dy;              //DY = 15000.f ;
        float dt;              //DT = 75.f ;
        float cen_lat;         //CEN_LAT = 39.f ;
        float cen_lon;         //CEN_LON = -101.f ;
        float truelat1;        //TRUELAT1 = 32.f ;
        float truelat2;        //TRUELAT2 = 46.f ;
        float moad_cen_lat;    //MOAD_CEN_LAT = 39.f ;
        float stand_lon;       //STAND_LON = -101.f ;
        float pole_lat;        //POLE_LAT = 90.f ;
        float pole_lon;        //POLE_LON = 0.f ;
        float gmt;             //GMT = 0.f ;

        int map_proj;          //MAP_PROJ = 1 ;
        int grid_id;           //GRID_ID = 1 ;
        int parent_id;         //PARENT_ID = 0 ;
        int parent_grid_ratio; //PARENT_GRID_RATIO = 1 ;
        int i_parent_start;    //I_PARENT_START = 1 ;
        int j_parent_start;    //J_PARENT_START = 1 ;

        int julyr;             //JULYR = 2011 ;
        int julday;            //JULDAY = 140 ;

        int num_land_cat;      //NUM_LAND_CAT = 24 ;
        int iswater;           //ISWATER = 16 ;
        int islake;            //ISLAKE = -1 ;
        int isice;             //ISICE = 24 ;
        int isurban;           //ISURBAN = 1 ;
        int issoilwater;       //ISOILWATER = 14 ;

        char mminlu[12];       //MMINLU = "USGS" ;
};
#endif


