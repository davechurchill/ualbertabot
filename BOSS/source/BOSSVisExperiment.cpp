#include "BOSSVisExperiment.h"

#include "GUI.h"

using namespace BOSS;

BOSSVisExperiment::BOSSVisExperiment()
    : _fps(0)
    , _scenarios(0)
    , _finished(true)
{

}

BOSSVisExperiment::BOSSVisExperiment(const rapidjson::Value & val, std::map< std::string, GameState > & stateMap, std::map< std::string, BuildOrder > & buildOrderMap)
    : _startTimes(val["scenarios"].Size(), std::vector<FrameCountType>())
    , _finishTimes(val["scenarios"].Size(), std::vector<FrameCountType>())
    , _nextActionIndexes(val["scenarios"].Size(), 0)
    , _fps(0)
    , _scenarios(0)
    , _finished(false)
{
    if (val.HasMember("fps") && val["fps"].IsInt())
    {
        _fps = val["fps"].GetInt();
    }

    const rapidjson::Value & scenarios = val["scenarios"];
    _scenarios = scenarios.Size();
    for (size_t i(0); i < scenarios.Size(); ++i)
    {
        const rapidjson::Value & scenario = scenarios[i];

        BOSS_ASSERT(scenario.HasMember("state") && scenario["state"].IsString(), "Scenario has no 'state' string");
        BOSS_ASSERT(scenario.HasMember("buildOrder") && scenario["buildOrder"].IsString(), "Scenario has no 'buildOrder' string");

        const std::string & stateName = scenario["state"].GetString();
        const std::string & boName = scenario["buildOrder"].GetString();

        BOSS_ASSERT(stateMap.find(stateName) != stateMap.end(), "State not found: %s", stateName.c_str());
        BOSS_ASSERT(buildOrderMap.find(boName) != buildOrderMap.end(), "Build Order not found: %s", boName.c_str());

        _states.push_back(stateMap[scenario["state"].GetString()]);
        _buildOrders.push_back(buildOrderMap[scenario["buildOrder"].GetString()]);
    }
}

const Position & BOSSVisExperiment::getLastDrawPosition() const
{
    return _lastDrawPosition;
}

void BOSSVisExperiment::draw()
{
    Position drawAt(0,0);
    for (size_t i(0); i < _scenarios; ++i)
    {
        PositionType endY = DrawScenario(drawAt, i);
        drawAt = Position(0, endY);
    }

    _lastDrawPosition = drawAt;
}

void BOSSVisExperiment::onFrame()
{
    if (_finished)
    {
        return;
    }
    
    for (size_t s(0); s < _states.size(); ++s)
    {
        bool didAction = false;

        if (_nextActionIndexes[s] < _buildOrders[s].size())
        {
            FrameCountType nextActionFrame = _states[s].whenCanPerform(_buildOrders[s][_nextActionIndexes[s]]);

            if (nextActionFrame == _states[s].getCurrentFrame())
            {
                ActionType type = _buildOrders[s][_nextActionIndexes[s]];
                FrameCountType finish = _states[s].getCurrentFrame() + _buildOrders[s][_nextActionIndexes[s]].buildTime();
                if (type.isBuilding() && !type.isAddon() && !type.isMorphed())
                {
                    finish += Constants::BUILDING_PLACEMENT;
                }

                _startTimes[s].push_back(_states[s].getCurrentFrame());
                _finishTimes[s].push_back(finish);

                _states[s].doAction(_buildOrders[s][_nextActionIndexes[s]]);

                didAction = true;
                //std::cout << states[s].getCurrentFrame() << " Action Performed: " << buildOrder[nextActionIndex].getName() << std::endl;
                _nextActionIndexes[s]++;
            }
        }
        
        if (!didAction)
        {
            _states[s].fastForward(_states[s].getCurrentFrame() + 3);
        }
    }

    draw();

    bool notFinished = false;
    for (size_t i(0); i < _states.size(); ++i)
    {
        if (_nextActionIndexes[i] < _buildOrders[i].size() || _states[i].getCurrentFrame() < _states[i].getLastActionFinishTime())
        {
            notFinished = true;
            return;
        }
    }

    _finished = !notFinished;
}

std::string BOSSVisExperiment::getTimeString(const FrameCountType & frameCount)
{
    std::stringstream min;
    min << (frameCount/24)/60;
    std::string minString = min.str();

    std::stringstream sec;
    sec << (frameCount/24)%60;
    std::string secString = sec.str();
    while (secString.length() < 2) secString = "0" + secString;

    std::string timeString = minString + ":" + secString;
    return timeString;
}

//const GameState & currentState, const std::vector<ActionType> & buildOrder, const size_t & boIndex, const std::vector<FrameCountType> & startTimes, const std::vector<FrameCountType> & finishTimes);
PositionType BOSSVisExperiment::DrawScenario(const Position & pos, const size_t scenario)
{
    const GameState & currentState                      = _states[scenario];
    const BuildOrder & buildOrder                       = _buildOrders[scenario];
    const std::vector<FrameCountType> & startTimes      = _startTimes[scenario];
    const std::vector<FrameCountType> & finishTimes     = _finishTimes[scenario];
    //const size_t & boIndex = _buildO

    const std::vector<ActionType> & allActions = ActionTypes::GetAllActionTypes(currentState.getRace());
    static const ActionType & Larva = ActionTypes::GetActionType("Zerg_Larva");
    static const ActionType & Hatchery = ActionTypes::GetActionType("Zerg_Hatchery");
    GLfloat black[4] = {0.0f, 0.0f, 0.0f, 1.0f};
    GLfloat white2[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat white[4] = {1.0f, 1.0f, 1.0f, 0.8f};
    GLfloat grey[4] = {0.1f, 0.1f, 0.1f, 1.0f};
    GLfloat grey2[4] = {0.4f, 0.4f, 0.4f, 1.0f};
    GLfloat red[4] = {0.6f, 0.0f, 0.0f, 1.0f};
    GLfloat blue[4] = {0.0f, 0.0f, 0.5f, 1.0f};
    GLfloat bblue[4] = {0.5f, 0.5f, 1.0f, 1.0f};
    GLfloat bred[4] = {1.0f, 0.0f, 0.0f, 1.0f};
    GLfloat green[4] = {0.0f, 0.4f, 0.0f, 1.0f};
    GLfloat bgreen[4] = {0.0f, 1.0f, 0.0f, 1.0f};

    std::string stateTime = getTimeString(currentState.getCurrentFrame());

    std::stringstream ssres;
    ssres << "Frame:    " << currentState.getCurrentFrame() <<"\n";
    ssres << "Minerals: " << currentState.getMinerals()/Constants::RESOURCE_SCALE << "\n";
    ssres << "Gas:      " << currentState.getGas()/Constants::RESOURCE_SCALE << "\n";
    ssres << "MWorkers: " << currentState.getNumMineralWorkers() << "\n";
    ssres << "GWorkers: " << currentState.getNumGasWorkers() << "\n";
    ssres << "C Supply: " << currentState.getUnitData().getCurrentSupply() << "\n";
    ssres << "M Supply: " << currentState.getUnitData().getMaxSupply() << "\n";
    GUITools::DrawString(pos + Position(20, 20), ssres.str(), white);

    Position completed = pos + Position(225,0);
    size_t cwidth = 64;
    if (currentState.getRace() == Races::Zerg)
    {
        const UnitCountType & numLarva = currentState.getHatcheryData().numLarva();

        GUI::Instance().DrawActionType(Larva, completed, cwidth);
        std::stringstream num; num << numLarva << "\n" << (Constants::ZERG_LARVA_TIMER-(currentState.getCurrentFrame() % Constants::ZERG_LARVA_TIMER));
        GUITools::DrawString(completed + Position(10, 20), num.str(), white);
        completed.add(cwidth, 0);
    }

    for (size_t a(0); a < allActions.size(); ++a)
    {
        const size_t numCompleted = currentState.getUnitData().getNumCompleted(allActions[a]);

        if (numCompleted > 0)
        {
            GUI::Instance().DrawActionType(allActions[a], completed, cwidth);
            std::stringstream num; num << numCompleted;
            GUITools::DrawString(completed + Position(10, 20), num.str(), white);
            completed.add(cwidth, 0);
        }
    }

    Position legal = pos + Position(225,80);
    ActionSet legalActions;
    currentState.getAllLegalActions(legalActions);
    GUITools::DrawString(legal + Position(0,-5), "Legal Actions", white);
    for (size_t a(0); a < legalActions.size(); ++a)
    {
        const ActionType & action = legalActions[a];
        GUI::Instance().DrawActionType(legalActions[a], legal, 32);
        legal.add(32, 0);
    }

    std::vector<int> layers(startTimes.size(), -1);
    int maxLayer = 0;

    for (size_t i(0); i < startTimes.size(); ++i)
    {
        FrameCountType start    = startTimes[i];
        FrameCountType finish   = finishTimes[i];

        std::vector<int> layerOverlap;
        // loop through everything up to this action and see which layers it can't be in
        for (size_t j(0); j < i; ++j)
        {
            if (start < finishTimes[j])
            {
                layerOverlap.push_back(layers[j]);
            }
        }

        // find a layer we can assign to this value
        int layerTest = 0;
        while (true)
        {
            if (std::find(layerOverlap.begin(), layerOverlap.end(), layerTest) == layerOverlap.end())
            {
                layers[i] = layerTest;
                if (layerTest > maxLayer)
                {
                    maxLayer = layerTest;
                }
                break;
            }

            layerTest++;
        }
    }

    float maxWidth = (float)GUI::Instance().Width() - 430;
    float maxFinishTime = 1;

    for (size_t i(0); i < _finishTimes.size(); ++i)
    {
        for (size_t j(0); j < _finishTimes[i].size(); ++j)
        {
            if (_finishTimes[i][j] > maxFinishTime)
            {
                maxFinishTime = (float)_finishTimes[i][j];
            }
        }
    }

    if (currentState.getCurrentFrame() < maxWidth && currentState.getCurrentFrame() > maxFinishTime)
    {
        maxFinishTime = (float)currentState.getCurrentFrame();
    }

    maxFinishTime = std::max(maxFinishTime, maxWidth);
    //maxFinishTime = std::max(currentState.getCurrentFrame()*1.10f, maxWidth);

    float scale = maxWidth / maxFinishTime;

    // draw it

    PositionType height         = 20;
    PositionType heightBuffer   = 3;
    Position concurrent         = pos + Position(25, 200);

    float framePos              =  (concurrent.x() + scale*currentState.getCurrentFrame());
    Position boxTopLeft         = Position(concurrent.x() - 10, concurrent.y() - 10);
    Position boxBottomRight     = Position(PositionType(concurrent.x() + maxWidth + 10), (PositionType)(concurrent.y() + (maxLayer)*(height + heightBuffer) + 10 + height));
    
    GUITools::DrawRect(boxTopLeft, boxBottomRight, grey2);
    GUITools::DrawRect(boxTopLeft + Position(1,1), boxBottomRight - Position(1,1), grey);

    for (size_t i(0); i < layers.size(); ++i)
    {
        float boxWidth = (finishTimes[i] - startTimes[i])*scale;

        Position topLeft(PositionType(concurrent.x() + startTimes[i]*scale), PositionType(concurrent.y() + (height + heightBuffer) * layers[i]));
        Position bottomRight(PositionType(topLeft.x() + boxWidth), topLeft.y() + height);

        std::string name = buildOrder[i].getName();
#ifndef EMSCRIPTEN
        size_t loc = name.find("_") + 1;
#else
        size_t loc = name.find(" ") + 1;
#endif
        if (loc != std::string::npos)
        {
            name = name.substr(loc);
        }

        GUITools::DrawRect(topLeft, bottomRight, white);
        GUITools::DrawRect(topLeft+Position(1,1), bottomRight-Position(1,1), finishTimes[i] < currentState.getCurrentFrame() ? red : blue);
        GUITools::DrawString(topLeft + Position(3, 13), name, white);
    }

    PositionType boWidth = std::min(32, (PositionType)(maxWidth / buildOrder.size()));
    for (size_t i(0); i < buildOrder.size(); ++i)
    {
        Position topLeft(concurrent.x() + i*boWidth, concurrent.y() - boWidth - 20);

        if (i < _nextActionIndexes[scenario])
        {
            GUITools::DrawRect(topLeft, topLeft + Position(boWidth, boWidth), grey2);
            GUITools::DrawRect(topLeft + Position(1,1), topLeft + Position(boWidth-1, boWidth-1), finishTimes[i] < currentState.getCurrentFrame() ? grey : blue);
        }

        if (i < startTimes.size())
        {
            //GUITools::DrawString(topLeft - Position(-2,20), getTimeString(startTimes[i]).c_str(), bblue);
            //GUITools::DrawString(topLeft - Position(-2,9), getTimeString(finishTimes[i]).c_str(), bred);
        }

        GUI::Instance().DrawActionType(buildOrder[i], topLeft, boWidth);

        std::string name = buildOrder[i].getName();
#ifndef EMSCRIPTEN
        size_t loc = name.find("_") + 1;
#else
        size_t loc = name.find(" ") + 1;
#endif
        if (loc != std::string::npos)
        {
            name = name.substr(loc, 2);
        }

        GUITools::DrawString(topLeft - Position(-1, 3), name, white);
    }

    if (currentState.getCurrentFrame() < maxFinishTime)
    {
        GUITools::DrawLine(Position((PositionType)framePos, concurrent.y() - 10), Position((PositionType)framePos, concurrent.y() + (maxLayer)*(height + heightBuffer) + 10 + height), 1, white);
        GUITools::DrawString(Position((PositionType)framePos, concurrent.y() + (maxLayer)*(height + heightBuffer) + 10 + height + 20), stateTime.c_str(), white);
    }

    for (FrameCountType timeFrame = 0; timeFrame < maxFinishTime; timeFrame += 24*30)
    {
        float xPos = concurrent.x() + timeFrame*scale;
        GUITools::DrawString(Position((PositionType)xPos, concurrent.y() + (maxLayer)*(height + heightBuffer) + 10 + height + 20), getTimeString(timeFrame).c_str(), white);
    }

    Position progress = concurrent + Position(PositionType(maxWidth + 20), 0);
    Position progressBar(175,20);
    Position progressBuffer(0, 3);

    GUITools::DrawString(progress - Position(0,5), "Actions in Progress:", white);
    for (size_t a(0); a < currentState.getUnitData().getNumActionsInProgress(); ++a)
    {
        size_t index = currentState.getUnitData().getNumActionsInProgress() - a - 1;

        const ActionType actionInProgress = currentState.getUnitData().getActionInProgressByIndex(index);
        const FrameCountType finishTime = currentState.getUnitData().getActionInProgressFinishTimeByIndex(index);
        
        //DrawActionType(actionInProgress, progress, width);
        double remainingTime = (finishTime - currentState.getCurrentFrame());
        double timeRatio = remainingTime / actionInProgress.buildTime();
        Position ratioBar((PositionType)(timeRatio * progressBar.x()), progressBar.y());

        GUITools::DrawRect(progress + progressBuffer, progress + progressBuffer + progressBar, white);
        GUITools::DrawRect(progress + progressBuffer + Position(1,1), progress + progressBuffer - Position(1,1) + progressBar, grey);
        GUITools::DrawRect(progress + progressBuffer + Position(1,1), progress + progressBuffer - Position(1,1) + ratioBar, blue);

        std::stringstream time; time << actionInProgress.getName() << " " << (int)remainingTime;
        GUITools::DrawString(progress + progressBuffer + Position(10, 13), time.str(), white);
        progress.add(0, progressBar.y() + progressBuffer.y());
    }

    Position buildings(progress.x() + 195, concurrent.y());
    const BuildingData & buildingData = currentState.getBuildingData();
    GUITools::DrawString(buildings - Position(0,5), "Completed Buildings", white);
    for (size_t i(0); i < buildingData.size(); ++i)
    {
        const BuildingStatus & buildingStatus = buildingData.getBuilding(i);

        const ActionType & type = buildingStatus._type;
        const FrameCountType & finishTime = buildingStatus._timeRemaining;
        const ActionType & makingType = buildingStatus._isConstructing;

        GUITools::DrawRect(buildings + progressBuffer, buildings + progressBuffer + progressBar, white);
        GUITools::DrawRect(buildings + progressBuffer + Position(1,1), buildings + progressBuffer - Position(1,1) + progressBar, grey);

        if (finishTime > 0)
        {
            double remainingTime = finishTime;
            double timeRatio = remainingTime / makingType.buildTime();
            Position ratioBar((PositionType)(timeRatio * progressBar.x()), progressBar.y());
        
            GUITools::DrawRect(buildings + progressBuffer + Position(1,1), buildings + progressBuffer - Position(1,1) + ratioBar, blue);
            std::stringstream time; time << type.getName() << " " << (int)remainingTime;
            
            GUITools::DrawString(buildings + progressBuffer + Position(10, 13), time.str(), white);
        }
        else
        {
            GUITools::DrawString(buildings + progressBuffer + Position(10, 13), type.getName(), white);
        }
        
        buildings.add(0, progressBar.y() + progressBuffer.y());
    }

    return concurrent.y() + (maxLayer)*(height + heightBuffer) + 10 + height + 20 + 30;
}