#ifndef _EAGLE_PARSER_H
#define _EAGLE_PARSER_H

#include <iostream>
#include <memory>
#include <string>

#include "modelparser.h"
#include "eagle_translator.h"

class EagleParser : public ModelParser
{
    public:
        void parse(YAMLHandler *yamlHandler, NVOptions* nvoptions,
                   ColorTable *colorTable, ControlWidget* controlPanel,
                   Locator* locator, Light* light) override;

        BaseTranslator* get_translator() override;

        void cleanup() override;

    protected:
        EAGLETranslator* eagle_translator;
};

#endif
