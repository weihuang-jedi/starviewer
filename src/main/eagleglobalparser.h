#ifndef _EAGLE_GLOBAL_PARSER_H
#define _EAGLE_GLOBAL_PARSER_H

#include <iostream>
#include <memory>
#include <string>

#include "modelparser.h"
#include "eagleglobal_translator.h"

class EagleGlobalParser : public ModelParser
{
    public:
        void parse(YAMLHandler *yamlHandler, NVOptions* nvoptions,
                   ColorTable *colorTable, ControlWidget* controlPanel,
                   Locator* locator, Light* light) override;

        BaseTranslator* get_translator() override;

        void cleanup() override;

    protected:
        EagleGlobalTranslator* eagleglobal_translator;
};

#endif
