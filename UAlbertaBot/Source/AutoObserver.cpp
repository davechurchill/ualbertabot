#include "AutoObserver.h"
#include "WorkerManager.h"

using namespace UAlbertaBot;

AutoObserver::AutoObserver()
    : _unitFollowFrames(0)
    , _cameraLastMoved(0)
    , _observerFollowingUnit(NULL)
{

}

void AutoObserver::onFrame()
{
    bool pickUnitToFollow = !_observerFollowingUnit || !_observerFollowingUnit->exists() || (BWAPI::Broodwar->getFrameCount() - _cameraLastMoved > _unitFollowFrames);

    if (pickUnitToFollow)
    {
	    for (BWAPI::UnitInterface * unit : BWAPI::Broodwar->self()->getUnits())
	    {
		    if (unit->isUnderAttack() || unit->isAttacking())
		    {
			    _cameraLastMoved = BWAPI::Broodwar->getFrameCount();
                _unitFollowFrames = 6;
                _observerFollowingUnit = unit;
                pickUnitToFollow = false;
                break;
		    }
        }
    }

    if (pickUnitToFollow)
    {
	    for (BWAPI::UnitInterface * unit : BWAPI::Broodwar->self()->getUnits())
	    {
		    if (unit->isBeingConstructed() && (unit->getRemainingBuildTime() < 12))
		    {
			    _cameraLastMoved = BWAPI::Broodwar->getFrameCount();
                _unitFollowFrames = 24;
                _observerFollowingUnit = unit;
                pickUnitToFollow = false;
                break;
		    }
        }
    }

    if (pickUnitToFollow)
    {
	    for (BWAPI::UnitInterface * unit : BWAPI::Broodwar->self()->getUnits())
	    {
		    if (WorkerManager::Instance().isWorkerScout(unit))
		    {
			    _cameraLastMoved = BWAPI::Broodwar->getFrameCount();
                _unitFollowFrames = 6;
                _observerFollowingUnit = unit;
                pickUnitToFollow = false;
                break;
		    }
        }
    }

    if (_observerFollowingUnit && _observerFollowingUnit->exists())
    {
        BWAPI::Broodwar->setScreenPosition(_observerFollowingUnit->getPosition() - BWAPI::Position(320, 180));
    }
}