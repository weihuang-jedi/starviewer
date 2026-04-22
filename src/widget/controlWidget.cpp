#include "controlWidget.h"
#include "colorTable.h"

#if 0
//If you just want to change the row height (instead of changing font size) create a new delegate class:

class RowHeightDelegate : public QItemDelegate
{
    Q_OBJECT
    public:
        QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
        {
            return QSize(1, 40); // the row height is now 40
        }
};

//And set it to your combobox:

ui->comboBox->setItemDelegate(new RowHeightDelegate());
#endif

ControlWidget::ControlWidget(QWidget* parent) : QWidget(parent)
{
  //Set window title
  //setWidgetTitle(tr("NV"));

  //Set layout of child widgets
    layout = new QGridLayout;
}

ControlWidget::~ControlWidget()
{
    delete layout;
}

void ControlWidget::setup()
{
    _setup_buttons();
    _setup_colormap();
    _setup_variables();
    _setup_sliderNspin();
    _setup_message();
}

void ControlWidget::_setup_buttons()
{
    proj_cb    = new QCheckBox("Perspective");
    axis_cb    = new QCheckBox("Axis");
  //ncl_cb     = new QCheckBox("NCL");

    bump_cb    = new QCheckBox("Bump");
    colorbar_cb= new QCheckBox("ColorBar");
    sphere_cb  = new QCheckBox("Sphere");
    flat_cb    = new QCheckBox("Flat");
    bgBW_cb    = new QCheckBox("BG-BW");
    onmap_cb   = new QCheckBox("OnMap");
  //title_cb   = new QCheckBox("Title");
    coastline_cb = new QCheckBox("CoastLine");

    x2_cb  = new QCheckBox("X2");
    y2_cb  = new QCheckBox("Y2");
    z2_cb  = new QCheckBox("Z2");
    t2_cb  = new QCheckBox("T2");

  //shader_cb     = new QCheckBox("Shader");
  //volume_cb     = new QCheckBox("Volume");

    proj_cb->setChecked(true);
    axis_cb->setChecked(false);
    axis_cb->setCheckState(Qt::Unchecked);
  //ncl_cb->setChecked(false);
  //ncl_cb->setCheckState(Qt::Unchecked);
    bump_cb->setChecked(false);
    bump_cb->setCheckState(Qt::Unchecked);
    colorbar_cb->setChecked(false);
    colorbar_cb->setCheckState(Qt::Unchecked);

    bgBW_cb->setChecked(false);
    bgBW_cb->setCheckState(Qt::Unchecked);
    onmap_cb->setChecked(false);
    onmap_cb->setCheckState(Qt::Unchecked);
  //title_cb->setChecked(true);
    coastline_cb->setChecked(false);

  //title_cb->setChecked(false);
  //title_cb->setCheckState(Qt::Unchecked);

    x2_cb->setChecked(false);
    x2_cb->setCheckState(Qt::Unchecked);
    y2_cb->setChecked(false);
    y2_cb->setCheckState(Qt::Unchecked);
    z2_cb->setChecked(false);
    z2_cb->setCheckState(Qt::Unchecked);
    t2_cb->setChecked(false);
    t2_cb->setCheckState(Qt::Unchecked);

  //shader_cb->setChecked(false);
  //shader_cb->setCheckState(Qt::Unchecked);
  //volume_cb->setChecked(false);
  //volume_cb->setCheckState(Qt::Unchecked);

    connect(proj_cb,    SIGNAL(stateChanged(int)), translator, SLOT(selectProjection(int)));
    connect(axis_cb,    SIGNAL(stateChanged(int)), translator, SLOT(selectAxis(int)));
    connect(coastline_cb,   SIGNAL(stateChanged(int)), translator, SLOT(selectCoastLine(int)));
    connect(bump_cb,    SIGNAL(stateChanged(int)), translator, SLOT(selectBump(int)));
    connect(colorbar_cb,SIGNAL(stateChanged(int)), translator, SLOT(selectColorBar(int)));
    connect(flat_cb,    SIGNAL(stateChanged(int)), translator, SLOT(selectFlat(int)));
    connect(bgBW_cb,    SIGNAL(stateChanged(int)), translator, SLOT(selectBackground(int)));
    connect(onmap_cb,   SIGNAL(stateChanged(int)), translator, SLOT(selectOnMap(int)));
    connect(sphere_cb,  SIGNAL(stateChanged(int)), translator, SLOT(selectSphere(int)));
  //connect(title_cb,   SIGNAL(stateChanged(int)), translator, SLOT(selectTitle(int)));
  //connect(ncl_cb,     SIGNAL(stateChanged(int)), translator, SLOT(selectNCL(int)));
  //connect(surface_cb, SIGNAL(stateChanged(int)), translator, SLOT(selectSurface(int)));
  //connect(shader_cb,  SIGNAL(stateChanged(int)), translator, SLOT(selectShader(int)));
  //connect(volume_cb,  SIGNAL(stateChanged(int)), translator, SLOT(selectVolume(int)));

    connect(x2_cb, SIGNAL(stateChanged(int)), translator, SLOT(selectX2(int)));
    connect(y2_cb, SIGNAL(stateChanged(int)), translator, SLOT(selectY2(int)));
    connect(z2_cb, SIGNAL(stateChanged(int)), translator, SLOT(selectZ2(int)));
    connect(t2_cb, SIGNAL(stateChanged(int)), translator, SLOT(selectT2(int)));

  //Group checkbox parameters
    QGroupBox*   button_box = new QGroupBox("Buttons");
    QGridLayout* button_lay = new QGridLayout;

    button_lay->addWidget(proj_cb,    0, 0);
    button_lay->addWidget(axis_cb,    0, 1);
    button_lay->addWidget(coastline_cb,0,2);

    button_lay->addWidget(bump_cb,    1, 0);
    button_lay->addWidget(colorbar_cb,1, 1);
    button_lay->addWidget(flat_cb,    1, 2);

    button_lay->addWidget(bgBW_cb,    2, 0);
    button_lay->addWidget(onmap_cb,   2, 1);
    button_lay->addWidget(sphere_cb,  2, 2);

  //button_lay->addWidget(title_cb,   3, 0);
  //button_lay->addWidget(ncl_cb,     3, 1);

    button_box->setLayout(button_lay);
    layout->addWidget(button_box,0,0);
}

void ControlWidget::_setup_colormap()
{
    int n;

  //Group color map names box, and save image
    QGroupBox* cmn_box = new QGroupBox("ColorMapNames");
    QGridLayout* cmn_lay = new QGridLayout;

  //Combo box for color map names
    QComboBox *colorMapName = new QComboBox;

    vector<string> clist = colorTable->get_clist();
  //cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__
  //     << ", file: <" << __FILE__ << ">" << endl;
  //cout << "Number of color maps: " << colorTable->get_nc() << endl;
    for(n = 0; n < colorTable->get_nc(); ++n)
    {
        colorMapName->addItem(clist[n].c_str());
      //cout << "color map No. " << n << ": " << clist[n] << endl;
    }

    connect(colorMapName, SIGNAL(currentIndexChanged(const QString&)),
            translator,   SLOT(selectColorMap(const QString&)));

    save_image = new QPushButton("SaveImage");
    save_image->setChecked(false);

    connect(save_image, SIGNAL(clicked()), translator, SLOT(selectSaveImage()));

    cmn_lay->addWidget(colorMapName, 0, 0);
    cmn_box->setLayout(cmn_lay);

    cmn_lay->addWidget(save_image, 0, 1);
    cmn_box->setLayout(cmn_lay);

    layout->addWidget(cmn_box,1,0);
}

void ControlWidget::_setup_variables()
{
    int n;

    QFont font;
    font.setPointSize(font.pointSize() + 5);
  //font.setPointSize(font.pointSize() + 10);

  //Group variable box
    QGroupBox* var_box = new QGroupBox("Variables");
    QGridLayout* var_lay = new QGridLayout;

  //Combo box for variable names
    QComboBox* box2dvar = new QComboBox;
    QComboBox* box3dvar = new QComboBox;

    box2dvar->setFont(font);
    box3dvar->setFont(font);

  //widget for info
    QLabel* var2d = new QLabel("2D Vars");
    QLabel* var3d = new QLabel("3D Vars");

    string *names2dvar = translator->get_ndvNames(2);
  //cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__
  //     << ", file: <" << __FILE__ << ">" << endl;
  //cout << "2d var size: " << translator->get_ndv(2) << endl;
    if(2 > translator->get_ndv(2))
        box2dvar->addItem("dummy");

    for(n = 0; n < translator->get_ndv(2); ++n)
    {
      //cout << "2d name " << n << ": " << names2dvar[n] << endl;
        box2dvar->addItem(names2dvar[n].c_str());
    }

    string *names3dvar = translator->get_ndvNames(3);
  //cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__
  //     << ", file: <" << __FILE__ << ">" << endl;
  //cout << "3d var size: " << translator->get_ndv(3) << endl;
    if(2 > translator->get_ndv(3))
        box3dvar->addItem("dummy");

    for(n = 0; n < translator->get_ndv(3); ++n)
    {
      //cout << "3d name " << n << ": " << names3dvar[n] << endl;
        box3dvar->addItem(names3dvar[n].c_str());
    }

    connect(box2dvar, SIGNAL(currentIndexChanged(const QString&)),
            translator,    SLOT(select2dVar(const QString&)));
    connect(box3dvar, SIGNAL(currentIndexChanged(const QString&)),
            translator,    SLOT(select3dVar(const QString&)));

    var_lay->addWidget(var2d, 0, 0);
    var_lay->addWidget(var3d, 0, 1);

    var_lay->addWidget(box2dvar, 1, 0);
    var_lay->addWidget(box3dvar, 1, 1);

    var_box->setLayout(var_lay);
    layout->addWidget(var_box,2,0);
}

void ControlWidget::_setup_sliderNspin()
{
  //Group slider parameters
    QGroupBox*   slider_box = new QGroupBox("Cross-Sections:");
    QGridLayout* slider_lay = new QGridLayout;

    sliderNspinX = new SliderNSpin(Qt::Horizontal, tr("X"),  this);
    sliderNspinY = new SliderNSpin(Qt::Horizontal, tr("Y"),  this);
    sliderNspinZ = new SliderNSpin(Qt::Horizontal, tr("Z"),  this);
    sliderNspinT = new SliderNSpin(Qt::Horizontal, tr("T"),  this);

    sliderNspinX->set_max(translator->get_nx());
    sliderNspinY->set_max(translator->get_ny());
    sliderNspinZ->set_max(translator->get_nz());
    sliderNspinT->set_max(translator->get_nt());

    xslider = sliderNspinX->get_slider();
    yslider = sliderNspinY->get_slider();
    zslider = sliderNspinZ->get_slider();
    tslider = sliderNspinT->get_slider();

    xspinBox = sliderNspinX->get_spinBox();
    yspinBox = sliderNspinY->get_spinBox();
    zspinBox = sliderNspinZ->get_spinBox();
    tspinBox = sliderNspinT->get_spinBox();

    translator->set_sliderX(sliderNspinX);
    translator->set_sliderY(sliderNspinY);
    translator->set_sliderZ(sliderNspinZ);
    translator->set_sliderT(sliderNspinT);

    connect(xslider,  SIGNAL(valueChanged(int)), translator, SLOT(setValueX(int)));
    connect(yslider,  SIGNAL(valueChanged(int)), translator, SLOT(setValueY(int)));
    connect(zslider,  SIGNAL(valueChanged(int)), translator, SLOT(setValueZ(int)));
    connect(tslider,  SIGNAL(valueChanged(int)), translator, SLOT(setValueT(int)));

    connect(xspinBox, SIGNAL(valueChanged(int)), translator, SLOT(setValueX(int)));
    connect(yspinBox, SIGNAL(valueChanged(int)), translator, SLOT(setValueY(int)));
    connect(zspinBox, SIGNAL(valueChanged(int)), translator, SLOT(setValueZ(int)));
    connect(tspinBox, SIGNAL(valueChanged(int)), translator, SLOT(setValueT(int)));

    slider_box->setLayout(slider_lay);

    slider_lay->addWidget(x2_cb, 0, 0);
    sliderNspinX->add2layout(slider_lay, 0, 1);

    slider_lay->addWidget(y2_cb, 1, 0);
    sliderNspinY->add2layout(slider_lay, 1, 1);

    slider_lay->addWidget(z2_cb, 2, 0);
    sliderNspinZ->add2layout(slider_lay, 2, 1);

    slider_lay->addWidget(t2_cb, 3, 0);
    sliderNspinT->add2layout(slider_lay, 3, 1);

    layout->addWidget(slider_box,3,0);
}

void ControlWidget::_setup_message()
{
  //Group Reset parameters
    QGroupBox* rstbox = new QGroupBox("Reset");
    QGridLayout* rstlay = new QGridLayout;

  //Pushbutton to reset view angle
    reset = new QPushButton("Reset");

  //Connect translator signals to display widgets
    connect(reset , SIGNAL(clicked(void)), translator, SLOT(reset(void)));

    rstlay->addWidget(reset,0,0);

    rstbox->setLayout(rstlay);
    layout->addWidget(rstbox,4,0);

  //-------------------------------------------------------------------------
  //Group Variable Info
    QGroupBox* dspbox = new QGroupBox("Variable Info");
    QGridLayout* dsplay = new QGridLayout;

  //widget for info
    info = new QLabel();

  //Connect translator signals to display widgets
    connect(translator, SIGNAL(info(QString)), info, SLOT(setText(QString)));

    dsplay->addWidget(info, 0,0);

    dspbox->setLayout(dsplay);
    layout->addWidget(dspbox,5,0);

  //-------------------------------------------------------------------------
    layout->setColumnMinimumWidth(0,2);
    layout->setColumnStretch(0,3);

    layout->setRowStretch(0,2);

  //Overall layout
    setLayout(layout);
}

#if 0
void ControlWidget::selectNCL()
{
    ncl_cb->setChecked(true);
    translator->selectNCL(2);
}
#endif

#include "controlWidget.moc"
