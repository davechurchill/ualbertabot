#pragma once
#include "UnitInfoManager.h"
#include "ScreenCanvas.h"

namespace AKBot
{
	using UAlbertaBot::UnitInfoManager;

	class UnitInfoManagerDebug
	{
		const UnitInfoManager& _unitInfo;
		const OpponentView& _opponentView;

		void drawExtendedInterface(ScreenCanvas& canvas) const;
		void drawUnitInformation(ScreenCanvas& canvas, int x, int y) const;
	public:
		UnitInfoManagerDebug(const OpponentView& opponentView, const UnitInfoManager& unitInfo);
		void draw(ScreenCanvas& canvas) const;
	};
}