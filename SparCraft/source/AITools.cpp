#include "AITools.h"

using namespace SparCraft;

const Unit & AITools::GetClosestEnemyUnit(const GameState & state, const size_t & player, const size_t & unitIndex, bool checkCloaked)
{
    const size_t enemyPlayer = state.getEnemy(player);
	const Unit & myUnit = state.getUnit(player,unitIndex);

	PositionType minDist(1000000);
	size_t minUnitInd(0);
    size_t minUnitID(0);

	Position currentPos = myUnit.currentPosition(state.getTime());

	for (size_t u(0); u < state.numUnits(enemyPlayer); ++u)
	{
        const Unit & enemyUnit = state.getUnit(enemyPlayer, u);
		if (checkCloaked&& enemyUnit.type().hasPermanentCloak())
		{
			bool invisible = true;
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
			if (invisible)
			{
				continue;
			}
		}
        PositionType distSq = myUnit.getDistanceSqToUnit(enemyUnit, state.getTime());

		if ((distSq < minDist))// || ((distSq == minDist) && (enemyUnit.getID() < minUnitID)))
		{
			minDist = distSq;
			minUnitInd = u;
            minUnitID = enemyUnit.getID();
		}
        else if ((distSq == minDist) && (enemyUnit.getID() < minUnitID))
        {
            minDist = distSq;
			minUnitInd = u;
            minUnitID = enemyUnit.getID();
        }
	}

	return state.getUnit(enemyPlayer, minUnitInd);
}

const Unit & AITools::GetClosestOurUnit(const GameState & state, const size_t & player, const size_t & unitIndex)
{
    const Unit & myUnit = state.getUnit(player,unitIndex);

	size_t minDist(1000000);
	size_t minUnitInd(0);

	Position currentPos = myUnit.currentPosition(state.getTime());

	for (size_t u(0); u < state.numUnits(player); ++u)
	{
		if (u == unitIndex || state.getUnit(player, u).canHeal())
		{
			continue;
		}

		//size_t distSq(myUnit.distSq(getUnit(enemyPlayer,u)));
		size_t distSq = currentPos.getDistanceSq(state.getUnit(player, u).currentPosition(state.getTime()));

		if (distSq < minDist)
		{
			minDist = distSq;
			minUnitInd = u;
		}
	}

	return state.getUnit(player, minUnitInd);
}


#define TABS(N) for (int i(0); i<N; ++i) { fprintf(stderr, "\t"); }

// print the state in a neat way
void AITools::StatePrint(const GameState & state, int indent)
{
	TABS(indent);
	std::cout << state.calculateHash(0) << "\n";
	fprintf(stderr, "State - Time: %d\n", state.getTime());

	for (size_t p(0); p<Constants::Num_Players; ++p)
	{
		for (size_t u(0); u < state.numUnits(p); ++u)
		{
			const Unit & unit = state.getUnit(p, u);

			TABS(indent);
			fprintf(stderr, "  P%d %5d %5d    (%3d, %3d)     %s\n", unit.getPlayerID(), unit.currentHP(), unit.firstTimeFree(), unit.x(), unit.y(), unit.name().c_str());
		}
	}
	fprintf(stderr, "\n\n");
}

std::string AITools::StateToString(const GameState & state)
{

	std::stringstream ss;

	ss << state.calculateHash(0) << "\n";
	ss << "Time: " << state.getTime() << std::endl;

	for (size_t p(0); p<Constants::Num_Players; ++p)
	{
		for (size_t u(0); u < state.numUnits(p); ++u)
		{
			const Unit & unit = state.getUnit(p, u);

			ss << "  P" << (int)unit.getPlayerID() << " " << unit.currentHP() << " (" << unit.x() << ", " << unit.y() << ") " << unit.name() << std::endl;
		}
	}
	ss << std::endl;

	return ss.str();
}

std::string AITools::StateToStringCompact(const GameState & state) 
{
	std::stringstream ss;

	for (size_t p(0); p<Constants::Num_Players; ++p)
	{
        std::map<BWAPI::UnitType, size_t> typeCount;

		for (size_t u(0); u < state.numUnits(p); ++u)
		{
			const Unit & unit = state.getUnit(p, u);

            if (typeCount.find(unit.type()) != std::end(typeCount))
            {
                typeCount[unit.type()]++;
            }
            else
            {
                typeCount[unit.type()] = 1;
            }
		}

        for (auto & kv : typeCount)
        {
            const BWAPI::UnitType & type = kv.first;
            const size_t count = kv.second;

            ss << "P" << (int)p << " " << count << " " << type.getName() << "\n";
        }
	}

	return ss.str();
}

Position AITools::CalculatePlayerCenter(const GameState & state, const size_t & player)
{
    Position center(0,0);

    for (size_t u(0); u < state.numUnits(player); ++u)
    {
        const Position & pos = state.getUnit(player, u).currentPosition(state.getTime());
        center.addPosition(pos);
    }

    center = Position((PositionType)((double)center.x() / state.numUnits(player)), (PositionType)((double)center.y() / state.numUnits(player)));

    return center;
}