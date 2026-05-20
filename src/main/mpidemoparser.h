#ifndef _MPIDEMO_PARSER_H
#define _MPIDEMO_PARSER_H

#include <iostream>
#include <memory>
#include <string>

#include "modelparser.h"
#include "mpitranslator.h"

class MPIDEMOParser : public ModelParser
{
    public:
        // Use '= default' so the compiler creates the vtable entry automatically
        virtual ~MPIDEMOParser() = default;

        void parse(YAMLHandler *yamlHandler, NVOptions* nvoptions,
                   ColorTable *colorTable, ControlWidget* controlPanel,
                   Locator* locator, Light* light) override;

        BaseTranslator* get_translator() { return mpidemo_translator; };

    protected:
        MPITranslator* mpidemo_translator;
};

#endif
