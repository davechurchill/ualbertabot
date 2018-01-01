#pragma once

#include "BotModule.h"
#include "GameCommander.h"
#include <iostream>
#include <fstream>
#include "HardCodedInfo.h"
#include "AutoObserver.h"
#include "rapidjson\document.h"
#include "BOSSManager.h"
#include "debug\GameDebug.h"
#include "ProductionManager.h"
#include "CombatCommander.h"
#include "BOSSManager.h"
#include "BWAPIScreenCanvas.h"
#include "commands\CommandManager.h"

namespace UAlbertaBot
{
	using AKBot::CommandManager;

class UAlbertaBot_Tournament : public BotModule
{
	BotConfiguration& _configuration;
	shared_ptr<MapTools> _mapTools;
	shared_ptr<BaseLocationManager> _baseLocationManager;
	shared_ptr<AutoObserver> _autoObserver;

	shared_ptr<GameCommander>       _gameCommander;
	shared_ptr<UnitInfoManager>     _unitInfoManager;
	shared_ptr<StrategyManager> _strategyManager;
	shared_ptr<CombatCommander> _combatCommander;
	AKBot::BWAPIScreenCanvas _canvas;
	shared_ptr<AKBot::GameDebug> _gameDebug;
	CommandManager _commandManager;

	void drawDebugInformation(AKBot::ScreenCanvas& _canvas);
public:

    UAlbertaBot_Tournament(
		BotConfiguration& configuration,
		shared_ptr<BaseLocationManager> baseLocationManager,
		shared_ptr<AutoObserver> autoObserver,
		shared_ptr<StrategyManager> strategyManager,
		shared_ptr<UnitInfoManager> unitInfoManager,
		shared_ptr<MapTools> mapTools,
		shared_ptr<CombatCommander> combatManager,
		shared_ptr<GameCommander> gameCommander,
		shared_ptr<AKBot::GameDebug> gameDebug);
	UAlbertaBot_Tournament(const UAlbertaBot_Tournament&) = delete;
    ~UAlbertaBot_Tournament();

    void	onStart() override;
    void	onFrame() override;
    void	onEnd(bool isWinner) override;
    void	onUnitDestroy(BWAPI::Unit unit) override;
    void	onUnitMorph(BWAPI::Unit unit) override;
    void	onSendText(std::string text) override;
    void	onUnitCreate(BWAPI::Unit unit) override;
    void	onUnitComplete(BWAPI::Unit unit) override;
    void	onUnitShow(BWAPI::Unit unit) override;
    void	onUnitHide(BWAPI::Unit unit) override;
    void	onUnitRenegade(BWAPI::Unit unit) override;

    const shared_ptr<UnitInfoManager> UnitInfo() const;
	AKBot::ScreenCanvas& getCanvas();
};

}