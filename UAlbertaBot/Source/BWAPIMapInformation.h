#pragma once
#include <BWAPI\Game.h>
#include "MapInformation.h"

namespace AKBot
{
	class BWAPIMapInformation: public MapInformation
	{
		BWAPI::Game* _game;
	public:
		BWAPIMapInformation(BWAPI::Game* game);
		int getWidth() const override;
		int getHeight() const override;
		bool isWalkable(int x, int y) const override;
		bool isVisible(int x, int y) const override;
		bool isBuildable(int tileX, int tileY) const override;
		bool isExplored(int tileX, int tileY) const override;
		bool isOccupied(int tileX, int tileY) const override;
		const BWAPI::Unitset& getStaticNeutralUnits() const override;
	};
}