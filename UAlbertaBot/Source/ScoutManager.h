#pragma once

#include "Common.h"
#include "MicroManager.h"
#include "InformationManager.h"

namespace UAlbertaBot
{
class ScoutManager 
{
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
    int				getClosestVertexIndex(BWAPI::Unit unit);
    BWAPI::Position	getFleePosition();
	BWAPI::Unit		getEnemyGeyser();
	BWAPI::Unit		closestEnemyWorker();
    void			fleeScout();
	void			moveScouts();
    void			drawScoutInformation(int x, int y);

	ScoutManager();

public:

    static ScoutManager & Instance();

	void update();

    void setWorkerScout(BWAPI::Unit unit);

	void onSendText(std::string text);
	void onUnitShow(BWAPI::Unit unit);
	void onUnitHide(BWAPI::Unit unit);
	void onUnitCreate(BWAPI::Unit unit);
	void onUnitRenegade(BWAPI::Unit unit);
	void onUnitDestroy(BWAPI::Unit unit);
	void onUnitMorph(BWAPI::Unit unit);
};
}