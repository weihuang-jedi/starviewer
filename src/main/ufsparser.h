#ifndef _UFS_PARSER_H
#define _UFS_PARSER_H

#include <iostream>
#include <memory>
#include <string>

#include "modelparser.h"
#include "ufs_translator.h"

class UFSParser : public ModelParser
{
    public:
        void parse(YAMLHandler *yamlHandler, NVOptions* nvoptions,
                   ColorTable *colorTable, ControlWidget* controlPanel,
                   Locator* locator, Light* light) override;

        BaseTranslator* get_translator() override;

        void cleanup() override;

    protected:
        UFSTranslator* ufs_translator;
};

#endif
