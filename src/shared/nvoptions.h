#ifndef _NVOptions_H
#define _NVOptions_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include <string>

#include <QtOpenGL/QGLWidget>
#include <QString>
#include <QtGui/QKeyEvent>

using namespace std;

typedef enum
{
    NOMODEL = 0x0000,
    WRF     = 0x0001,
    UFS     = 0x0010,
    UFSINCR = 0x0020,
    MPAS    = 0x0100,
    POP     = 0x0200,
    MPIDEMO = 0x1000
} ModelType;

typedef enum
{
    NV_NONE = 0,
    NV_ISPERSPECTIVE,
    NV_AXISON,
    NV_BGBLACK,
    NV_BUMPON,
    NV_FLATON,
    NV_3DON,
    NV_COLORBARON,
    NV_TITLEON,
    NV_COASTLINEON,
    NV_MCUBON,
    NV_ONMAPON,
    NV_SHADERON,
    NV_ANIMATIONON,
    NV_SAVEJPG,
    NV_STARTSAVE,
    NV_LICON,
    NV_LICONLY,
    NV_LIC3D,
    NV_VECTORON,
    NV_VECTORONLY,
    NV_VECTOR_LONGER,
    NV_VECTOR_SHORTER,
    NV_VECTOR_WIDER,
    NV_VECTOR_NARROWER,
    NV_VECTOR_MORE,
    NV_VECTOR_LESS,
    NV_VEC3D,
    NV_ISOSURFACEON,
    NV_ISOSURFACEONLY,
    NV_INSPECTORON,
    NV_GRIDON,
    NV_DATAVIEWON,
    NV_CLEARDATAVIEW,
    NV_PIXELON,
    NV_COPYON,
    NV_SMOOTHON,
    NV_SHARPENON,
    NV_BLURON,
    NV_EROSIONON,
    NV_DILATIONON,
    NV_LAPLACIANON,
    NV_PREWITTON,
    NV_SOBELON,
    NV_BLACKWHITEON,
    NV_ONHEIGHTSURFACE,
    NV_HASX2,
    NV_HASY2,
    NV_HASZ2,
    NV_HAST2,
    NV_SUBSET,
    NV_HASMINMAX,
    NV_MPAS_SUBSET,
    NV_RESET,
    NV_UPDATECOLORTABLE,
    NV_TRAJECTORY_ON,
    NV_TRAJECTORY_ONLY,
    NV_TRAJECTORY_WIDER,
    NV_TRAJECTORY_NARROWER,
    NV_CLIP_ON,
    NV_STATUS_CHANGED,
    NV_NUMBEROFCBS,
} NV_cbType;

class NVOptions
{
    public:
        NVOptions();   	//  Constructor
       ~NVOptions();	//  Destructor

        void set_cb(NV_cbType cb, bool v);
        bool get_cb(NV_cbType cb);

        void set_ShaderPasses(int n) { numberShaderPasses = n; };
        int get_ShaderPasses() { return numberShaderPasses; };

        void set_screenWidth(int w) { screenWidth = w; };
        void set_screenHeight(int h) { screenHeight = h; };

        int get_screenWidth() { return screenWidth; };
        int get_screenHeight() { return screenHeight; };

        void set_position(QPoint p) { position = p; };
        QPoint get_position() { return position; };

        void set_clicked(bool v) { clicked = v; };
        bool hasClicked() { return clicked; };

        void set_xpos(int n) { xpos = n; };
        void set_ypos(int n) { ypos = n; };
        void set_zpos(int n) { zpos = n; };
        void set_tpos(int n) { tpos = n; };

        int get_xpos() { return xpos; };
        int get_ypos() { return ypos; };
        int get_zpos() { return zpos; };
        int get_tpos() { return tpos; };

        void set_xsec(int n) { xsec = n; };
        void set_ysec(int n) { ysec = n; };
        void set_zsec(int n) { zsec = n; };
        void set_tsec(int n) { tsec = n; };

        int get_xsec() { return xsec; };
        int get_ysec() { return ysec; };
        int get_zsec() { return zsec; };
        int get_tsec() { return tsec; };

        void set_xsec2(int n) { xsec2 = n; };
        void set_ysec2(int n) { ysec2 = n; };
        void set_zsec2(int n) { zsec2 = n; };
        void set_tsec2(int n) { tsec2 = n; };

        int get_xsec2() { return xsec2; };
        int get_ysec2() { return ysec2; };
        int get_zsec2() { return zsec2; };
        int get_tsec2() { return tsec2; };

        void set_model(ModelType m) { model = m; };
        ModelType get_model() { return model; };

        void set_minlon(double v) { minlon = v; };
        void set_maxlon(double v) { maxlon = v; };
        void set_minlat(double v) { minlat = v; };
        void set_maxlat(double v) { maxlat = v; };

        double get_minlon() { return minlon; };
        double get_maxlon() { return maxlon; };
        double get_minlat() { return minlat; };
        double get_maxlat() { return maxlat; };

        void set_trueminimum(double v) { trueminimum = v; };
        void set_truemaximum(double v) { truemaximum = v; };
        void set_userminimum(double v) { userminimum = v; };
        void set_usermaximum(double v) { usermaximum = v; };

        double get_trueminimum() { return trueminimum; };
        double get_truemaximum() { return truemaximum; };
        double get_userminimum() { return userminimum; };
        double get_usermaximum() { return usermaximum; };

    protected:
        bool* cb_options;

        ModelType model;

        int screenWidth, screenHeight;

        int numberShaderPasses;
        int xpos, ypos, zpos, tpos;
        int xsec, ysec, zsec, tsec;
        int xsec2, ysec2, zsec2, tsec2;

        double minlon, maxlon;
        double minlat, maxlat;

        double trueminimum, truemaximum;
        double userminimum, usermaximum;

        QPoint position;
        bool clicked;

        string _varname;
        string _title;
        string _timeinfo;
        string _position;
};
#endif

