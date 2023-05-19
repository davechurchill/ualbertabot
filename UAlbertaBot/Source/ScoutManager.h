#pragma once

#include "Common.h"
#include <BaseLocation.h>
#include "bwem.h"

namespace UAlbertaBot
{
class ScoutManager
{
    friend class Global;

    BWAPI::Unit		m_workerScout       = nullptr;
    std::string		m_scoutStatus       = "None";
    std::string		m_gasStealStatus    = "None";
    int				m_numWorkerScouts   = 0;
    int				m_previousScoutHP   = 0;
    bool			m_scoutUnderAttack  = false;
    bool			m_didGasSteal       = false;
    bool			m_gasStealFinished  = false;
    bool            m_rushFalse         = true;

    bool enemyWorkerInRadius();
    bool immediateThreat();
    void gasSteal();
    void fleeScout();
    void moveScouts();
    void drawScoutInformation(int x, int y);
    void followPerimeter(const BaseLocation* enemyBaseLocation);
    void rushWarning();
    //void gettingRushed();
    BWAPI::Position	getFleePosition();
    BWAPI::Unit		getEnemyGeyser();
    BWAPI::Unit		closestEnemyWorker();

    ScoutManager();

public:

    void update();
    int lap = 0;

    void setWorkerScout(BWAPI::Unit unit);
};
}