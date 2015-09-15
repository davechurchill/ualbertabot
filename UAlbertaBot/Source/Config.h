#pragma once

#include "BWAPI.h"
#include <cassert>

namespace Config
{
	namespace Modules
	{
		extern bool USING_GAMECOMMANDER;			// toggle GameCommander, effectively UAlbertaBot
		extern bool USING_SCOUTMANAGER;
		extern bool USING_COMBATCOMMANDER;
		extern bool USING_UNIT_COMMAND_MGR;
		extern bool USING_MACRO_SEARCH;				// toggle use of Build Order Search, currently no backup
		extern bool USING_STRATEGY_IO;				// toggle the use of file io for strategy
		extern bool USING_BUILD_ORDER_DEMO;			
	}
    
    namespace BotInfo
    {
        extern std::string BotName;
        extern std::string Authors;
        extern BWAPI::Race BotRace;
        extern bool PrintInfoOnStart;
    }

    namespace Strategy
    {
        extern BWAPI::Race StrategyRace;
        extern std::string StrategyName;
    }

    namespace BWAPIOptions
    {
        extern int SetLocalSpeed;
        extern int SetFrameSkip;
        extern bool EnableUserInput;
        extern bool EnableCompleteMapInformation;
    }

	namespace Tournament
	{
		extern const int GAME_END_FRAME;	
	}

	namespace FileIO
	{
		extern const char * FILE_SETTINGS;				
	}

	namespace Debug
	{
        extern bool PrintModuleTimeout;				
        extern bool PrintBuildOrderSearchInfo;
        extern bool DrawResourceInfo;
        extern bool DrawBuildOrderInfo;
        extern bool DrawProductionInfo;
        extern bool DrawWorkerInfo;
        extern bool DrawModuleTimers;
        extern bool DrawReservedBuildingTiles;
        extern bool DrawCombatSimulationInfo;
        extern bool DrawBuildingInfo;
        extern bool DrawMouseCursorInfo;
        extern bool DrawEnemyUnitInfo;
        extern bool DrawBWTAInfo;
        extern bool DrawMapGrid;
        extern bool DrawUnitTargetInfo;
        extern bool DrawSquadInfo;
        extern bool AllDebugOff;

        extern std::string ErrorLogFilename;

		extern BWAPI::Color COLOR_LINE_TARGET;
		extern BWAPI::Color COLOR_LINE_MINERAL;
		extern BWAPI::Color COLOR_UNIT_NEAR_ENEMY;
		extern BWAPI::Color COLOR_UNIT_NOTNEAR_ENEMY;
	}

	namespace Micro
	{
		extern bool WORKER_DEFENSE;
		extern int  WORKER_DEFENSE_PER_UNIT;

		extern int COMBAT_RADIUS;						// radius of combat to consider units for Micro Search
		extern int COMBAT_REGROUP_RADIUS;				// radius of units around frontmost unit we consider in regroup calculation
		extern int UNIT_NEAR_ENEMY_RADIUS;				// radius to consider a unit 'near' to an enemy unit
		extern int MICRO_SEARCH_MAX_TIMER;
		extern int MICRO_SEARCH_MIN_TIMER;
	}

    namespace Macro
    {
        extern int BuildingSpacing;
        extern int PylonSpacing;
    }

	namespace Tools
	{
		extern int MAP_GRID_SIZE;
	}
}