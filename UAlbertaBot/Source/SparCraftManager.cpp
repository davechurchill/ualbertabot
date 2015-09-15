#include "SparCraftManager.h"

using namespace UAlbertaBot;

SparCraftManager::SparCraftManager() 
    : gameOver(false)

{
    SparCraft::init();
}


void SparCraftManager::onStart()
{
    SparCraft::GameState state(extractGameState());

}

void SparCraftManager::update()
{
    if (!gameOver)
    {
        // for each unit that we control, do some book keeping
        for (BWAPI::UnitInterface* unit : BWAPI::Broodwar->self()->getUnits())
        {
            BWAPI::Broodwar->drawTextMap(unit->getPosition().x-20, unit->getPosition().y-12, "%d", unit->getID());

            // if it is a combat unit
            if (isCombatUnit(unit))
            {
                if (Config::Debug::DrawUnitTargetInfo && unit->getTarget())
                {
                    BWAPI::Broodwar->drawLineMap(unit->getPosition().x-2, unit->getPosition().y-2, 	unit->getTarget()->getPosition().x, unit->getTarget()->getPosition().y, BWAPI::Colors::White);
                }
            }
        }

        performSparCraft();
    }
}

void SparCraftManager::getMoves(SparCraft::GameState & state, std::vector<SparCraft::UnitAction> & moveVec)
{
    const SparCraft::IDType playerID = getPlayerID(BWAPI::Broodwar->self());
    SparCraft::MoveArray moves;
    state.generateMoves(moves, playerID);

    SparCraft::PlayerPtr gsPlayer(new SparCraft::Player_PortfolioGreedySearch(playerID, SparCraft::PlayerModels::NOKDPS, 1, 0, 0));

    gsPlayer->getMoves(state, moves, moveVec);
}

void SparCraftManager::performSparCraft()
{
    SparCraft::GameState currentState = extractGameState();

    int currentFrame = BWAPI::Broodwar->getFrameCount();

    for (BWAPI::UnitInterface* unit : BWAPI::Broodwar->enemy()->getUnits())
    {
        BWAPI::Broodwar->drawTextMap(unit->getPosition().x, unit->getPosition().y, "%d", unit->getGroundWeaponCooldown());
    }

    // draw our units
    for (size_t u(0); u<currentState.numUnits(SparCraft::Players::Player_One); ++u)
    {
        SparCraft::Unit & unit = currentState.getUnit(SparCraft::Players::Player_One, u);
        BWAPI::Broodwar->drawCircleMap(unit.x(), unit.y(), 5, BWAPI::Colors::Green);
        BWAPI::Broodwar->drawCircleMap(unit.x(), unit.y(), unit.range(), BWAPI::Colors::Grey);

        std::pair<int, int> cooldown = getUnitCooldown(BWAPI::Broodwar->getUnit(unit.ID()), unit);

        BWAPI::UnitInterface* realUnit = getUnit(unit);

        BWAPI::Broodwar->drawTextMap(unit.x(), unit.y(), "%d (%d %d %d)", unit.ID(), cooldown.first-currentFrame, cooldown.second-currentFrame, BWAPI::Broodwar->getUnit(unit.ID())->getGroundWeaponCooldown());
        BWAPI::Broodwar->drawLineMap(unit.x(), unit.y(), realUnit->getPosition().x, realUnit->getPosition().y, BWAPI::Colors::Purple);
    }

    // TODO: Check why zealots aren't being given commands

    // draw their units
    for (size_t u(0); u<currentState.numUnits(SparCraft::Players::Player_Two); ++u)
    {
        SparCraft::Unit & unit = currentState.getUnit(SparCraft::Players::Player_Two, u);
        drawUnitHP(BWAPI::Broodwar->getUnit(unit.ID()));
        //BWAPI::Broodwar->drawCircleMap(unit.x, unit.y, 5, BWAPI::Colors::Red);
    }

    // draw our moves if we are the player to move
    const SparCraft::IDType whoCanMove = currentState.whoCanMove();
    if ((whoCanMove == SparCraft::Players::Player_One) || (whoCanMove == SparCraft::Players::Player_Both))
    {
        // get the best move tuple from the current state
        //MoveTuple bestTuple = getMoveTuple(currentState, Search::PlayerModels::AlphaBeta);
        std::vector<SparCraft::UnitAction> moveVec;
        getMoves(currentState, moveVec);

        // extract all of the moves possible from the current state
        SparCraft::MoveArray moves;
        currentState.generateMoves(moves, SparCraft::Players::Player_One);

        // draw the best move for each unit
        for (size_t u(0); u<moves.numUnits(); ++u)
        {
            // the move for the first unit to move
            SparCraft::UnitAction move = moveVec[u];

            // the unit for which the move is to be performed
            SparCraft::Unit & unit = currentState.getUnit(SparCraft::Players::Player_One, move.unit());
            BWAPI::Broodwar->drawCircleMap(unit.x(), unit.y(), 5, BWAPI::Colors::Red);

            // draw the move this unit should do
            drawUnitMove(currentState, unit, move);
            drawUnitCooldown(BWAPI::Broodwar->getUnit(unit.ID()));
            drawUnitHP(BWAPI::Broodwar->getUnit(unit.ID()));

            // do the move
            doUnitMove(currentState, unit, move);
        }
    }
}

void SparCraftManager::doUnitMove(SparCraft::GameState & currentState, SparCraft::Unit & unit, SparCraft::UnitAction & move)
{
    SparCraft::IDType enemyPlayer = (unit.player() + 1) % 2;

    BWAPI::UnitInterface* u = BWAPI::Broodwar->getUnit(unit.ID());

    if (move._moveType == SparCraft::UnitActionTypes::ATTACK)
    {
        BWAPI::Broodwar->drawTextMap(unit.x()+5, unit.y()+5, "A");

        SparCraft::Unit & enemyUnit(currentState.getUnit(enemyPlayer, move._moveIndex));

        UnitCommandManager::Instance().smartAttackUnit(u, BWAPI::Broodwar->getUnit(enemyUnit.ID()));
    }
    else if (move._moveType == SparCraft::UnitActionTypes::MOVE)
    {
        BWAPI::Broodwar->drawTextMap(unit.x()+5, unit.y()+5, "M");

        SparCraft::Position pos(SparCraft::Constants::Move_Dir[move._moveIndex][0], SparCraft::Constants::Move_Dir[move._moveIndex][1]);
        SparCraft::Position dest(unit.x() + (pos.x() * 4*SparCraft::Constants::Move_Distance), unit.y() + (pos.y() * 4*SparCraft::Constants::Move_Distance));

        UnitCommandManager::Instance().smartMove(u, BWAPI::Position(dest.x(), dest.y()));
    }
    else if (move._moveType == SparCraft::UnitActionTypes::RELOAD)
    {
        UnitCommandManager::Instance().smartReload(u);
    }
}

void SparCraftManager::drawAttackDebug()
{
    char * trueFix = "\x07";
    char * falseFix = "\x06";

    for (BWAPI::UnitInterface* unit : BWAPI::Broodwar->self()->getUnits())
    {
        int x = unit->getPosition().x;
        int y = unit->getPosition().y + 9;

        BWAPI::Broodwar->drawTextMap(x, y, "%s isAttacking", unit->isAttacking() ? trueFix : falseFix);
        BWAPI::Broodwar->drawTextMap(x, y+10, "%s isAttackFrame", unit->isAttackFrame() ? trueFix : falseFix);
        BWAPI::Broodwar->drawTextMap(x, y+20, "%s isMoving", unit->isMoving() ? trueFix : falseFix);
    }
}

void SparCraftManager::drawUnitMove(SparCraft::GameState & currentState, SparCraft::Unit & unit, SparCraft::UnitAction & move)
{
    SparCraft::IDType enemyPlayer = (unit.player() + 1) % 2;

    if (move._moveType == SparCraft::UnitActionTypes::ATTACK)
    {
        SparCraft::Unit & enemyUnit(currentState.getUnit(enemyPlayer,move._moveIndex));

        BWAPI::Broodwar->drawLineMap(unit.x(), unit.y(), enemyUnit.x(), enemyUnit.y(), BWAPI::Colors::Cyan);
    }
    else if (move._moveType == SparCraft::UnitActionTypes::MOVE)
    {
        SparCraft::Position pos(SparCraft::Constants::Move_Dir[move._moveIndex][0], SparCraft::Constants::Move_Dir[move._moveIndex][1]);
        SparCraft::Position dest(unit.x() + (pos.x() * 32), unit.y() + (pos.y() * 32));

        BWAPI::Broodwar->drawLineMap(unit.x(), unit.y(), dest.x(), dest.y(), BWAPI::Colors::Yellow);
    }
}

void SparCraftManager::drawUnitCooldown(BWAPI::UnitInterface* unit)
{
    double totalCooldown = unit->getType().groundWeapon().damageCooldown();
    double remainingCooldown = unit->getGroundWeaponCooldown();
    double percCooldown = remainingCooldown / (totalCooldown+1);

    int w = 32;
    int h = 4;

    int cw = w - (int)(w * percCooldown);
    int ch = h;

    int x1 = unit->getPosition().x - w/2;
    int y1 = unit->getPosition().y - 16;

    BWAPI::Broodwar->drawBoxMap(x1, y1, x1 + w, y1 + h, BWAPI::Colors::Grey, true);
    BWAPI::Broodwar->drawBoxMap(x1, y1, x1 + cw, y1 + ch, BWAPI::Colors::Red, true);
}

void SparCraftManager::drawUnitHP(BWAPI::UnitInterface* unit)
{
    double totalHP = unit->getType().maxHitPoints() + unit->getType().maxShields();
    double currentHP = unit->getHitPoints() + unit->getShields();
    double percHP = currentHP / (totalHP+1);

    int w = 32;
    int h = 4;

    int cw = (int)(w * percHP);
    int ch = h;

    int x1 = unit->getPosition().x - w/2;
    int y1 = unit->getPosition().y - 12;

    BWAPI::Broodwar->drawBoxMap(x1, y1, x1 + w, y1 + h, BWAPI::Colors::Grey, true);
    BWAPI::Broodwar->drawBoxMap(x1, y1, x1 + cw, y1 + ch, BWAPI::Colors::Green, true);
}

const SparCraft::IDType SparCraftManager::getPlayerID(BWAPI::PlayerInterface * player) const
{
    return (player == BWAPI::Broodwar->self()) ? SparCraft::Players::Player_One : SparCraft::Players::Player_Two;
}

const bool SparCraftManager::isCombatUnit(BWAPI::UnitInterface* unit) const
{
    assert(unit != NULL);

    // no workers or buildings allowed
    if (unit && unit->getType().isWorker() || unit->getType().isBuilding())
    {
        return false;
    }

    // check for various types of combat units
    if (unit->getType().canAttack() || 
        unit->getType() == BWAPI::UnitTypes::Terran_Medic ||
        unit->getType() == BWAPI::UnitTypes::Protoss_High_Templar ||
        unit->getType() == BWAPI::UnitTypes::Protoss_Observer)
    {
        return true;
    }

    return false;
}

BWAPI::UnitInterface* SparCraftManager::getUnit(SparCraft::Unit & unit)
{
    for (BWAPI::UnitInterface* u : BWAPI::Broodwar->getAllUnits())
    {
        if (u->getID() == unit.ID())
        {
            return u;
        }
    }

    return NULL;
}

const std::pair<int, int> SparCraftManager::getUnitCooldown(BWAPI::UnitInterface* unit, SparCraft::Unit & u) const
{
    int attackCooldown(0);
    int moveCooldown(0);

    BWAPI::UnitCommand lastCommand = unit->getLastCommand();
    int lastCommandFrame = unit->getLastCommandFrame();
    int currentFrame = BWAPI::Broodwar->getFrameCount();
    int framesSinceCommand = currentFrame - lastCommandFrame;

    if ((unit->getType() == BWAPI::UnitTypes::Protoss_Dragoon) && (unit->getLastCommand().getType() == BWAPI::UnitCommandTypes::Attack_Unit))
    {
        // dragoons are one of only 2 unit types whose attack can be canceled by the in-game targeter being called too early so
        // this hack makes up for that by taking it's stop-delay into account
        attackCooldown = BWAPI::Broodwar->getFrameCount() + std::max(0, unit->getGroundWeaponCooldown()-SparCraft::AnimationFrameData::getAttackFrames(unit->getType()).first);
    }
    else
    {
        attackCooldown = BWAPI::Broodwar->getFrameCount() + std::max(0, unit->getGroundWeaponCooldown()-2);
    }

    // if the last attack was an attack command
    if (lastCommand.getType() == BWAPI::UnitCommandTypes::Attack_Unit)
    {
        moveCooldown = BWAPI::Broodwar->getFrameCount() + std::max(0, u.attackInitFrameTime() - framesSinceCommand);

        //BWAPI::Broodwar->drawTextScreen(100,100, "%d, %d", attackCooldown-currentFrame, moveCooldown-currentFrame);
    }
    // if the last command was a move command
    else if (lastCommand.getType() == BWAPI::UnitCommandTypes::Move)
    {
        moveCooldown = currentFrame;
    }

    if (moveCooldown - BWAPI::Broodwar->getFrameCount() < 4 || unit->isMoving())
    {
        moveCooldown = currentFrame;
    }

    moveCooldown = std::min(moveCooldown, attackCooldown);

    return std::pair<int, int>(attackCooldown, moveCooldown);
}

//Unit(const BWAPI::UnitType unitType, const Position & pos, const IDType & unitID, const IDType & playerID, 
//		 const HealthType & hp, const HealthType & energy, const TimeType & tm, const TimeType & ta);
SparCraft::Unit SparCraftManager::getSparCraftUnit(const BWAPI::UnitInterface* bwapiUnit, BWAPI::GameWrapper & game)
{
    BWAPI::UnitType         type        = bwapiUnit->getType();
    SparCraft::Position     pos         = SparCraft::Position(bwapiUnit->getPosition().x, bwapiUnit->getPosition().y); 
    SparCraft::IDType       unitID      = (SparCraft::IDType)bwapiUnit->getID();
    SparCraft::IDType       playerID    = (SparCraft::IDType)getPlayerID(bwapiUnit->getPlayer());
    SparCraft::HealthType   health      = (SparCraft::HealthType)(bwapiUnit->getHitPoints() + bwapiUnit->getShields());
    SparCraft::HealthType   energy      = (SparCraft::HealthType)(bwapiUnit->getEnergy());
    SparCraft::TimeType     timeCanMove = (SparCraft::TimeType)(game->getFrameCount());
    SparCraft::TimeType     timeCanAtk  = (SparCraft::TimeType)(game->getFrameCount() + bwapiUnit->getGroundWeaponCooldown() + bwapiUnit->getAirWeaponCooldown());

    return SparCraft::Unit(type, pos, unitID, playerID, health, energy, timeCanMove, timeCanAtk);
}

// constructor based on a BWAPI unit, used by UAlbertaBot
/*Unit::Unit(BWAPI::UnitInterface* unit, BWAPI::Game * game, const IDType & playerID, const TimeType & gameTime)
    : _unitType             (unit->getType() == BWAPI::UnitTypes::Terran_Medic ? BWAPI::UnitTypes::Terran_Marine : unit->getType())
    , _range                (unit->getType().groundWeapon().maxRange() + 32)
    , _position             (Position(unit->getPosition().x, unit->getPosition().y))
    , _unitID               ((IDType)unit->getID())
    , _playerID             (playerID)
    , _currentHP            ((HealthType)(unit->getHitPoints() + unit->getShields()))
    , _currentEnergy        ((HealthType)unit->getEnergy())
    , _timeCanMove          ((TimeType)(game->getFrameCount()))
    , _timeCanAttack        ((TimeType)(game->getFrameCount() + unit->getGroundWeaponCooldown() + unit->getAirWeaponCooldown()))
    , _previousActionTime   (gameTime)
    , _prevCurrentPosTime   (0)
    , _previousPosition     (Position(unit->getPosition().x, unit->getPosition().y))
{
}*/

// get an abstract GameState object from the current BWAPI state
SparCraft::GameState SparCraftManager::extractGameState()
{
    // construct a state with the current time
    SparCraft::GameState state;
    state.setTime(BWAPI::Broodwar->getFrameCount());

    // add each of our fighting units
    for (BWAPI::UnitInterface* unit : BWAPI::Broodwar->self()->getUnits())
    {
        if (isCombatUnit(unit))
        {
            SparCraft::Unit u = getSparCraftUnit(unit, BWAPI::Broodwar);
            std::pair<int, int> cd = getUnitCooldown(unit, u);
            u.setCooldown(cd.first, cd.second);
            state.addUnitWithID(u);
        }
    }

    for (BWAPI::UnitInterface* unit : BWAPI::Broodwar->enemy()->getUnits())
    {
        if (isCombatUnit(unit))
        {
            SparCraft::Unit u = getSparCraftUnit(unit, BWAPI::Broodwar);
            u.setCooldown(BWAPI::Broodwar->getFrameCount(), BWAPI::Broodwar->getFrameCount() + unit->getGroundWeaponCooldown());
            state.addUnitWithID(u);
        }
    }

    if (state.numUnits(0) == 0 && state.numUnits(1) == 0)
    {
        gameOver = true;
    }
    else if (state.numUnits(1) == 0)
    {
        gameOver = true;
    }
    else if (state.numUnits(0) == 0)
    {
        gameOver = true;
    }

    if (gameOver)
    {	
        BWAPI::Broodwar->restartGame();
    }

    state.finishedMoving();
    return state;
}
