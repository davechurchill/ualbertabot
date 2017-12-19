#include "Common.h"
#include "UAlbertaBot_Tournament.h"
#include "JSONTools.h"
#include "ParseUtils.h"
#include "UnitUtil.h"
#include "Micro.h"
#include "BotFactory.h"
#include <commands\BWAPICommandExecutor.h>
#include <commands\UnitCommandExecutor.h>
#include <commands\ProductionCommandExecutor.h>
#include <commands\WorkerCommandExecutor.h>
#include <commands\BaseLocationCommandExecutor.h>
#include <commands\StrategyManagerCommandExecutor.h>
#include <commands\MicroCommandExecutor.h>
#include <commands\CombatCommanderCommandExecutor.h>

using namespace UAlbertaBot;
using namespace AKBot;

UAlbertaBot_Tournament::UAlbertaBot_Tournament(
	BotConfiguration& configuration,
	shared_ptr<BaseLocationManager> baseLocationManager,
	shared_ptr<AutoObserver> autoObserver,
	shared_ptr<StrategyManager> strategyManager,
	shared_ptr<UnitInfoManager> unitInfoManager,
	shared_ptr<MapTools> mapTools,
	shared_ptr<CombatCommander> combatManager,
	shared_ptr<GameCommander> gameCommander,
	shared_ptr<AKBot::GameDebug> gameDebug)
	: _configuration(configuration)
	, _gameDebug(std::move(gameDebug))
	, _baseLocationManager(std::move(baseLocationManager))
	, _autoObserver(std::move(autoObserver))
	, _unitInfoManager(std::move(unitInfoManager))
	, _strategyManager(std::move(strategyManager))
	, _mapTools(std::move(mapTools))
	, _combatCommander(std::move(combatManager))
	, _gameCommander(std::move(gameCommander))
{
	// parse the configuration file for the bot's strategies

	auto unitCommandExecutor = std::make_unique<UnitCommandExecutor>(configuration.Debug);
	_commandManager.registerExecutor(std::move(unitCommandExecutor));
	auto productionCommandExecutor = std::make_unique<ProductionCommandExecutor>(configuration.Debug);
	_commandManager.registerExecutor(std::move(productionCommandExecutor));
	auto workerCommandExecutor = std::make_unique<WorkerCommandExecutor>(configuration.Debug);
	_commandManager.registerExecutor(std::move(workerCommandExecutor));
	auto baseLocationCommandExecutor = std::make_unique<BaseLocationCommandExecutor>(configuration.Debug);
	_commandManager.registerExecutor(std::move(baseLocationCommandExecutor));
	auto strategyManagerCommandExecutor = std::make_unique<StrategyManagerCommandExecutor>(*_strategyManager.get());
	_commandManager.registerExecutor(std::move(strategyManagerCommandExecutor));
	auto microCommandExecutor = std::make_unique<MicroCommandExecutor>(configuration.Micro);
	_commandManager.registerExecutor(std::move(microCommandExecutor));
	auto combatCommanderCommandExecutor = std::make_unique<CombatCommanderCommandExecutor>(configuration.Debug);
	_commandManager.registerExecutor(std::move(combatCommanderCommandExecutor));

	auto executor = std::make_unique<BWAPICommandExecutor>(BWAPI::BroodwarPtr);
	_commandManager.registerExecutor(std::move(executor));
}

UAlbertaBot_Tournament::~UAlbertaBot_Tournament()
{
}

// This gets called when the bot starts!
void UAlbertaBot_Tournament::onStart()
{
	// Initialize SparCraft, the combat simulation package
	SparCraft::init();
	auto configurationFile = ParseUtils::FindConfigurationLocation(_configuration.SparCraft.SparCraftConfigFile);
	SparCraft::AIParameters::Instance().parseFile(configurationFile);

	// Initialize BOSS, the Build Order Search System
	BOSS::init();

	// Set our BWAPI options here    
	auto& bwapiOptions = _configuration.BWAPIOptions;
	BWAPI::Broodwar->setLocalSpeed(bwapiOptions.SetLocalSpeed);
	BWAPI::Broodwar->setFrameSkip(bwapiOptions.SetFrameSkip);

	if (bwapiOptions.EnableCompleteMapInformation)
	{
		BWAPI::Broodwar->enableFlag(BWAPI::Flag::CompleteMapInformation);
	}

	if (bwapiOptions.EnableUserInput)
	{
		BWAPI::Broodwar->enableFlag(BWAPI::Flag::UserInput);
	}

	auto& botInfoOptions = _configuration.BotInfo;
	if (botInfoOptions.PrintInfoOnStart)
	{
		BWAPI::Broodwar->printf("Hello! I am %s, written by %s", botInfoOptions.BotName.c_str(), botInfoOptions.Authors.c_str());
	}

	if (_configuration.Strategy.UsingStrategyIO)
	{
		_strategyManager->readResults();
		_strategyManager->setLearnedStrategy();
	}

	_unitInfoManager->onStart();
	_mapTools->onStart();
	_baseLocationManager->onStart(_mapTools);
	_gameCommander->onStart();

	if (_configuration.Debug.DrawUnitTargetInfo)
	{
		Micro::SetOnAttackUnit([this](const BWAPI::Unit&attacker, const BWAPI::Unit&target)
		{
			auto& canvas = this->getCanvas();
			Micro::drawAction(canvas, attacker->getPosition(), target->getPosition(), Micro::AttackUnitColor);
		});
		Micro::SetOnAttackMove([this](const BWAPI::Unit&attacker, const BWAPI::Position &targetPosition)
		{
			auto& canvas = this->getCanvas();
			Micro::drawAction(canvas, attacker->getPosition(), targetPosition, Micro::AttackMoveColor);
		});
		Micro::SetOnMove([this](const BWAPI::Unit&attacker, const BWAPI::Position &targetPosition)
		{
			auto& canvas = this->getCanvas();
			Micro::drawAction(canvas, attacker->getPosition(), targetPosition, Micro::MoveColor);
		});
		Micro::SetOnRepair([this](const BWAPI::Unit& unit, const BWAPI::Unit& target)
		{
			auto& canvas = this->getCanvas();
			Micro::drawAction(canvas, unit->getPosition(), target->getPosition(), Micro::RepairColor);
		});
		Micro::SetOnRightClick([this](const BWAPI::Unit& unit, const BWAPI::Unit& target)
		{
			auto& canvas = this->getCanvas();
			Micro::drawAction(canvas, unit->getPosition(), target->getPosition(), Micro::RightClickColor);
		});
	}
}

void UAlbertaBot_Tournament::onEnd(bool isWinner) 
{
	_strategyManager->onEnd(isWinner);
}

const shared_ptr<UnitInfoManager> UAlbertaBot_Tournament::UnitInfo() const
{
    return _unitInfoManager;
}

AKBot::ScreenCanvas & UAlbertaBot::UAlbertaBot_Tournament::getCanvas()
{
	return _canvas;
}

void UAlbertaBot_Tournament::onFrame()
{
	auto enemy = _gameCommander->getOpponentView()->defaultEnemy();
	if (enemy == nullptr)
	{
		return;
	}

	auto currentFrame = BWAPI::Broodwar->getFrameCount();

	// update all of the internal information managers
    _mapTools->update(currentFrame);
	_strategyManager->update(currentFrame);
    _unitInfoManager->update();
    _baseLocationManager->update(_unitInfoManager);

    // update the game commander
	_gameCommander->update(currentFrame);

	// Draw debug information
	drawDebugInformation(_canvas);

    if (_configuration.Modules.UsingAutoObserver)
    {
        _autoObserver->onFrame(currentFrame);
    }
}

void UAlbertaBot_Tournament::drawDebugInformation(AKBot::ScreenCanvas& canvas)
{
	_gameDebug->draw(canvas);
}

void UAlbertaBot_Tournament::onUnitDestroy(BWAPI::Unit unit)
{
	auto currentFrame = BWAPI::Broodwar->getFrameCount();
	_unitInfoManager->onUnitDestroy(unit); 
    _gameCommander->onUnitDestroy(unit, currentFrame);
}

void UAlbertaBot_Tournament::onUnitMorph(BWAPI::Unit unit)
{
	_unitInfoManager->onUnitMorph(unit);
    _gameCommander->onUnitMorph(unit);
}

void UAlbertaBot_Tournament::onSendText(std::string text) 
{ 
	_commandManager.execute(text);
}

void UAlbertaBot_Tournament::onUnitCreate(BWAPI::Unit unit)
{ 
	_unitInfoManager->onUnitCreate(unit); 
    _gameCommander->onUnitCreate(unit);
}

void UAlbertaBot_Tournament::onUnitComplete(BWAPI::Unit unit)
{
	_unitInfoManager->onUnitComplete(unit);
    _gameCommander->onUnitComplete(unit);
}

void UAlbertaBot_Tournament::onUnitShow(BWAPI::Unit unit)
{ 
	_unitInfoManager->onUnitShow(unit);
    _gameCommander->onUnitShow(unit);
}

void UAlbertaBot_Tournament::onUnitHide(BWAPI::Unit unit)
{ 
	_unitInfoManager->onUnitHide(unit);
    _gameCommander->onUnitHide(unit);
}

void UAlbertaBot_Tournament::onUnitRenegade(BWAPI::Unit unit)
{ 
	_unitInfoManager->onUnitRenegade(unit);
    _gameCommander->onUnitRenegade(unit);
}