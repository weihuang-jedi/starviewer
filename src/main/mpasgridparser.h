#ifndef _MPASGRID_PARSER_H
#define _MPASGRID_PARSER_H

#include <iostream>
#include <memory>
#include <string>

#include "modelparser.h"
#include "mpasgridtranslator.h"

class MPASGridParser : public ModelParser
{
    public:
        void parse(YAMLHandler *yamlHandler, NVOptions* nvoptions,
                   ColorTable *colorTable, ControlWidget* controlPanel,
                   Locator* locator, Light* light) override;

        BaseTranslator* get_translator() override;

        void cleanup() override;

    protected:
        MPASGridTranslator* mpasgridtranslator;
};

#endif
