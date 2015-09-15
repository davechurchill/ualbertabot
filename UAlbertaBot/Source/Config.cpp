#include "Config.h"
#include "UABAssert.h"

namespace Config
{
    namespace Strategy
    {
        std::string StrategyName = "Protoss_ZealotRush";
        BWAPI::Race StrategyRace = BWAPI::Races::Protoss;
    }

	namespace Modules							    // toggle various modules of UAlbertaBot, must be const
	{
        // the default tournament bot modules
		bool USING_GAMECOMMANDER		= true;	    // toggle GameCommander, effectively UAlbertaBot
		bool USING_SCOUTMANAGER			= true;
		bool USING_COMBATCOMMANDER		= true;
		bool USING_MACRO_SEARCH			= true;	    // toggle use of Build Order Search, currently no backup
		bool USING_STRATEGY_IO			= false;	// toggle the use of file io for strategy
        bool USING_UNIT_COMMAND_MGR		= false;    // handles all unit commands
		
        // extra things, don't enable unless you know what they are
		bool USING_BUILD_ORDER_DEMO		= false;
	}

    namespace BotInfo
    {
        std::string BotName = "UAlbertaBot";
        std::string Authors = "Dave Churchill";
        BWAPI::Race BotRace = BWAPI::Races::Protoss;
        bool PrintInfoOnStart = false;
    }

    namespace BWAPIOptions
    {
        int SetLocalSpeed = 42;
        int SetFrameSkip = 0;
        bool EnableUserInput = true;
        bool EnableCompleteMapInformation = false;
    }
    
	namespace Tournament						
	{
		extern const int GAME_END_FRAME			= 86400;	
	}
    
	namespace Debug								
	{
        bool PrintModuleTimeout             = true;	
        bool PrintBuildOrderSearchInfo      = true;
        bool DrawResourceInfo               = false;
        bool DrawBuildOrderInfo             = false;
        bool DrawProductionInfo             = false;
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

        std::string ErrorLogFilename            = "UAB_ErrorLog.txt";

		BWAPI::Color COLOR_LINE_TARGET			= BWAPI::Colors::White;
		BWAPI::Color COLOR_LINE_MINERAL			= BWAPI::Colors::Cyan;
		BWAPI::Color COLOR_UNIT_NEAR_ENEMY		= BWAPI::Colors::Red;
		BWAPI::Color COLOR_UNIT_NOTNEAR_ENEMY	= BWAPI::Colors::Green;
	}

	namespace Micro								// micromanagement options
	{
		bool WORKER_DEFENSE						= true;		// whether or not we defend with workers when combat units die
		int WORKER_DEFENSE_PER_UNIT				= 2;		// how many workers to assign to each unit attacking us
		
		int COMBAT_RADIUS						= 1000;		// radius of combat to consider units for Micro Search
		int COMBAT_REGROUP_RADIUS				= 300;		// radius of units around frontmost unit we consider in regroup calculation
		int UNIT_NEAR_ENEMY_RADIUS				= 600;		// radius to consider a unit 'near' to an enemy unit
		int MICRO_SEARCH_MAX_TIMER				= 0;		// maximum amount of time to allot to micro search (ms)
		int MICRO_SEARCH_MIN_TIMER				= 0;		// minimum amount of time to allot to micro search (ms)
	}

    namespace Macro
    {
        int BuildingSpacing                     = 1;
        int PylonSpacing                        = 3;
    }

	namespace Tools								// options for various tools
	{
		extern int MAP_GRID_SIZE				= 320;		// size of grid spacing in MapGrid
	}
}