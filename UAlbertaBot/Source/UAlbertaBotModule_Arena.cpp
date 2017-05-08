/*
 +----------------------------------------------------------------------+
 | UAlbertaBot Arena Version                                            |
 | Author: David Churchill <dave.churchill@gmail.com>                   |
 +----------------------------------------------------------------------+
 */

#include "UAlbertaBotModule_Arena.h"
#include "Config.h"
#include "ParseUtils.h"
#include "DebugTools.h"
#include "Micro.h"

using namespace UAlbertaBot;

UAlbertaBotModule_Arena::UAlbertaBotModule_Arena()
    : _isSparCraftInitialized(false)
    , _wins(3, 0)
    , _battles(0)
    , _battleEnded(true)
{

}

// This gets called when the bot starts!
void UAlbertaBotModule_Arena::onStart()
{
    // Parse the bot's configuration file if it has one, change this file path to where your config file is
    // Any relative path name will be relative to Starcraft installation folder
    ParseUtils::ParseConfigFile(Config::ConfigFile::ConfigFileLocation);

    // Initialize the SparCraft project settings
    initializeSparCraft();

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
}

SparCraft::GameState UAlbertaBotModule_Arena::GetSparCraftState() const
{
    SparCraft::GameState state;

    for (const auto & unit : BWAPI::Broodwar->getAllUnits())
    {
        size_t playerID = GetSparCraftPlayerID(unit->getPlayer());

        if (playerID != SparCraft::Players::Player_None && SparCraft::System::UnitTypeSupported(unit->getType()))
        {
            state.addUnit(GetSparCraftUnit(unit));
        }
    }

    state.setTime(BWAPI::Broodwar->getFrameCount());

    return state;
}

SparCraft::Move UAlbertaBotModule_Arena::GetSparCraftPlayerMove(const SparCraft::GameState & state, const size_t & playerID) const
{
    SparCraft::PlayerPtr player = SparCraft::AIParameters::Instance().getPlayer(playerID, Config::SparCraft::ArenaPlayerName);

    SparCraft::Move move;
    player->getMove(state, move);

    BWAPI::Broodwar->drawTextScreen(BWAPI::Position(100, 10), "%d", move.size());

    return move;
}

SparCraft::Unit UAlbertaBotModule_Arena::GetSparCraftUnit(BWAPI::Unit unit) const
{
    SparCraft::Unit sUnit(unit->getType(),
        SparCraft::Position(unit->getPosition()),
        0,
        GetSparCraftPlayerID(unit->getPlayer()),
        unit->getHitPoints() + unit->getShields(),
        0,
        BWAPI::Broodwar->getFrameCount() + GetTimeCanMove(unit),
        BWAPI::Broodwar->getFrameCount() + GetTimeCanAttack(unit));

    sUnit.setBWAPIUnitID(unit->getID());

    return sUnit;
}

size_t UAlbertaBotModule_Arena::GetSparCraftPlayerID(BWAPI::Player player) const
{
    if (player == BWAPI::Broodwar->self())
    {
        return SparCraft::Players::Player_One;
    }
    else if (player == BWAPI::Broodwar->enemy())
    {
        return SparCraft::Players::Player_Two;
    }

    return SparCraft::Players::Player_None;
}

void UAlbertaBotModule_Arena::DrawSparCraftMove(const SparCraft::GameState & state, const SparCraft::Move & move) const
{
    for (size_t a(0); a < move.size(); ++a)
    {
        const auto & action = move[a];
        const SparCraft::Unit & sUnit = state.getUnitByID(action.getID());
        const BWAPI::Unit & unit = BWAPI::Broodwar->getUnit(sUnit.getBWAPIUnitID());

        if (action.type() == SparCraft::ActionTypes::MOVE)
        {
            BWAPI::Position dest(action.pos().x(), action.pos().y());

            BWAPI::Broodwar->drawLineMap(unit->getPosition(), dest, BWAPI::Colors::White);
        }
        else if (action.type() == SparCraft::ActionTypes::ATTACK)
        {
            const SparCraft::Unit & sTargetUnit = state.getUnitByID(action.getTargetID());
            const BWAPI::Unit & targetUnit = BWAPI::Broodwar->getUnit(sTargetUnit.getBWAPIUnitID());
            const BWAPI::Position & dest = targetUnit->getPosition();

            BWAPI::Broodwar->drawLineMap(unit->getPosition(), dest, BWAPI::Colors::Red);
        }
    }
}

void UAlbertaBotModule_Arena::DoSparCraftMove(const SparCraft::GameState & state, const SparCraft::Move & move) const
{
    for (size_t a(0); a < move.size(); ++a)
    {
        const auto & action = move[a];
        const SparCraft::Unit & sUnit = state.getUnitByID(action.getID());
        const BWAPI::Unit & unit = BWAPI::Broodwar->getUnit(sUnit.getBWAPIUnitID());

        if (action.type() == SparCraft::ActionTypes::MOVE)
        {
            BWAPI::Position dest(action.pos().x(), action.pos().y());
            
            unit->rightClick(dest);
            //Micro::SmartMove(unit, dest);
            
            BWAPI::Broodwar->drawLineMap(unit->getPosition(), dest, BWAPI::Colors::Yellow);
        }
        else if (action.type() == SparCraft::ActionTypes::ATTACK)
        {
            const SparCraft::Unit & sTargetUnit = state.getUnitByID(action.getTargetID());
            const BWAPI::Unit & targetUnit = BWAPI::Broodwar->getUnit(sTargetUnit.getBWAPIUnitID());
            const BWAPI::Position & dest = targetUnit->getPosition();

            //unit->rightClick(targetUnit);
            Micro::SmartAttackUnit(unit, targetUnit);

            BWAPI::Broodwar->drawLineMap(unit->getPosition(), dest, BWAPI::Colors::Green);
        }
    }
}

int UAlbertaBotModule_Arena::GetTimeSinceLastAttack(BWAPI::Unit unit) const
{
    return unit->getType().groundWeapon().damageCooldown() - unit->getGroundWeaponCooldown();
}

void UAlbertaBotModule_Arena::DrawSparCraftState(const SparCraft::GameState & state, int x, int y) const
{
    size_t numUnits = 0;
    std::stringstream ss;
    for (size_t p(0); p < 2; ++p)
    {
        for (size_t u(0); u < state.numUnits(p); ++u)
        {
            const SparCraft::Unit & unit = state.getUnit(p, u);

            ss << unit.getID() << " " << unit.type().getName() << " (" << unit.pos().x() << "," << unit.pos().y() << ")\n";          
        }
    }

    BWAPI::Broodwar->drawTextScreen(BWAPI::Position(x, y), ss.str().c_str());
}

void UAlbertaBotModule_Arena::DrawUnitHPBars() const
{
    if (!Config::Debug::DrawUnitHealthBars) { return; }

    for (const auto & unit : BWAPI::Broodwar->getAllUnits())
    {
        DebugTools::DrawUnitHPBar(unit->getType(), unit->getPosition(), unit->getHitPoints(), unit->getShields());

        std::stringstream ss;
        ss << unit->getGroundWeaponCooldown() << " " << GetTimeSinceLastAttack(unit) << " " << GetTimeCanMove(unit) << " " << GetTimeCanAttack(unit);

        BWAPI::Broodwar->drawTextMap(unit->getPosition() - BWAPI::Position(0, 30), ss.str().c_str());

        if (unit->getType().getRace() != BWAPI::Races::None && unit->getPlayer() == BWAPI::Broodwar->self())
        {
            BWAPI::Broodwar->drawLineMap(unit->getPosition() + BWAPI::Position(3, 3), unit->getOrderTargetPosition() + BWAPI::Position(3, 3), BWAPI::Colors::Red);
        }
        
        
    }

    std::stringstream ss;
    ss << "Wins:   " << _wins[0] << "\nLosses: " << (_battles - _wins[0]) << "\nWin %%:  " << ((double)_wins[0]/_battles);

    BWAPI::Broodwar->drawTextScreen(BWAPI::Position(10, 200), ss.str().c_str());
}

int UAlbertaBotModule_Arena::GetTimeCanMove(BWAPI::Unit unit) const
{
    if (unit->getGroundWeaponCooldown() == 0)
    {
        return 0;
    }

    int lastAttack = GetTimeSinceLastAttack(unit);
    int attackFrames = SparCraft::AnimationFrameData::getAttackFrames(unit->getType()).first;

    return std::max(attackFrames - lastAttack, 0);
}

int UAlbertaBotModule_Arena::GetTimeCanAttack(BWAPI::Unit unit) const
{
    return 0;

    int attackTime = unit->getGroundWeaponCooldown();
    attackTime -= 6;
    
    if (attackTime < 0)
    {
        attackTime = 0;
    }

    return attackTime;
}

void UAlbertaBotModule_Arena::onEnd(bool isWinner)
{

}

void UAlbertaBotModule_Arena::onFrame()
{
    printInfo();
    DrawUnitHPBars();
    _oberver.onFrame();

    SparCraft::GameState state = GetSparCraftState();

    if (!state.gameOver())
    {
        //DrawSparCraftState(state, 10, 10);
        auto move = GetSparCraftPlayerMove(state, GetSparCraftPlayerID(BWAPI::Broodwar->self()));
        DoSparCraftMove(state, move);
        _battleEnded = false;
    }
    else
    {
        if (!_battleEnded)
        {
            _battleEnded = true;
            _battles++;
            _wins[state.winner()]++;
        }
    }
}

void UAlbertaBotModule_Arena::onUnitDestroy(BWAPI::Unit unit)
{

}

void UAlbertaBotModule_Arena::onUnitMorph(BWAPI::Unit unit)
{

}

void UAlbertaBotModule_Arena::onSendText(std::string text)
{

}

void UAlbertaBotModule_Arena::onUnitCreate(BWAPI::Unit unit)
{

}

void UAlbertaBotModule_Arena::onUnitComplete(BWAPI::Unit unit)
{

}

void UAlbertaBotModule_Arena::onUnitShow(BWAPI::Unit unit)
{

}

void UAlbertaBotModule_Arena::onUnitHide(BWAPI::Unit unit)
{

}

void UAlbertaBotModule_Arena::onUnitRenegade(BWAPI::Unit unit)
{

}

void UAlbertaBotModule_Arena::initializeSparCraft()
{
    try
    {
        // Initialize SparCraft, the combat simulation package
        SparCraft::init();

        // Read the SparCraft configuration file
        SparCraft::AIParameters::Instance().parseFile(Config::SparCraft::SparCraftConfigFile);
        _isSparCraftInitialized = true;
    }
    catch (SparCraft::SparCraftException e)
    {
        _lastSparCraftException = e;
        _isSparCraftInitialized = false;
    }
}

void UAlbertaBotModule_Arena::printInfo()
{
    char red = '\x08';
    char green = '\x07';
    char white = '\x04';

    if (!Config::ConfigFile::ConfigFileFound)
    {
        BWAPI::Broodwar->drawBoxScreen(0, 0, 450, 100, BWAPI::Colors::Black, true);
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
        BWAPI::Broodwar->drawBoxScreen(0, 0, 450, 100, BWAPI::Colors::Black, true);
        BWAPI::Broodwar->setTextSize(BWAPI::Text::Size::Huge);
        BWAPI::Broodwar->drawTextScreen(10, 5, "%c%s Config File Parse Error", red, Config::BotInfo::BotName.c_str());
        BWAPI::Broodwar->setTextSize(BWAPI::Text::Size::Default);
        BWAPI::Broodwar->drawTextScreen(10, 30, "%c%s will not run without a properly formatted configuration file", white, Config::BotInfo::BotName.c_str());
        BWAPI::Broodwar->drawTextScreen(10, 45, "%cThe configuration file was found, but could not be parsed. Check that it is valid JSON", white);
        BWAPI::Broodwar->drawTextScreen(10, 60, "%cFile Not Parsed: %c %s", white, green, Config::ConfigFile::ConfigFileLocation.c_str());
        return;
    }
    else if (!_isSparCraftInitialized)
    {
        BWAPI::Broodwar->drawBoxScreen(0, 0, 450, 100, BWAPI::Colors::Black, true);
        BWAPI::Broodwar->setTextSize(BWAPI::Text::Size::Huge);
        BWAPI::Broodwar->drawTextScreen(10, 5, "%c%s SparCraft Init Error", red, Config::BotInfo::BotName.c_str());
        BWAPI::Broodwar->setTextSize(BWAPI::Text::Size::Default);
        BWAPI::Broodwar->drawTextScreen(10, 30, "%c%s will not run without SparCraft successfully initialized", white, Config::BotInfo::BotName.c_str());
        BWAPI::Broodwar->drawTextScreen(10, 45, "%cThe Exception thrown by SparCraft was:", white);
        BWAPI::Broodwar->drawTextScreen(10, 60, "%c%s", red, _lastSparCraftException.what());
        return;
    }
}
