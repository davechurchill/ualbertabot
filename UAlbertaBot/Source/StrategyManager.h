#pragma once

#include "Common.h"
#include "BWTA.h"
#include "BuildOrderQueue.h"
#include "InformationManager.h"
#include "WorkerManager.h"
#include <sys/stat.h>
#include <cstdlib>
#include "OpeningBuildOrders.h"

namespace UAlbertaBot
{

typedef std::pair<int, int> IntPair;
typedef std::pair<MetaType, size_t> MetaPair;
typedef std::vector<MetaPair> MetaPairVector;

class StrategyManager 
{
	StrategyManager();
	~StrategyManager() {}

   /* std::vector<std::string>	protossOpeningBook;
	std::vector<std::string>	terranOpeningBook;
	std::vector<std::string>	zergOpeningBook;*/

	std::string					readDir;
	std::string					writeDir;
	std::vector<IntPair>		results;
	std::vector<int>			usableStrategies;
	int							currentStrategy;

	BWAPI::Race					selfRace;
	BWAPI::Race					enemyRace;

	bool						firstAttackSent;

	void	addStrategies();
	void	setStrategy();
	void	readResults();
	void	writeResults();

	const	int					getScore(BWAPI::PlayerInterface * player) const;
	const	double				getUCBValue(const size_t & strategy) const;
	
	// protoss strategy
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

	enum { ProtossZealotRush=0, ProtossDarkTemplar=1, ProtossDragoons=2, NumProtossStrategies=3 };
	enum { TerranMarineRush=0, NumTerranStrategies=1 };
	enum { ZergZerglingRush=0, NumZergStrategies=1 };

	static	StrategyManager &	Instance();

			void				onEnd(const bool isWinner);
	
	const	bool				regroup(int numInRadius);
	const	bool				doAttack(const std::set<BWAPI::UnitInterface*> & freeUnits);
	const	int				    defendWithWorkers();
	const	bool				rushDetected();

	const	int					getCurrentStrategy();

	const	MetaPairVector		getBuildOrderGoal();
	const	std::vector<MetaType>	getOpeningBookBuildOrder() const;
};
}