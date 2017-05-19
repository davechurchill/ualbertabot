#pragma once

#include "Common.h"
#include "BuildOrderQueue.h"
#include "BuildOrder.h"

namespace UAlbertaBot
{
typedef std::pair<MetaType, size_t> MetaPair;
typedef std::vector<MetaPair> MetaPairVector;

struct Strategy
{
    std::string _name;
    BWAPI::Race _race;
    int         _wins;
    int         _losses;
    BuildOrder  _buildOrder;

    Strategy()
        : _name("None")
        , _race(BWAPI::Races::None)
        , _wins(0)
        , _losses(0)
    {
    
    }

    Strategy(const std::string & name, const BWAPI::Race & race, const BuildOrder & buildOrder)
        : _name(name)
        , _race(race)
        , _wins(0)
        , _losses(0)
        , _buildOrder(buildOrder)
    {
    
    }
};

class StrategyManager 
{
	BWAPI::Race					    _selfRace;
	BWAPI::Race					    _enemyRace;
    std::map<std::string, Strategy> _strategies;
    int                             _totalGamesPlayed;
    BuildOrder                      _emptyBuildOrder;

	        void	                writeResults();
	const	int					    getScore(BWAPI::Player player) const;
	const	bool				    shouldExpandNow() const;
    const	MetaPairVector		    getProtossBuildOrderGoal() const;
	const	MetaPairVector		    getTerranBuildOrderGoal() const;
	const	MetaPairVector		    getZergBuildOrderGoal() const;

public:
    
	StrategyManager();

            void                    update();
			void				    onEnd(const bool isWinner);
            void                    addStrategy(const std::string & name, Strategy & strategy);
            void                    setLearnedStrategy();
            void	                readResults();
	const	MetaPairVector		    getBuildOrderGoal() const;
	const	BuildOrder &            getOpeningBookBuildOrder() const;
};
}