#pragma once

namespace AKBot
{
	class MapInformation
	{
	public:
		virtual ~MapInformation() = default;
		virtual int getWidth() const = 0;
		virtual int getHeight() const = 0;
		virtual bool isWalkable(int x, int y) const = 0;
		virtual bool isVisible(int x, int y) const = 0;
		virtual bool isBuildable(int x, int y) const = 0;
	};
}