#pragma once
#include "MapInformation.h"

namespace AKBot
{
	class BWAPIMapInformation: public MapInformation
	{
	public:
		int getWidth() const override;
		int getHeight() const override;
		bool isWalkable(int x, int y) const override;
		bool isVisible(int x, int y) const override;
		bool isBuildable(int x, int y) const override;
	};
}