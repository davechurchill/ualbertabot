#include "ActionGenerators.h"

using namespace SparCraft;

const size_t Num_Directions     = 4;    // number of directions that units can move
const size_t Max_Ordered_Moves  = 10;   // max number of ordered moves in a search depth
const size_t Move_Distance  = 16;       // distance moved for a 'move' command

void ActionGenerators::GenerateCompassActions(const GameState & state, const size_t & player, MoveArray & moves)
{
	moves.clear();

    // which is the enemy player
	size_t enemyPlayer  = state.getEnemy(player);

    // make sure this player can move right now
    const size_t canMove = state.whoCanMove();
    if (canMove == enemyPlayer)
    {
        SPARCRAFT_ASSERT(false, "GameState Error - Called generateMoves() for a player that cannot currently move");
    }

	// we are interested in all simultaneous moves
	// so return all units which can move at the same time as the first
	TimeType firstUnitMoveTime = state.getTimeNextUnitCanAct(player);

	for (size_t unitIndex(0); unitIndex < state.numUnits(player); ++unitIndex)
	{
		// unit reference
		const Unit & unit = state.getUnit(player,unitIndex);

		// if this unit can't move at the same time as the first
		if (unit.firstTimeFree() > firstUnitMoveTime)
		{
			// stop checking
			continue;
		}

		if (unit.previousActionTime() == state.getTime() && state.getTime() != 0)
		{
            SPARCRAFT_ASSERT(false, "Previous Move Took 0 Time: %s", unit.previousAction().moveString().c_str());
		}

		// generate attack moves
		if (unit.canAttackNow())
		{
			for (size_t u(0); u < state.numUnits(enemyPlayer); ++u)
			{
				const Unit & enemyUnit = state.getUnit(enemyPlayer, u);
				bool invisible = false;
				if (enemyUnit.type().hasPermanentCloak())
				{
					invisible = true;
					for (size_t detectorIndex(0); detectorIndex < state.numUnits(player); ++detectorIndex)
					{
						// unit reference
						const Unit & detector = state.getUnit(player, detectorIndex);
						if (detector.type().isDetector() && detector.canSeeTarget(enemyUnit, state.getTime()))
						{
							invisible = false;
							break;
						}
					}
				}
				if (!invisible && unit.canAttackTarget(enemyUnit, state.getTime()) && enemyUnit.isAlive())
				{
					moves.add(Action(unit.getID(), player, ActionTypes::ATTACK, enemyUnit.getID()));
                    //moves.add(Action(unitIndex, player, ActionTypes::ATTACK, unit.getID()));
				}
			}
		}
		else if (unit.canHealNow())
		{
			for (size_t u(0); u< state.numUnits(player); ++u)
			{
				// units cannot heal themselves in broodwar
				if (u == unitIndex)
				{
					continue;
				}

				const Unit & ourUnit = state.getUnit(player, u);
				if (unit.canHealTarget(ourUnit, state.getTime()) && ourUnit.isAlive())
				{
					moves.add(Action(unit.getID(), player, ActionTypes::HEAL, ourUnit.getID()));
                    //moves.add(Action(unitIndex, player, ActionTypes::HEAL, unit.getID()));
				}
			}
		}
		// generate the wait move if it can't attack yet
		else
		{
			if (!unit.canHeal())
			{
				moves.add(Action(unit.getID(), player, ActionTypes::RELOAD, 0));
			}
		}
		
		// generate movement moves
		if (unit.isMobile())
		{
            // In order to not move when we could be shooting, we want to move for the minimum of:
            // 1) default move distance move time
            // 2) time until unit can attack, or if it can attack, the next cooldown
            double timeUntilAttack          = unit.nextAttackActionTime() - state.getTime();
            timeUntilAttack                 = timeUntilAttack == 0 ? unit.attackCooldown() : timeUntilAttack;

            // the default move duration
            double defaultMoveDuration      = (double)Move_Distance / unit.speed();

            // if we can currently attack
			double chosenTime = timeUntilAttack != 0 ? std::min(timeUntilAttack, defaultMoveDuration) : defaultMoveDuration;

            // the chosen movement distance
            int moveDistance       = (int)(chosenTime * unit.speed());

            // DEBUG: If chosen move distance is ever 0, something is wrong
            if (moveDistance == 0)
            {
                SPARCRAFT_ASSERT(false, "Move Action with distance 0 generated. timeUntilAttack: %d, speed: %d", timeUntilAttack, unit.speed());
            }

            // we are only generating moves in the cardinal direction specified in common.h
			for (size_t d(0); d<Num_Directions; ++d)
			{			
                // the direction of this movement
              	Position dir(Constants::Move_Dir[d][0], Constants::Move_Dir[d][1]);
            
                if (moveDistance == 0)
                {
                    printf("%lf %lf %lf\n", timeUntilAttack, defaultMoveDuration, chosenTime);
                }

                // the final destination position of the unit
                Position dest = unit.pos() + Position(moveDistance*dir.x(), moveDistance*dir.y());

                // if that poisition on the map is walkable
                if (state.isWalkable(dest) || (unit.type().isFlyer() && state.isFlyable(dest)))
				{
                    // add the move to the MoveArray
					moves.add(Action(unit.getID(), player, ActionTypes::MOVE, d, dest));
				}
			}
		}

		// if no moves were generated for this unit, it must be issued a 'PASS' move
		if (moves.numMoves(moves.numUnits()-1) == 0)
		{
			moves.add(Action(unit.getID(), player, ActionTypes::PASS, 0));
		}
	}
}