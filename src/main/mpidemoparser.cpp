#include "mpidemoparser.h"

void MPIDEMOParser::parse(YAMLHandler *yamlHandler, NVOptions* nvoptions,
		          ColorTable *colorTable, ControlWidget* controlPanel,
			  Locator* locator, Light* light)
{
    cout << "MPI demo..." << endl;
    string fileName = "notneed";
    bool isFileList = false;
    mpidemo_translator = new MPITranslator(colorTable, nvoptions,
                                         fileName, isFileList);

    mpidemo_translator->setup();
    mpidemo_translator->set_light(light);
    mpidemo_translator->set_locator(locator);

    controlPanel->set_colortable(colorTable);
    controlPanel->set_translator(mpidemo_translator);
    controlPanel->setup();

    controlPanel->disable_onmap();
    controlPanel->disable_sphere();
    controlPanel->disable_flat();
    controlPanel->disable_x2();
    controlPanel->disable_y2();
    controlPanel->disable_z2();
    controlPanel->disable_t2();
    mpidemo_translator->updateSliders();
}

