#pragma once

#include "Common.h"
#include "ActionType.h"
#include "GameState.h"
#include "BuildOrder.h"
#include "Position.hpp"

namespace BOSS
{

class Rectangle
{
public:

    std::string labelText;
    Position topLeft;
    Position bottomRight;
    Rectangle(const std::string & label, Position & tl, const Position & br) : labelText(label), topLeft(tl), bottomRight(br) { }
};

class BuildOrderPlot
{
    const GameState         _initialState;
    const BuildOrder        _buildOrder;
        
    std::vector<int>        _startTimes;
    std::vector<int>        _finishTimes;
    std::vector<int>        _layers;
    std::vector<double>     _armyValues;
    std::vector< std::pair<int,int> > _minerals;
    std::vector< std::pair<int,int> > _gas;
    std::vector<Rectangle>  _rectangles;

    std::vector<BuildOrderPlot> _otherPlots;

    int                     _maxLayer;
    int                     _maxFinishTime;
    int                     _boxHeight;
    int                     _boxHeightBuffer;

    void calculateStartEndTimes();
    void calculatePlot();


public:

    BuildOrderPlot(const GameState & initialState, const BuildOrder & buildOrder);

    void writeResourcePlot(const std::string & filename);
    void writeRectanglePlot(const std::string & filename);
    void writeArmyValuePlot(const std::string & filename);
    void writeHybridPlot(const std::string & filename);

    void addPlot(const BuildOrderPlot & plot);

    static std::string GetFileNameFromPath(const std::string & path);
    static std::string RemoveFileExtension(const std::string & path);
    static void WriteGnuPlot(const std::string & filename, const std::string & data, const std::string & args);
};

}