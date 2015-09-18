#pragma once

#include "BWAPI.h"
#include <cassert>

namespace Config
{
    namespace Modules
    {
        extern bool UsingGameCommander;			
        extern bool UsingScoutManager;
        extern bool UsingCombatCommander;
        extern bool UsingUnitCommandManager;
        extern bool UsingBuildOrderSearch;  
        extern bool UsingAutoObserver;
        extern bool UsingStrategyIO;            
        extern bool UsingBuildOrderDemo;			
    }
    
    namespace BotInfo
    {
        extern std::string BotName;
        extern std::string Authors;
        extern bool PrintInfoOnStart;
    }

    namespace Strategy
    {
        extern std::string ProtossStrategyName;
        extern std::string TerranStrategyName;
        extern std::string ZergStrategyName;
        extern std::string StrategyName;
        extern std::string ReadDir;
        extern std::string WriteDir;
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
        extern bool DrawBuildOrderSearchInfo;
        extern bool DrawUnitHealthBars;
        extern bool DrawResourceInfo;
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
        extern bool UseSparcraftSimulation;

        extern bool WorkerDefense;
        extern int  WorkerDefensePerUnit;

        extern int CombatRadius;                
        extern int CombatRegroupRadius;         
        extern int UnitNearEnemyRadius;         
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