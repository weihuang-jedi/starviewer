#ifndef _MainWindow_H
#define _MainWindow_H

#include "yamlhandler.h"

#include "basetranslator.h"
#include "colorTable.h"
#include "light.h"
#include "locator.h"

#include "controlWidget.h"
#include "displayWidget.h"

#include "lightWidget.h"
#include "animationWidget.h"
#include "inspectorWidget.h"
#include "licWidget.h"
#include "clipWidget.h"
#include "vectorWidget.h"
#include "trajectoryWidget.h"
#include "isosurfaceWidget.h"
#include "pixelWidget.h"
#include "dataviewWidget.h"
#include "subsetWidget.h"
#include "minmaxWidget.h"

#include "mpitranslator.h"
#include "ufs_translator.h"
#include "ufsincr_translator.h"
#include "mpastranslator.h"
#include "mom6_translator.h"
#include "eagle_translator.h"
#include "eagleglobal_translator.h"
#include "eaglelam_translator.h"

#include "modelparser.h"
#include "modelparserFactory.h"

#include <QMainWindow>
QT_BEGIN_NAMESPACE
class QAction;
class QActionGroup;
class QLabel;
class QMenu;
QT_END_NAMESPACE

class MPIDEMOParser;
class UFSParser;
class UFSINCRParser;
class EagleParser;

class MainWindow : public QMainWindow
{
    Q_OBJECT

    public:
        MainWindow(string yamlfile);
       ~MainWindow();

    private slots:
        void newFile();
        void open();
        void save();
        void print();
        void undo();
        void redo();
        void about();

      //Applications
        void mpidemo();
        void ufs();
        void ufsincr();
        void ufsmom6();
        void mpas();
        void eagle();
        void eagleglobal();
        void eaglelam();

        void animation_func();
        void inspector_func();
        void light_func();
        void lic_func();
        void vector_func();
        void trajectory_func();
        void clip_func();
        void isosurface_func();
        void pixel_func();
        void dataview_func();
        void subset_func();
        void minmax_func();

    protected:
        ColorTable *colorTable;
        NVOptions* nvoptions;
        Locator* locator;
        Light* light;

        BaseTranslator* translator;
        ControlWidget* controlPanel;
        DisplayWidget* display;
        AnimationWidget* animationWidget;
        InspectorWidget* inspectorWidget;
        LightWidget* lightWidget;
        LICWidget* licWidget;
        VectorWidget* vectorWidget;
        TrajectoryWidget* trajectoryWidget;
        ClipWidget* clipWidget;
        IsoSurfaceWidget* isosurfaceWidget;
        PixelWidget* pixelWidget;
        DataViewWidget* dataviewWidget;
        SubsetWidget* subsetWidget;
        MinMaxWidget* minmaxWidget;

	unique_ptr<ModelParser> myParser;

        int screenWidth;
        int screenHeight;

        void _setup();

        void contextMenuEvent(QContextMenuEvent *event);

        void createActions();
        void createMenus();
    
        QMenu *fileMenu;
        QMenu *editMenu;
        QMenu *appsMenu;
        QMenu *toolMenu;
        QMenu *helpMenu;

        QActionGroup *alignmentGroup;
        QAction *newAct;
        QAction *openAct;
        QAction *saveAct;
        QAction *printAct;
        QAction *undoAct;
        QAction *redoAct;
        QAction *aboutAct;

        QAction *ufsAct;
        QAction *ufsincrAct;
        QAction *ufsmom6Act;
        QAction *mpasAct;
        QAction *eagleAct;
        QAction *eagleglobalAct;
        QAction *eaglelamAct;
        QAction *mpidemoAct;

        QAction *animationAct;
        QAction *inspectorAct;
        QAction *lightAct;
        QAction *licAct;
        QAction *vectorAct;
        QAction *trajectoryAct;
        QAction *clipAct;
        QAction *isosurfaceAct;
        QAction *pixelAct;
        QAction *dataviewAct;
        QAction *subsetAct;
        QAction *minmaxAct;

    private:
        int nInstance;
        int numberOfWidget;

	string yamlfile;

	void _setup_controlPanel();
	void _setup_display();
};
#endif

