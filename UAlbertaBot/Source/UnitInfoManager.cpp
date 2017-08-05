#include "Common.h"
#include "UnitInfoManager.h"
#include "Global.h"
#include "UnitUtil.h"

using namespace UAlbertaBot;

UnitInfoManager::UnitInfoManager(shared_ptr<AKBot::OpponentView> opponentView)
	: _opponentView(opponentView)
{
	
}

void UnitInfoManager::onStart()
{

}

void UnitInfoManager::update() 
{
	updateUnitInfo();
}

void UnitInfoManager::updateUnitInfo() 
{
	for (auto & unit : UnitUtil::getEnemyUnits())
	{
		updateUnit(unit);
	}

	for (auto & unit : _opponentView->self()->getUnits())
	{
		updateUnit(unit);
	}

	// remove bad enemy units
	_unitData[_opponentView->self()].removeBadUnits();
	for (const auto& enemyPlayer : _opponentView->enemies())
	{
		_unitData[enemyPlayer].removeBadUnits();
	}
}

const std::map<int, UnitInfo> & UnitInfoManager::getUnitInfoMap(BWAPI::Player player) const
{
	return getUnitData(player).getUnitInfoMap();
}

bool UnitInfoManager::isEnemyUnit(BWAPI::Unit unit)
{
	return unit->getPlayer() == Global::getEnemy();
}

void UnitInfoManager::updateUnit(BWAPI::Unit unit)
{
    if (!(unit->getPlayer() == _opponentView->self() || isEnemyUnit(unit)))
    {
        return;
    }

    _unitData[unit->getPlayer()].updateUnit(unit);
}

// is the unit valid?
bool UnitInfoManager::isValidUnit(BWAPI::Unit unit) 
{
	// we only care about our units and enemy units
	if (unit->getPlayer() != _opponentView->self() && !isEnemyUnit(unit))
	{
		return false;
	}

	// if it's a weird unit, don't bother
	if (unit->getType() == BWAPI::UnitTypes::None || unit->getType() == BWAPI::UnitTypes::Unknown ||
		unit->getType() == BWAPI::UnitTypes::Zerg_Larva || unit->getType() == BWAPI::UnitTypes::Zerg_Egg) 
	{
		return false;
	}

	// if the position isn't valid throw it out
	if (!unit->getPosition().isValid()) 
	{
		return false;	
	}

	// s'all good baby baby
	return true;
}

void UnitInfoManager::onUnitDestroy(BWAPI::Unit unit) 
{ 
    if (unit->getType().isNeutral())
    {
        return;
    }

    _unitData[unit->getPlayer()].removeUnit(unit);
}

void UnitInfoManager::getNearbyForce(std::vector<UnitInfo> & unitInfo, BWAPI::Position p, BWAPI::Player player, int radius) const
{
	bool hasBunker = false;
	// for each unit we know about for that player
	for (const auto & kv : getUnitData(player).getUnitInfoMap())
	{
		const UnitInfo & ui(kv.second);
        
		// if it's a combat unit we care about
		// and it's finished! 
		if (UnitUtil::IsCombatUnitType(ui.type) && ui.completed)
		{
			// determine its attack range
			int range = 0;
			if (ui.type.groundWeapon() != BWAPI::WeaponTypes::None)
			{
				range = ui.type.groundWeapon().maxRange() + 40;
			}

			// if it can attack into the radius we care about
			if (ui.lastPosition.getDistance(p) <= (radius + range))
			{
				// add it to the vector
				unitInfo.push_back(ui);
			}
		}
		else if (ui.type.isDetector() && ui.lastPosition.getDistance(p) <= (radius + 250))
        {
			// add it to the vector
			unitInfo.push_back(ui);
        }
	}
}

const UnitData & UnitInfoManager::getUnitData(BWAPI::Player player) const
{
    return _unitData.find(player)->second;
}

bool UnitInfoManager::enemyHasCloakedUnits() const
{
	auto enemy = Global::getEnemy();
	if (enemy == nullptr)
	{
		return false;
	}

    for (const auto & kv : getUnitData(enemy).getUnitInfoMap())
	{
		const UnitInfo & ui(kv.second);

        if (ui.type.isCloakable())
        {
            return true;
        }

        if (ui.type == BWAPI::UnitTypes::Zerg_Lurker || ui.type == BWAPI::UnitTypes::Zerg_Lurker_Egg)
        {
            return true;
        }

        // assume they're going dts
        if (ui.type == BWAPI::UnitTypes::Protoss_Citadel_of_Adun)
        {
            return true;
        }

        if (ui.type == BWAPI::UnitTypes::Protoss_Observatory)
        {
            return true;
        }
    }

    
	return false;
}
