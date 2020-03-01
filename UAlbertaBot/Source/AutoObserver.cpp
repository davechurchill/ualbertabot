#include "AutoObserver.h"
#include "WorkerManager.h"
#include "Global.h"

using namespace UAlbertaBot;

AutoObserver::AutoObserver()
{

}

void AutoObserver::onFrame()
{
    bool pickUnitToFollow = !m_observerFollowingUnit || !m_observerFollowingUnit->exists() || (BWAPI::Broodwar->getFrameCount() - m_cameraLastMoved > m_unitFollowFrames);

    if (pickUnitToFollow)
    {
	    for (auto & unit : BWAPI::Broodwar->self()->getUnits())
	    {
		    if (unit->isUnderAttack() || unit->isAttacking())
		    {
			    m_cameraLastMoved = BWAPI::Broodwar->getFrameCount();
                m_unitFollowFrames = 6;
                m_observerFollowingUnit = unit;
                pickUnitToFollow = false;
                break;
		    }
        }
    }

    if (pickUnitToFollow)
    {
	    for (auto & unit : BWAPI::Broodwar->self()->getUnits())
	    {
		    if (unit->isBeingConstructed() && (unit->getRemainingBuildTime() < 12))
		    {
			    m_cameraLastMoved = BWAPI::Broodwar->getFrameCount();
                m_unitFollowFrames = 24;
                m_observerFollowingUnit = unit;
                pickUnitToFollow = false;
                break;
		    }
        }
    }

    if (pickUnitToFollow)
    {
	    for (auto & unit : BWAPI::Broodwar->self()->getUnits())
	    {
		    if (Global::Workers().isWorkerScout(unit))
		    {
			    m_cameraLastMoved = BWAPI::Broodwar->getFrameCount();
                m_unitFollowFrames = 6;
                m_observerFollowingUnit = unit;
                pickUnitToFollow = false;
                break;
		    }
        }
    }

    if (m_observerFollowingUnit && m_observerFollowingUnit->exists())
    {
        BWAPI::Broodwar->setScreenPosition(m_observerFollowingUnit->getPosition() - BWAPI::Position(320, 180));
    }
}