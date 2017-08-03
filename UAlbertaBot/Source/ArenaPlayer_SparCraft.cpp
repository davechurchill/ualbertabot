#include "ArenaPlayer_SparCraft.h"
#include "Micro.h"

using namespace UAlbertaBot;

ArenaPlayer_SparCraft::ArenaPlayer_SparCraft()
{
    _name = "ArenaPlayer_SparCraft";
}

void ArenaPlayer_SparCraft::onStart()
{
    try
    {
        // Initialize SparCraft, the combat simulation package
        SparCraft::init();

        // Read the SparCraft configuration file
        SparCraft::AIParameters::Instance().parseFile(Config::SparCraft::SparCraftConfigFile);
       

        std::cout << "SparCraft initialized\n";
    }
    catch (SparCraft::SparCraftException e)
    {
        std::cout << "Error: SparCraft not initialized properly\n";
        exit(-1);
    }
}

void ArenaPlayer_SparCraft::onFrame()
{

}

void ArenaPlayer_SparCraft::onBattleBegin()
{

}

void ArenaPlayer_SparCraft::onBattleEnd()
{

}

void ArenaPlayer_SparCraft::PlaySparCraftSimulation(const SparCraft::GameState & state)
{
	auto& aiParameters = SparCraft::AIParameters::Instance();
	auto sparcraftSelfPlayer = GetSparCraftPlayerID(BWAPI::Broodwar->self());
	auto sparcraftEnemyPlayer = GetSparCraftPlayerID(BWAPI::Broodwar->enemy());
    SparCraft::PlayerPtr player = aiParameters.getPlayer(sparcraftSelfPlayer, Config::Arena::ArenaPlayerName);
    SparCraft::PlayerPtr enemy =  aiParameters.getPlayer(sparcraftEnemyPlayer, "AttackC");

    SparCraft::Game g(state, player, enemy);
    g.play();
}

SparCraft::Move ArenaPlayer_SparCraft::GetSparCraftPlayerMove(const SparCraft::GameState & state, const size_t & playerID) const
{
    SparCraft::PlayerPtr player = SparCraft::AIParameters::Instance().getPlayer(playerID, Config::Arena::ArenaPlayerName);

    SparCraft::Move move;
    player->getMove(state, move);

    return move;
}

SparCraft::GameState ArenaPlayer_SparCraft::GetSparCraftState(int currentFrame) const
{
    SparCraft::GameState state;

    for (const auto & unit : BWAPI::Broodwar->getAllUnits())
    {
        size_t playerID = GetSparCraftPlayerID(unit->getPlayer());

        if (playerID != SparCraft::Players::Player_None && SparCraft::System::UnitTypeSupported(unit->getType()))
        {
            state.addUnit(GetSparCraftUnit(unit, currentFrame));
        }
    }

    state.setTime(currentFrame);

    return state;
}

SparCraft::Unit ArenaPlayer_SparCraft::GetSparCraftUnit(BWAPI::Unit unit, int currentFrame) const
{
    SparCraft::Unit sUnit(unit->getType(),
        SparCraft::Position(unit->getPosition()),
        0,
        GetSparCraftPlayerID(unit->getPlayer()),
        unit->getHitPoints() + unit->getShields(),
        0,
        currentFrame + GetTimeCanMove(unit),
        currentFrame + GetTimeCanAttack(unit));

    sUnit.setBWAPIUnitID(unit->getID());

    return sUnit;
}

size_t ArenaPlayer_SparCraft::GetSparCraftPlayerID(BWAPI::Player player) const
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

void ArenaPlayer_SparCraft::DrawSparCraftMove(AKBot::ScreenCanvas& canvas, const SparCraft::GameState & state, const SparCraft::Move & move) const
{
    for (size_t a(0); a < move.size(); ++a)
    {
        const auto & action = move[a];
        const SparCraft::Unit & sUnit = state.getUnitByID(action.getID());
        const BWAPI::Unit & unit = BWAPI::Broodwar->getUnit(sUnit.getBWAPIUnitID());

        if (action.type() == SparCraft::ActionTypes::MOVE)
        {
            BWAPI::Position dest(action.pos().x(), action.pos().y());

            canvas.drawLineMap(unit->getPosition(), dest, BWAPI::Colors::White);
        }
        else if (action.type() == SparCraft::ActionTypes::ATTACK)
        {
            const SparCraft::Unit & sTargetUnit = state.getUnitByID(action.getTargetID());
            const BWAPI::Unit & targetUnit = BWAPI::Broodwar->getUnit(sTargetUnit.getBWAPIUnitID());
            const BWAPI::Position & dest = targetUnit->getPosition();

            canvas.drawLineMap(unit->getPosition(), dest, BWAPI::Colors::Red);
        }
    }
}

void ArenaPlayer_SparCraft::DoSparCraftMove(
	AKBot::ScreenCanvas& canvas,
	const SparCraft::GameState & state,
	const SparCraft::Move & move,
	int currentFrame) const
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
            
            canvas.drawLineMap(unit->getPosition(), dest, BWAPI::Colors::Yellow);
        }
        else if (action.type() == SparCraft::ActionTypes::ATTACK)
        {
            const SparCraft::Unit & sTargetUnit = state.getUnitByID(action.getTargetID());
            const BWAPI::Unit & targetUnit = BWAPI::Broodwar->getUnit(sTargetUnit.getBWAPIUnitID());
            const BWAPI::Position & dest = targetUnit->getPosition();

            //unit->rightClick(targetUnit);
            Micro::SmartAttackUnit(unit, targetUnit, currentFrame);

            canvas.drawLineMap(unit->getPosition(), dest, BWAPI::Colors::Green);
        }
    }
}


int ArenaPlayer_SparCraft::GetTimeSinceLastAttack(BWAPI::Unit unit) const
{
    return unit->getType().groundWeapon().damageCooldown() - unit->getGroundWeaponCooldown();
}

void ArenaPlayer_SparCraft::DrawSparCraftState(AKBot::ScreenCanvas& canvas, const SparCraft::GameState & state, int x, int y) const
{
    std::stringstream ss;
    for (size_t p(0); p < 2; ++p)
    {
        for (size_t u(0); u < state.numUnits(p); ++u)
        {
            const SparCraft::Unit & unit = state.getUnit(p, u);

            ss << unit.getID() << " " << unit.type().getName() << " (" << unit.pos().x() << "," << unit.pos().y() << ")\n";          
        }
    }

    canvas.drawTextScreen(BWAPI::Position(x, y), ss.str().c_str());
}


int ArenaPlayer_SparCraft::GetTimeCanMove(BWAPI::Unit unit) const
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

int ArenaPlayer_SparCraft::GetTimeCanAttack(BWAPI::Unit unit) const
{
    int attackTime = unit->getGroundWeaponCooldown();

    // some slight tweaks on a unitType basis
    switch (unit->getType())
    {
		case BWAPI::UnitTypes::Enum::Terran_Wraith:   { attackTime -= 13; break; }
        case BWAPI::UnitTypes::Enum::Terran_Vulture:  { attackTime -= 1;  break; }
        default:                                { attackTime -= 2;  break; }
    }
        
    if (attackTime < 0)
    {
        attackTime = 0;
    }

    return attackTime;
}
