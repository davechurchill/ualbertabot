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
    /*std::shared_ptr<MapTools>            m_mapTools;
    std::shared_ptr<BuildingPlacer>      m_buildingPlacer;
    std::shared_ptr<BaseLocationManager> m_baseLocationManager;
    std::shared_ptr<InformationManager>  m_informationManager;
    std::shared_ptr<StrategyManager>     m_strategyManager;
    std::shared_ptr<WorkerManager>       m_workerManager;
    std::shared_ptr<BOSSManager>         m_bossManager;
    std::shared_ptr<ProductionManager>   m_productionManager;
    std::shared_ptr<BuildingManager>     m_buildingManager;
    std::shared_ptr<ScoutManager>        m_scoutManager;*/
    
    Global();
    void init();
    static Global & Instance();

public:

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