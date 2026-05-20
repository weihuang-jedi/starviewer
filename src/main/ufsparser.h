#ifndef _UFS_PARSER_H
#define _UFS_PARSER_H

#include <iostream>
#include <memory>
#include <string>

#include "modelparser.h"

class UFSParser : public ModelParser {
public:
    void parse(YAMLHandler *yamlHandler, NVOptions* nvoptions,
               ColorTable *colorTable, ControlWidget* controlPanel,
               Locator* locator, Light* light) override
    {
        cout << "Parsing UFS lon-lat grid..." << endl;
    }
};

#endif
