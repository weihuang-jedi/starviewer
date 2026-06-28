#ifndef MODEL_PARSER_FACTORY_H
#define MODEL_PARSER_FACTORY_H

#include "nvoptions.h"
#include "mpidemoparser.h"
#include "ufsparser.h"
#include "ufsincrparser.h"
#include "mpasparser.h"
#include "ufsmom6parser.h"
#include "eagleparser.h"
#include "eagleglobalparser.h"
#include "eaglelamparser.h"
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
            case ModelType::UFSMOM6:
                return unique_ptr<ModelParser>(new UFSMOM6Parser());
            case ModelType::MPAS:
                return unique_ptr<ModelParser>(new MPASParser());
            case ModelType::EAGLE:
                return unique_ptr<ModelParser>(new EagleParser());
            case ModelType::EAGLEGLOBAL:
                return unique_ptr<ModelParser>(new EagleGlobalParser());
            case ModelType::EAGLELAM:
                return unique_ptr<ModelParser>(new EagleLAMParser());
            default:
                return nullptr;
        }
    }
};

#endif
