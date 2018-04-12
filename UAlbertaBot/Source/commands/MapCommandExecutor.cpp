#include "MapCommandExecutor.h"
#include <boost/algorithm/string/predicate.hpp>
#include <fstream>
#include <BWAPI/Position.h>
#include <BWAPI.h>

using UAlbertaBot::MapTools;
using namespace std;

AKBot::MapCommandExecutor::MapCommandExecutor(UAlbertaBot::MapTools& map)
	: _map(map)
{
}

void AKBot::MapCommandExecutor::onCommand(const std::string & command)
{
	if (command == "dump map") {
		ofstream map_buildable("buildable.txt");
		map_buildable << _map.getWidth() << " " << _map.getHeight() << endl;
		for (auto y = 0; y < _map.getHeight(); y++)
		{
			for (auto x = 0; x < _map.getWidth(); x++)
			{
				auto buildable = _map.isBuildableTile(BWAPI::TilePosition(x, y));
				if (x != 0)
				{
					map_buildable << ",";
				}

				map_buildable << (buildable ? 1 : 0);
			}

			map_buildable << endl;
		}

		map_buildable.close();


		ofstream map_walkable("walkable.txt");
		map_walkable << 4 * _map.getWidth() << " " << 4 * _map.getHeight() << endl;
		for (auto y = 0; y < (4 * _map.getHeight()); y++)
		{
			for (auto x = 0; x < (4 * _map.getWidth()); x++)
			{
				auto walkable = _map.isWalkable(BWAPI::Position(BWAPI::WalkPosition(x, y)));
				if (x != 0)
				{
					map_walkable << ",";
				}

				map_walkable << (walkable ? 1 : 0);
			}

			map_walkable << endl;
		}

		map_walkable.close();
	}
}

bool AKBot::MapCommandExecutor::isSupported(const std::string & command) const
{
	if (boost::starts_with(command, "dump map")) {
		return true;
	}

	return false;
}

