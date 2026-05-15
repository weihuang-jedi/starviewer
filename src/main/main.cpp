#include "mainwindow.h"

using namespace std;

bool isTextFile(const char* flnm)
{
    ifstream f(flnm);
    int c;

    while((c = f.get()) != EOF && c <= 127) 
        ;

#if 1
    return (c == EOF);
#else
    if(c == EOF)
    {
        printf("file: <%s> is a text file\n", argv[1]);
        return true;
    }
    else
    {
        printf("file: <%s> is a not a plan text file\n", argv[1]);
        return false;
    }
#endif
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    int n;
    ifstream in;
    string default_filename = "$NV_DATA/uvt.nc";
    string filename = "$NV_DATA/uvt.nc";
    string tmpstr;
    string camse_mappingFilename = "unknown";

    string yamlfile = "config.yaml";

    bool isList = false;
    bool hasYAMLfile = false;

    NVOptions* nvoptions = new NVOptions();

    MainWindow *mainwindow;
    QSurfaceFormat format;
    format.setProfile(QSurfaceFormat::CoreProfile);

    n = 1;
    while(n < argc)
    {
        tmpstr = argv[n];

        transform(tmpstr.begin(), tmpstr.end(), tmpstr.begin(), ::tolower);

      //cout << "\targv[" << n << "]: <" << argv[n] << ">" << endl;
      //cout << "\ttmpstr: <" << tmpstr << ">" << endl;

#if 0
        if(0 == tmpstr.compare("-l"))
        {
            isList = true;
            ++n;
            continue;
        }

        if(0 == tmpstr.compare("-f"))
        {
            isList = false;
            ++n;
            continue;
        }
#endif
        if(0 == tmpstr.compare("-camfv"))
            nvoptions->set_model(CAMFV);
        else if(0 == tmpstr.compare("-camse"))
            nvoptions->set_model(CAMSE);
      //else if(0 == tmpstr.compare("-wrf"))
      //    nvoptions->set_model(WRF);
        else if(0 == tmpstr.compare("-ufs"))
            nvoptions->set_model(UFS);
      //else if(0 == tmpstr.compare("-mpas"))
      //    nvoptions->set_model(MPAS);
      //else if(0 == tmpstr.compare("-pop"))
      //    nvoptions->set_model(POP);
      //else if(0 == tmpstr.compare("-radx"))
      //    nvoptions->set_model(RADX);
      //else if(0 == tmpstr.compare("-vtk"))
      //    nvoptions->set_model(VTK);
        else if(0 == tmpstr.compare("-test"))
            nvoptions->set_model(TEST);
        else if(0 == tmpstr.compare("-mpi"))
            nvoptions->set_model(MPIDEMO);
      //else if(0 == tmpstr.compare("-hdf"))
      //    nvoptions->set_model(HDF);
        else if(0 == tmpstr.compare("-subset"))
            nvoptions->set_cb(NV_MPAS_SUBSET, true);
        else if(0 == tmpstr.compare("-mappingfile"))
        {
            ++n;
            camse_mappingFilename = argv[n];
            ++n;
            continue;
        }
	else if(0 == tmpstr.compare("-yaml"))
        {
            ++n;
            yamlfile = argv[n];
	    hasYAMLfile = true;
        }
        else
        {
            filename = argv[n];
        }

        ++n;
    }

    in.open(filename.c_str(), ios::in | ios::binary);
    if(in.is_open())
    {
        in.close();
    }
    else
    {
        filename = default_filename;
    }

  //initializeNcl();

    isList = isTextFile(filename.c_str());

  //cout << "\tfile: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\tfilename: <" << filename << ">, isList = " << isList << endl;

    if (hasYAMLfile) 
        mainwindow = new MainWindow(yamlfile);
    else
        mainwindow = new MainWindow(filename, isList,
                                    camse_mappingFilename,
                                    nvoptions);

    mainwindow->show();

    app.exec();

  //finalizeNcl();

  //delete nvoptions;

    return 0;
}

