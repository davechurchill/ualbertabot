#include "Common.h"
#include "UAlbertaBotModule.h"
#include "JSONTools.h"
#include "ParseUtils.h"
#include "UnitUtil.h"
#include "Global.h"

using namespace UAlbertaBot;

UAlbertaBotModule::UAlbertaBotModule()
    : _gameCommander(*this)
{

}

UAlbertaBotModule::~UAlbertaBotModule()
{
    Global::SetModule(nullptr);
}

// This gets called when the bot starts!
void UAlbertaBotModule::onStart()
{
    // Initialize SparCraft, the combat simulation package
    SparCraft::init();
    SparCraft::AIParameters::Instance().parseFile(Config::SparCraft::SparCraftConfigFile);

    // Initialize BOSS, the Build Order Search System
    BOSS::init();

    // Initialize all the global classes in UAlbertaBot 
    Global::SetModule(this);

    // Parse the bot's configuration file if it has one, change this file path to where your config file is
    // Any relative path name will be relative to Starcraft installation folder
    ParseUtils::ParseConfigFile(Config::ConfigFile::ConfigFileLocation, _strategyManager);

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
    _baseLocationManager.onStart();
    _gameCommander.onStart();
}

void UAlbertaBotModule::onEnd(bool isWinner) 
{
	_strategyManager.onEnd(isWinner);
}

WorkerManager & UAlbertaBotModule::Workers()
{
    return _workerManager;
}

const UnitInfoManager & UAlbertaBotModule::UnitInfo() const
{
    return _unitInfoManager;
}

const StrategyManager & UAlbertaBotModule::Strategy() const
{
    return _strategyManager;
}

const BaseLocationManager & UAlbertaBotModule::Bases() const
{
    return _baseLocationManager;
}

const MapTools & UAlbertaBotModule::Map() const
{
    return _mapTools;
}

void UAlbertaBotModule::onFrame()
{
    // update all of the internal information managers
    _mapTools.update();
    _strategyManager.update();
    _unitInfoManager.update();
    _workerManager.update();
    _baseLocationManager.update();

    // update the game commander
	_gameCommander.update(); 
  

    if (Config::Modules::UsingAutoObserver)
    {
        _autoObserver.onFrame();
    }

    drawErrorMessages();
}

void UAlbertaBotModule::drawErrorMessages() const
{
    char red = '\x08';
    char green = '\x07';
    char white = '\x04';

    if (!Config::ConfigFile::ConfigFileFound)
    {
        BWAPI::Broodwar->drawBoxScreen(0,0,450,100, BWAPI::Colors::Black, true);
        BWAPI::Broodwar->setTextSize(BWAPI::Text::Size::Huge);
        BWAPI::Broodwar->drawTextScreen(10, 5, "%c%s Config File Not Found", red, Config::BotInfo::BotName.c_str());
        BWAPI::Broodwar->setTextSize(BWAPI::Text::Size::Default);
        BWAPI::Broodwar->drawTextScreen(10, 30, "%c%s will not run without its configuration file", white, Config::BotInfo::BotName.c_str());
        BWAPI::Broodwar->drawTextScreen(10, 45, "%cCheck that the file below exists. Incomplete paths are relative to Starcraft directory", white);
        BWAPI::Broodwar->drawTextScreen(10, 60, "%cYou can change this file location in Config::ConfigFile::ConfigFileLocation", white);
        BWAPI::Broodwar->drawTextScreen(10, 75, "%cFile Not Found (or is empty): %c %s", white, green, Config::ConfigFile::ConfigFileLocation.c_str());
        return;
    }
    else if (!Config::ConfigFile::ConfigFileParsed)
    {
        BWAPI::Broodwar->drawBoxScreen(0,0,450,100, BWAPI::Colors::Black, true);
        BWAPI::Broodwar->setTextSize(BWAPI::Text::Size::Huge);
        BWAPI::Broodwar->drawTextScreen(10, 5, "%c%s Config File Parse Error", red, Config::BotInfo::BotName.c_str());
        BWAPI::Broodwar->setTextSize(BWAPI::Text::Size::Default);
        BWAPI::Broodwar->drawTextScreen(10, 30, "%c%s will not run without a properly formatted configuration file", white, Config::BotInfo::BotName.c_str());
        BWAPI::Broodwar->drawTextScreen(10, 45, "%cThe configuration file was found, but could not be parsed. Check that it is valid JSON", white);
        BWAPI::Broodwar->drawTextScreen(10, 60, "%cFile Not Parsed: %c %s", white, green, Config::ConfigFile::ConfigFileLocation.c_str());
        return;
    }
}

void UAlbertaBotModule::onUnitDestroy(BWAPI::Unit unit)
{
	_workerManager.onUnitDestroy(unit);
	_unitInfoManager.onUnitDestroy(unit); 
    _gameCommander.onUnitDestroy(unit);
}

void UAlbertaBotModule::onUnitMorph(BWAPI::Unit unit)
{
	_unitInfoManager.onUnitMorph(unit);
	_workerManager.onUnitMorph(unit);
    _gameCommander.onUnitMorph(unit);
}

void UAlbertaBotModule::onSendText(std::string text) 
{ 
	ParseUtils::ParseTextCommand(text);
}

void UAlbertaBotModule::onUnitCreate(BWAPI::Unit unit)
{ 
	_unitInfoManager.onUnitCreate(unit); 
    _gameCommander.onUnitCreate(unit);
}

void UAlbertaBotModule::onUnitComplete(BWAPI::Unit unit)
{
	_unitInfoManager.onUnitComplete(unit);
    _gameCommander.onUnitComplete(unit);
}

void UAlbertaBotModule::onUnitShow(BWAPI::Unit unit)
{ 
	_unitInfoManager.onUnitShow(unit); 
	_workerManager.onUnitShow(unit);
    _gameCommander.onUnitShow(unit);
}

void UAlbertaBotModule::onUnitHide(BWAPI::Unit unit)
{ 
	_unitInfoManager.onUnitHide(unit); 
    _gameCommander.onUnitHide(unit);
}

void UAlbertaBotModule::onUnitRenegade(BWAPI::Unit unit)
{ 
	_unitInfoManager.onUnitRenegade(unit); 
    _gameCommander.onUnitRenegade(unit);
}