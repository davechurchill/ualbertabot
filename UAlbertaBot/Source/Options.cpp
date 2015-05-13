#include "Options.h"

namespace Options
{

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
        bool USING_REPLAY_VISUALIZER	= false;	// cannot be on while gamecommander is on
        bool USING_MICRO_SEARCH			= false;	// toggle use of Micro Search, if false script used
        bool USING_ENHANCED_INTERFACE	= false;	// toggle EnhancedUI, not needed for UAlbertaBot
		bool USING_BUILD_ORDER_DEMO		= false;
		bool USING_HIGH_LEVEL_SEARCH	= false;		// todo: update for different bot modes
        bool USING_COMBAT_PREDICTOR     = false;		// instead of SparCraft simulation, needs the training files! 
		bool USING_BATTLE_LOG			= false;		// logging all battles for future training


		void checkOptions()							// checks to see if options are set in a sane manner
		{
			if (USING_GAMECOMMANDER)
			{
				assert(!USING_ENHANCED_INTERFACE);
				assert(!USING_MICRO_SEARCH);
			}
			if (USING_HIGH_LEVEL_SEARCH){
				assert(!USING_COMBATCOMMANDER);
			}
		}
	}
    
    namespace BotModes
    {
        int CURRENT_BOT_MODE = BotModes::AIIDE_TOURNAMENT;

        void SetBotMode(int mode)
        {
            if (mode < BotModes::NUM_MODES)
            {
                CURRENT_BOT_MODE = mode;
            }
            else
            {
                exit(0);
            }

            if (mode == BotModes::AIIDE_TOURNAMENT)
            {
                Modules::USING_GAMECOMMANDER		= true;	
				Modules::USING_SCOUTMANAGER			= true;	
				Modules::USING_COMBATCOMMANDER		= true;
		        Modules::USING_MACRO_SEARCH			= true;	
		        Modules::USING_STRATEGY_IO			= false;
                Modules::USING_UNIT_COMMAND_MGR		= false; 
		
                Modules::USING_REPLAY_VISUALIZER	= false;
                Modules::USING_MICRO_SEARCH			= false; 
                Modules::USING_ENHANCED_INTERFACE	= false;
            }
            else if (mode == BotModes::CIG_TOURNAMENT)
            {
                Modules::USING_GAMECOMMANDER		= true;	
				Modules::USING_SCOUTMANAGER			= true;	
				Modules::USING_COMBATCOMMANDER		= true;
		        Modules::USING_MACRO_SEARCH			= true;	
		        Modules::USING_STRATEGY_IO			= false;
                Modules::USING_UNIT_COMMAND_MGR		= false; 
		
                Modules::USING_REPLAY_VISUALIZER	= false;
                Modules::USING_MICRO_SEARCH			= false;
                Modules::USING_ENHANCED_INTERFACE	= false;
            }
            else if (mode == BotModes::MICRO_SEARCH_TEST)
            {
                Modules::USING_GAMECOMMANDER		= false;	
				Modules::USING_SCOUTMANAGER			= false;	
				Modules::USING_COMBATCOMMANDER		= false;
		        Modules::USING_MACRO_SEARCH			= false;	
		        Modules::USING_STRATEGY_IO			= false;
                Modules::USING_UNIT_COMMAND_MGR		= true; 
		
                Modules::USING_REPLAY_VISUALIZER	= false;
                Modules::USING_MICRO_SEARCH			= true;
                Modules::USING_ENHANCED_INTERFACE	= false;
            }
            else if (mode == BotModes::REPLAY_VIS_TEST)
            {
                Modules::USING_GAMECOMMANDER		= false;	
				Modules::USING_SCOUTMANAGER			= false;	
				Modules::USING_COMBATCOMMANDER		= false;
		        Modules::USING_MACRO_SEARCH			= false;	
		        Modules::USING_STRATEGY_IO			= false;
                Modules::USING_UNIT_COMMAND_MGR		= false; 
		
                Modules::USING_REPLAY_VISUALIZER	= true;
                Modules::USING_MICRO_SEARCH			= false;
                Modules::USING_ENHANCED_INTERFACE	= false;
            }
			else if (mode == BotModes::BUILD_ORDER_DEMO)
            {
                Modules::USING_GAMECOMMANDER		= true;	
				Modules::USING_SCOUTMANAGER			= true;	
				Modules::USING_COMBATCOMMANDER		= true;
		        Modules::USING_MACRO_SEARCH			= true;	
		        Modules::USING_STRATEGY_IO			= false;
                Modules::USING_UNIT_COMMAND_MGR		= true; 
		
                Modules::USING_REPLAY_VISUALIZER	= false;
                Modules::USING_MICRO_SEARCH			= false;
                Modules::USING_ENHANCED_INTERFACE	= false;
				Modules::USING_BUILD_ORDER_DEMO		= true;
            }
        }
    }

	namespace Tournament						// settings for the AIIDE tournament
	{
		extern const int GAME_END_FRAME			= 86400;	
	}

	namespace FileIO							// settings for file read/write
	{
		extern const char * FILE_SETTINGS		= "file_settings.txt";
	}

	namespace Debug								// debugging options
	{
		bool DRAW_UALBERTABOT_DEBUG				= true;	// draws debug information for UAlbertaBot
		bool DRAW_DEBUG_INTERFACE				= false;	    // draws debug information for EnhancedUI

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

	namespace Tools								// options for various tools
	{
		extern int MAP_GRID_SIZE				= 320;		// size of grid spacing in MapGrid
	}
}