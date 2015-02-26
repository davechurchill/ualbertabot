#include "BOSS.h"
#include "BOSSExperiments.h"
#include "BuildOrderTester.h"
#include "GUI.h"

using namespace BOSS;

#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif

BOSS::BOSSExperiments experiments;
std::string returnString("TestReturn");

void mainLoop()
{


    GUI::Instance().OnFrame();
}

extern "C" 
{
const char * ResetExperiment(char * paramString)
{
    std::cout << "Param String Received" << std::endl;

    std::string params(paramString);

    experiments = BOSS::BOSSExperiments();

    if (params.length() == 0)
    {
        #ifndef EMSCRIPTEN
            experiments.loadExperimentsFromFile("../asset/config/webconfig.txt");
        #else
            experiments.loadExperimentsFromFile("asset/config/webconfig.txt");
        #endif
    }
    else
    {
        experiments.loadExperimentsFromString(params);
    }

    if (experiments.getVisExperiments().size() > 0)
    {
        GUI::Instance().SetVisExperiment(experiments.getVisExperiments()[0]);
    }
 
    return returnString.c_str();
}
}

void doCombatExperiment()
{
    BOSS::BOSSExperiments exp;
    exp.loadExperimentsFromFile("../asset/config/config.txt");

    for (size_t i(0); i < exp.getCombatSearchExperiments().size(); ++i)
    {
        exp.getCombatSearchExperiments()[i].run();
    }
}

#include "BuildOrderPlot.h"
void testBuildOrderPlot()
{
    const std::string buildOrderJSON = "[ \"Protoss_Probe\", \"Protoss_Probe\", \"Protoss_Probe\", \"Protoss_Probe\", \"Protoss_Pylon\", \"Protoss_Probe\", \"Protoss_Probe\", \"Protoss_Gateway\", \"Protoss_Probe\", \"Protoss_Assimilator\", \"Protoss_Probe\", \"Protoss_Probe\", \"Protoss_Cybernetics_Core\", \"Protoss_Probe\", \"Protoss_Probe\", \"Protoss_Gateway\", \"Singularity_Charge\", \"Protoss_Dragoon\", \"Protoss_Gateway\", \"Protoss_Pylon\", \"Protoss_Dragoon\", \"Protoss_Dragoon\", \"Protoss_Probe\", \"Protoss_Gateway\", \"Protoss_Pylon\", \"Protoss_Probe\", \"Protoss_Dragoon\", \"Protoss_Dragoon\", \"Protoss_Dragoon\"]";

    GameState state(Races::Protoss);
    state.setStartingState();

    BuildOrder buildOrder = JSONTools::GetBuildOrder(buildOrderJSON);

    BuildOrderPlot plot(state, buildOrder);
    plot.writeRectanglePlot("gnuplot/testnew.gpl");
    plot.writeArmyValuePlot("gnuplot/testnewarmy.gpl");
}

int main(int argc, char *argv[])
{
    BOSS::init();
    
    //testBuildOrderPlot();

    doCombatExperiment();
/*
    ResetExperiment("");

    if (experiments.getVisExperiments().size() > 0)
    {
        GUI::Instance().OnStart();
        GUI::Instance().SetVisExperiment(experiments.getVisExperiments()[0]);
    }

#ifndef EMSCRIPTEN
    while (true)
    {
        mainLoop();
    }
#else
    emscripten_set_main_loop(mainLoop,0,true);
#endif*/

    return 0;
}

