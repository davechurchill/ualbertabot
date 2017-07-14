#include "CombatSimulation.h"
#include "Global.h"
#include "UnitUtil.h"

using namespace UAlbertaBot;

CombatSimulation::CombatSimulation(const AKBot::OpponentView& opponentView, const UnitInfoManager& unitInfo)
	: _opponentView(opponentView)
	, _unitInfo(unitInfo)
{
}

// sets the starting states based on the combat units within a radius of a given position
// this center will most likely be the position of the forwardmost combat unit we control
void CombatSimulation::setCombatUnits(const BWAPI::Position & center, const int radius)
{
	SparCraft::GameState s;

	BWAPI::Broodwar->drawCircleMap(center.x, center.y, 10, BWAPI::Colors::Red, true);

	std::vector<BWAPI::Unit> ourCombatUnits;
	std::vector<UnitInfo> enemyCombatUnits;

	Global::Map().GetUnitsInRadius(ourCombatUnits, center, Config::Micro::CombatRegroupRadius, true, false);
	for (auto& enemyPlayer : _opponentView.enemies())
	{
		_unitInfo.getNearbyForce(enemyCombatUnits, center, enemyPlayer, Config::Micro::CombatRegroupRadius);
	}

	for (auto & unit : ourCombatUnits)
	{
        if (unit->getType().isWorker() || unit->getHitPoints() == 0 || unit->getType().isBuilding())
        {
            continue;
        }

        if (UnitUtil::IsCombatUnit(unit) && SparCraft::System::UnitTypeSupported(unit->getType()))
		{
            try
            {
			    s.addUnit(getSparCraftUnit(unit));
            }
            catch (int e)
            {
                e=1;
                BWAPI::Broodwar->printf("Problem Adding Self Unit with ID: %d", unit->getID());
            }
		}
	}

	for (UnitInfo & ui : enemyCombatUnits)
	{ 
        if (ui.type.isWorker() || ui.lastHealth == 0 || ui.type == BWAPI::UnitTypes::Unknown)
        {
            continue;
        }
        
        // if it's a bunker that has a nontrivial amount of hit points, replace it by 5 marines
        if (ui.type == BWAPI::UnitTypes::Terran_Bunker && ui.lastHealth > 10)
        {
            double hpRatio = static_cast<double>(ui.lastHealth) / ui.type.maxHitPoints();

            SparCraft::Unit marine( BWAPI::UnitTypes::Terran_Marine,
                            SparCraft::Position(ui.lastPosition), 
                            ui.unitID, 
                            getSparCraftPlayerID(ui.player), 
                            static_cast<int>(BWAPI::UnitTypes::Terran_Marine.maxHitPoints() * hpRatio), 
                            0,
		                    BWAPI::Broodwar->getFrameCount(), 
                            BWAPI::Broodwar->getFrameCount());	

            for (size_t i(0); i < 5; ++i)
            {
                s.addUnit(marine);
            }
            
            continue;
        }

        if (!ui.type.isFlyer() && SparCraft::System::UnitTypeSupported(ui.type) && ui.completed)
		{
            try
            {
			    s.addUnit(getSparCraftUnit(ui));
            }
            catch (int e)
            {
                BWAPI::Broodwar->printf("Problem Adding Enemy Unit with ID: %d %d", ui.unitID, e);
            }
		}
	}

	_state = s;
}

// Gets a SparCraft unit from a BWAPI::Unit, used for our own units since we have all their info
const SparCraft::Unit CombatSimulation::getSparCraftUnit(BWAPI::Unit unit) const
{
    return SparCraft::Unit( unit->getType(),
                            SparCraft::Position(unit->getPosition()), 
                            unit->getID(), 
                            getSparCraftPlayerID(unit->getPlayer()), 
                            unit->getHitPoints() + unit->getShields(), 
                            0,
		                    BWAPI::Broodwar->getFrameCount(), 
                            BWAPI::Broodwar->getFrameCount());	
}

// Gets a SparCraft unit from a UnitInfo struct, needed to get units of enemy behind FoW
const SparCraft::Unit CombatSimulation::getSparCraftUnit(const UnitInfo & ui) const
{
	BWAPI::UnitType type = ui.type;

    // this is a hack, treat medics as a marine for now
	if (type == BWAPI::UnitTypes::Terran_Medic)
	{
		type = BWAPI::UnitTypes::Terran_Marine;
	}

    return SparCraft::Unit( ui.type, 
                            SparCraft::Position(ui.lastPosition), 
                            ui.unitID, 
                            getSparCraftPlayerID(ui.player), 
                            ui.lastHealth, 
                            0,
		                    BWAPI::Broodwar->getFrameCount(), 
                            BWAPI::Broodwar->getFrameCount());	
}

double CombatSimulation::simulateCombat()
{
    try
    {
	    SparCraft::GameState s1(_state);
        size_t selfID = getSparCraftPlayerID(BWAPI::Broodwar->self());
        size_t enemyID = getSparCraftPlayerID(Global::getEnemy());

        SparCraft::PlayerPtr selfNOK(new SparCraft::Player_AttackClosest(selfID));
        SparCraft::PlayerPtr enemyNOK(new SparCraft::Player_AttackClosest(enemyID));

        SparCraft::PlayerPtr p1 =  SparCraft::AIParameters::Instance().getPlayer(selfID, Config::SparCraft::CombatSimPlayerName);
        SparCraft::PlayerPtr p2 =  SparCraft::AIParameters::Instance().getPlayer(enemyID, Config::SparCraft::CombatSimPlayerName);


	    SparCraft::Game g (s1, p1, p2, 2000);

	    g.play();
	
	    double eval = SparCraft::Eval::Eval(g.getState(), SparCraft::Players::Player_One, SparCraft::EvaluationMethods::LTD2).val();
        //std::cout << "LTD2: " << SparCraft::Eval::LTD2(g.getState(), 0) << ", " << SparCraft::Eval::LTD2(g.getState(), 1) << "\n";

        if (Config::Debug::DrawCombatSimulationInfo)
        {
            std::stringstream ss1;
            ss1 << "Initial State:\n";
            ss1 << SparCraft::AITools::StateToStringCompact(s1) << "\n\n";

            std::stringstream ss2;

            ss2 << "Predicted Outcome: " << eval << "\n";
            ss2 << SparCraft::AITools::StateToStringCompact(g.getState()) << "\n";

            BWAPI::Broodwar->drawTextScreen(150,200,"%s", ss1.str().c_str());
            BWAPI::Broodwar->drawTextScreen(300,200,"%s", ss2.str().c_str());

	        BWAPI::Broodwar->drawTextScreen(240, 280, "Combat Sim : %lf", eval);
        }
        
	    return eval;
    }
    catch (int e)
    {
        BWAPI::Broodwar->printf("SparCraft FatalError, simulateCombat() threw");

        return e;
    }
}

const SparCraft::GameState & CombatSimulation::getSparCraftState() const
{
	return _state;
}

const size_t CombatSimulation::getSparCraftPlayerID(BWAPI::Player player) const
{
	if (player == _opponentView.self())
	{
		return SparCraft::Players::Player_One;
	}
	else if (player == Global::getEnemy())
	{
		return SparCraft::Players::Player_Two;
	}

	return SparCraft::Players::Player_None;
}