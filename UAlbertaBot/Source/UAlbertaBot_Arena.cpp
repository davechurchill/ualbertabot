/*
 +----------------------------------------------------------------------+
 | UAlbertaBot Arena Version                                            |
 | Author: David Churchill <dave.churchill@gmail.com>                   |
 +----------------------------------------------------------------------+
 */

#include "UAlbertaBot_Arena.h"
#include "Config.h"
#include "ParseUtils.h"
#include "DebugTools.h"
#include "Micro.h"
#include "ArenaPlayer_AttackClosest.h"
#include <BWAPI/Game.h>
#include <BWAPI/Flag.h>
#include <BWAPI/Unitset.h>

using namespace UAlbertaBot;
using namespace AKBot;


UAlbertaBot_Arena::UAlbertaBot_Arena()
    : _results      (3, 0)  // 0 = wins, 1 = losses, 2 = draws
    , _battles      (0)
    , _prevIsBattle (false)
{

}

// This gets called when the bot starts!
void UAlbertaBot_Arena::onStart()
{
	bool ConfigFileFound = true;
	bool ConfigFileParsed = true;

    // Parse the bot's configuration file if it has one, change this file path to where your config file is
    // Any relative path name will be relative to Starcraft installation folder
	auto configurationFile = ParseUtils::FindConfigurationLocation("AK_Config.json");
    ParseUtils::ParseConfigFile(configurationFile, Config, ConfigFileFound, ConfigFileParsed);
    
	auto bwapiOptions = Config.BWAPIOptions;
	auto botInfo = Config.BotInfo;

    // Set our BWAPI options here    
    BWAPI::Broodwar->setLocalSpeed(bwapiOptions.SetLocalSpeed);
    BWAPI::Broodwar->setFrameSkip(bwapiOptions.SetFrameSkip);

    // Set the starting date
    _startDate = Assert::currentDateTime();

    // set the arena player
    _arenaPlayer = std::shared_ptr<ArenaPlayer>(new ArenaPlayer_AttackClosest());

    if (bwapiOptions.EnableCompleteMapInformation)
    {
        BWAPI::Broodwar->enableFlag(BWAPI::Flag::CompleteMapInformation);
    }

    if (bwapiOptions.EnableUserInput)
    {
        BWAPI::Broodwar->enableFlag(BWAPI::Flag::UserInput);
    }

    if (botInfo.PrintInfoOnStart)
    {
        BWAPI::Broodwar->printf("Hello! I am %s, written by %s", botInfo.BotName.c_str(), botInfo.Authors.c_str());
    }
}

void UAlbertaBot_Arena::onFrame()
{
	auto currentFrame = BWAPI::Broodwar->getFrameCount();
	drawUnitHPBars(_canvas);

    // if there is a battle ongoing
    if (isBattle())
    {
        // if there was no battle last frame, this is a new battle
        if (!_prevIsBattle)
        {
            _arenaPlayer->onBattleBegin();
        }

        _arenaPlayer->onFrame(currentFrame);
    }
    // otherwise there is currently no battle
    else
    {
        // if this is the first frame that there was no battle, the battle just ended
        // we will record the battle statistics and tell the player that the battle ended
        if (_prevIsBattle)
        {
            _battles++;
            _results[winner()]++;
        }
    }

    std::stringstream ss;
    ss << BWAPI::Broodwar->mapFileName() << "\nPlayer: " << _arenaPlayer->getName() << "\n";
    ss << "Battles: " << _battles << "\nWins:   " << _results[0] << "\nLosses: " << _results[1] << "\nDraws:  " << _results[2] << "\nScore:  " << ((double)(_results[0] + (double)_results[2]/2)/_battles);
    _resultString = ss.str();
    _canvas.drawTextScreen(10, 10, _resultString.c_str());

    _prevIsBattle = isBattle();
}

size_t UAlbertaBot_Arena::numUnits(BWAPI::Player player) const
{
    size_t numUnits = 0;

    for (auto unit : player->getUnits())
    {
        if (unit->getType().getID() != 101 && unit->getHitPoints() > 0)
        {
            numUnits++;
        }
    }

    return numUnits;
}

bool UAlbertaBot_Arena::isBattle()
{
	return numUnits(BWAPI::Broodwar->self()) > 0 || numUnits(BWAPI::Broodwar->enemy()) > 0;
}

int UAlbertaBot_Arena::winner()
{
    if (numUnits(BWAPI::Broodwar->self()) == 0)
    {
        return 1;
    }

    if (numUnits(BWAPI::Broodwar->enemy()) == 0)
    {
        return 0;
    }

    return 2;
}

void UAlbertaBot_Arena::drawUnitHPBars(ScreenCanvas& canvas) const
{
    if (!Config.Debug.DrawUnitHealthBars) { 
		return;
	}

    for (const auto & unit : BWAPI::Broodwar->getAllUnits())
    {
        DebugTools::DrawUnitHPBar(canvas, unit->getType(), unit->getPosition(), unit->getHitPoints(), unit->getShields());

        if (unit->getType().getRace() != BWAPI::Races::None && unit->getPlayer() == BWAPI::Broodwar->self())
        {
            canvas.drawLineMap(unit->getPosition() + BWAPI::Position(3, 3), unit->getOrderTargetPosition() + BWAPI::Position(3, 3), BWAPI::Colors::Red);
        }
    }

}

void UAlbertaBot_Arena::onEnd(bool)
{

}

void UAlbertaBot_Arena::onUnitDestroy(BWAPI::Unit)
{

}

void UAlbertaBot_Arena::onUnitMorph(BWAPI::Unit)
{

}

void UAlbertaBot_Arena::onSendText(std::string)
{

}

void UAlbertaBot_Arena::onUnitCreate(BWAPI::Unit)
{

}

void UAlbertaBot_Arena::onUnitComplete(BWAPI::Unit)
{

}

void UAlbertaBot_Arena::onUnitShow(BWAPI::Unit)
{

}

void UAlbertaBot_Arena::onUnitHide(BWAPI::Unit)
{

}

void UAlbertaBot_Arena::onUnitRenegade(BWAPI::Unit)
{

}

void UAlbertaBot_Arena::printInfo()
{

}
