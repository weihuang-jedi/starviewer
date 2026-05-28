#include "mainwindow.h"

using namespace std;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow *mainwindow;

    bool hasYAMLfile = false;
    string yamlfile = "config.yaml";
    string tmpstr;
    int n = 1;

    QSurfaceFormat format;
    format.setProfile(QSurfaceFormat::CompatibilityProfile);
    QSurfaceFormat::setDefaultFormat(format);

    while(n < argc)
    {
        tmpstr = argv[n];

        transform(tmpstr.begin(), tmpstr.end(), tmpstr.begin(), ::tolower);

        // cout << "\targv[" << n << "]: <" << argv[n] << ">" << endl;
        // cout << "\ttmpstr: <" << tmpstr << ">" << endl;

	if(0 == tmpstr.compare("-yaml"))
        {
            ++n;
            yamlfile = argv[n];
	    hasYAMLfile = true;
            // cout << "\tyamlfile: <" << yamlfile << ">" << endl;
        }
        else
        {
            cout << "Unprocessed argv[" << n << "]: " << argv[n] << endl;
        }

        ++n;
    }

    if (hasYAMLfile)
    {
        // cout << "\tyamlfile: <" << yamlfile << ">" << endl;
        mainwindow = new MainWindow(yamlfile);
    }
    else
    {
	cout << "Usage: " << argv[0] << " -yaml yaml-file" << endl;
        return 0;
    }

    mainwindow->show();

    app.exec();

    return 0;
}

