#include "mpasparser.h"

void MPASParser::parse(YAMLHandler *yamlHandler, NVOptions* nvoptions,
                      ColorTable *colorTable, ControlWidget* controlPanel,
                      Locator* locator, Light* light)
{
    vector<string> datafiles = yamlHandler->get_datafiles();
    bool isList = false;
    string flnm = datafiles[0];
    string mfnm = "unknown";

    // cout << "\nEnter function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
    // cout << "Parsing MPAS grid..." << endl;
    // cout << "flnm: " << flnm << endl;
    // cout << "nvoptions->get_model(): " << nvoptions->get_model() << endl;

    mpastranslator = new MPASTranslator(colorTable, nvoptions,
                                       flnm, isList, mfnm);

    // if(locator)
    //     cout << "locator->on(): " << locator->on() << endl;
    // else
    //     cout << "locator is null." << endl;

    // if(light)
    //     cout << "light->on(): " << light->on() << endl;
    // else
    //     cout << "light is null." << endl;

    if(mpastranslator)
        cout << "mpastranslator: " << mpastranslator << endl;
    else
        cout << "mpastranslator is null." << endl;

    // cout << "function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
    mpastranslator->setup();
    mpastranslator->set_light(light);
    mpastranslator->set_locator(locator);

    // cout << "function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
    controlPanel->set_colortable(colorTable);
    controlPanel->set_translator(mpastranslator);
    controlPanel->setup();

    // controlPanel->disable_onmap();
    controlPanel->disable_x2();
    controlPanel->disable_y2();
    controlPanel->disable_z2();
    controlPanel->disable_t2();
    mpastranslator->updateSliders();
    // cout << "Leave function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
}

BaseTranslator* MPASParser::get_translator()
{
    return mpastranslator;
}

void MPASParser::cleanup()
{
    delete mpastranslator;
}
