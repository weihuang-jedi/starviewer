#include "eagleparser.h"

void EagleParser::parse(YAMLHandler *yamlHandler, NVOptions* nvoptions,
                      ColorTable *colorTable, ControlWidget* controlPanel,
                      Locator* locator, Light* light)
{
    // cout << "\nEnter function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
    vector<string> datafiles = yamlHandler->get_datafiles();
    string flnm = datafiles[0];

    // cout << "Parsing Eagle lon-lat grid..." << endl;
    // cout << "flnm: " << flnm << endl;
    // cout << "nvoptions->get_model(): " << nvoptions->get_model() << endl;

    eagle_translator = new EAGLETranslator(colorTable, nvoptions, flnm);

    // if(locator)
    //     cout << "locator->on(): " << locator->on() << endl;
    // else
    //     cout << "locator is null." << endl;

    // if(light)
    //     cout << "light->on(): " << light->on() << endl;
    // else
    //     cout << "light is null." << endl;

    // if(eagle_translator)
    //     cout << "eagle_translator: " << eagle_translator << endl;
    // else
    //     cout << "eagle_translator is null." << endl;

    // cout << "function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
    eagle_translator->setup();
    eagle_translator->set_light(light);
    eagle_translator->set_locator(locator);

    // cout << "function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
    controlPanel->set_colortable(colorTable);
    controlPanel->set_translator(eagle_translator);
    controlPanel->setup();

    // controlPanel->disable_onmap();
    controlPanel->disable_x2();
    controlPanel->disable_y2();
    controlPanel->disable_z2();
    controlPanel->disable_t2();
    eagle_translator->updateSliders();
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
