#include "ufsmom6parser.h"

void UFSMOM6Parser::parse(YAMLHandler *yamlHandler, NVOptions* nvoptions,
                          ColorTable *colorTable, ControlWidget* controlPanel,
                          Locator* locator, Light* light)
{
    vector<string> mom6files = yamlHandler->get_mom6files();

    // cout << "\nEnter function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
    cout << "Parsing UFSMOM6 tiled grid..." << endl;
    // cout << "nvoptions->get_model(): " << nvoptions->get_model() << endl;

    ufsmom6_translator = new MOM6Translator(colorTable, nvoptions, mom6files);

    // if(ufsmom6_translator)
    //     cout << "ufsmom6_translator: " << ufsmom6_translator << endl;
    // else
    //     cout << "ufsmom6_translator is null." << endl;

    // cout << "function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
    ufsmom6_translator->setup();
    ufsmom6_translator->set_light(light);
    ufsmom6_translator->set_locator(locator);

    // cout << "function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
    controlPanel->set_colortable(colorTable);
    controlPanel->set_translator(ufsmom6_translator);
    controlPanel->setup();

    controlPanel->disable_onmap();
    controlPanel->disable_x2();
    controlPanel->disable_y2();
    controlPanel->disable_z2();
    controlPanel->disable_t2();
    ufsmom6_translator->updateSliders();
    // cout << "Leave function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
}

BaseTranslator* UFSMOM6Parser::get_translator()
{
    return ufsmom6_translator;
}

void UFSMOM6Parser::cleanup()
{
    delete ufsmom6_translator;
}
