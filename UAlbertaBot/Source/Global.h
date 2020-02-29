#pragma once

#include <memory>

namespace UAlbertaBot
{

class MapTools;
class BuildingPlacerManager;
class BaseLocationManager;
class InformationManager;
class StrategyManager;
class WorkerManager;
class BOSSManager;
class ProductionManager;
class BuildingManager;
class ScoutManager;


class Global
{
    MapTools *            m_mapTools            = nullptr;
    BuildingPlacerManager * m_buildingPlacer    = nullptr;
    BaseLocationManager * m_baseLocationManager = nullptr;
    InformationManager *  m_informationManager  = nullptr;
    StrategyManager *     m_strategyManager     = nullptr;
    WorkerManager *       m_workerManager       = nullptr;
    BOSSManager *         m_bossManager         = nullptr;
    ProductionManager *   m_productionManager   = nullptr;
    BuildingManager *     m_buildingManager     = nullptr;
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
    static BuildingPlacerManager & BuildingPlacer();
    static MapTools & Map();
    static BaseLocationManager & Bases();
    static InformationManager & Info();
    static StrategyManager & Strategy();
    static WorkerManager & Workers();
    static BOSSManager & BOSS();
    static ProductionManager & Production();
    static BuildingManager & Buildings();
    static ScoutManager & Scout();
};

}


/*    MapTools *            m_mapTools;
    BuildingPlacer *      m_buildingPlacer;
    BaseLocationManager * m_baseLocationManager;
    InformationManager *  m_informationManager;
    StrategyManager *     m_strategyManager;
    WorkerManager *       m_workerManager;
    BOSSManager *         m_bossManager;
    ProductionManager *   m_productionManager;
    MapGrid *             m_mapGrid;
    BuildingManager *     m_buildingManager;
    ScoutManager *        m_scoutManager;
    */