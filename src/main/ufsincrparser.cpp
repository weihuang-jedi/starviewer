#include "ufsincrparser.h"

void UFSINCRParser::parse(YAMLHandler *yamlHandler, NVOptions* nvoptions,
                          ColorTable *colorTable, ControlWidget* controlPanel,
                          Locator* locator, Light* light)
{
    vector<string> gridfiles = yamlHandler->get_gridfiles();
    vector<string> incrfiles = yamlHandler->get_incrfiles();

    // cout << "\nEnter function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
    cout << "Parsing UFSINCR tiled grid..." << endl;
    // cout << "nvoptions->get_model(): " << nvoptions->get_model() << endl;

    ufsincr_translator = new UFSINCRTranslator(colorTable, nvoptions,
                                               gridfiles, incrfiles);

    // if(ufsincr_translator)
    //     cout << "ufsincr_translator: " << ufsincr_translator << endl;
    // else
    //     cout << "ufsincr_translator is null." << endl;

    // cout << "function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
    ufsincr_translator->setup();
    ufsincr_translator->set_light(light);
    ufsincr_translator->set_locator(locator);

    // cout << "function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
    controlPanel->set_colortable(colorTable);
    controlPanel->set_translator(ufsincr_translator);
    controlPanel->setup();

    controlPanel->disable_onmap();
    controlPanel->disable_x2();
    controlPanel->disable_y2();
    controlPanel->disable_z2();
    controlPanel->disable_t2();
    ufsincr_translator->updateSliders();
    // cout << "Leave function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
}

BaseTranslator* UFSINCRParser::get_translator()
{
    return ufsincr_translator;
}

void UFSINCRParser::cleanup()
{
    delete ufsincr_translator;
}
