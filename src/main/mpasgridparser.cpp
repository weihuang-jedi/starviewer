#include "mpasparser.h"
#include "mpasgridparser.h"

void MPASGridParser::parse(YAMLHandler *yamlHandler, NVOptions* nvoptions,
                           ColorTable *colorTable, ControlWidget* controlPanel,
                           Locator* locator, Light* light)
{
    vector<string> mpasstaticfiles = yamlHandler->get_mpasstaticfiles();
    string static_flnm = mpasstaticfiles[0];
    vector<string> mpasdatafiles = yamlHandler->get_mpasdatafiles();
    string data_flnm = mpasdatafiles[0];

    // cout << "enter <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    // cout << "\t static_flnm: <" << static_flnm << ">, data_flnm: <" << data_flnm << ">" << endl;

    mpasgridtranslator = new MPASGridTranslator(colorTable, nvoptions, static_flnm, data_flnm);

    mpasgridtranslator->setup();
    mpasgridtranslator->set_light(light);
    mpasgridtranslator->set_locator(locator);

    controlPanel->set_colortable(colorTable);
    controlPanel->set_translator(mpasgridtranslator);
    controlPanel->setup();

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
