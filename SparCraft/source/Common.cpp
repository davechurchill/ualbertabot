#include "Common.h"

// SEARCH PARAMETERS
char SPARCRAFT_LOGFILE[100] { "sparcraft_error_log.txt" };

namespace SparCraft
{
    namespace System
    {
        void checkSupportedUnitType(const BWAPI::UnitType & type)
        {
            if (type == BWAPI::UnitTypes::None || type == BWAPI::UnitTypes::Unknown)
            {
                SPARCRAFT_ASSERT(false, "Unknown unit type in experiment file, not supported");
            }

            if (type == BWAPI::UnitTypes::Protoss_Corsair || 
                type == BWAPI::UnitTypes::Zerg_Devourer || 
                type == BWAPI::UnitTypes::Zerg_Scourge ||
                type == BWAPI::UnitTypes::Terran_Valkyrie)
            {
                SPARCRAFT_ASSERT(false, "Units with just air weapons currently not supported correctly: %s", type.getName().c_str());
            }

            if (type.isBuilding() && !(type == BWAPI::UnitTypes::Protoss_Photon_Cannon || type == BWAPI::UnitTypes::Zerg_Sunken_Colony || type == BWAPI::UnitTypes::Terran_Missile_Turret))
            {
                SPARCRAFT_ASSERT(false, "Non-attacking buildings not currently supported: %s", type.getName().c_str());
            }

            if (type.isSpellcaster())
            {
                SPARCRAFT_ASSERT(false, "Spell casting units not currently supported: ", type.getName().c_str());
            }

             // Don't support units loading other units yet
            if (type == BWAPI::UnitTypes::Terran_Vulture_Spider_Mine || 
                type == BWAPI::UnitTypes::Protoss_Carrier || 
                type == BWAPI::UnitTypes::Protoss_Interceptor || 
                type == BWAPI::UnitTypes::Protoss_Reaver ||
                type == BWAPI::UnitTypes::Protoss_Scarab ||
                type == BWAPI::UnitTypes::Zerg_Broodling)
            {

                SPARCRAFT_ASSERT(false, "Units which have unit projectiles not supported: %s", type.getName().c_str());
            }
        }

        bool isSupportedUnitType(const BWAPI::UnitType & type)
        {
            if (type == BWAPI::UnitTypes::None || type == BWAPI::UnitTypes::Unknown)
            {
                return false;
            }

            if (type == BWAPI::UnitTypes::Protoss_Corsair || 
                type == BWAPI::UnitTypes::Zerg_Devourer || 
                type == BWAPI::UnitTypes::Zerg_Scourge ||
                type == BWAPI::UnitTypes::Terran_Valkyrie)
            {
                return false;
            }

            if (type.isBuilding() && !(type == BWAPI::UnitTypes::Protoss_Photon_Cannon || type == BWAPI::UnitTypes::Zerg_Sunken_Colony || type == BWAPI::UnitTypes::Terran_Missile_Turret))
            {
                return false;
            }

            if (type.isSpellcaster())
            {
                return false;
            }

            // Don't support units loading other units yet
            if (type == BWAPI::UnitTypes::Terran_Vulture_Spider_Mine || 
                type == BWAPI::UnitTypes::Protoss_Carrier || 
                type == BWAPI::UnitTypes::Protoss_Interceptor || 
                type == BWAPI::UnitTypes::Protoss_Reaver ||
                type == BWAPI::UnitTypes::Protoss_Scarab ||
                type == BWAPI::UnitTypes::Zerg_Broodling)
            {
                return false;
            }

            return true;
        }
    }
};
