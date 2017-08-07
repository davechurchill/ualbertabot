#pragma once
#include "Common.h"
#include "BuildOrder.h"
#include <BWAPI/Race.h>

namespace UAlbertaBot
{
	typedef std::pair<MetaType, size_t> MetaPair;
	typedef std::vector<MetaPair> MetaPairVector;

	struct Strategy
	{
		std::string _name;
		BWAPI::Race _race;
		int         _wins;
		int         _losses;
		BuildOrder  _buildOrder;

		Strategy()
			: _name("None")
			, _race(BWAPI::Races::None)
			, _wins(0)
			, _losses(0)
		{

		}

		Strategy(const std::string & name, const BWAPI::Race & race, const BuildOrder & buildOrder)
			: _name(name)
			, _race(race)
			, _wins(0)
			, _losses(0)
			, _buildOrder(buildOrder)
		{

		}
	};
}
