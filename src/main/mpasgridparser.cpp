#include "mpasgridparser.h"

void MPASGridParser::parse(YAMLHandler *yamlHandler, NVOptions* nvoptions,
                           ColorTable *colorTable, ControlWidget* controlPanel,
                           Locator* locator, Light* light)
{
    vector<string> mpasgridstaticfiles = yamlHandler->get_mpasstaticfiles();
    string static_flnm = mpasstaticfiles[0];
    vector<string> mpasgriddatafiles = yamlHandler->get_mpasdatafiles();
    string data_flnm = mpasgriddatafiles[0];

    mpasgridtranslator = new MPASGridTranslator(colorTable, nvoptions, static_flnm, data_flnm);

    mpasgridtranslator->setup();
    mpasgridtranslator->set_light(light);
    mpasgridtranslator->set_locator(locator);

    controlPanel->set_colortable(colorTable);
    controlPanel->set_translator(mpastranslator);
    controlPanel->setup();

    // controlPanel->disable_onmap();
    controlPanel->disable_x2();
    controlPanel->disable_y2();
    controlPanel->disable_z2();
    controlPanel->disable_t2();
    mpasgridtranslator->updateSliders();
}

BaseTranslator* MPASGridParser::get_translator()
{
    return mpasgridtranslator;
}

void MPASGridParser::cleanup()
{
    delete mpasgridtranslator;
}
