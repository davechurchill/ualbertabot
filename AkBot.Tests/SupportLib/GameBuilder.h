#pragma once
#include <memory>
#include <BWAPI\Client\GameData.h>
#include "GameImpl.h"
#include "UnitHelper.h"

namespace AKBot
{
	namespace Tests
	{
		class GameBuilder
		{
		private:
			std::shared_ptr<BWAPI::GameData> _data;
			std::shared_ptr<AKBot::GameImpl> impl;
		public:
			GameBuilder() : _data(std::make_shared<BWAPI::GameData>()), impl() {}
			AKBot::GameImpl* getGame()
			{
				if (impl.get() == nullptr) {
					impl = std::make_shared<AKBot::GameImpl>(_data.get());
				}

				return impl.get();
			}
			BWAPI::GameData& data() { return *_data; }
			int nextUnitId() const { return _data->initialUnitCount; }

			GameBuilder& registerUnitDiscoverUnit(int unitId);

			GameBuilder& discoverMineral(BWAPI::Position position, int resources);
			GameBuilder& discoverMineral(int x, int y, int resources);

			GameBuilder& discoverGeyser(BWAPI::Position position, int resources);
			GameBuilder& discoverGeyser(int x, int y, int resources);
			GameBuilder& setP2PForces();
			GameBuilder& setPlayers(int playersCount);
			GameBuilder& setupEmptyMap(int width, int height);
			UnitBuilder nextUnit();
		};
	}
}