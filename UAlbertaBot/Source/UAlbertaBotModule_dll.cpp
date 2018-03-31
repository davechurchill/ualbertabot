#include "UAlbertaBotModule_dll.h"
#include "UAlbertaBot_Tournament.h"
#include "UAlbertaBot_Arena.h"
#include "ParseUtils.h"
#include "BWAPIOpponentView.h"
#include "debug\BWAPIPrintLogger.h"
#include "BotFactory.h"

using namespace UAlbertaBot;

bool ConfigFileFound = true;
bool ConfigFileParsed = true;
std::string configFileLocation = "AK_Config.json";
BotConfiguration Config;

UAlbertaBotModule_dll::UAlbertaBotModule_dll()
{
    // parse the bot's configuration file, if it is not found or isn't valid, the program will exit
	auto configurationFile = ParseUtils::FindConfigurationLocation(configFileLocation);
    ParseUtils::ParseConfigFile(configurationFile, Config, ConfigFileFound, ConfigFileParsed);

    if (!ConfigFileFound || !ConfigFileParsed)
    {
        return;
    }

	std::string botMode = Config.BotInfo.BotMode;
	auto m = createBot(botMode, Config, configurationFile);
	_module = m.getBot();
	if (!m.isValid())
	{
		std::cerr << "Unknown bot module selected: " << botMode << "\n";
	}
}

void UAlbertaBotModule_dll::onStart() override
{
    if (!ConfigFileFound || !ConfigFileParsed)
    {
        return;
    }

    _module->onStart();
}

void UAlbertaBotModule_dll::onFrame() override
{
    char red = '\x08';
    char green = '\x07';
    char white = '\x04';

	auto& botInfo = Config.BotInfo;
    if (!ConfigFileFound)
    {
        BWAPI::Broodwar->drawBoxScreen(0,0,450,100, BWAPI::Colors::Black, true);
        BWAPI::Broodwar->setTextSize(BWAPI::Text::Size::Huge);
        BWAPI::Broodwar->drawTextScreen(10, 5, "%c%s Config File Not Found", red, botInfo.BotName.c_str());
        BWAPI::Broodwar->setTextSize(BWAPI::Text::Size::Default);
        BWAPI::Broodwar->drawTextScreen(10, 30, "%c%s will not run without its configuration file", white, botInfo.BotName.c_str());
        BWAPI::Broodwar->drawTextScreen(10, 45, "%cCheck that the file below exists. Incomplete paths are relative to Starcraft directory", white);
        BWAPI::Broodwar->drawTextScreen(10, 60, "%cYou can change this file location in Config::ConfigFile::ConfigFileLocation", white);
        BWAPI::Broodwar->drawTextScreen(10, 75, "%cFile Not Found (or is empty): %c %s", white, green, configFileLocation.c_str());
        return;
    }
    else if (!ConfigFileParsed)
    {
        BWAPI::Broodwar->drawBoxScreen(0,0,450,100, BWAPI::Colors::Black, true);
        BWAPI::Broodwar->setTextSize(BWAPI::Text::Size::Huge);
        BWAPI::Broodwar->drawTextScreen(10, 5, "%c%s Config File Parse Error", red, botInfo.BotName.c_str());
        BWAPI::Broodwar->setTextSize(BWAPI::Text::Size::Default);
        BWAPI::Broodwar->drawTextScreen(10, 30, "%c%s will not run without a properly formatted configuration file", white, botInfo.BotName.c_str());
        BWAPI::Broodwar->drawTextScreen(10, 45, "%cThe configuration file was found, but could not be parsed. Check that it is valid JSON", white);
        BWAPI::Broodwar->drawTextScreen(10, 60, "%cFile Not Parsed: %c %s", white, green, configFileLocation.c_str());
        return;
    }

    _module->onFrame();
}

void UAlbertaBotModule_dll::onEnd(bool isWinner) override
{
    if (_module.get() == nullptr) return;

    _module->onEnd(isWinner);
}

void UAlbertaBotModule_dll::onUnitDestroy(BWAPI::Unit unit) override
{
    if (_module.get() == nullptr) return;

    _module->onUnitDestroy(unit);
}

void UAlbertaBotModule_dll::onUnitMorph(BWAPI::Unit unit) override
{
    if (_module.get() == nullptr) return;

    _module->onUnitMorph(unit);
}

void UAlbertaBotModule_dll::onSendText(std::string text) override
{
    if (_module.get() == nullptr) return;

    _module->onSendText(text);
}

void UAlbertaBotModule_dll::onUnitCreate(BWAPI::Unit unit) override
{
    if (_module.get() == nullptr) return;

    _module->onUnitCreate(unit);
}

void UAlbertaBotModule_dll::onUnitComplete(BWAPI::Unit unit) override
{
    if (_module.get() == nullptr) return;

    _module->onUnitComplete(unit);
}

void UAlbertaBotModule_dll::onUnitShow(BWAPI::Unit unit) override
{
    if (_module.get() == nullptr) return;

    _module->onUnitShow(unit);
}

void UAlbertaBotModule_dll::onUnitHide(BWAPI::Unit unit) override
{
    if (_module.get() == nullptr) return;

    _module->onUnitHide(unit);
}

void UAlbertaBotModule_dll::onUnitRenegade(BWAPI::Unit unit) override
{
    if (_module.get() == nullptr) return;

    _module->onUnitRenegade(unit);
}

void UAlbertaBotModule_dll::onNukeDetect(BWAPI::Position target) override
{
	if (_module.get() == nullptr) return;

	_module->onNukeDetect(target);
}

void UAlbertaBotModule_dll::onUnitEvade(BWAPI::Unit unit) override
{
	if (_module.get() == nullptr) return;

	_module->onUnitEvade(unit);
}