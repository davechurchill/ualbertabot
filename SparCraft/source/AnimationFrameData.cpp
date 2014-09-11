#include "AnimationFrameData.h"

using namespace SparCraft;

std::vector<AttackFrameData> AnimationFrameData::attackFrameData;

void AnimationFrameData::init()
{
    // allocate the vector according to UnitType size
    attackFrameData = std::vector<AttackFrameData>(BWAPI::UnitTypes::allUnitTypes().size(), AttackFrameData(0,0));

    // Protoss Units
    attackFrameData[BWAPI::UnitTypes::Protoss_Probe.getID()]                = AttackFrameData(2, 2);
    attackFrameData[BWAPI::UnitTypes::Protoss_Zealot.getID()]               = AttackFrameData(8, 7);
    attackFrameData[BWAPI::UnitTypes::Protoss_Dragoon.getID()]              = AttackFrameData(7, 3);
    attackFrameData[BWAPI::UnitTypes::Protoss_Dark_Templar.getID()]         = AttackFrameData(9, 9);
    attackFrameData[BWAPI::UnitTypes::Protoss_Scout.getID()]                = AttackFrameData(2, 2);
    attackFrameData[BWAPI::UnitTypes::Protoss_Corsair.getID()]              = AttackFrameData(8, 8);
    attackFrameData[BWAPI::UnitTypes::Protoss_Arbiter.getID()]              = AttackFrameData(2, 2);
    attackFrameData[BWAPI::UnitTypes::Protoss_Archon.getID()]               = AttackFrameData(1, 1);
    attackFrameData[BWAPI::UnitTypes::Protoss_Photon_Cannon.getID()]        = AttackFrameData(1, 1);

    // Terran Units
    attackFrameData[BWAPI::UnitTypes::Terran_SCV.getID()]                   = AttackFrameData(2, 2);
    attackFrameData[BWAPI::UnitTypes::Terran_Marine.getID()]                = AttackFrameData(8, 6);
    attackFrameData[BWAPI::UnitTypes::Terran_Firebat.getID()]               = AttackFrameData(8, 8);
    attackFrameData[BWAPI::UnitTypes::Terran_Ghost.getID()]                 = AttackFrameData(3, 2);
    attackFrameData[BWAPI::UnitTypes::Terran_Vulture.getID()]               = AttackFrameData(1, 1);
    attackFrameData[BWAPI::UnitTypes::Terran_Goliath.getID()]               = AttackFrameData(1, 1);
    attackFrameData[BWAPI::UnitTypes::Terran_Siege_Tank_Tank_Mode.getID()]  = AttackFrameData(1, 1);
    attackFrameData[BWAPI::UnitTypes::Terran_Siege_Tank_Siege_Mode.getID()] = AttackFrameData(1, 1);
    attackFrameData[BWAPI::UnitTypes::Terran_Wraith.getID()]                = AttackFrameData(2, 2);
    attackFrameData[BWAPI::UnitTypes::Terran_Battlecruiser.getID()]         = AttackFrameData(2, 2);
    attackFrameData[BWAPI::UnitTypes::Terran_Valkyrie.getID()]              = AttackFrameData(40, 40);
    attackFrameData[BWAPI::UnitTypes::Terran_Missile_Turret.getID()]        = AttackFrameData(1, 1);

    // Zerg Units
    attackFrameData[BWAPI::UnitTypes::Zerg_Drone.getID()]                   = AttackFrameData(2, 2);
    attackFrameData[BWAPI::UnitTypes::Zerg_Zergling.getID()]                = AttackFrameData(5, 5);
    attackFrameData[BWAPI::UnitTypes::Zerg_Hydralisk.getID()]               = AttackFrameData(3, 2);
    attackFrameData[BWAPI::UnitTypes::Zerg_Lurker.getID()]                  = AttackFrameData(2, 2);
    attackFrameData[BWAPI::UnitTypes::Zerg_Ultralisk.getID()]               = AttackFrameData(14, 14);
    attackFrameData[BWAPI::UnitTypes::Zerg_Mutalisk.getID()]                = AttackFrameData(1, 1);
    attackFrameData[BWAPI::UnitTypes::Zerg_Devourer.getID()]                = AttackFrameData(9, 9);
    attackFrameData[BWAPI::UnitTypes::Zerg_Sunken_Colony.getID()]           = AttackFrameData(1, 1);
    attackFrameData[BWAPI::UnitTypes::Zerg_Spore_Colony.getID()]            = AttackFrameData(1, 1);
}

const AttackFrameData & AnimationFrameData::getAttackFrames(const BWAPI::UnitType & type)
{
    return attackFrameData[type.getID()];
}