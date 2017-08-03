#include "Common.h"
#include "UAlbertaBot_Tournament.h"
#include "JSONTools.h"
#include "ParseUtils.h"
#include "UnitUtil.h"
#include "Global.h"
#include "BaseLocationManagerDebug.h"
#include "WorkerManagerDebug.h"
#include "MapToolsDebug.h"
#include "UnitInfoManagerDebug.h"
#include "GameCommanderDebug.h"
#include "Micro.h"

using namespace UAlbertaBot;
using namespace AKBot;

UAlbertaBot_Tournament::UAlbertaBot_Tournament(const AKBot::OpponentView& opponentView, const AKBot::Logger& logger)
	: _opponentView(opponentView)
	, _baseLocationManager(opponentView)
	, _gameCommander(*this, opponentView, _baseLocationManager, logger)
	, _mapTools(BWAPI::Broodwar->mapWidth(), BWAPI::Broodwar->mapHeight(), opponentView, logger)
	, _strategyManager("", opponentView, _unitInfoManager, _baseLocationManager, logger)
	, _unitInfoManager(opponentView)
	, _autoObserver(opponentView)
	, _workerManager(opponentView, logger)
{
	// parse the configuration file for the bot's strategies
	auto configurationFile = ParseUtils::FindConfigurationLocation(Config::ConfigFile::ConfigFileLocation);
	ParseUtils::ParseStrategy(configurationFile, this->_strategyManager);
}

UAlbertaBot_Tournament::~UAlbertaBot_Tournament()
{
	Global::SetModule(nullptr);
}

// This gets called when the bot starts!
void UAlbertaBot_Tournament::onStart()
{
	// Initialize SparCraft, the combat simulation package
	SparCraft::init();
	auto configurationFile = ParseUtils::FindConfigurationLocation(Config::SparCraft::SparCraftConfigFile);
	SparCraft::AIParameters::Instance().parseFile(configurationFile);

	// Initialize BOSS, the Build Order Search System
	BOSS::init();

	// Initialize all the global classes in UAlbertaBot 
	Global::SetModule(this);

	// Set our BWAPI options here    
	BWAPI::Broodwar->setLocalSpeed(Config::BWAPIOptions::SetLocalSpeed);
	BWAPI::Broodwar->setFrameSkip(Config::BWAPIOptions::SetFrameSkip);

	if (Config::BWAPIOptions::EnableCompleteMapInformation)
	{
		BWAPI::Broodwar->enableFlag(BWAPI::Flag::CompleteMapInformation);
	}

	if (Config::BWAPIOptions::EnableUserInput)
	{
		BWAPI::Broodwar->enableFlag(BWAPI::Flag::UserInput);
	}

	if (Config::BotInfo::PrintInfoOnStart)
	{
		BWAPI::Broodwar->printf("Hello! I am %s, written by %s", Config::BotInfo::BotName.c_str(), Config::BotInfo::Authors.c_str());
	}

	if (Config::Modules::UsingStrategyIO)
	{
		_strategyManager.readResults();
		_strategyManager.setLearnedStrategy();
	}

	_unitInfoManager.onStart();
	_mapTools.onStart();
	_baseLocationManager.onStart(_mapTools);
	_gameCommander.onStart();

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

void UAlbertaBot_Tournament::onEnd(bool isWinner) 
{
	_strategyManager.onEnd(isWinner);
}

WorkerManager & UAlbertaBot_Tournament::Workers()
{
    return _workerManager;
}

const UnitInfoManager & UAlbertaBot_Tournament::UnitInfo() const
{
    return _unitInfoManager;
}

const StrategyManager & UAlbertaBot_Tournament::Strategy() const
{
    return _strategyManager;
}

const BaseLocationManager & UAlbertaBot_Tournament::Bases() const
{
    return _baseLocationManager;
}

const MapTools & UAlbertaBot_Tournament::Map() const
{
    return _mapTools;
}

AKBot::ScreenCanvas & UAlbertaBot::UAlbertaBot_Tournament::getCanvas()
{
	return _canvas;
}

void UAlbertaBot_Tournament::onFrame()
{
	auto currentFrame = BWAPI::Broodwar->getFrameCount();

    // update all of the internal information managers
    _mapTools.update(currentFrame);
	_strategyManager.update();
    _unitInfoManager.update();
    _workerManager.update(currentFrame);
    _baseLocationManager.update(_unitInfoManager);

    // update the game commander
	_gameCommander.update(currentFrame);

	// Draw debug information
	drawDebugInformation(_canvas);

    if (Config::Modules::UsingAutoObserver)
    {
        _autoObserver.onFrame(currentFrame);
    }

    drawErrorMessages();
}

void UAlbertaBot_Tournament::drawDebugInformation(AKBot::ScreenCanvas& canvas)
{
	if (Config::Debug::DrawLastSeenTileInfo)
	{
		MapToolsDebug mapDebug(_mapTools, _baseLocationManager);
		mapDebug.drawLastSeen(canvas, _baseLocationManager);
	}

	WorkerManagerDebug debug(_workerManager.getWorkerData());
	debug.draw(canvas);

	// draw the debug information for each base location
	BaseLocationManagerDebug baseLocationManagerDebug(_opponentView, _baseLocationManager, _mapTools);
	baseLocationManagerDebug.draw(canvas);

	UnitInfoManagerDebug unitInfoDebug(_opponentView, _unitInfoManager);
	unitInfoDebug.draw(canvas);

	GameCommanderDebug gameCommanderDebug(_gameCommander);
	gameCommanderDebug.draw(canvas);
}

void UAlbertaBot_Tournament::drawErrorMessages() const
{
}

void UAlbertaBot_Tournament::onUnitDestroy(BWAPI::Unit unit)
{
	auto currentFrame = BWAPI::Broodwar->getFrameCount();
	_workerManager.onUnitDestroy(unit, currentFrame);
	_unitInfoManager.onUnitDestroy(unit); 
    _gameCommander.onUnitDestroy(unit, currentFrame);
}

void UAlbertaBot_Tournament::onUnitMorph(BWAPI::Unit unit)
{
	_unitInfoManager.onUnitMorph(unit);
	_workerManager.onUnitMorph(unit);
    _gameCommander.onUnitMorph(unit);
}

void UAlbertaBot_Tournament::onSendText(std::string text) 
{ 
	ParseUtils::ParseTextCommand(text);
    BWAPI::Broodwar->sendText("%s", text.c_str());
}

void UAlbertaBot_Tournament::onUnitCreate(BWAPI::Unit unit)
{ 
	_unitInfoManager.onUnitCreate(unit); 
    _gameCommander.onUnitCreate(unit);
}

void UAlbertaBot_Tournament::onUnitComplete(BWAPI::Unit unit)
{
	_unitInfoManager.onUnitComplete(unit);
    _gameCommander.onUnitComplete(unit);
}

void UAlbertaBot_Tournament::onUnitShow(BWAPI::Unit unit)
{ 
	_unitInfoManager.onUnitShow(unit); 
	_workerManager.onUnitShow(unit);
    _gameCommander.onUnitShow(unit);
}

void UAlbertaBot_Tournament::onUnitHide(BWAPI::Unit unit)
{ 
	_unitInfoManager.onUnitHide(unit); 
    _gameCommander.onUnitHide(unit);
}

void UAlbertaBot_Tournament::onUnitRenegade(BWAPI::Unit unit)
{ 
	_unitInfoManager.onUnitRenegade(unit); 
    _gameCommander.onUnitRenegade(unit);
}