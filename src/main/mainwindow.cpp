#include "mpidemoparser.h"
#include "ufsparser.h"
#include "ufsincrparser.h"
#include "mainwindow.h"

MainWindow::MainWindow(string yamlfile)
{
    nvoptions = new NVOptions();
    QDesktopWidget *desktop = QApplication::desktop();

    ModelType userConfig = ModelType::NOMODEL;

    screenWidth = desktop->width();
    screenHeight = desktop->height(); 

    YAMLHandler *yamlHandler = new YAMLHandler(yamlfile.c_str());
    yamlHandler->read_yaml();

    nvoptions = new NVOptions();

    string tmpstr = yamlHandler->get_model();
    // cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
    // cout << "\ttmpstr: " << tmpstr << endl;
    if(0 == tmpstr.compare("ufs"))
    {
        nvoptions->set_model(UFS);
        userConfig = ModelType::UFS;
    }
    else if(0 == tmpstr.compare("ufsincr"))
    {
        nvoptions->set_model(UFSINCR);
        userConfig = ModelType::UFSINCR;
    }
    else if(0 == tmpstr.compare("mpidemo"))
    {
        nvoptions->set_model(MPIDEMO);
        userConfig = ModelType::MPIDEMO;
    }
    else if(0 == tmpstr.compare("pop"))
    {
        nvoptions->set_model(POP);
    }
    else if(0 == tmpstr.compare("mpas"))
    {
        nvoptions->set_model(MPAS);
        userConfig = ModelType::MPAS;
    }
    else if(0 == tmpstr.compare("wrf"))
    {
        nvoptions->set_model(WRF);
    }

    // cout << "\tfile: " << __FILE__ << ", line: " << __LINE__ << endl;
    // cout << "\tnvoptions->get_model(): " << nvoptions->get_model() << endl;

    nInstance = 0;
    numberOfWidget = 0;

    createActions();
    createMenus();

    setMinimumSize(480, 320);
    resize(960, 640);

    light = new Light();

    locator = new Locator();

    colorTable = new ColorTable();

    controlPanel = new ControlWidget();
    display = new DisplayWidget();

    // cout << "\tfile: " << __FILE__ << ", line: " << __LINE__ << endl;

    // Use the factory to generate the object
    myParser = ModelParserFactory::createParser(userConfig);

    if (myParser) {
        // cout << "\tfile: " << __FILE__ << ", line: " << __LINE__ << endl;
        // Polymorphism handles execution automatically
        myParser->parse(yamlHandler, nvoptions, colorTable,
			controlPanel, locator, light);
        // cout << "\tfile: " << __FILE__ << ", line: " << __LINE__ << endl;
        translator = myParser->get_translator();
        // cout << "\ttranslator: " << translator << endl;
        // cout << "\tfile: " << __FILE__ << ", line: " << __LINE__ << endl;
    } else {
        cerr << "Unknown Model." << endl;
    }

    // cout << "\tfile: " << __FILE__ << ", line: " << __LINE__ << endl;
    // cout << "\ttranslator: " << translator << endl;

    // unordered_map<string, function<unique_ptr<ModelParser>()>> registry;

    // cout << "\t\tfile: " << __FILE__ << ", line: " << __LINE__ << endl;

    _setup();

    // cout << "Leave MainWindow: file: " << __FILE__ << ", line: " << __LINE__ << endl;
}

MainWindow::~MainWindow()
{
  //cout << "\tEnter function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
    delete light;
    delete locator;
    delete colorTable;
    delete controlPanel;
    delete display;
    delete nvoptions;
  //cout << "\tLeave function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
}

void MainWindow::_setup()
{
    // cout << "\nEnter function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
       cout << "\tnvoptions->get_model(): " << nvoptions->get_model() << endl;
    // cout << "\tMPIDEMO: " << MPIDEMO << endl;
    // cout << "\tUFS: " << UFS << endl;
       cout << "\tUFSINCR: " << UFSINCR << endl;
    switch(nvoptions->get_model())
    {
        case UFS:
            // cout << "\tfunction: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
            setWindowTitle(tr("UFS MODEL"));
            ufs();
            break;
        case UFSINCR:
            // cout << "\tfunction: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
            setWindowTitle(tr("UFS MODEL"));
            ufsincr();
            break;
        case MPIDEMO:
            // cout << "\tfunction: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
            setWindowTitle(tr("NV to demo MPI"));
            mpidemo();
            break;
      //case MPAS:
      //    mpas();
      //    break;
      //case POP:
      //    pop();
      //    break;
      //case WRF:
      //    wrf();
      //    break;
        default:
            mpidemo();
            break;
    }

    // cout << "Leave function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
}

void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
    cout << "\nEnter function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
    QMenu menu(this);

  //menu.addAction(wrfAct);
    menu.addAction(ufsAct);
    menu.addAction(ufsincrAct);
  //menu.addAction(mpasAct);
  //menu.addAction(popAct);
    menu.addAction(mpidemoAct);

    menu.exec(event->globalPos());
    // cout << "Leave function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
}

void MainWindow::newFile()
{
}

void MainWindow::open()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
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
    controlPanel->move(0, 0);
    controlPanel->show();
}

void MainWindow::_setup_display()
{
    int x, y;
    // cout << "\nEnter function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;

    x = screenWidth / 2;
    y = screenHeight / 2;

    setCentralWidget(display);
    // cout << "\tfunction: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;

    display->set_translator(translator);
    // cout << "\tfunction: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
    display->setup();
    // cout << "\tfunction: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
    display->move(x, y);
    // cout << "\tfunction: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
    display->show();
    // cout << "Leave function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
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

void MainWindow::mpidemo()
{
    setWindowTitle(tr("NV to demo MPI"));

    _setup_controlPanel();

    _setup_display();
}

void MainWindow::ufs()
{
    setWindowTitle(tr("NV for UFS"));

    _setup_controlPanel();

    _setup_display();
}

void MainWindow::ufsincr()
{
    setWindowTitle(tr("NV for UFS Tiled Increment"));

    _setup_controlPanel();

    _setup_display();
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("About Menu"),
            tr("The <b>Menu</b> example shows how to create "
               "menu-bar menus and context menus."));
}

void MainWindow::createActions()
{
    // cout << "\nEnter function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
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

  //wrfAct = new QAction(tr("&WRF"), this);
  //wrfAct->setShortcut(QKeySequence::Global);
  //wrfAct->setStatusTip(tr("Try to activate 'wrf' application"));
  //connect(wrfAct, SIGNAL(triggered()), this, SLOT(wrf()));

    ufsAct = new QAction(tr("&UFS"), this);
  //ufsAct->setShortcut(QKeySequence::Global);
    ufsAct->setStatusTip(tr("Try to activate 'ufs' application"));
    connect(ufsAct, SIGNAL(triggered()), this, SLOT(ufs()));

    ufsincrAct = new QAction(tr("&UFS"), this);
  //ufsincrAct->setShortcut(QKeySequence::Global);
    ufsincrAct->setStatusTip(tr("Try to activate 'ufsincr' application"));
    connect(ufsincrAct, SIGNAL(triggered()), this, SLOT(ufsincr()));

  //popAct = new QAction(tr("&POP"), this);
  //popAct->setShortcut(QKeySequence::Global);
  //popAct->setStatusTip(tr("Try to activate 'pop' application"));
  //connect(popAct, SIGNAL(triggered()), this, SLOT(pop()));

    // cout << "\t" << __PRETTY_FUNCTION__ << ", in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
    mpidemoAct = new QAction(tr("&MPIDEMO"), this);
  //mpidemoAct->setShortcut(QKeySequence::Global);
    mpidemoAct->setStatusTip(tr("Try to activate 'mpidemo' application"));
    connect(mpidemoAct, SIGNAL(triggered()), this, SLOT(mpidemo()));
    // cout << "\t" << __PRETTY_FUNCTION__ << ", in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;

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
    // cout << "Leave function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
}

void MainWindow::createMenus()
{
    // cout << "\nEnter function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
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
  //appsMenu->addAction(wrfAct);
    appsMenu->addAction(ufsAct);
    appsMenu->addAction(ufsincrAct);
  //appsMenu->addAction(mpasAct);
  //appsMenu->addAction(popAct);
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
    // cout << "Leave function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
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

