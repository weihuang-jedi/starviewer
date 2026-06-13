#ifndef MODEL_PARSER_FACTORY_H
#define MODEL_PARSER_FACTORY_H

#include "nvoptions.h"
#include "mpidemoparser.h"
#include "ufsparser.h"
#include "ufsincrparser.h"
#include "eagleparser.h"
#include "eagleglobalparser.h"
#include <memory>  // Explicitly ensure smart pointers are available

class ModelParserFactory {
public:
    // Factory Method
    static unique_ptr<ModelParser> createParser(ModelType type) {
        switch (type) {
            case ModelType::MPIDEMO:
                return unique_ptr<ModelParser>(new MPIDEMOParser());
            case ModelType::UFS:
                return unique_ptr<ModelParser>(new UFSParser());
            case ModelType::UFSINCR:
                return unique_ptr<ModelParser>(new UFSINCRParser());
            case ModelType::EAGLE:
                return unique_ptr<ModelParser>(new EagleParser());
            case ModelType::EAGLEGLOBAL:
                return unique_ptr<ModelParser>(new EagleGlobalParser());
            default:
                return nullptr;
        }
    }
};

#endif
