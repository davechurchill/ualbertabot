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

using namespace UAlbertaBot;

UAlbertaBot_Arena::UAlbertaBot_Arena()
    : _isSparCraftInitialized(false)
    , _wins(SparCraft::Players::Size, 0)
    , _simWins(SparCraft::Players::Size, 0)
    , _battles(0)
    , _battleEnded(true)
    , _prevNumUnits(0)
{

}

// This gets called when the bot starts!
void UAlbertaBot_Arena::onStart()
{
    // Parse the bot's configuration file if it has one, change this file path to where your config file is
    // Any relative path name will be relative to Starcraft installation folder
    ParseUtils::ParseConfigFile(Config::ConfigFile::ConfigFileLocation);

    // Initialize the SparCraft project settings
    initializeSparCraft();

    // Set our BWAPI options here    
    BWAPI::Broodwar->setLocalSpeed(Config::BWAPIOptions::SetLocalSpeed);
    BWAPI::Broodwar->setFrameSkip(Config::BWAPIOptions::SetFrameSkip);

    // Set the starting date
    _startDate = Assert::currentDateTime();

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

void UAlbertaBot_Arena::onFrame()
{
    printInfo();
    DrawUnitHPBars();

    int numUnits = 0;
    for (auto & unit : BWAPI::Broodwar->getAllUnits())
    {
        if (unit->getType() == BWAPI::UnitTypes::Terran_Marine)
        {
            numUnits++;
        }
    }

    SparCraft::GameState state = GetSparCraftState();
    
    if (_prevNumUnits < 3 && numUnits == 3)
    {
        PlaySparCraftSimulation(state);
    }
    
  
    std::string resultsDir = "C:/Libraries/ualbertabot/UAlbertaBot/bin/results/";

    std::stringstream filenamess;
    filenamess << _startDate << "___" << BWAPI::Broodwar->mapFileName() << "_" << Config::Arena::ArenaPlayerName << "_MP" << SparCraft::Config::Units::GroundUnitMovePenalty << ".txt";
    std::string filename = filenamess.str();

    for (size_t i=0; i<filename.size(); ++i)
    {
        if (filename[i] == ':') filename[i] = '-';
    }

    if (!state.gameOver())
    {
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
        
            int wins = _wins[SparCraft::Players::Player_One];
            int losses = _wins[SparCraft::Players::Player_Two];
            int draws = _wins[SparCraft::Players::Player_None];

            std::stringstream ss;
            ss << filename << " \x04\n\n";
            ss << BWAPI::Broodwar->mapFileName() << "\nPlayer: " << Config::Arena::ArenaPlayerName << "\n";
            ss << "MovePenalty: " << SparCraft::Config::Units::GroundUnitMovePenalty << "\n\n";
            ss << SparCraft::AIParameters::Instance().getPlayer(0, "PGS")->getDescription() << "\n";
            ss << "Battles: " << _battles << "\nWins:   " << wins << "\nLosses: " << losses << "\nDraws:  " << draws << "\nScore:  " << ((double)(wins + (double)draws/2)/_battles);
    
            _resultString = ss.str();

            // output results to file every 10 battles
            if (_battles % Config::Arena::ArenaOutputResults == 0)
            {
                std::ofstream out(resultsDir + filename);
                out << ss.str();
                out.close();
            }

            if (_battles == Config::Arena::ArenaBattles)
            {
                exit(0);
            }
        }
    }

    _prevNumUnits = numUnits;
}


void UAlbertaBot_Arena::PlaySparCraftSimulation(const SparCraft::GameState & state)
{
    SparCraft::PlayerPtr player = SparCraft::AIParameters::Instance().getPlayer(GetSparCraftPlayerID(BWAPI::Broodwar->self()), Config::Arena::ArenaPlayerName);
    SparCraft::PlayerPtr enemy =  SparCraft::AIParameters::Instance().getPlayer(GetSparCraftPlayerID(BWAPI::Broodwar->enemy()), "AttackC");

    SparCraft::Game g(state, player, enemy);
    g.play();

    _simWins[g.getState().winner()]++;
}

SparCraft::Move UAlbertaBot_Arena::GetSparCraftPlayerMove(const SparCraft::GameState & state, const size_t & playerID) const
{
    SparCraft::PlayerPtr player = SparCraft::AIParameters::Instance().getPlayer(playerID, Config::Arena::ArenaPlayerName);

    SparCraft::Move move;
    player->getMove(state, move);

    return move;
}

SparCraft::GameState UAlbertaBot_Arena::GetSparCraftState() const
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

SparCraft::Unit UAlbertaBot_Arena::GetSparCraftUnit(BWAPI::Unit unit) const
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

size_t UAlbertaBot_Arena::GetSparCraftPlayerID(BWAPI::Player player) const
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

void UAlbertaBot_Arena::DrawSparCraftMove(const SparCraft::GameState & state, const SparCraft::Move & move) const
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

void UAlbertaBot_Arena::DoSparCraftMove(const SparCraft::GameState & state, const SparCraft::Move & move) const
{
    for (size_t a(0); a < move.size(); ++a)
    {
        const auto & action = move[a];
        const SparCraft::Unit & sUnit = state.getUnitByID(action.getID());
        const BWAPI::Unit & unit = BWAPI::Broodwar->getUnit(sUnit.getBWAPIUnitID());

        if (action.type() == SparCraft::ActionTypes::MOVE)
        {
            BWAPI::Position dest(action.pos().x(), action.pos().y());
            BWAPI::Position delta = dest - unit->getPosition();
            delta = BWAPI::Position(delta.x * 5, delta.y * 5);

            
            unit->rightClick(unit->getPosition() + delta);
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

int UAlbertaBot_Arena::GetTimeSinceLastAttack(BWAPI::Unit unit) const
{
    return unit->getType().groundWeapon().damageCooldown() - unit->getGroundWeaponCooldown();
}

void UAlbertaBot_Arena::DrawSparCraftState(const SparCraft::GameState & state, int x, int y) const
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

void UAlbertaBot_Arena::DrawUnitHPBars() const
{
    if (!Config::Debug::DrawUnitHealthBars) { return; }

    for (const auto & unit : BWAPI::Broodwar->getAllUnits())
    {
        DebugTools::DrawUnitHPBar(unit->getType(), unit->getPosition(), unit->getHitPoints(), unit->getShields());

        std::stringstream ss;
        ss << unit->getID() << " " << unit->getGroundWeaponCooldown() << " " << GetTimeSinceLastAttack(unit) << " " << GetTimeCanMove(unit) << " " << GetTimeCanAttack(unit);

        BWAPI::Broodwar->drawTextMap(unit->getPosition() - BWAPI::Position(0, 30), ss.str().c_str());

        if (unit->getType().getRace() != BWAPI::Races::None && unit->getPlayer() == BWAPI::Broodwar->self())
        {
            BWAPI::Broodwar->drawLineMap(unit->getPosition() + BWAPI::Position(3, 3), unit->getOrderTargetPosition() + BWAPI::Position(3, 3), BWAPI::Colors::Red);
        }
    }

    BWAPI::Broodwar->drawTextScreen(BWAPI::Position(10, 10), _resultString.c_str());
}

int UAlbertaBot_Arena::GetTimeCanMove(BWAPI::Unit unit) const
{
    int groundCooldown = unit->getGroundWeaponCooldown();
    if (groundCooldown == 0)
    {
        return 0;
    }

    int lastAttack = GetTimeSinceLastAttack(unit);
    int attackFrames = SparCraft::Config::Units::GetAttackFrames(unit->getType()).first;
    int time = std::max(attackFrames - lastAttack, 0);

    if (time > 6)
    {
        int a = 6;
    }

    return time;
}

int UAlbertaBot_Arena::GetTimeCanAttack(BWAPI::Unit unit) const
{
    int attackTime = unit->getGroundWeaponCooldown();

    // some slight tweaks on a unitType basis
    switch (unit->getType())
    {
        case BWAPI::UnitTypes::Terran_Wraith:   { attackTime -= 13; break; }
        case BWAPI::UnitTypes::Terran_Vulture:  { attackTime -= 1;  break; } 
        default:                                { attackTime -= 2;  break; }
    }
        
    if (attackTime < 0)
    {
        attackTime = 0;
    }

    return attackTime;
}

void UAlbertaBot_Arena::onEnd(bool isWinner)
{

}

void UAlbertaBot_Arena::onUnitDestroy(BWAPI::Unit unit)
{

}

void UAlbertaBot_Arena::onUnitMorph(BWAPI::Unit unit)
{

}

void UAlbertaBot_Arena::onSendText(std::string text)
{

}

void UAlbertaBot_Arena::onUnitCreate(BWAPI::Unit unit)
{

}

void UAlbertaBot_Arena::onUnitComplete(BWAPI::Unit unit)
{

}

void UAlbertaBot_Arena::onUnitShow(BWAPI::Unit unit)
{

}

void UAlbertaBot_Arena::onUnitHide(BWAPI::Unit unit)
{

}

void UAlbertaBot_Arena::onUnitRenegade(BWAPI::Unit unit)
{

}

void UAlbertaBot_Arena::initializeSparCraft()
{
    try
    {
        // Initialize SparCraft, the combat simulation package
        SparCraft::init();

        // Read the SparCraft configuration file
        SparCraft::AIParameters::Instance().parseFile(Config::SparCraft::SparCraftConfigFile);
        _isSparCraftInitialized = true;

        std::cout << "SparCraft initialized\n";
    }
    catch (SparCraft::SparCraftException e)
    {
        _lastSparCraftException = e;
        _isSparCraftInitialized = false;

        std::cout << "Error: SparCraft not initialized properly\n";
    }
}

void UAlbertaBot_Arena::printInfo()
{

}
