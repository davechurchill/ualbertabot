#include "CombatCommanderDebug.h"
#include <iostream>

using std::ofstream;
using std::ostream;
using std::ios;
using std::endl;

ostream& operator<<(ostream& ost, const BWAPI::Unit& ls)
{
	ost << "Unit(Type: " << ls->getType().getName() << ", Position: " << ls->getPosition() << ", " 
		<< "HP/Shld: " << ls->getHitPoints() << "/" << ls->getShields() 
		<< ")";
	return ost;
}

ostream& operator<<(ostream& ost, const AKBot::RangeUnitObservation& observation)
{
	ost << "Has ranged targets: " << observation.rangedTargets << ". "
		<< "Distance to order target: " << observation.orderDistance << ". "
		<< "Want retreat: " << observation.shouldRetreat << ". ";
	if (observation.shouldMove)
	{
		ost << "Move with attack to " << observation.targetPosition;
	}

	if (observation.shouldAttack)
	{
		ost << "Attacking " << observation.targetUnit;
	}

	if (observation.shouldKiteTarget)
	{
		ost << "Kite " << observation.targetUnit;
	}

	if (observation.shouldMutaDance)
	{
		ost << "Mutalisk dance for " << observation.targetUnit;
	}

	return ost;
}

ostream& operator<<(ostream& ost, const AKBot::MeeleUnitObservation& observation)
{
	ost << "Has melee targets: " << observation.meleeTargets << ". "
		<< "Distance to order target: " << observation.orderDistance << ". "
		<< "Want retreat: " << observation.shouldRetreat << ". ";
	if (observation.shouldMove)
	{
		ost << "Moving to " << observation.targetPosition;
	}

	if (observation.shouldAttack)
	{
		ost << "Attacking " << observation.targetUnit;
	}

	return ost;
}

namespace AKBot
{
	CombatCommanderDebug::CombatCommanderDebug(
		shared_ptr<CombatCommander> combatCommander,
		const BotDebugConfiguration& debugConfiguration)
		: _combatCommander(combatCommander)
		, _debugConfiguration(debugConfiguration)
	{
	}
	void CombatCommanderDebug::draw(ScreenCanvas & canvas) const
	{
		auto& squadData = _combatCommander->getSquadData();
		drawSquadData(canvas, squadData);
		if (_debugConfiguration.DrawSquadInfo)
		{
			drawSquadInformation(canvas, squadData, 200, 30);
		}

		if (_debugConfiguration.TraceCombatManagerLogic)
		{
			traceCombatManager();
		}
	}

	void CombatCommanderDebug::drawSquadData(AKBot::ScreenCanvas& canvas, const SquadData& squadData) const
	{
		for (const auto & kv : squadData.getSquads())
		{
			auto& squad = kv.second;
			drawSquad(canvas, squad);
		}
	}

	void CombatCommanderDebug::drawSquadInformation(AKBot::ScreenCanvas& canvas, const SquadData& squadData, int x, int y) const
	{
		canvas.drawTextScreen(x, y, "\x04Squads");
		canvas.drawTextScreen(x, y + 20, "\x04NAME");
		canvas.drawTextScreen(x + 150, y + 20, "\x04SIZE");
		canvas.drawTextScreen(x + 200, y + 20, "\x04LOCATION");

		int yspace = 0;

		for (auto & kv : squadData.getSquads())
		{
			const auto & squad = kv.second;

			auto & units = squad.getUnits();
			const auto & order = squad.getSquadOrder();

			canvas.drawTextScreen(x, y + 40 + ((yspace) * 10), "\x03%s", squad.getName().c_str());
			canvas.drawTextScreen(x + 150, y + 40 + ((yspace) * 10), "\x03%d", units.size());
			canvas.drawTextScreen(x + 200, y + 40 + ((yspace++) * 10), "\x03(%d,%d)", order.getPosition().x, order.getPosition().y);

			canvas.drawCircleMap(order.getPosition(), 10, BWAPI::Colors::Green, true);
			canvas.drawCircleMap(order.getPosition(), order.getRadius(), BWAPI::Colors::Red, false);
			canvas.drawTextMap(order.getPosition() + BWAPI::Position(0, 12), "%s", squad.getName().c_str());

			for (const BWAPI::Unit unit : units)
			{
				canvas.drawTextMap(unit->getPosition() + BWAPI::Position(0, 10), "%s", squad.getName().c_str());
			}
		}
	}

	void CombatCommanderDebug::drawRegroupStatus(AKBot::ScreenCanvas& canvas, const Squad& squad) const
	{
		if (squad.getSquadOrder().getType() == UAlbertaBot::SquadOrderTypes::Attack)
		{
			canvas.drawTextScreen(200, 350, "%s", squad.getRegroupStatus().c_str());
		}
	}

	void CombatCommanderDebug::drawNearbyUnits(AKBot::ScreenCanvas& canvas, const Squad& squad) const
	{
		for (auto & unit : squad.getUnits())
		{
			int x = unit->getPosition().x;
			int y = unit->getPosition().y;

			int left = unit->getType().dimensionLeft();
			int right = unit->getType().dimensionRight();
			int top = unit->getType().dimensionUp();
			int bottom = unit->getType().dimensionDown();

			auto color = squad.isNearEnemy(unit)
				? _debugConfiguration.ColorUnitNearEnemy
				: _debugConfiguration.ColorUnitNotNearEnemy;
			canvas.drawBoxMap(x - left, y - top, x + right, y + bottom, color);
		}
	}

	void CombatCommanderDebug::drawRegroupPosition(AKBot::ScreenCanvas& canvas, BWAPI::Position regroupPosition) const
	{
		if (_debugConfiguration.DrawCombatSimulationInfo)
		{
			canvas.drawTextScreen(200, 150, "REGROUP");
		}

		canvas.drawCircleMap(regroupPosition.x, regroupPosition.y, 30, BWAPI::Colors::Purple, true);
	}

	void CombatCommanderDebug::drawSquad(AKBot::ScreenCanvas& canvas, const Squad& squad) const
	{
		if (_debugConfiguration.DrawSquadInfo)
		{
			drawRegroupStatus(canvas, squad);
			drawNearbyUnits(canvas, squad);
		}

		if (squad.getNeedToRegroup())
		{
			drawRegroupPosition(canvas, squad.getLastRegroupPosition());
		}
		else
		{
			if (_debugConfiguration.DrawUnitTargetInfo)
			{
				drawOrder(canvas, squad, squad.getSquadOrder());
			}
		}
	}

	void CombatCommanderDebug::drawOrder(AKBot::ScreenCanvas& canvas, const Squad& squad, const SquadOrder& order) const
	{
		auto& meleeManager = squad.getMeleeManager();
		auto& rangedManager = squad.getRangedManager();
		auto& tankManager = squad.getTankManager();
		auto& transportManager = squad.getTransportManager();
		if (_debugConfiguration.DrawUnitTargetInfo)
		{
			for (auto & meleeUnit : meleeManager.getUnits())
			{
				canvas.drawLineMap(
					meleeUnit->getPosition().x,
					meleeUnit->getPosition().y,
					meleeUnit->getTargetPosition().x,
					meleeUnit->getTargetPosition().y,
					_debugConfiguration.ColorLineTarget);
			}

			for (auto & rangedUnit : rangedManager.getUnits())
			{
				canvas.drawLineMap(rangedUnit->getPosition(), rangedUnit->getTargetPosition(), BWAPI::Colors::Purple);
			}

			for (auto & unit : tankManager.getUnits())
			{
				canvas.drawLineMap(unit->getPosition(), unit->getTargetPosition(), BWAPI::Colors::Purple);
			}

			for (auto & unit : transportManager.getUnits())
			{
				canvas.drawLineMap(unit->getPosition(), unit->getTargetPosition(), BWAPI::Colors::Purple);
			}
		}
		
		drawOrderText(canvas, meleeManager, order);
		drawOrderText(canvas, rangedManager, order);
		drawOrderText(canvas, tankManager, order);
		drawOrderText(canvas, squad.getMedicManager(), order);
		drawOrderText(canvas, squad.getDetectorManager(), order);

		drawTransportInformation(canvas, transportManager, 0, 0);
	}
	
	void CombatCommanderDebug::drawOrderText(AKBot::ScreenCanvas& canvas, const MicroManager& manager, const SquadOrder & order) const
	{
		for (auto & unit : manager.getUnits())
		{
			canvas.drawTextMap(unit->getPosition().x, unit->getPosition().y, "%s", order.getStatus().c_str());
		}
	}

	void CombatCommanderDebug::drawTransportInformation(AKBot::ScreenCanvas& canvas, const TransportManager& transportManager, int x, int y) const
	{
		if (x && y)
		{
			//canvas.drawTextScreen(x, y, "ScoutInfo: %s", _scoutStatus.c_str());
		}

		auto vertices = transportManager.getMapEdgeVertices();
		for (size_t i(0); i < vertices.size(); ++i)
		{
			canvas.drawCircleMap(vertices[i], 4, BWAPI::Colors::Green, false);
			canvas.drawTextMap(vertices[i], "%d", i);
		}
	}

	void CombatCommanderDebug::traceCombatManager() const
	{
		ofstream outfile;
		outfile.open("combatmanager.txt", ios::out | ios::app);
		outfile << "=======================================================" << endl;
		auto frameCount = BWAPI::Broodwar->getFrameCount();
		outfile << "Frame: " << frameCount << endl << endl;
		auto& squadData = _combatCommander->getSquadData();
		for (auto const & kv : squadData.getSquads())
		{
			const auto & squad = kv.second;
			outfile << "Squad " << squad.getName() << endl;
			outfile << "Units: ";
			auto squadUnits = squad.getUnits();
			if (squadUnits.empty())
			{
				outfile << "No units" << endl;
				continue;
			}

			outfile << endl;

			for (auto const & squadUnit : squad.getUnits())
			{
				outfile << squadUnit << endl;
			}

			outfile << endl;

			auto const& meleeManager = squad.getMeleeManager();
			outfile << "Melee Targets:" << endl;
			auto const& meleeTargets = meleeManager.getTargets();
			for (auto const& meleeTarget : meleeTargets)
			{
				outfile << meleeTarget << endl;
			}

			outfile << "Melee information: ";
			auto const& squadMeleeObservations = meleeManager.getObservations();
			if (!squadMeleeObservations.empty())
			{
				outfile << endl;
				for (auto const & observationData : squadMeleeObservations)
				{
					auto unit = observationData.first;
					auto observation = observationData.second;
					outfile << unit << ". " << observation << endl;
				}
			}
			else
			{
				outfile << "No observations";
			}

			outfile << endl;

			auto const& rangedManager = squad.getRangedManager();
			outfile << "Ranged Targets:" << endl;
			auto const& rangedTargets = rangedManager.getTargets();
			for (auto const& rangedTarget : rangedTargets)
			{
				outfile << rangedTarget << endl;
			}

			outfile << "Ranged information: ";
			auto const& squadRangedObservations = rangedManager.getObservations();
			if (!squadRangedObservations.empty())
			{
				outfile << endl;
				for (auto const & observationData : squadRangedObservations)
				{
					auto unit = observationData.first;
					auto observation = observationData.second;
					outfile << unit << ". " << observation << endl;
				}
			}
			else
			{
				outfile << "No observations";
			}

			outfile << endl;
		}

		outfile.close();
	}
}