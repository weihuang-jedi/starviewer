#include "eaglelamparser.h"

void EagleLAMParser::parse(YAMLHandler *yamlHandler, NVOptions* nvoptions,
                              ColorTable *colorTable, ControlWidget* controlPanel,
                              Locator* locator, Light* light)
{
    // cout << "\nEnter function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
    vector<string> datafiles = yamlHandler->get_datafiles();

    cout << "Parsing Eagle global..." << endl;
    // for (int n = 0; n < datafiles.size(); ++n)
    //     cout << "datafiles[" << n << "]: <" << datafiles[n] << ">" << endl;
    // cout << "nvoptions->get_model(): " << nvoptions->get_model() << endl;

    eagleglobal_translator = new EagleLAMTranslator(colorTable, nvoptions, datafiles);

    // cout << "function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
    eagleglobal_translator->setup();
    eagleglobal_translator->set_light(light);
    eagleglobal_translator->set_locator(locator);

    // cout << "function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
    controlPanel->set_colortable(colorTable);
    controlPanel->set_translator(eagleglobal_translator);
    controlPanel->setup();

    // controlPanel->disable_onmap();
    controlPanel->disable_x2();
    controlPanel->disable_y2();
    controlPanel->disable_z2();
    controlPanel->disable_t2();
    eagleglobal_translator->updateSliders();
    // cout << "Leave function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
}

BaseTranslator* EagleLAMParser::get_translator()
{
    return eagleglobal_translator;
}

void EagleLAMParser::cleanup()
{
    delete eagleglobal_translator;
}
