#include "ufsparser.h"

void UFSParser::parse(YAMLHandler *yamlHandler, NVOptions* nvoptions,
                      ColorTable *colorTable, ControlWidget* controlPanel,
                      Locator* locator, Light* light)
{
    cout << "Parsing UFS lon-lat grid..." << endl;
    bool isList = false;
    string flnm = "unknown";
    string mfnm = "unknown";

    ufs_translator = new UFSTranslator(colorTable, nvoptions,
                                       flnm, isList, mfnm);
}

BaseTranslator* UFSParser::get_translator()
{
    return ufs_translator;
}

void UFSParser::cleanup()
{
    delete ufs_translator;
}
