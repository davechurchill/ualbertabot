#include "SparCraft.h"

namespace SparCraft
{
    void init()
    {
        // Initialize BWAPI data
        BWAPI::BWAPI_init();

        // Initialize Data for Attack Frame Animations
        SparCraft::AnimationFrameData::init();

        // Initialize Random Data for State Hashing
        SparCraft::Hash::initHash();

        // Initialize Weapon and Unit Property Data
        SparCraft::WeaponProperties::Init();
	    SparCraft::UnitProperties::Init();
    
        // Initialize EnumData Class Types
        SparCraft::EnumDataInit();
    }
}