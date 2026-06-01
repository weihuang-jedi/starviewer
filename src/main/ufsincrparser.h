#ifndef _UFSINCR_PARSER_H
#define _UFSINCR_PARSER_H

#include <iostream>
#include <memory>
#include <string>

#include "modelparser.h"
#include "ufsincr_translator.h"

class UFSINCRParser : public ModelParser
{
    public:
        void parse(YAMLHandler *yamlHandler, NVOptions* nvoptions,
                   ColorTable *colorTable, ControlWidget* controlPanel,
                   Locator* locator, Light* light) override;

        BaseTranslator* get_translator() override;

        void cleanup() override;

    protected:
        UFSINCRTranslator* ufsincr_translator;
};

#endif
