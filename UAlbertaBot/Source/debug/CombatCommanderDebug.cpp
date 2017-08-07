#include "CombatCommanderDebug.h"

namespace AKBot
{
	CombatCommanderDebug::CombatCommanderDebug(shared_ptr<CombatCommander> combatCommander)
		: _combatCommander(combatCommander)
	{
	}
	void CombatCommanderDebug::draw(ScreenCanvas & canvas) const
	{
		auto& squadData = _combatCommander->getSquadData();
		drawSquadData(canvas, squadData);
		if (Config::Debug::DrawSquadInfo)
		{
			drawSquadInformation(canvas, squadData, 200, 30);
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
				? Config::Debug::ColorUnitNearEnemy
				: Config::Debug::ColorUnitNotNearEnemy;
			canvas.drawBoxMap(x - left, y - top, x + right, y + bottom, color);
		}
	}

	void CombatCommanderDebug::drawRegroupPosition(AKBot::ScreenCanvas& canvas, BWAPI::Position regroupPosition) const
	{
		if (Config::Debug::DrawCombatSimulationInfo)
		{
			canvas.drawTextScreen(200, 150, "REGROUP");
		}

		canvas.drawCircleMap(regroupPosition.x, regroupPosition.y, 30, BWAPI::Colors::Purple, true);
	}

	void CombatCommanderDebug::drawSquad(AKBot::ScreenCanvas& canvas, const Squad& squad) const
	{
		if (Config::Debug::DrawSquadInfo)
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
			if (Config::Debug::DrawUnitTargetInfo)
			{
				drawOrder(canvas, squad, squad.getSquadOrder());
			}
		}
	}

	void CombatCommanderDebug::drawOrder(AKBot::ScreenCanvas& canvas, const Squad& squad, const SquadOrder& order) const
	{
		auto& meleeManager = squad.getMeleeManager();
		for (auto & meleeUnit : meleeManager.getUnits())
		{
			if (Config::Debug::DrawUnitTargetInfo)
			{
				canvas.drawLineMap(
					meleeUnit->getPosition().x,
					meleeUnit->getPosition().y,
					meleeUnit->getTargetPosition().x,
					meleeUnit->getTargetPosition().y,
					Config::Debug::ColorLineTarget);
			}
		}
		
		drawOrderText(canvas, meleeManager, order);
		drawOrderText(canvas, squad.getRangedManager(), order);
		drawOrderText(canvas, squad.getTankManager(), order);
		drawOrderText(canvas, squad.getMedicManager(), order);
		drawOrderText(canvas, squad.getDetectorManager(), order);

		drawTransportInformation(canvas, squad.getTransportManager(), 0, 0);
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
}