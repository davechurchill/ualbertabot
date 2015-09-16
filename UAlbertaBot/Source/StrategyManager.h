#pragma once

#include "Common.h"
#include "BWTA.h"
#include "BuildOrderQueue.h"
#include "InformationManager.h"
#include "WorkerManager.h"
#include "BuildOrder.h"

namespace UAlbertaBot
{

typedef std::pair<int, int> IntPair;
typedef std::pair<MetaType, size_t> MetaPair;
typedef std::vector<MetaPair> MetaPairVector;

class StrategyManager 
{
	StrategyManager();
	~StrategyManager() {}

	BWAPI::Race					        _selfRace;
	BWAPI::Race					        _enemyRace;
    std::map<std::string, BuildOrder>   _openingBuildOrders;
    BuildOrder                          _emptyBuildOrder;

    std::map<std::string, std::pair<int, int>> _results;

	bool						firstAttackSent;

	void	setStrategy();
	void	readResults();
	void	writeResults();

	const	int					getScore(BWAPI::PlayerInterface * player) const;
	const	double				getUCBValue(const size_t & strategy) const;
	
	// strategy functions, add your own here
	const	bool				expandProtossZealotRush() const;
	const	std::string			getProtossZealotRushOpeningBook() const;
	const	MetaPairVector		getProtossZealotRushBuildOrderGoal() const;

	const	bool				expandProtossDarkTemplar() const;
	const	std::string			getProtossDarkTemplarOpeningBook() const;
	const	MetaPairVector		getProtossDarkTemplarBuildOrderGoal() const;

	const	bool				expandProtossDragoons() const;
	const	std::string			getProtossDragoonsOpeningBook() const;
	const	MetaPairVector		getProtossDragoonsBuildOrderGoal() const;

	const	MetaPairVector		getTerranBuildOrderGoal() const;
	const	MetaPairVector		getZergBuildOrderGoal() const;

	const	MetaPairVector		getProtossOpeningBook() const;
	const	MetaPairVector		getTerranOpeningBook() const;
	const	MetaPairVector		getZergOpeningBook() const;

public:
    
	static	StrategyManager &	    Instance();

			void				    onEnd(const bool isWinner);
            void                    addOpeningBuildOrder(const std::string & name, BuildOrder & buildOrder);
	
	const	bool				    regroup(int numInRadius);
	const	bool				    doAttack(const std::set<BWAPI::UnitInterface*> & freeUnits);
	const	int				        defendWithWorkers();
	const	bool				    rushDetected();

	const	int					    getCurrentStrategy();

	const	MetaPairVector		    getBuildOrderGoal();
	const	BuildOrder &            getOpeningBookBuildOrder() const;
};
}