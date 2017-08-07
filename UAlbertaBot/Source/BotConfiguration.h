#pragma once
#include <BWAPI/UnitType.h>
#include <BWAPI/Color.h>
#include <cassert>
#include <set>
#include <string>

struct BotInfoConfiguration
{
	std::string BotName = "Andrey Kurdiumov";
	std::string Authors = "Andrey Kurdiumov, Dave Churchill";
	bool PrintInfoOnStart = false;
	std::string BotMode = "Tournament"; // must be 'Tournament' or 'Arena'
};

struct BotModulesConfiguration
{
	bool UsingBuildOrderSearch = true;     // toggle use of Build Order Search, currently no backup
	bool UsingAutoObserver = false;
	bool UsingStrategyIO = false;    // toggle the use of file io for strategy
	
	// extra things, don't enable unless you know what they are
	bool UsingBuildOrderDemo = false;
};

struct BotArenaConfiguration
{
	std::string ArenaPlayerName = "AttackWC_NOK";
	int ArenaOutputResults = 10;
	int ArenaBattles = 1000;
	std::string SparCraftPlayerName;
};

struct BotSparCraftConfiguration
{
	std::string SparCraftConfigFile = "SparCraft_Config.txt";
	std::string CombatSimPlayerName = "AttackC";
};

struct BotStrategyConfiguration
{
	std::string ProtossStrategyName = "Protoss_ZealotRush";
	std::string TerranStrategyName = "Terran_MarineRush";
	std::string ZergStrategyName = "Zerg_3HatchMuta";
	std::string StrategyName = "Protoss_ZealotRush";
	std::string ReadDir = "bwapi-data/read/";
	std::string WriteDir = "bwapi-data/write/";
	bool ScoutHarassEnemy = true;
	bool UseEnemySpecificStrategy = false;
	bool FoundEnemySpecificStrategy = false;
};

struct BotBWAPIOptionsConfiguration
{
	int SetLocalSpeed = 42;
	int SetFrameSkip = 0;
	bool EnableUserInput = true;
	bool EnableCompleteMapInformation = false;
};

struct BotTournamentConfiguration
{
	int GameEndFrame = 86400;
};

struct BotDebugConfiguration
{
	bool DrawGameInfo = true;
	bool DrawUnitHealthBars = true;
	bool DrawProductionInfo = true;
	bool DrawBuildOrderSearchInfo = false;
	bool DrawScoutInfo = false;
	bool DrawResourceInfo = false;
	bool DrawWorkerInfo = false;
	bool DrawModuleTimers = false;
	bool DrawReservedBuildingTiles = false;
	bool DrawCombatSimulationInfo = false;
	bool DrawBuildingInfo = false;
	bool DrawMouseCursorInfo = false;
	bool DrawEnemyUnitInfo = false;
	bool DrawBWTAInfo = false;
	bool DrawLastSeenTileInfo = false;
	bool DrawUnitTargetInfo = false;
	bool DrawSquadInfo = false;
	bool DrawBOSSStateInfo = false;
	bool PrintModuleTimeout = false;

	std::string ErrorLogFilename = "AKBot_ErrorLog.txt";
	bool LogAssertToErrorFile = false;

	BWAPI::Color ColorLineTarget = BWAPI::Colors::White;
	BWAPI::Color ColorLineMineral = BWAPI::Colors::Cyan;
	BWAPI::Color ColorUnitNearEnemy = BWAPI::Colors::Red;
	BWAPI::Color ColorUnitNotNearEnemy = BWAPI::Colors::Green;
};

struct BotMicroConfiguration
{
	bool UseSparcraftSimulation = true;
	bool KiteWithRangedUnits = true;
	std::set<BWAPI::UnitType> KiteLongerRangedUnits;
	bool WorkersDefendRush = false;
	int RetreatMeleeUnitShields = 0;
	int RetreatMeleeUnitHP = 0;
	int CombatRadius = 1000;     // radius of combat to consider units for Micro Search
	int CombatRegroupRadius = 300;      // radius of units around frontmost unit we consider in regroup calculation
	int UnitNearEnemyRadius = 600;      // radius to consider a unit 'near' to an enemy unit
};

struct BotMacroConfiguration
{
	int BOSSFrameLimit = 160;
	int WorkersPerRefinery = 3;
	int BuildingSpacing = 1;
	int PylonSpacing = 3;
};

struct BotToolsConfiguration
{
	int MAP_GRID_SIZE = 320;      // size of grid spacing in MapGrid
};

struct BotConfiguration
{
	BotModulesConfiguration Modules;
	BotInfoConfiguration BotInfo;
	BotArenaConfiguration Arena;
	BotSparCraftConfiguration SparCraft;
	BotStrategyConfiguration Strategy;
	BotBWAPIOptionsConfiguration BWAPIOptions;
	BotTournamentConfiguration Tournament;
	BotDebugConfiguration Debug;
	BotMicroConfiguration Micro;
	BotMacroConfiguration Macro;
	BotToolsConfiguration Tools;
};
