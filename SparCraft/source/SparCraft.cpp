#include "SparCraft.h"

namespace SparCraft
{
    bool isInit = false;

    void init()
    {
        if (!isInit)
        {
            std::cout << "Initializing SparCraft...\n";

            // Initialize Data for Attack Frame Animations
            SparCraft::Config::InitAttackFrames();

            // Initialize Weapon and Unit Property Data
            SparCraft::WeaponProperties::Init();
	        SparCraft::UnitProperties::Init();

            isInit = true;
        }
    }
}