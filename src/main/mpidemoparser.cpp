#include "mpidemoparser.h"

void MPIDEMOParser::parse(YAMLHandler *yamlHandler, NVOptions* nvoptions,
		          ColorTable *colorTable, ControlWidget* controlPanel,
			  Locator* locator, Light* light)
{
    cout << "\tEnter function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
    cout << "MPI demo..." << endl;
    mpidemo_translator = new MPITranslator(colorTable, nvoptions);

    if(locator)
        cout << "locator->on(): " << locator->on() << endl;
    else
        cout << "locator is null." << endl;

    if(light)
        cout << "light->on(): " << light->on() << endl;
    else
        cout << "light is null." << endl;

    if(mpidemo_translator)
        cout << "mpidemo_translator: " << mpidemo_translator << endl;
    else
        cout << "mpidemo_translator is null." << endl;

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
    cout << "\tLeave function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
}

BaseTranslator* MPIDEMOParser::get_translator()
{
    cout << "\tEnter function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
    BaseTranslator* translator = mpidemo_translator;

    if(mpidemo_translator)
        cout << "mpidemo_translator: " << mpidemo_translator << endl;
    else
        cout << "mpidemo_translator is null." << endl;

    if(translator)
        cout << "translator: " << translator << endl;
    else
        cout << "translator is null." << endl;
    cout << "\tLeave function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
    return translator;
}

void MPIDEMOParser::cleanup()
{
    delete mpidemo_translator;
}
