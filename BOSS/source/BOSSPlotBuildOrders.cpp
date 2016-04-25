#include "BOSSPlotBuildOrders.h"
#include "BuildOrderPlot.h"
#include "BOSSParameters.h"

using namespace BOSS;

BOSSPlotBuildOrders::BOSSPlotBuildOrders(const std::string & name, const rapidjson::Value & val)
{
    BOSS_ASSERT(val.HasMember("Scenarios") && val["Scenarios"].IsArray(), "Experiment has no Scenarios array");
    BOSS_ASSERT(val.HasMember("OutputDir") && val["OutputDir"].IsString(), "Experiment has no OutputFile string");

    _outputDir = val["OutputDir"].GetString();

    const rapidjson::Value & scenarios = val["Scenarios"];
        
    for (size_t i(0); i < scenarios.Size(); ++i)
    {
        const rapidjson::Value & scenario = scenarios[i];

        BOSS_ASSERT(scenario.HasMember("State") && scenario["State"].IsString(), "Scenario has no 'state' string");
        BOSS_ASSERT(scenario.HasMember("BuildOrder") && scenario["BuildOrder"].IsString(), "Scenario has no 'buildOrder' string");
        
        _states.push_back(BOSSParameters::Instance().GetState(scenario["State"].GetString()));
        _buildOrders.push_back(BOSSParameters::Instance().GetBuildOrder(scenario["BuildOrder"].GetString()));
        _buildOrderNames.push_back(scenario["BuildOrder"].GetString());
    }
}

void BOSSPlotBuildOrders::doPlots()
{
    for (size_t i(0); i < _buildOrders.size(); ++i)
    {
        BuildOrderPlot plot(_states[i], _buildOrders[i]);
        
        plot.writeRectanglePlot(_outputDir + _buildOrderNames[i] + ".gpl");
        plot.writeArmyValuePlot(_outputDir + _buildOrderNames[i] + "_army");
        plot.writeResourcePlot(_outputDir + _buildOrderNames[i] + "_resource");
    }

    BuildOrderPlot allPlots(_states[0], _buildOrders[0]);
    for (size_t i(1); i < _buildOrders.size(); ++i)
    {
        BuildOrderPlot plot(_states[i], _buildOrders[i]);
        allPlots.addPlot(plot);
    }

    allPlots.writeRectanglePlot(_outputDir + "BOall");
}