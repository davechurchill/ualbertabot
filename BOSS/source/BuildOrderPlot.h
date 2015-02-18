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

    std::string label;
    Position topLeft;
    Position bottomRight;
    Rectangle(const std::string & label, Position & tl, const Position & br) : topLeft(tl), bottomRight(br) { }
};

class BuildOrderPlot
{
    const GameState         _initialState;
    const BuildOrder        _buildOrder;
        
    std::vector<int>        _startTimes;
    std::vector<int>        _finishTimes;
    std::vector<int>        _layers;
    std::vector<Rectangle>  _rectangles;

    int                     _maxLayer;
    int                     _maxFinishTime;
    int                     _boxHeight;
    int                     _boxHeightBuffer;

    void calculateStartEndTimes();
    void calculatePlot();

public:

    BuildOrderPlot(const GameState & initialState, const BuildOrder & buildOrder);

    void writeGNUPlotScript(const std::string & filename);
};

}