#include "BuildOrderPlot.h"

using namespace BOSS;

BuildOrderPlot::BuildOrderPlot(const GameState & initialState, const BuildOrder & buildOrder)
    : _initialState(initialState)
    , _buildOrder(buildOrder)
    , _boxHeight(20)
    , _boxHeightBuffer(3)
    , _maxLayer(0)
    , _maxFinishTime(0)
{
    calculateStartEndTimes();
    calculatePlot();
}

void BuildOrderPlot::calculateStartEndTimes()
{
    GameState state(_initialState);

    BOSS_ASSERT(_buildOrder.isLegalFromState(state), "Build order isn't legal!");

    for (size_t i(0); i < _buildOrder.size(); ++i)
    {
        const ActionType & type = _buildOrder[i];

        state.doAction(type);

        _startTimes.push_back(state.getCurrentFrame());

        FrameCountType finish = state.getCurrentFrame() + type.buildTime();
        if (type.isBuilding() && !type.isAddon() && !type.isMorphed())
        {
            finish += Constants::BUILDING_PLACEMENT;
        }

        _finishTimes.push_back(finish);

        _maxFinishTime = std::max(_maxFinishTime, finish);
    }
}

void BuildOrderPlot::calculatePlot()
{
    _layers = std::vector<int>(_startTimes.size(), -1); 

    // determine the layers for each action
    for (size_t i(0); i < _startTimes.size(); ++i)
    {
        FrameCountType start    = _startTimes[i];
        FrameCountType finish   = _finishTimes[i];

        std::vector<int> layerOverlap;
        // loop through everything up to this action and see which layers it can't be in
        for (size_t j(0); j < i; ++j)
        {
            if (start < _finishTimes[j])
            {
                layerOverlap.push_back(_layers[j]);
            }
        }

        // find a layer we can assign to this value
        int layerTest = 0;
        while (true)
        {
            if (std::find(layerOverlap.begin(), layerOverlap.end(), layerTest) == layerOverlap.end())
            {
                _layers[i] = layerTest;
                if (layerTest > _maxLayer)
                {
                    _maxLayer = layerTest;
                }
                break;
            }

            layerTest++;
        }
    }

    for (size_t i(0); i < _buildOrder.size(); ++i)
    {
        Position topLeft(_startTimes[i], _layers[i] * (_boxHeight + _boxHeightBuffer));
        Position bottomRight(_finishTimes[i], topLeft.y() + _boxHeight);

        _rectangles.push_back(Rectangle(_buildOrder[i].getShortName(), topLeft, bottomRight));
    }
}

void BuildOrderPlot::writeGNUPlotScript(const std::string & filename)
{
    std::stringstream ss;
    int maxY = (_maxLayer + 1) * (_boxHeight + _boxHeightBuffer) + 15;

    //ss << "set title \"Title Goes Here\"" << std::endl;
    //ss << "set xlabel \"Time (frames)\"" << std::endl;
    ss << "set style rect fc lt -1 fs solid 0.15" << std::endl;
    ss << "set xrange [" << -(_maxFinishTime*.03) << ":" << 1.03*_maxFinishTime << "]" << std::endl;
    ss << "set yrange [-15:" << maxY << "]" << std::endl;
    ss << "unset ytics" << std::endl;
    ss << "set grid xtics" << std::endl;
    ss << "set nokey" << std::endl;
    ss << "set size ratio " << (0.05 * _maxLayer) << std::endl;
    ss << "set terminal wxt size 1500,300" << std::endl;

    for (size_t i(0); i < _rectangles.size(); ++i)
    {
        const Rectangle & rect = _rectangles[i];

        ss << "set object " << (i+1) << " rect from " << rect.topLeft.x() << "," << rect.topLeft.y() << " to " << rect.bottomRight.x() << "," << rect.bottomRight.y() << " lw 1";

        if (_buildOrder[i].isWorker())
        {
            ss << " fc rgb \"cyan\"";
        }
        else if (_buildOrder[i].isSupplyProvider())
        {
            ss << " fc rgb \"gold\"";
        }
        else if (_buildOrder[i].isRefinery())
        {
            ss << " fc rgb \"green\"";
        }
        else if (_buildOrder[i].isBuilding())
        {
            ss << " fc rgb \"brown\"";
        }

        ss << std::endl;
    }

    ss << "plot -10000" << std::endl;

    std::ofstream out(filename);
    out << ss.str();
    out.close();
}