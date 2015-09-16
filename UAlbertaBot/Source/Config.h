#pragma once

#include "BWAPI.h"
#include <cassert>

namespace Config
{
    namespace Modules
    {
        extern bool UsingGameCommander;			// toggle GameCommander, effectively UAlbertaBot
        extern bool UsingScoutManager;
        extern bool UsingCombatCommander;
        extern bool UsingUnitCommandManager;
        extern bool UsingBuildOrderSearch;				// toggle use of Build Order Search, currently no backup
        extern bool UsingStrategyIO;				// toggle the use of file io for strategy
        extern bool UsingBuildOrderDemo;			
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
        extern int GameEndFrame;	
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

        extern BWAPI::Color ColorLineTarget;
        extern BWAPI::Color ColorLineMineral;
        extern BWAPI::Color ColorUnitNearEnemy;
        extern BWAPI::Color ColorUnitNotNearEnemy;
    }

    namespace Micro
    {
        extern bool WorkerDefense;
        extern int  WorkerDefensePerUnit;

        extern int CombatRadius;						// radius of combat to consider units for Micro Search
        extern int CombatRegroupRadius;				// radius of units around frontmost unit we consider in regroup calculation
        extern int UnitNearEnemyRadius;				// radius to consider a unit 'near' to an enemy unit
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