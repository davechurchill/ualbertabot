#include "Constants.h"
#include "Common.h"

namespace BOSS
{
    namespace Races
    {
        RaceID GetRaceID(BWAPI::Race r)
        {
            if (r == BWAPI::Races::Protoss)
            {
                return Races::Protoss;
            }
            else if (r == BWAPI::Races::Terran)
            {
                return Races::Terran;
            }
            else if (r == BWAPI::Races::Zerg)
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

        std::string GetRaceName(RaceID race)
        {
            if (race == Races::Protoss)
            {
                return "Protoss";
            }
            else if (race == Races::Terran)
            {
                return "Terran";
            }
            else if (race == Races::Zerg)
            {
                return "Zerg";
            }
            else
            {
                return "None";
            }
        }
    }

}

