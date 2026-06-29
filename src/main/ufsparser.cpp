#include "ufsparser.h"

void UFSParser::parse(YAMLHandler *yamlHandler, NVOptions* nvoptions,
                      ColorTable *colorTable, ControlWidget* controlPanel,
                      Locator* locator, Light* light)
{
    // cout << "\nEnter function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
    // cout << "Parsing UFS lon-lat grid..." << endl;
    // cout << "nvoptions->get_model(): " << nvoptions->get_model() << endl;

    ufs_translator = new UFSTranslator(colorTable, nvoptions, yamlHandler);

    ufs_translator->setup();
    ufs_translator->set_light(light);
    ufs_translator->set_locator(locator);

    controlPanel->set_colortable(colorTable);
    controlPanel->set_translator(ufs_translator);
    controlPanel->setup();

    // controlPanel->disable_onmap();
    controlPanel->disable_x2();
    controlPanel->disable_y2();
    controlPanel->disable_z2();
    controlPanel->disable_t2();
    ufs_translator->updateSliders();
}

BaseTranslator* UFSParser::get_translator()
{
    return ufs_translator;
}

void UFSParser::cleanup()
{
    delete ufs_translator;
}
