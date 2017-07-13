#include "UAlbertaBotModule_dll.h"
#include "UAlbertaBot_Tournament.h"
#include "UAlbertaBot_Arena.h"
#include "ParseUtils.h"
#include "BWAPIOpponentView.h"

using namespace UAlbertaBot;

UAlbertaBotModule_dll::UAlbertaBotModule_dll()
{
    // parse the bot's configuration file, if it is not found or isn't valid, the program will exit
	auto configurationFile = ParseUtils::FindConfigurationLocation(Config::ConfigFile::ConfigFileLocation);
    ParseUtils::ParseConfigFile(configurationFile);

    if (!Config::ConfigFile::ConfigFileFound || !Config::ConfigFile::ConfigFileParsed)
    {
        return;
    }

    if (Config::BotInfo::BotMode == "Tournament")
    {
        _module = std::shared_ptr<BotModule>(new UAlbertaBot_Tournament(std::make_shared<BWAPIOpponentView>()));
    }
    else if (Config::BotInfo::BotMode == "Arena")
    {
        _module = std::shared_ptr<BotModule>(new UAlbertaBot_Arena());
    }
    else
    {
        std::cerr << "Unknown bot module selected: " << Config::BotInfo::BotMode << "\n";
    }
}

void UAlbertaBotModule_dll::onStart()
{
    if (!Config::ConfigFile::ConfigFileFound || !Config::ConfigFile::ConfigFileParsed)
    {
        return;
    }

    _module->onStart();
}

void UAlbertaBotModule_dll::onFrame()
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

    _module->onFrame();
}

void UAlbertaBotModule_dll::onEnd(bool isWinner)
{
    if (_module.get() == nullptr) return;

    _module->onEnd(isWinner);
}

void UAlbertaBotModule_dll::onUnitDestroy(BWAPI::Unit unit)
{
    if (_module.get() == nullptr) return;

    _module->onUnitDestroy(unit);
}

void UAlbertaBotModule_dll::onUnitMorph(BWAPI::Unit unit)
{
    if (_module.get() == nullptr) return;

    _module->onUnitMorph(unit);
}

void UAlbertaBotModule_dll::onSendText(std::string text)
{
    if (_module.get() == nullptr) return;

    _module->onSendText(text);
}

void UAlbertaBotModule_dll::onUnitCreate(BWAPI::Unit unit)
{
    if (_module.get() == nullptr) return;

    _module->onUnitCreate(unit);
}

void UAlbertaBotModule_dll::onUnitComplete(BWAPI::Unit unit)
{
    if (_module.get() == nullptr) return;

    _module->onUnitComplete(unit);
}

void UAlbertaBotModule_dll::onUnitShow(BWAPI::Unit unit)
{
    if (_module.get() == nullptr) return;

    _module->onUnitShow(unit);
}

void UAlbertaBotModule_dll::onUnitHide(BWAPI::Unit unit)
{
    if (_module.get() == nullptr) return;

    _module->onUnitHide(unit);
}

void UAlbertaBotModule_dll::onUnitRenegade(BWAPI::Unit unit)
{
    if (_module.get() == nullptr) return;

    _module->onUnitRenegade(unit);
}