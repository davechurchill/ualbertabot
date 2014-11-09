#include "Constants.h"
#include "Common.h"

namespace BOSS
{
    namespace Races
    {
        RaceID GetRaceID(BWAPIDATA::Race r)
        {
            if (r == BWAPIDATA::Races::Protoss)
            {
                return Races::Protoss;
            }
            else if (r == BWAPIDATA::Races::Terran)
            {
                return Races::Terran;
            }
            else if (r == BWAPIDATA::Races::Zerg)
            {
                return Races::Zerg;
            }
            else
            {
                return Races::None;
            }
        }

        RaceID GetRaceID(const std::string & race)
        {
            if (race.compare("Protoss") == 0)
            {
                return Races::Protoss;
            }
            else if (race.compare("Terran") == 0)
            {
                return Races::Terran;
            }
            else if (race.compare("Zerg") == 0)
            {
                return Races::Zerg;
            }
            else
            {
                return Races::None;
            }
        }
    }

}

