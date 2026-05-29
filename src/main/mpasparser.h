#ifndef _MPAS_PARSER_H
#define _MPAS_PARSER_H

#include <iostream>
#include <memory>
#include <string>

#include "modelparser.h"
#include "mpastranslator.h"

class MPASParser : public ModelParser
{
    public:
        void parse(YAMLHandler *yamlHandler, NVOptions* nvoptions,
                   ColorTable *colorTable, ControlWidget* controlPanel,
                   Locator* locator, Light* light) override;

        BaseTranslator* get_translator() override;

        void cleanup() override;

    protected:
        MPASTranslator* mpastranslator;
};

#endif
