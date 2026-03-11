//$Id: spreadsheet.cpp 4865 2014-01-01 15:49:23Z starviewer $

#include "spreadsheet.h"

//Constructor
SpreadSheet::SpreadSheet(QWidget* parent) : QWidget(parent)
{
    evaluator = new Evaluator();
    level = 0;
    setContextMenuPolicy(Qt::ActionsContextMenu);
}

SpreadSheet::~SpreadSheet()
{
    delete evaluator;
}

void SpreadSheet::set_geometry(Geometry* gm)
{
    int i, j, j1;
    QString label;
    QString lonLabel;
    QString latLabel;

    geometry = gm;

    nx = geometry->get_nx();
    ny = geometry->get_ny();
    nz = geometry->get_nz();

    table = new QTableWidget(ny, nx, this);
    table->resize(900, 600);

  //QTableView::horizontalHeader()
  //QHeaderView::setResizeMode()

    if(geometry->get_has1dLon() && geometry->get_has1dLat())
    {
        lon = geometry->get_lon();
        lat = geometry->get_lat();
        lev = geometry->get_lev();

      //label = "Lat\\Lon";
      //table->setHorizontalHeaderItem(0, new QTableWidgetItem(label));

        for(i = 0; i < nx; ++i)
        {
            lonLabel = QString::number(lon[i]);
            table->setHorizontalHeaderItem(i+1, new QTableWidgetItem(lonLabel));
        }

        for(j = 0; j < ny; ++j)
        {
            j1 = ny - 1 - j;
            latLabel = QString::number(lat[j1]);
            table->setVerticalHeaderItem(j+1, new QTableWidgetItem(latLabel));
        }
    }
    else
    {
        label = "Index";
        table->setHorizontalHeaderItem(0, new QTableWidgetItem(label));

        for(i = 0; i < nx; ++i)
        {
            lonLabel = QString::number(i);
            table->setHorizontalHeaderItem(i, new QTableWidgetItem(lonLabel));
        }

        for(j = 0; j < ny; ++j)
        {
            j1 = ny - 1 - j;
            latLabel = QString::number(j1);
            table->setVerticalHeaderItem(j, new QTableWidgetItem(latLabel));
        }
    }

    table->setItemPrototype(table->item(nx, ny));

  //table->setItemDelegate(new SpreadSheetDelegate());
}

void SpreadSheet::setup(string vn, float *var)
{
    int i, j, j1, n;
    QString label;
    size_t gridsize = 0;

    cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    cout << "\tvariable name: " << vn << endl;

    name = vn;
    _var = var;

    nx = geometry->get_nx();
    ny = geometry->get_ny();
    nz = geometry->get_nz();

    gridsize = nx * ny * nz;

    evaluator->set_FillValue(fillValue, hasFillValue);
    evaluator->set_value(gridsize, _var);

    hasFillValue = geometry->get_hasFillValue();
    if(hasFillValue)
    {
        fillValue = geometry->get_fillValue();
    }

    vMinimum = evaluator->get_min();
    vMaximum = evaluator->get_max();

    if(geometry->get_has1dLon() && geometry->get_has1dLat())
    {
        for(j = 0; j < ny; ++j)
        {
            j1 = ny - 1 - j;
            n = (level * ny + j1) * nx;
            for(i = 0; i < nx; ++i)
            {
                label = QString::number(_var[n + i]);
                table->setItem(j, i, new QTableWidgetItem(label));
            }
        }
    }
    else
    {
        for(j = 0; j < ny; ++j)
        {
            j1 = ny - 1 - j;
            n = (level * ny + j1) * nx;
            for(i = 0; i < nx; ++i)
            {
                label = QString::number(_var[n + i]);
                table->setItem(j, i, new QTableWidgetItem(label));
            }
        }
    }
}

void SpreadSheet::clear()
{
    foreach (QTableWidgetItem *i, table->selectedItems())
        i->setText("");
}

void SpreadSheet::resize(int width, int height)
{
    cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    cout << "\twidth = " << width << ", height = " << height << endl;

    widgetWidth = width;
    widgetHeight = height;

  //table->resizeColumnsToContents();
  //table->resizeRowsToContents();
  //table->resize(widgetWidth, widgetHeight);
}

