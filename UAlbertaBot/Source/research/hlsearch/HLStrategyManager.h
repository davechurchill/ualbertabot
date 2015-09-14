#pragma once

#include "Common.h"
#include "BWTA.h"
#include "BuildOrderQueue.h"
#include "InformationManager.h"
#include "WorkerManager.h"
#include <sys/stat.h>
#include <cstdlib>
#include "OpeningBuildOrders.h"
#include "HLUnitData.h"

namespace UAlbertaBot
{

typedef std::pair<int, int> IntPair;
typedef std::pair<MetaType, size_t> MetaPair;
typedef std::vector<MetaPair> MetaPairVector;

class StrategyManager 
{
	StrategyManager();
	~StrategyManager() {}

	std::string					readDir;
	std::string					writeDir;
	std::vector<IntPair>		results;
	std::vector<int>			usableStrategies;
	int							currentStrategy;
	std::unordered_map<int ,std::unordered_map<int, std::unordered_map<short, short> > > strategyChoices;//race->strategy->choicepoint->option
	static std::unordered_map<int, std::unordered_map<int, std::unordered_map<short, short> > > defaultStrategyChoices;

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
	static bool					expandProtossZealotRush(
		const HLUnitData &selfUnitData,
		const WorkerData &selfWorkerData,
		int frame,
		int frameAdjust = 0,
		int zealotAdjust = 0);
	const	MetaPairVector		getProtossZealotRushBuildOrderGoal() const;
	static	MetaPairVector		getProtossZealotRushBuildOrderGoal(
		const HLUnitData &selfUnitData,
		const HLUnitData &enemyUnitData,
		const WorkerData &selfWorkerData,
		int frame);

	const	MetaPairVector		getProtossDarkTemplarBuildOrderGoal() const;
	static	MetaPairVector		getProtossDarkTemplarBuildOrderGoal(
		const HLUnitData &selfUnitData,
		const HLUnitData &enemyUnitData,
		const WorkerData &selfWorkerData,
		int frame);

	const	MetaPairVector		getProtossDragoonsBuildOrderGoal() const;
	static 	MetaPairVector		getProtossDragoonsBuildOrderGoal(
		const HLUnitData &selfUnitData,
		const HLUnitData &enemyUnitData,
		const WorkerData &selfWorkerData,
		int frame);

	const	MetaPairVector		getProtossHighLevelSearchBuildOrderGoal() const;
	static	MetaPairVector		getProtossHighLevelSearchBuildOrderGoal(
		const HLUnitData &selfUnitData,
		const HLUnitData &enemyUnitData,
		const WorkerData &selfWorkerData,
		int frame,
		const std::unordered_map<short, short> &choices,
		bool useDefaultChoicesIfNeeded);

	const	MetaPairVector		getTerranBuildOrderGoal() const;
	static 	MetaPairVector		getTerranBuildOrderGoal(
		const HLUnitData &selfUnitData,
		const HLUnitData &enemyUnitData,
		const WorkerData &selfWorkerData,
		int frame); 
	
	const	MetaPairVector		getZergBuildOrderGoal() const;
	static 	MetaPairVector		getZergBuildOrderGoal(
		const HLUnitData &selfUnitData,
		const HLUnitData &enemyUnitData,
		const WorkerData &selfWorkerData,
		int frame);

	//const	MetaPairVector		getProtossOpeningBook() const;
	//const	MetaPairVector		getTerranOpeningBook() const;
	//const	MetaPairVector		getZergOpeningBook() const;

public:

	enum { ProtossZealotRush=0, ProtossDarkTemplar=1, ProtossDragoons=2, ProtossHighLevelSearch=3, NumProtossStrategies=4 };
	enum { TerranMarineRush=0, NumTerranStrategies=1 };
	enum { ZergZerglingRush=0, NumZergStrategies=1 };

	static	StrategyManager &	Instance();

			void				onEnd(const bool isWinner);
	
	const	bool				regroup(int numInRadius);
	const	bool				doAttack(const std::set<BWAPI::UnitInterface*> & freeUnits);
	const	int				    defendWithWorkers();
	const	bool				rushDetected();

	const	int					getCurrentStrategy() const;
	static int					getNumStrategies(BWAPI::Race race);
	void						setCurrentStrategy(int newStrategy, const std::unordered_map<short, short> &choices);//for high level search to set the strategy
	const	MetaPairVector		getBuildOrderGoal();
	const	std::vector<MetaType>	getOpeningBookBuildOrder() const;

	static std::vector<MetaType>	getOpeningBookBuildOrder(int strategy, BWAPI::Race race);
	static MetaPairVector		getBuildOrderGoal(
		const HLUnitData &selfUnitData,
		const HLUnitData &enemyUnitData,
		const WorkerData &selfWorkerData,
		int frame,
		int strategy,
		BWAPI::Race race,
		const std::unordered_map<short, short> &choices);
};
struct ChoicePoint{
	short _point, _options;
	ChoicePoint(short point, short options) :_point(point), _options(options){};
};
}