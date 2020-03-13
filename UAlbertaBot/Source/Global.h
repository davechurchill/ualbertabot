#pragma once

#include <memory>

namespace UAlbertaBot
{

class MapTools;
class BaseLocationManager;
class InformationManager;
class StrategyManager;
class WorkerManager;
class ProductionManager;
class ScoutManager;


class Global
{
    MapTools *            m_mapTools            = nullptr;
    BaseLocationManager * m_baseLocationManager = nullptr;
    InformationManager *  m_informationManager  = nullptr;
    StrategyManager *     m_strategyManager     = nullptr;
    WorkerManager *       m_workerManager       = nullptr;
    ProductionManager *   m_productionManager   = nullptr;
    ScoutManager *        m_scoutManager        = nullptr;
    
    template <class T>
    void reset(T *& ptr)
    {
        delete ptr;
        ptr = nullptr;
    }

    template <class T>
    static T * get(T *& ptr)
    {
        if (ptr == nullptr) { ptr = new T(); }
        return ptr;
    }

    Global();
    static Global & Instance();
    void init();

public:
    
    static void GameStart();
    static MapTools & Map();
    static BaseLocationManager & Bases();
    static InformationManager & Info();
    static StrategyManager & Strategy();
    static WorkerManager & Workers();
    static ProductionManager & Production();
    static ScoutManager & Scout();
};

}
