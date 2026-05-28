#include "ufsmom6parser.h"

void UFSMOM6Parser::parse(YAMLHandler *yamlHandler, NVOptions* nvoptions,
                          ColorTable *colorTable, ControlWidget* controlPanel,
                          Locator* locator, Light* light)
{
    vector<string> mom6files = yamlHandler->get_mom6files();

    // cout << "\nEnter function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
    cout << "Parsing UFSMOM6 model..." << endl;
    // cout << "nvoptions->get_model(): " << nvoptions->get_model() << endl;

    mom6_translator = new MOM6Translator(colorTable, nvoptions, mom6files);

    // if(mom6_translator)
    //     cout << "mom6_translator: " << mom6_translator << endl;
    // else
    //     cout << "mom6_translator is null." << endl;

    // cout << "function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
    mom6_translator->setup();
    mom6_translator->set_light(light);
    mom6_translator->set_locator(locator);

    // cout << "function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
    controlPanel->set_colortable(colorTable);
    controlPanel->set_translator(mom6_translator);
    controlPanel->setup();

    controlPanel->disable_onmap();
    controlPanel->disable_x2();
    controlPanel->disable_y2();
    controlPanel->disable_z2();
    controlPanel->disable_t2();
    mom6_translator->updateSliders();
    // cout << "Leave function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
}

BaseTranslator* UFSMOM6Parser::get_translator()
{
    return mom6_translator;
}

void UFSMOM6Parser::cleanup()
{
    delete mom6_translator;
}
