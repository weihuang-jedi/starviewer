#include "eagleparser.h"

void Eagle::parse(YAMLHandler *yamlHandler, NVOptions* nvoptions,
                      ColorTable *colorTable, ControlWidget* controlPanel,
                      Locator* locator, Light* light)
{
    vector<string> datafiles = yamlHandler->get_datafiles();
    bool isList = false;
    string flnm = datafiles[0];
    string mfnm = "unknown";

    // cout << "\nEnter function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
    // cout << "Parsing Eagle lon-lat grid..." << endl;
    // cout << "flnm: " << flnm << endl;
    // cout << "nvoptions->get_model(): " << nvoptions->get_model() << endl;

    eagle_translator = new EAGLETranslator(colorTable, nvoptions,
                                       flnm, isList, mfnm);

    // if(locator)
    //     cout << "locator->on(): " << locator->on() << endl;
    // else
    //     cout << "locator is null." << endl;

    // if(light)
    //     cout << "light->on(): " << light->on() << endl;
    // else
    //     cout << "light is null." << endl;

    if(ufs_translator)
        cout << "ufs_translator: " << ufs_translator << endl;
    else
        cout << "ufs_translator is null." << endl;

    // cout << "function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
    ufs_translator->setup();
    ufs_translator->set_light(light);
    ufs_translator->set_locator(locator);

    // cout << "function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
    controlPanel->set_colortable(colorTable);
    controlPanel->set_translator(ufs_translator);
    controlPanel->setup();

    // controlPanel->disable_onmap();
    controlPanel->disable_x2();
    controlPanel->disable_y2();
    controlPanel->disable_z2();
    controlPanel->disable_t2();
    ufs_translator->updateSliders();
    // cout << "Leave function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
}

BaseTranslator* EagleParser::get_translator()
{
    return eagle_translator;
}

void EagleParser::cleanup()
{
    delete eagle_translator;
}
