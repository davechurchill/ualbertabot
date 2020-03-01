#pragma once

#include "Common.h"

namespace UAlbertaBot
{
class ScoutManager 
{
    friend class Global;

	BWAPI::Unit		m_workerScout;
    std::string		m_scoutStatus;
    std::string		m_gasStealStatus;
	int				m_numWorkerScouts;
	bool			m_scoutUnderAttack;
    bool			m_didGasSteal;
    bool			m_gasStealFinished;
    int				m_previousScoutHP;

	bool			enemyWorkerInRadius();
    bool			immediateThreat();
    void			gasSteal();
    BWAPI::Position	getFleePosition();
	BWAPI::Unit		getEnemyGeyser();
	BWAPI::Unit		closestEnemyWorker();
    void			fleeScout();
	void			moveScouts();
    void			drawScoutInformation(int x, int y);

	ScoutManager();

public:

	void update();

    void setWorkerScout(BWAPI::Unit unit);
};
}