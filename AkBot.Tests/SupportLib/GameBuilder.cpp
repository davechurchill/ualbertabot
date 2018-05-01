#include "GameBuilder.h"

using namespace AKBot::Tests;

UnitBuilder AKBot::Tests::GameBuilder::nextUnit()
{
	auto& unit = _data->units[_data->initialUnitCount];
	auto unitId = _data->initialUnitCount;
	_data->initialUnitCount++;
	return UnitBuilder(unit, unitId);
}

GameBuilder& AKBot::Tests::GameBuilder::registerUnitDiscoverUnit(int unitId)
{
	auto& lastEvent = _data->events[_data->eventCount];
	_data->eventCount++;
	lastEvent.type = BWAPI::EventType::Enum::UnitDiscover;
	lastEvent.v1 = unitId;
	return *this;
}

GameBuilder& AKBot::Tests::GameBuilder::discoverMineral(BWAPI::Position position, int resources)
{
	auto mineral = nextUnit()
		.player(_data->neutral)
		.position(position)
		.mineral(resources);
	registerUnitDiscoverUnit(mineral.data().id);
	return *this;
}

GameBuilder& AKBot::Tests::GameBuilder::discoverMineral(int x, int y, int resources)
{
	return discoverMineral(BWAPI::Position(x, y), resources);
}

GameBuilder& AKBot::Tests::GameBuilder::discoverGeyser(BWAPI::Position position, int resources)
{
	auto geyser = nextUnit()
		.player(_data->neutral)
		.position(position)
		.geyser(resources);
	registerUnitDiscoverUnit(geyser.data().id);
	return *this;
}

GameBuilder& AKBot::Tests::GameBuilder::discoverGeyser(int x, int y, int resources)
{
	return discoverGeyser(BWAPI::Position(x, y), resources);
}

GameBuilder & AKBot::Tests::GameBuilder::setP2PForces()
{
	static char forceNames[][32] = {
		"Players",
		"Observers",
		"Force3",
		"Force4",
		""
	};
	_data->forceCount = 3;
	for (auto i = 0; i < _data->forceCount; i++)
	{
		strcpy_s(_data->forces[i].name, forceNames[i]);
	}

	return *this;
}

GameBuilder & AKBot::Tests::GameBuilder::setPlayers(int playersCount)
{
	static char neutralName[25] = "Neutral";
	static char playerNames[][25] = {
		"Player1",
		"Player2",
		"Player3",
		"Player4",
		"Player5",
		"Player6",
		"Player7",
		"Player8",
		"Player9",
		"Player10",
		"Player11",
		"Player12"
	};

	for (auto i = 0; i < playersCount; i++)
	{
		strcpy_s(_data->players[i].name, playerNames[i]);
		for (auto j = 0; j < playersCount; j++)
		{
			if (j != i)
			{
				_data->players[i].isEnemy[j] = true;
			}
		}
	}

	_data->players[playersCount].isNeutral = true;
	strcpy_s(_data->players[playersCount].name, neutralName);
	_data->playerCount = playersCount + 1;
	_data->neutral = playersCount;

	return *this;
}

GameBuilder & AKBot::Tests::GameBuilder::setupEmptyMap(int width, int height)
{
	_data->mapWidth = width;
	_data->mapHeight = height;

	return *this;
}
