#pragma once
#include "Common.h"
#include "BuildOrder.h"
#include <BWAPI/Race.h>

namespace UAlbertaBot
{
	typedef std::pair<MetaType, size_t> MetaPair;
	typedef std::vector<MetaPair> MetaPairVector;

	struct StrategyConfiguration
	{
		std::string _name;
		BWAPI::Race _race;
		int         _wins;
		int         _losses;
		BuildOrder  _buildOrder;

		StrategyConfiguration(const std::string & name, const BWAPI::Race & race, const BuildOrder & buildOrder)
			: _name(name)
			, _race(race)
			, _wins(0)
			, _losses(0)
			, _buildOrder(buildOrder)
		{

		}
	};
}
