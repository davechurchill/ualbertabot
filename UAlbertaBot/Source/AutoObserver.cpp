#include "AutoObserver.h"
#include "Global.h"
#include <BWAPI.h>

using namespace UAlbertaBot;

AutoObserver::AutoObserver(shared_ptr<AKBot::OpponentView> opponentView, shared_ptr<WorkerManager> workerManager)
	: _opponentView(opponentView)
	, _workerManager(workerManager)
	, _unitFollowFrames(0)
    , _cameraLastMoved(0)
    , _observerFollowingUnit(nullptr)
{

}

void AutoObserver::onFrame(int currentFrame)
{
    bool pickUnitToFollow = !_observerFollowingUnit || !_observerFollowingUnit->exists() || (currentFrame - _cameraLastMoved > _unitFollowFrames);

    if (pickUnitToFollow)
    {
	    for (auto & unit : _opponentView->self()->getUnits())
	    {
		    if (unit->isUnderAttack() || unit->isAttacking() || unit->getGroundWeaponCooldown() > 0)
		    {
			    _cameraLastMoved = currentFrame;
                _unitFollowFrames = 6;
                _observerFollowingUnit = unit;
                pickUnitToFollow = false;
                break;
		    }
        }
    }

    if (pickUnitToFollow)
    {
	    for (auto & unit : _opponentView->self()->getUnits())
	    {
		    if (unit->isBeingConstructed() && (unit->getRemainingBuildTime() < 12))
		    {
			    _cameraLastMoved = currentFrame;
                _unitFollowFrames = 24;
                _observerFollowingUnit = unit;
                pickUnitToFollow = false;
                break;
		    }
        }
    }

    if (pickUnitToFollow)
    {
	    for (auto & unit : _opponentView->self()->getUnits())
	    {
		    if (_workerManager->isWorkerScout(unit))
		    {
			    _cameraLastMoved = currentFrame;
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