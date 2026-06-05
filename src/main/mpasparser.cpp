#include "mpasparser.h"

void MPASParser::parse(YAMLHandler *yamlHandler, NVOptions* nvoptions,
                      ColorTable *colorTable, ControlWidget* controlPanel,
                      Locator* locator, Light* light)
{
    vector<string> mpasstaticfiles = yamlHandler->get_mpasstaticfiles();
    string static_flnm = mpasstaticfiles[0];
    vector<string> mpasdatafiles = yamlHandler->get_mpasdatafiles();
    string data_flnm = mpasdatafiles[0];

    mpastranslator = new MPASTranslator(colorTable, nvoptions, static_flnm, data_flnm);

    mpastranslator->setup();
    mpastranslator->set_light(light);
    mpastranslator->set_locator(locator);

    controlPanel->set_colortable(colorTable);
    controlPanel->set_translator(mpastranslator);
    controlPanel->setup();

    // controlPanel->disable_onmap();
    controlPanel->disable_x2();
    controlPanel->disable_y2();
    controlPanel->disable_z2();
    controlPanel->disable_t2();
    mpastranslator->updateSliders();
}

BaseTranslator* MPASParser::get_translator()
{
    return mpastranslator;
}

void MPASParser::cleanup()
{
    delete mpastranslator;
}
