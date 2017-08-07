#pragma once

#include "CombatCommander.h"
#include "ScreenCanvas.h"

namespace AKBot
{
	using UAlbertaBot::CombatCommander;
	using UAlbertaBot::SquadData;
	using UAlbertaBot::Squad;
	using UAlbertaBot::SquadOrder;
	using UAlbertaBot::MicroManager;
	using UAlbertaBot::TransportManager;

	class CombatCommanderDebug
	{
		shared_ptr<CombatCommander> _combatCommander;

		void drawSquadData(AKBot::ScreenCanvas& canvas, const SquadData& squadData) const;
		void drawSquad(AKBot::ScreenCanvas& canvas, const Squad& squad) const;
		void drawSquadInformation(AKBot::ScreenCanvas& canvas, const SquadData& squadData, int x, int y) const;
		void drawRegroupPosition(AKBot::ScreenCanvas& canvas, BWAPI::Position regroupPosition) const;
		void drawRegroupStatus(AKBot::ScreenCanvas& canvas, const Squad& squad) const;
		void drawNearbyUnits(AKBot::ScreenCanvas& canvas, const Squad& squad) const;
		void drawOrder(AKBot::ScreenCanvas& canvas, const Squad& squad, const SquadOrder& order) const;
		void drawOrderText(AKBot::ScreenCanvas& canvas, const MicroManager& manager, const SquadOrder & order) const;
		void drawTransportInformation(ScreenCanvas& canvas, const TransportManager& transportManager, int x, int y) const;
	public:
		CombatCommanderDebug(shared_ptr<CombatCommander> combatCommander);
		void draw(ScreenCanvas& canvas) const;
	};
}