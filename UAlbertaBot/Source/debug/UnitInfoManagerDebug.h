#pragma once
#include "..\UnitInfoManager.h"
#include "..\ScreenCanvas.h"
#include "DebugInfoProvider.h"
#include "..\BotConfiguration.h"
#include "..\UnitInformation.h"

namespace AKBot
{
	using UAlbertaBot::UnitInfoManager;

	class UnitInfoManagerDebug : public DebugInfoProvider
	{
		shared_ptr<UnitInfoManager> _unitInfo;
		std::shared_ptr<OpponentView> _opponentView;
		std::shared_ptr<AKBot::UnitInformation> _unitInformation;
		const BotDebugConfiguration& _debugConfiguration;

		void drawExtendedInterface(ScreenCanvas& canvas) const;
		void drawUnitInformation(ScreenCanvas& canvas, int x, int y) const;
	public:
		UnitInfoManagerDebug(
			std::shared_ptr<OpponentView> opponentView,
			shared_ptr<UnitInfoManager> unitInfo,
			shared_ptr<AKBot::UnitInformation> unitInformation,
			const BotDebugConfiguration& debugConfiguration);
		UnitInfoManagerDebug(const UnitInfoManagerDebug&) = delete;
		void draw(ScreenCanvas& canvas, int currentFrame);
	};
}