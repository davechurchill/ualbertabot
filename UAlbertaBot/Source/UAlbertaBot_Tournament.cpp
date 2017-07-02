#include "Common.h"
#include "UAlbertaBot_Tournament.h"
#include "JSONTools.h"
#include "ParseUtils.h"
#include "UnitUtil.h"
#include "Global.h"

using namespace UAlbertaBot;

UAlbertaBot_Tournament::UAlbertaBot_Tournament()
    : _gameCommander(*this)
{

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

void UAlbertaBot_Tournament::onFrame()
{
    // update all of the internal information managers
    _mapTools.update();
    _strategyManager.update();
    _unitInfoManager.update();
    _workerManager.update();
    _baseLocationManager.update(Global::UnitInfo());

    // update the game commander
	_gameCommander.update(); 
  

    if (Config::Modules::UsingAutoObserver)
    {
        _autoObserver.onFrame();
    }

    drawErrorMessages();
}

void UAlbertaBot_Tournament::drawErrorMessages() const
{
    char red = '\x08';
    char green = '\x07';
    char white = '\x04';
}

void UAlbertaBot_Tournament::onUnitDestroy(BWAPI::Unit unit)
{
	_workerManager.onUnitDestroy(unit);
	_unitInfoManager.onUnitDestroy(unit); 
    _gameCommander.onUnitDestroy(unit);
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