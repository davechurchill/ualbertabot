#pragma once

#include "BOSS.h"
#include "GUITools.h"
#include "JSONTools.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"

namespace BOSS
{

class BOSSVisExperiment
{
    std::vector< GameState >                        _states;
    std::vector< BuildOrder >                       _buildOrders;
    std::vector< std::vector<FrameCountType> >      _startTimes;
    std::vector< std::vector<FrameCountType> >      _finishTimes;
    std::vector< size_t >                           _nextActionIndexes;

    double                                          _fps;
    size_t                                          _scenarios;

    bool                                            _finished;
    Position                                        _lastDrawPosition;

    PositionType    DrawScenario(const Position & pos, const size_t scenario);

    std::string getTimeString(const FrameCountType & frameCount);

    void draw();

public:

    BOSSVisExperiment();
    BOSSVisExperiment(const rapidjson::Value & experimentVal, std::map< std::string, GameState > & stateMap, std::map< std::string, BuildOrder > & buildOrderMap);

    void onFrame();

    const Position & getLastDrawPosition() const;

};
}
