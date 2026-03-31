#include "mainwindow.h"

MainWindow::MainWindow(string flnm, bool isList,
                       string camse_mfnm, NVOptions* opt)
{
    QDesktopWidget *desktop = QApplication::desktop();

  //cout << "\nEnter MainWindow: file: " << __FILE__ << ", line: " << __LINE__ << endl;

    isFileList = isList;
    fileName = QString(flnm.c_str());
    camse_mappingFilename = camse_mfnm;
    nvoptions = opt;

    screenWidth = desktop->width();
    screenHeight = desktop->height(); 

    nInstance = 0;
    numberOfWidget = 0;

  //cout << "\t\tfile: " << __FILE__ << ", line: " << __LINE__ << endl;

    createActions();
    createMenus();

    setMinimumSize(480, 320);
    resize(960, 640);

  //cout << "\t\tfile: " << __FILE__ << ", line: " << __LINE__ << endl;

    light = new Light();

  //cout << "\t\tfile: " << __FILE__ << ", line: " << __LINE__ << endl;

    locator = new Locator();

  //cout << "\t\tfile: " << __FILE__ << ", line: " << __LINE__ << endl;

    colorTable = new ColorTable();

  //cout << "\t\tfile: " << __FILE__ << ", line: " << __LINE__ << endl;

    controlPanel = new ControlWidget();
    display = new DisplayWidget();

  //cout << "\t\tfile: " << __FILE__ << ", line: " << __LINE__ << endl;

    _setup();

  //cout << "Leave MainWindow: file: " << __FILE__ << ", line: " << __LINE__ << endl;
}

MainWindow::~MainWindow()
{
  //cout << "\tEnter function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
    delete light;
    delete locator;
    delete colorTable;
    delete controlPanel;
    delete display;
  //cout << "\tLeave function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
}

void MainWindow::_setup()
{
  //cout << "\nEnter function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;

    switch(nvoptions->get_model())
    {
      //case CAMSE:
      //    camse();
      //    break;
      //case POP:
      //    pop();
      //    break;
      //case MPAS:
      //    mpas();
      //    break;
      //case WRF:
          //cout << "\tfile: <" << __FILE__ << ">, function: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << endl;
      //    wrf();
      //    break;
        case UFS:
          //cout << "\tfile: <" << __FILE__ << ">, function: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << endl;
            ufs();
            break;
      //case RADX:
      //    radx();
      //    break;
      //case VTK:
      //    vtk();
      //    break;
        case TEST:
            test();
            break;
        case MPIDEMO:
            mpidemo();
            break;
      //case HDF:
      //    hdf();
      //    break;
        default:
            general();
            break;
    }
  //cout << "Leave function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
}

void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);

    menu.addAction(generalAct);
  //menu.addAction(wrfAct);
    menu.addAction(ufsAct);
  //menu.addAction(mpasAct);
  //menu.addAction(camseAct);
  //menu.addAction(popAct);
  //menu.addAction(hdfAct);
  //menu.addAction(radxAct);
  //menu.addAction(vtkAct);
    menu.addAction(testAct);
    menu.addAction(mpidemoAct);

    menu.exec(event->globalPos());
}

void MainWindow::newFile()
{
}

void MainWindow::open()
{
    fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                    QDir::currentPath());

  //fprintf(stderr, "\nFile: %s, line: %d\n", __FILE__, __LINE__);
    fprintf(stderr, "\tFileName: <%s>\n\n", fileName.toStdString().c_str());
}

void MainWindow::save()
{
}

void MainWindow::print()
{
}

void MainWindow::undo()
{
}

void MainWindow::redo()
{
}

void MainWindow::_setup_controlPanel()
{
    translator->setup();
    translator->set_light(light);
    translator->set_locator(locator);

    controlPanel->set_colortable(colorTable);
    controlPanel->set_translator(translator);
    controlPanel->setup();

    switch(nvoptions->get_model())
    {
        case WRF:
           //controlPanel->selectNCL();
           //controlPanel->disable_ncl();
             break;
        case UFS:
           //controlPanel->selectNCL();
           //controlPanel->disable_ncl();
             break;
        case POP:
             controlPanel->disable_ncl();
             controlPanel->disable_bump();
             controlPanel->disable_onmap();
             controlPanel->disable_sphere();
             break;
        case MPAS:
             controlPanel->disable_ncl();
             controlPanel->disable_bump();
             controlPanel->disable_onmap();
             controlPanel->disable_sphere();
           //controlPanel->disable_surface();
             controlPanel->disable_x2();
             controlPanel->disable_y2();
             controlPanel->disable_z2();
             controlPanel->disable_t2();
             break;
        case CAMSE:
           //cout << "\tfunction: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
             controlPanel->disable_ncl();
             controlPanel->disable_onmap();
             controlPanel->disable_sphere();
           //controlPanel->disable_surface();
             break;
        case RADX:
             controlPanel->disable_ncl();
             controlPanel->disable_onmap();
             controlPanel->disable_sphere();
             controlPanel->disable_flat();
             controlPanel->disable_x2();
             controlPanel->disable_y2();
             controlPanel->disable_z2();
             controlPanel->disable_t2();
             break;
        case MPIDEMO:
             controlPanel->disable_ncl();
             controlPanel->disable_onmap();
             controlPanel->disable_sphere();
             controlPanel->disable_flat();
             controlPanel->disable_x2();
             controlPanel->disable_y2();
             controlPanel->disable_z2();
             controlPanel->disable_t2();
             translator->updateSliders();
             break;
        case HDF:
             controlPanel->disable_ncl();
             controlPanel->disable_bump();
             controlPanel->disable_onmap();
             controlPanel->disable_sphere();
             break;
        default:
           //controlPanel->disable_ncl();
             controlPanel->disable_flat();
             break;
    }

    controlPanel->move(0, 0);
    controlPanel->show();
}

void MainWindow::_setup_display()
{
    int x, y;

    x = screenWidth / 2;
    y = screenHeight / 2;

    setCentralWidget(display);

    display->set_translator(translator);
    display->setup();
    display->move(x, y);
    display->show();
}

void MainWindow::general()
{
    general_translator = new GeneralTranslator(colorTable, nvoptions,
                                               fileName.toStdString(),
                                               isFileList);
    translator = general_translator;

    setWindowTitle(tr("NV"));

    _setup_controlPanel();
    _setup_display();
}

/*
void MainWindow::wrf()
{
    wrf_translator = new WRFTranslator(colorTable, nvoptions,
                                       fileName.toStdString(),
                                       isFileList);
    translator = wrf_translator;

    setWindowTitle(tr("NV for WRF"));

    _setup_controlPanel();

    _setup_display();
}
*/

void MainWindow::ufs()
{
    ufs_translator = new UFSTranslator(colorTable, nvoptions,
                                       fileName.toStdString(),
                                       isFileList);
    translator = ufs_translator;

    setWindowTitle(tr("NV for UFS"));

    _setup_controlPanel();

    _setup_display();
}

/*
void MainWindow::mpas()
{
    mpas_translator = new MPASTranslator(colorTable, nvoptions,
                                         fileName.toStdString(),
                                         isFileList);

    translator = mpas_translator;

    setWindowTitle(tr("NV for MPAS"));

    _setup_controlPanel();
    _setup_display();
}

void MainWindow::camse()
{
  //cout << "\nEnter Funciton: " << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;

    camse_translator = new CAMseTranslator(colorTable, nvoptions,
                                           fileName.toStdString(),
                                           isFileList,
                                           camse_mappingFilename);
    translator = camse_translator;

    setWindowTitle(tr("NV for CAM-SE"));

  //cout << "\tFunciton: " << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
    _setup_controlPanel();
  //cout << "\tFunciton: " << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
    _setup_display();

  //cout << "Leave Funciton: " << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
}

void MainWindow::pop()
{
    pop_translator = new POPTranslator(colorTable, nvoptions,
                                       fileName.toStdString(),
                                       isFileList);
    translator = pop_translator;

    setWindowTitle(tr("NV for POP"));

    _setup_controlPanel();
    _setup_display();
}
*/

#ifdef UseRADX
void MainWindow::radx()
{
    nvoptions->set_zsec(0);

    radx_translator = new RadxTranslator(colorTable, nvoptions,
                                         fileName.toStdString(),
                                         isFileList);

    translator = radx_translator;

    setWindowTitle(tr("NV for RADX"));

    _setup_controlPanel();
    _setup_display();
}
#endif

#if 0
void MainWindow::vtk()
{
    vtk_translator = new VTKTranslator(colorTable, nvoptions,
                                       fileName.toStdString(),
                                       isFileList);
    translator = vtk_translator;

    setWindowTitle(tr("NV for VTK"));

    _setup_controlPanel();
    _setup_display();
}
#endif

void MainWindow::test()
{
    test_translator = new TestTranslator(colorTable, nvoptions,
                                         fileName.toStdString(),
                                         isFileList);

    translator = test_translator;

    setWindowTitle(tr("NV for TEST"));

    _setup_controlPanel();

    _setup_display();
}

void MainWindow::mpidemo()
{
    mpidemo_translator = new MPITranslator(colorTable, nvoptions,
                                         fileName.toStdString(),
                                         isFileList);

    translator = mpidemo_translator;

    setWindowTitle(tr("NV to demo MPI"));

    _setup_controlPanel();

    _setup_display();
}

/*
void MainWindow::hdf()
{
    hdf_translator = new HDFTranslator(colorTable, nvoptions,
                                       fileName.toStdString(),
                                       isFileList);
    translator = hdf_translator;

    setWindowTitle(tr("NV for HDF"));

    _setup_controlPanel();
    _setup_display();
}
*/

void MainWindow::about()
{
    QMessageBox::about(this, tr("About Menu"),
            tr("The <b>Menu</b> example shows how to create "
               "menu-bar menus and context menus."));
}

void MainWindow::createActions()
{
    newAct = new QAction(tr("&New"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("Create a new file"));
    connect(newAct, SIGNAL(triggered()), this, SLOT(newFile()));

    openAct = new QAction(tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    saveAct = new QAction(tr("&Save"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save the document to disk"));
    connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

    printAct = new QAction(tr("&Print..."), this);
    printAct->setShortcuts(QKeySequence::Print);
    printAct->setStatusTip(tr("Print the document"));
    connect(printAct, SIGNAL(triggered()), this, SLOT(print()));

    undoAct = new QAction(tr("&Undo"), this);
    undoAct->setShortcuts(QKeySequence::Undo);
    undoAct->setStatusTip(tr("Undo the last operation"));
    connect(undoAct, SIGNAL(triggered()), this, SLOT(undo()));

    redoAct = new QAction(tr("&Redo"), this);
    redoAct->setShortcuts(QKeySequence::Redo);
    redoAct->setStatusTip(tr("Redo the last operation"));
    connect(redoAct, SIGNAL(triggered()), this, SLOT(redo()));

    aboutAct = new QAction(tr("&About"), this);
    aboutAct->setStatusTip(tr("Show the application's About box"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    generalAct = new QAction(tr("&General"), this);
  //generalAct->setShortcut(QKeySequence::Global);
    generalAct->setStatusTip(tr("Try to activate 'general' application"));
    connect(generalAct, SIGNAL(triggered()), this, SLOT(general()));

  //wrfAct = new QAction(tr("&WRF"), this);
  //wrfAct->setShortcut(QKeySequence::Global);
  //wrfAct->setStatusTip(tr("Try to activate 'wrf' application"));
  //connect(wrfAct, SIGNAL(triggered()), this, SLOT(wrf()));

    ufsAct = new QAction(tr("&UFS"), this);
  //ufsAct->setShortcut(QKeySequence::Global);
    ufsAct->setStatusTip(tr("Try to activate 'ufs' application"));
    connect(ufsAct, SIGNAL(triggered()), this, SLOT(ufs()));

  //mpasAct = new QAction(tr("&MPAS"), this);
  //mpasAct->setShortcut(QKeySequence::Global);
  //mpasAct->setStatusTip(tr("Try to activate 'mpas' application"));
  //connect(mpasAct, SIGNAL(triggered()), this, SLOT(mpas()));

  //camseAct = new QAction(tr("&CAMse"), this);
  //camseAct->setShortcut(QKeySequence::Global);
  //camseAct->setStatusTip(tr("Try to activate 'camse' application"));
  //connect(camseAct, SIGNAL(triggered()), this, SLOT(camse()));

  //popAct = new QAction(tr("&POP"), this);
  //popAct->setShortcut(QKeySequence::Global);
  //popAct->setStatusTip(tr("Try to activate 'pop' application"));
  //connect(popAct, SIGNAL(triggered()), this, SLOT(pop()));

#ifdef UseRADX
    radxAct = new QAction(tr("&RADX"), this);
  //radxAct->setShortcut(QKeySequence::Global);
    radxAct->setStatusTip(tr("Try to activate 'radx' application"));
    connect(radxAct, SIGNAL(triggered()), this, SLOT(radx()));
#endif

#if 0
    vtkAct = new QAction(tr("&VTK"), this);
  //vtkAct->setShortcut(QKeySequence::Global);
    vtkAct->setStatusTip(tr("Try to activate 'vtk' application"));
    connect(vtkAct, SIGNAL(triggered()), this, SLOT(vtk()));
#endif

    testAct = new QAction(tr("&TEST"), this);
  //testAct->setShortcut(QKeySequence::Global);
    testAct->setStatusTip(tr("Try to activate 'test' application"));
    connect(testAct, SIGNAL(triggered()), this, SLOT(test()));

    mpidemoAct = new QAction(tr("&MPIDEMO"), this);
  //mpidemoAct->setShortcut(QKeySequence::Global);
    mpidemoAct->setStatusTip(tr("Try to activate 'mpidemo' application"));
    connect(mpidemoAct, SIGNAL(triggered()), this, SLOT(mpidemo()));

  //hdfAct = new QAction(tr("&HDF"), this);
  //hdfAct->setShortcut(QKeySequence::Global);
  //hdfAct->setStatusTip(tr("Try to activate 'hdf' application"));
  //connect(hdfAct, SIGNAL(triggered()), this, SLOT(hdf()));

    animationAct = new QAction(tr("&Animation"), this);
  //animationAct->setShortcut(QKeySequence::Global);
    animationAct->setStatusTip(tr("Activate animation"));
    connect(animationAct, SIGNAL(triggered()), this, SLOT(animation_func()));

    inspectorAct = new QAction(tr("&Inspector"), this);
  //inspectorAct->setShortcut(QKeySequence::Global);
    inspectorAct->setStatusTip(tr("Activate inspector"));
    connect(inspectorAct, SIGNAL(triggered()), this, SLOT(inspector_func()));

    dataviewAct = new QAction(tr("&DataView"), this);
  //dataviewAct->setShortcut(QKeySequence::Global);
    dataviewAct->setStatusTip(tr("Activate dataview"));
    connect(dataviewAct, SIGNAL(triggered()), this, SLOT(dataview_func()));

    subsetAct = new QAction(tr("&Subset"), this);
  //subsetAct->setShortcut(QKeySequence::Global);
    subsetAct->setStatusTip(tr("Activate subset"));
    connect(subsetAct, SIGNAL(triggered()), this, SLOT(subset_func()));

    minmaxAct = new QAction(tr("&MinMax"), this);
  //minmaxAct->setShortcut(QKeySequence::Global);
    minmaxAct->setStatusTip(tr("Activate minmax"));
    connect(minmaxAct, SIGNAL(triggered()), this, SLOT(minmax_func()));

    vectorAct = new QAction(tr("&Vector"), this);
  //vectorAct->setShortcut(QKeySequence::Global);
    vectorAct->setStatusTip(tr("Activate vector"));
    connect(vectorAct, SIGNAL(triggered()), this, SLOT(vector_func()));

    trajectoryAct = new QAction(tr("&Trajectory"), this);
  //trajectoryAct->setShortcut(QKeySequence::Global);
    trajectoryAct->setStatusTip(tr("Activate trajectory"));
    connect(trajectoryAct, SIGNAL(triggered()), this, SLOT(trajectory_func()));

    clipAct = new QAction(tr("&Clip"), this);
  //clipAct->setShortcut(QKeySequence::Global);
    clipAct->setStatusTip(tr("Activate clip"));
    connect(clipAct, SIGNAL(triggered()), this, SLOT(clip_func()));

    licAct = new QAction(tr("&Lic"), this);
  //licAct->setShortcut(QKeySequence::Global);
    licAct->setStatusTip(tr("Activate lic"));
    connect(licAct, SIGNAL(triggered()), this, SLOT(lic_func()));

    isosurfaceAct = new QAction(tr("&IsoSurface"), this);
  //isosurfaceAct->setShortcut(QKeySequence::Global);
    isosurfaceAct->setStatusTip(tr("Activate isosurface"));
    connect(isosurfaceAct, SIGNAL(triggered()), this, SLOT(isosurface_func()));

    pixelAct = new QAction(tr("&Pixel"), this);
  //pixelAct->setShortcut(QKeySequence::Global);
    pixelAct->setStatusTip(tr("Activate pixel"));
    connect(pixelAct, SIGNAL(triggered()), this, SLOT(pixel_func()));

    lightAct = new QAction(tr("&Light"), this);
  //lightAct->setShortcut(QKeySequence::Global);
    lightAct->setStatusTip(tr("Activate light"));
    connect(lightAct, SIGNAL(triggered()), this, SLOT(light_func()));
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
  //fileMenu->addAction(newAct);

    fileMenu->addAction(openAct);
  //fileMenu->addAction(saveAct);
  //fileMenu->addAction(printAct);
  //fileMenu->addSeparator();

    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(undoAct);
    editMenu->addAction(redoAct);
    editMenu->addSeparator();

    appsMenu = menuBar()->addMenu(tr("&PlotTypes"));
    appsMenu->addAction(generalAct);
  //appsMenu->addAction(wrfAct);
    appsMenu->addAction(ufsAct);
  //appsMenu->addAction(mpasAct);
  //appsMenu->addAction(camseAct);
  //appsMenu->addAction(popAct);
  //appsMenu->addAction(hdfAct);
  //appsMenu->addAction(radxAct);
  //appsMenu->addAction(vtkAct);
    appsMenu->addAction(testAct);
    appsMenu->addAction(mpidemoAct);
    appsMenu->addSeparator();

    toolMenu = menuBar()->addMenu(tr("&Tools"));
    toolMenu->addAction(animationAct);
    toolMenu->addAction(lightAct);
    toolMenu->addAction(inspectorAct);
    toolMenu->addAction(licAct);
    toolMenu->addAction(vectorAct);
    toolMenu->addAction(trajectoryAct);
    toolMenu->addAction(clipAct);
    toolMenu->addAction(isosurfaceAct);
    toolMenu->addAction(pixelAct);
    toolMenu->addAction(dataviewAct);
    toolMenu->addAction(subsetAct);
    toolMenu->addAction(minmaxAct);
    toolMenu->addSeparator();

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);
}

void MainWindow::animation_func()
{
    int x, y;
    
    x = 0;
    y = screenHeight;

    animationWidget = new AnimationWidget();
    animationWidget->set_translator(translator);
    animationWidget->setup();
    animationWidget->move(x, y);
    animationWidget->show();
}

void MainWindow::light_func()
{
    int x, y;

    ++numberOfWidget;

    x = (int) (0.1 * (double) numberOfWidget * screenWidth);
    while(x >= screenWidth)
        x -= screenWidth;

    y = 0;

    lightWidget = new LightWidget();
    lightWidget->set_light(light);
    lightWidget->set_translator(translator);
    lightWidget->setup();
    lightWidget->move(x, y);
    lightWidget->show();
}

void MainWindow::inspector_func()
{
    int x, y;

    x = (int) (0.1 * (double) numberOfWidget * screenWidth);
    while(x >= screenWidth)
        x -= screenWidth;

    y = 0;

    inspectorWidget = new InspectorWidget();
    inspectorWidget->set_locator(locator);
    inspectorWidget->set_nvoptions(nvoptions);
    inspectorWidget->set_translator(translator);
    inspectorWidget->setup();

    switch(nvoptions->get_model())
    {
        case POP:
        case MPAS:
        case CAMSE:
             inspectorWidget->set_lon(360);
             inspectorWidget->set_lat(180);
             break;
        default:
             break;
    }

    inspectorWidget->move(x, y);
    inspectorWidget->show();
}

void MainWindow::lic_func()
{
    int x, y;

    ++numberOfWidget;

    x = (int) (0.1 * (double) numberOfWidget * screenWidth);
    while(x >= screenWidth)
        x -= screenWidth;

    y = 0;

    licWidget = new LICWidget();
    licWidget->set_translator(translator);
    licWidget->setup();
    licWidget->move(x, y);
    licWidget->show();
}

void MainWindow::vector_func()
{
    int x, y;

    ++numberOfWidget;

    x = (int) (0.1 * (double) numberOfWidget * screenWidth);
    while(x >= screenWidth)
        x -= screenWidth;

    y = 0;

    vectorWidget = new VectorWidget();
    vectorWidget->set_nvoptions(nvoptions);
    vectorWidget->set_translator(translator);
    vectorWidget->setup();
    vectorWidget->move(x, y);
    vectorWidget->show();
}

void MainWindow::trajectory_func()
{
    int x, y;

    ++numberOfWidget;

    x = (int) (0.1 * (double) numberOfWidget * screenWidth);
    while(x >= screenWidth)
        x -= screenWidth;

    y = 0;

    trajectoryWidget = new TrajectoryWidget();
    trajectoryWidget->set_nvoptions(nvoptions);
    trajectoryWidget->set_translator(translator);
    trajectoryWidget->setup();
    trajectoryWidget->move(x, y);
    trajectoryWidget->show();
}

void MainWindow::clip_func()
{
    int x, y;

    ++numberOfWidget;

    x = (int) (0.1 * (double) numberOfWidget * screenWidth);
    while(x >= screenWidth)
        x -= screenWidth;

    y = 0;

    clipWidget = new ClipWidget();
    clipWidget->set_nvoptions(nvoptions);
    clipWidget->setup();
    clipWidget->move(x, y);
    clipWidget->show();
}

void MainWindow::isosurface_func()
{
    int x, y;

    ++numberOfWidget;

    x = (int) (0.1 * (double) numberOfWidget * screenWidth);
    while(x >= screenWidth)
        x -= screenWidth;

    y = 0;

    isosurfaceWidget = new IsoSurfaceWidget();
    isosurfaceWidget->set_translator(translator);
    isosurfaceWidget->setup();
    isosurfaceWidget->move(x, y);
    isosurfaceWidget->show();
}

void MainWindow::pixel_func()
{
    int x, y;

    ++numberOfWidget;

    x = (int) (0.1 * (double) numberOfWidget * screenWidth);
    while(x >= screenWidth)
        x -= screenWidth;

    y = 0;

    pixelWidget = new PixelWidget();
    pixelWidget->set_nvoptions(nvoptions);
    pixelWidget->set_translator(translator);
    pixelWidget->setup();
    pixelWidget->move(x, y);
    pixelWidget->show();
}

void MainWindow::dataview_func()
{
    int x, y;

    ++numberOfWidget;

    x = (int) (0.1 * (double) numberOfWidget * screenWidth);
    while(x >= screenWidth)
        x -= screenWidth;

    y = 0;

    dataviewWidget = new DataViewWidget();
  //dataviewWidget->set_nvoptions(nvoptions);
    dataviewWidget->set_translator(translator);
    dataviewWidget->setup();
    dataviewWidget->move(x, y);
    dataviewWidget->show();
}

void MainWindow::subset_func()
{
    int x, y;

    ++numberOfWidget;

    x = (int) (0.1 * (double) numberOfWidget * screenWidth);
    while(x >= screenWidth)
        x -= screenWidth;

    y = 0;

    subsetWidget = new SubsetWidget();
    subsetWidget->set_nvoptions(nvoptions);
    subsetWidget->set_translator(translator);
    subsetWidget->setup();
    subsetWidget->move(x, y);
    subsetWidget->show();
}

void MainWindow::minmax_func()
{
    int x, y;

    ++numberOfWidget;

    x = (int) (0.1 * (double) numberOfWidget * screenWidth);
    while(x >= screenWidth)
        x -= screenWidth;

    y = 0;

    minmaxWidget = new MinMaxWidget();
    minmaxWidget->set_translator(translator);
    minmaxWidget->setup();
    minmaxWidget->move(x, y);
    minmaxWidget->show();
}

