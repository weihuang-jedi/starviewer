#ifndef _BaseTranslator_H
#define _BaseTranslator_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <sys/stat.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>

#include <QKeyEvent>
#include <QImage>
#include <QGLWidget>
#include <QString>
#include <QTimer>

#include "util.h"
#include "light.h"
#include "locator.h"
#include "nvoptions.h"
#include "colorTable.h"
#include "sliderNspin.h"

using namespace std;

class BaseTranslator : public QGLWidget
{
    Q_OBJECT		//  Qt magic macro
    public:
        BaseTranslator(ColorTable* ct,
                       NVOptions* opts,
                       QWidget* parent=0);    	//  Constructor
       virtual ~BaseTranslator();			//  Destructor

        QSize sizeHint() const {return QSize(900,600);}   //  Default size of widget

        virtual void setup() = 0;
        virtual void set_filename(QString flnm);
        virtual void updateSliders();
        void set_light(Light* l);
        void set_locator(Locator* l);

        void set_sliderX(SliderNSpin *s) { sliderNspinX = s; };
        void set_sliderY(SliderNSpin *s) { sliderNspinY = s; };
        void set_sliderZ(SliderNSpin *s) { sliderNspinZ = s; };
        void set_sliderT(SliderNSpin *s) { sliderNspinT = s; };

        void set_nx(int n) { nx = n; };
        void set_ny(int n) { ny = n; };
        void set_nz(int n) { nz = n; };
        void set_nt(int n) { nt = n; };

        int  get_nx() { return nx; };
        int  get_ny() { return ny; };
        int  get_nz() { return nz; };
        int  get_nt() { return nt; };

        string get_title() { return _title; };

        virtual int get_ndv(int n) = 0;
        virtual string* get_ndvNames(int n) = 0;

        NVOptions* get_nvoptions() { return nvoptions; };

    public slots:
        void reset(void);           //  Reset view angles
        void selectProjection(int p);
        void selectAxis(int f);
        void selectBackground(int f);

        virtual void update_frame();

        virtual void selectShader(int f);
        virtual void selectColorMap(const QString& str) = 0;

        virtual void select1dVar(const QString& str) = 0;
        virtual void select2dVar(const QString& str) = 0;
        virtual void select3dVar(const QString& str) = 0;

        void setValueX(int n);
        void setValueY(int n);
        void setValueZ(int n);
        void setValueT(int n);

        void selectX2(int f);
        void selectY2(int f);
        void selectZ2(int f);
        void selectT2(int f);

        virtual void nextFrame();
        virtual void backFrame();

        void selectMCub(int f);
        void select3D(int f);
        void selectOnMap(int n);
        void selectBump(int f);
        void selectSaveImage();
        void selectColorBar(int f);
        void selectFlat(int n);

        void selectTitle(int n);
        void selectCoastLine(int n);

        void selectAnimation(int n);
        void selectJPG4Movie(int n);

        void selectIsoSurface(int n);
        void selectVector(int n);
        void selectLIC(int n);

        void selectIsoOnly(int n);
        void selectVecOnly(int n);
        void selectLIConly(int n);

        void selectVec3D(int n);
        void selectLIC3D(int n);

        void selectSubset(int n);
        void selectMinMax(int n);

        virtual void selectDataview(int n);
        virtual void selectClearDataview();

    signals:
        void info(QString text);    //  Signal for display info
        void frame(QString text);   //  Signal for display frame
        void mouse_msg(QString text);    //  Signal for mouse-clicked-info
        void locator_msg(QString text);    //  Signal for locator info

    protected:
        int    xRot;	//  Display angles
        int    yRot;	//  Display angles
        int    zRot;	//  Display angles
        bool   mouse;	//  Mouse pressed
        QPoint pos;	//  Mouse position
        double dim;	//  Display size
        double m_d_left_plane, m_d_right_plane, m_d_bottom_plane, m_d_top_plane, m_d_near_plane, m_d_far_plane;

        double fovy;
        double aspect;	//  Sceen aspect ratio
        double zNear;
        double zFar;

        int    ori_xRot;    //  Display angles
        int    ori_yRot;    //  Display angles
        int    ori_zRot;    //  Display angles

        double ori_dim;     //  Display size
        double ori_fovy;
        double ori_aspect;  //  Sceen aspect ratio
        double ori_zNear;
        double ori_zFar;
    
        int    cur_xRot;    //  Display angles
        int    cur_yRot;    //  Display angles
        int    cur_zRot;    //  Display angles

        double cur_dim;     //  Display size
        double cur_fovy;
        double cur_aspect;  //  Sceen aspect ratio
        double cur_zNear;
        double cur_zFar;

        void initializeGL();                   //  Initialize widget
        void resizeGL(int width, int height);  //  Resize widget

        virtual void paintGL() = 0;
        virtual void show() = 0;

        NVOptions*  nvoptions;
        ColorTable* colorTable;

        Light* light;
        Locator* locator;

        ofstream outf;

        void mousePressEvent(QMouseEvent*);    //  Mouse pressed
        void mouseReleaseEvent(QMouseEvent*);  //  Mouse released
        void mouseMoveEvent(QMouseEvent*);     //  Mouse moved
        void wheelEvent(QWheelEvent*);         //  Mouse wheel

        void keyPressEvent(QKeyEvent *);
        void keyReleaseEvent(QKeyEvent *);

        double deg2rad;
        float  d2r;

        float _time;

        void set_modelview();
        void project();                        //  Set projection
        void _initialize();
        void SaveImage();
        void SaveImage(char *flnm);

        void x_axis(double x, double length, double angle);
        void y_axis(double y, double length, double angle);
        void z_axis(double z, double length, double angle);

        SliderNSpin *sliderNspinX;
        SliderNSpin *sliderNspinY;
        SliderNSpin *sliderNspinZ;
        SliderNSpin *sliderNspinT;

        int nx, ny, nz, nt;

      //Make these in the base:

      //void update_sliderNspin();

        void _set_current_time();

        bool _isDummy;
        bool _hasFileList;

        string _filename;
        string _varname;
        string _title;
        string _timeinfo;
        string _position;
        string* _timestr;

        int _curFile;
        int _maxFile;

        int _glbTime;
        int _preTime;
        int _curTime;
        int _maxTime;
        int* _nTimes;

        int sWidth, sHeight;

        double _minval, _maxval;

        vector<string> _names2dvar;
        vector<string> _names3dvar;
        vector<string> _names4dvar;

      //Unified methods:
        void _displayColorBar();

        void writeHeader();

        void setBackgroundColor();
        void setLabelColor();
        void setViewOptions();

        void drawAxis();
        void drawColorBar();

        QString _varinfo;
        QString _frameinfo;
        QString _mouseinfo;
        QString _locatorinfo;

        virtual void writeVarInfo();
        virtual void writeFrameInfo();
        virtual void writeMouseMsg();
        virtual void writeLocatorMsg();

        bool need_save_status;
        void save_status();

        void _update_colortable();
        void _backupStatus();
        void _restoreStatus();

        bool _jpgNotSaved;
        bool _saveJpg;
        bool _startSave;

        bool FileExists(char *filename);

        void SaveJpg();
        void SaveJpg(int n);
};
#endif

