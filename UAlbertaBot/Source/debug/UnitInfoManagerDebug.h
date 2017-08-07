#pragma once
#include "UnitInfoManager.h"
#include "ScreenCanvas.h"
#include "DebugInfoProvider.h"

namespace AKBot
{
	using UAlbertaBot::UnitInfoManager;

	class UnitInfoManagerDebug : public DebugInfoProvider
	{
		shared_ptr<UnitInfoManager> _unitInfo;
		std::shared_ptr<OpponentView> _opponentView;

		void drawExtendedInterface(ScreenCanvas& canvas) const;
		void drawUnitInformation(ScreenCanvas& canvas, int x, int y) const;
	public:
		UnitInfoManagerDebug(std::shared_ptr<OpponentView> opponentView, shared_ptr<UnitInfoManager> unitInfo);
		void draw(ScreenCanvas& canvas);
	};
}