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
    }

}

