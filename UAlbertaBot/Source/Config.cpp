#include "Config.h"
#include "UABAssert.h"

namespace Config
{
    namespace Strategy
    {
        std::string ProtossStrategyName     = "Protoss_ZealotRush";
        std::string TerranStrategyName      = "Terran_MarineRush";
        std::string ZergStrategyName        = "Zerg_3HatchMuta";
        std::string StrategyName            = "Protoss_ZealotRush";
        std::string ReadDir                 = "bwapi-data/read/";
        std::string WriteDir                = "bwapi-data/write/";
    }

    namespace Modules							    
    {
        // the default tournament bot modules
        bool UsingGameCommander             = true;     // toggle GameCommander, effectively UAlbertaBot
        bool UsingScoutManager              = true;
        bool UsingCombatCommander           = true;
        bool UsingBuildOrderSearch          = true;     // toggle use of Build Order Search, currently no backup
        bool UsingAutoObserver              = false;
        bool UsingStrategyIO                = false;    // toggle the use of file io for strategy
        bool UsingUnitCommandManager        = false;    // handles all unit commands
		
        // extra things, don't enable unless you know what they are
        bool UsingBuildOrderDemo            = false;
    }

    namespace BotInfo
    {
        std::string BotName                 = "UAlbertaBot";
        std::string Authors                 = "Dave Churchill";
        bool PrintInfoOnStart               = false;
    }

    namespace BWAPIOptions
    {
        int SetLocalSpeed                   = 42;
        int SetFrameSkip                    = 0;
        bool EnableUserInput                = true;
        bool EnableCompleteMapInformation   = false;
    }
    
    namespace Tournament						
    {
        int GameEndFrame                    = 86400;	
    }
    
    namespace Debug								
    {
        bool PrintModuleTimeout             = true;	
        bool DrawBuildOrderSearchInfo       = true;
        bool DrawUnitHealthBars             = true;
        bool DrawProductionInfo             = true;
        bool DrawResourceInfo               = false;
        bool DrawWorkerInfo                 = false;
        bool DrawModuleTimers               = false;
        bool DrawReservedBuildingTiles      = false;
        bool DrawCombatSimulationInfo       = false;
        bool DrawBuildingInfo               = false;
        bool DrawMouseCursorInfo            = false;
        bool DrawEnemyUnitInfo              = false;
        bool DrawBWTAInfo                   = false;
        bool DrawMapGrid                    = false;
        bool DrawUnitTargetInfo             = false;
        bool DrawSquadInfo                  = false;
        bool AllDebugOff                    = false;

        std::string ErrorLogFilename        = "UAB_ErrorLog.txt";

        BWAPI::Color ColorLineTarget        = BWAPI::Colors::White;
        BWAPI::Color ColorLineMineral       = BWAPI::Colors::Cyan;
        BWAPI::Color ColorUnitNearEnemy     = BWAPI::Colors::Red;
        BWAPI::Color ColorUnitNotNearEnemy  = BWAPI::Colors::Green;
    }

    namespace Micro								
    {
        bool UseSparcraftSimulation         = true;
        bool WorkerDefense                  = true;     // whether or not we defend with workers when combat units die
        int WorkerDefensePerUnit            = 2;        // how many workers to assign to each unit attacking us
		
        int CombatRadius                    = 1000;     // radius of combat to consider units for Micro Search
        int CombatRegroupRadius             = 300;      // radius of units around frontmost unit we consider in regroup calculation
        int UnitNearEnemyRadius             = 600;      // radius to consider a unit 'near' to an enemy unit
    }

    namespace Macro
    {
        int BuildingSpacing                 = 1;
        int PylonSpacing                    = 3;
    }

    namespace Tools								
    {
        extern int MAP_GRID_SIZE            = 320;      // size of grid spacing in MapGrid
    }
}