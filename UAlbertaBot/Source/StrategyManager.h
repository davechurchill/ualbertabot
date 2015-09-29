#pragma once

#include "Common.h"
#include "BWTA.h"
#include "BuildOrderQueue.h"
#include "InformationManager.h"
#include "WorkerManager.h"
#include "BuildOrder.h"

namespace UAlbertaBot
{
typedef std::pair<MetaType, size_t> MetaPair;
typedef std::vector<MetaPair> MetaPairVector;

struct Strategy
{
    std::string name;
    BWAPI::Race race;
    BuildOrder buildOrder;
};

class StrategyManager 
{
	StrategyManager();

	BWAPI::Race					        _selfRace;
	BWAPI::Race					        _enemyRace;
    std::map<std::string, Strategy>     _openingBuildOrders;
    BuildOrder                          _emptyBuildOrder;

    std::map<std::string, std::pair<int, int>> _results;

	bool						    firstAttackSent;

	void	setStrategy();
	void	readResults();
	void	writeResults();

	const	int					    getScore(BWAPI::Player player) const;
	const	double				    getUCBValue(const size_t & strategy) const;
	
	const	bool				    shouldExpandNow() const;

    const	MetaPairVector		    getProtossBuildOrderGoal() const;
	const	MetaPairVector		    getTerranBuildOrderGoal() const;
	const	MetaPairVector		    getZergBuildOrderGoal() const;

public:
    
	static	StrategyManager &	    Instance();

			void				    onEnd(const bool isWinner);
            void                    addStrategy(const std::string & name, Strategy & strategy);
	
	const	bool				    regroup(int numInRadius);
	const	bool				    doAttack(const std::set<BWAPI::Unit> & freeUnits);
	const	int				        defendWithWorkers();
	const	bool				    rushDetected();

	const	int					    getCurrentStrategy();

	const	MetaPairVector		    getBuildOrderGoal();
	const	BuildOrder &            getOpeningBookBuildOrder() const;
};
}