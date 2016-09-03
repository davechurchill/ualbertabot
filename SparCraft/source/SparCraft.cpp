#include "SparCraft.h"

namespace SparCraft
{
    void init()
    {
        // Initialize Data for Attack Frame Animations
        SparCraft::AnimationFrameData::init();

        // Initialize Weapon and Unit Property Data
        SparCraft::WeaponProperties::Init();
	    SparCraft::UnitProperties::Init();
    }
}