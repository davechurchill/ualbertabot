#include "BuildOrderPlot.h"
#include "Eval.h"

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

        _armyValues.push_back(Eval::ArmyTotalResourceSum(state));

        std::pair<int, int> mineralsBefore(state.getCurrentFrame(), state.getMinerals() + type.mineralPrice());
        std::pair<int, int> mineralsAfter(state.getCurrentFrame(), state.getMinerals());

        std::pair<int, int> gasBefore(state.getCurrentFrame(), state.getGas() + type.gasPrice());
        std::pair<int, int> gasAfter(state.getCurrentFrame(), state.getGas());

        _minerals.push_back(mineralsBefore);
        _minerals.push_back(mineralsAfter);
        _gas.push_back(gasBefore);
        _gas.push_back(gasAfter);
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

void BuildOrderPlot::addPlot(const BuildOrderPlot & plot)
{
    _otherPlots.push_back(plot);
}

void BuildOrderPlot::writeResourcePlot(const std::string & filename)
{
    std::string noext = RemoveFileExtension(filename);
    std::stringstream mineralss;

    for (size_t i(0); i < _minerals.size(); ++i)
    {
        mineralss << _minerals[i].first << " " << _minerals[i].second/Constants::RESOURCE_SCALE << std::endl;
    }

    std::stringstream gasss;

    for (size_t i(0); i < _gas.size(); ++i)
    {
        gasss << _gas[i].first << " " << _gas[i].second/Constants::RESOURCE_SCALE << std::endl;
    }

    WriteGnuPlot(noext + "_minerals", mineralss.str(), " with lines ");
    WriteGnuPlot(noext + "_gas", gasss.str(), " with lines ");
}

void BuildOrderPlot::writeRectanglePlot(const std::string & filename)
{
    std::stringstream ss;
    int maxY = (_maxLayer + 1) * (_boxHeight + _boxHeightBuffer) + 15;

    for (size_t p(0); p < _otherPlots.size(); ++p)
    {
        maxY += (_otherPlots[p]._maxLayer + 2) * (_boxHeight + _boxHeightBuffer) + 15;
    }

    //ss << "set title \"Title Goes Here\"" << std::endl;
    //ss << "set xlabel \"Time (frames)\"" << std::endl;
    ss << "set style rect fc lt -1 fs transparent solid 0.15" << std::endl;
    ss << "set xrange [" << -(_maxFinishTime*.03) << ":" << 1.03*_maxFinishTime << "]" << std::endl;
    ss << "set yrange [-15:" << maxY << "]" << std::endl;
    ss << "unset ytics" << std::endl;
    ss << "set grid xtics" << std::endl;
    ss << "set nokey" << std::endl;
    //ss << "set size ratio " << (0.05 * _maxLayer) << std::endl;
    ss << "set terminal wxt size 960,300" << std::endl;
    ss << "plotHeight = " << 1000 << std::endl;
    ss << "boxHeight = " << _boxHeight << std::endl;
    ss << "boxHeightBuffer = " << _boxHeightBuffer << std::endl;
    ss << "boxWidthScale = " << 1.0 << std::endl;

    for (size_t i(0); i < _buildOrder.size(); ++i)
    {
        const Rectangle & rect = _rectangles[i];
        const int rectWidth = (rect.bottomRight.x() - rect.topLeft.x());
        const int rectCenterX = rect.bottomRight.x() - (rectWidth / 2);
        
        std::stringstream pos;
        pos << "(boxWidthScale * " << rectCenterX << "),";
        pos << "((boxHeight + boxHeightBuffer) * " << _layers[i] << " + boxHeight/2)";

        ss << "set object " << (i+1) << " rect at ";
        ss << pos.str();
        ss << " size ";
        ss << "(boxWidthScale * " << (rectWidth) << "),";
        ss << "(boxHeight) ";
        //ss << "(boxWidthScale * " << _finishTimes[i] << "),";
        //ss << "((boxHeight + boxHeightBuffer) * " << _layers[i] << " + boxHeight) ";
        ss << "lw 1";

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
        else if (_buildOrder[i].isUpgrade())
        {
            ss << " fc rgb \"purple\"";
        }
        else if (_buildOrder[i].isTech())
        {
            ss << " fc rgb \"magenta\"";
        }

        ss << std::endl;

        ss << "set label " << (i+1) << " at " << pos.str() << " \"" << _buildOrder[i].getShortName() << "\" front center";
        ss << std::endl;
    }

    int currentLayer = _maxLayer + 2;
    int currentObject = _buildOrder.size();

    for (size_t p(0); p < _otherPlots.size(); ++p)
    {
        const BuildOrder & buildOrder = _otherPlots[p]._buildOrder;

        for (size_t i(0); i < buildOrder.size(); ++i)
        {
            const Rectangle & rect = _otherPlots[p]._rectangles[i];
            const int rectWidth = (rect.bottomRight.x() - rect.topLeft.x());
            const int rectCenterX = rect.bottomRight.x() - (rectWidth / 2);
        
            std::stringstream pos;
            pos << "(boxWidthScale * " << rectCenterX << "),";
            pos << "((boxHeight + boxHeightBuffer) * " << (_otherPlots[p]._layers[i] + currentLayer) << " + boxHeight/2)";

            ss << "set object " << (currentObject+i+1) << " rect at ";
            ss << pos.str();
            ss << " size ";
            ss << "(boxWidthScale * " << (rectWidth) << "),";
            ss << "(boxHeight) ";
            //ss << "(boxWidthScale * " << _finishTimes[i] << "),";
            //ss << "((boxHeight + boxHeightBuffer) * " << _layers[i] << " + boxHeight) ";
            ss << "lw 1";

            if (buildOrder[i].isWorker())
            {
                ss << " fc rgb \"cyan\"";
            }
            else if (buildOrder[i].isSupplyProvider())
            {
                ss << " fc rgb \"gold\"";
            }
            else if (buildOrder[i].isRefinery())
            {
                ss << " fc rgb \"green\"";
            }
            else if (buildOrder[i].isBuilding())
            {
                ss << " fc rgb \"brown\"";
            }
            else if (buildOrder[i].isUpgrade())
            {
                ss << " fc rgb \"purple\"";
            }
            else if (buildOrder[i].isTech())
            {
                ss << " fc rgb \"magenta\"";
            }

            ss << std::endl;

            ss << "set label " << (currentObject+i+1) << " at " << pos.str() << " \"" << buildOrder[i].getShortName() << "\" front center";
            ss << std::endl;
        }

        currentLayer += _otherPlots[p]._maxLayer + 2;
        currentObject += buildOrder.size();
    }

    ss << "plot -10000" << std::endl;

    std::ofstream out(filename);
    out << ss.str();
    out.close();
}

void BuildOrderPlot::writeArmyValuePlot(const std::string & filename)
{
    std::stringstream datass;
    for (size_t i(0); i < _buildOrder.size(); ++i)
    {
        datass << _startTimes[i] << " " << _armyValues[i]/Constants::RESOURCE_SCALE << std::endl;
    }
 
    WriteGnuPlot(filename, datass.str(), " with steps");
}

void BuildOrderPlot::WriteGnuPlot(const std::string & filename, const std::string & data, const std::string & args)
{
    std::string file = RemoveFileExtension(GetFileNameFromPath(filename));
    std::string noext = RemoveFileExtension(filename);

    std::ofstream dataout(noext + "_data.txt");
    dataout << data;
    dataout.close();

    std::stringstream ss;
    ss << "set xlabel \"Time (frames)\"" << std::endl;
    ss << "set ylabel \"Resource Over Time\"" << std::endl;
    ss << "plot \"" << (file + "_data.txt") << "\" " << args << std::endl;

    std::ofstream out(noext + ".gpl");
    out << ss.str();
    out.close();
}

std::string BuildOrderPlot::GetFileNameFromPath(const std::string & path)
{
    std::string temp(path);

    const size_t last_slash_idx = temp.find_last_of("\\/");
    if (std::string::npos != last_slash_idx)
    {
        temp.erase(0, last_slash_idx + 1);
    }
    
    return temp;
}

std::string BuildOrderPlot::RemoveFileExtension(const std::string & path)
{
    std::string temp(path);

    const size_t period_idx = temp.rfind('.');
    if (std::string::npos != period_idx)
    {
        temp.erase(period_idx);
    }

    return temp;
}