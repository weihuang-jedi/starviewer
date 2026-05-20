#ifndef _MODEL_PARSER_H
#define _MODEL_PARSER_H

#include <iostream>
#include <memory>
#include <string>

#include "yamlhandler.h"
#include "nvoptions.h"
#include "colorTable.h"
#include "controlWidget.h"
#include "light.h"
#include "locator.h"
#include "basetranslator.h"

using namespace std;

// Abstract Base Class
class ModelParser {
public:
    virtual ~ModelParser() = default; // Essential for virtual destruction
    virtual void parse(YAMLHandler *yamlHandler, NVOptions* nvoptions,
                       ColorTable *colorTable, ControlWidget* controlPanel,
                       Locator* locator, Light* light) = 0; // Pure virtual function
    virtual BaseTranslator* get_translator() = 0;
    virtual void cleanup() = 0;

protected:
    BaseTranslator* translator;
};

class UFSINCRParser : public ModelParser {
public:
    void parse(YAMLHandler *yamlHandler, NVOptions* nvoptions,
               ColorTable *colorTable, ControlWidget* controlPanel,
               Locator* locator, Light* light) override {
        cout << "Parsing UFS tiled grid..." << endl;
    }
};

class MPASParser : public ModelParser {
public:
    void parse(YAMLHandler *yamlHandler, NVOptions* nvoptions,
               ColorTable *colorTable, ControlWidget* controlPanel,
               Locator* locator, Light* light) override {
        cout << "Parsing unstructured MPAS geodesic Voronoi grid..." << endl;
    }
};

#endif
