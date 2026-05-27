#ifndef _UFSMOM6_PARSER_H
#define _UFSMOM6_PARSER_H

#include <iostream>
#include <memory>
#include <string>

#include "modelparser.h"
#include "mom6_translator.h"

class UFSMOM6Parser : public ModelParser
{
    public:
        void parse(YAMLHandler *yamlHandler, NVOptions* nvoptions,
                   ColorTable *colorTable, ControlWidget* controlPanel,
                   Locator* locator, Light* light) override;

        BaseTranslator* get_translator() override;

        void cleanup() override;

    protected:
        MOM6Translator* ufsmom6_translator;
};

#endif
